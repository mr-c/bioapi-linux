/*-----------------------------------------------------------------------
 * File: FF_DATA.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the interface to the flatfile data file. This file holds
 * the value for the semantic information, attributes, indexes, and the data
 * object.
 */

#define DEFINE_FF_DATA
#include "ff_data.h"
#include "maf_collectn.h"
#include "maf_util.h"

#define CAST_HANDLE(hData) \
	FF_DATA_PTR pThis; \
	ASSERT(hData); \
	pThis = (FF_DATA_PTR)hData;

#define DATA_FILE_IDENTIFIER 0x2CF06AD5

struct ffd_header
{
	uint32 HeaderVersion;
	uint32 NumFields;
};

typedef struct ffd_header FFD_HEADER;
typedef FFD_HEADER * FFD_HEADER_PTR;

/* Number of bytes in a single block, all records are made up of one or more blocks */
#define BLOCK_SIZE 128
#define DATA_START_POSITION GET_DATA_START_POS(sizeof(FFD_HEADER))

/*-----------------------------------------------------------------------------
 * Name: ff_data_WriteNewFile
 *
 * Description:
 * Writes an empty data file to the disk.
 *
 * Parameters:
 * szFileName (input) : Name of the data file
 * NumFields (input) : Number of fields in the database. This equal the number
 * of indexes + the number of attributes + 1 (for the data object).
 *
 * Return value:
 * CSSM_OK on success (no errors), else CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN ff_data_WriteNewFile(const char *szFileName, uint32 NumFields, const char *FileMode)
{
	CSSM_DATA dataFileHeader;
	FFD_HEADER FileHeader;
	FileHeader.HeaderVersion = FIX_BYTE_SEX(0x100);
	FileHeader.NumFields = FIX_BYTE_SEX(NumFields);

	dataFileHeader.Length = sizeof(FileHeader);
	dataFileHeader.Data = (uint8 *)&FileHeader;

	return ffport_mmf_CreateFile(szFileName, DATA_FILE_IDENTIFIER, &dataFileHeader, FileMode);
}


/*-------------------------------------------------------------- Construction & Destruction --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ff_data_Construct																		  *
 * ff_data_Deconstruct																		  *
 *--------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Name: ff_data_Construct
 *
 * Description:
 * Constructs a FF_DATA, and returns a handle to it
 *
 * Parameters:
 * szDataFileName (input) : Name of the data file
 * szFreeListFileName (input) : Name of the free list file
 * NumFields (input) : Number Of fields to store in the data file (PureAttribute + Indexes + 1)
 *
 * Return value:
 * CSSM_OK on success (no errors), else CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN ff_data_Construct(
	FF_DATA_HANDLE hData,
	const char * szDataFileName,
	const char * szFreeListFileName,
	uint32 NumFields,
	const DAL_MODULE_PARAMETERS * Parameters)
{
	CSSM_RETURN retValue = CSSM_OK;
	CSSM_DATA dataFileHeader;
	FFD_HEADER FileHeader;
	CAST_HANDLE(hData);

	memset(pThis, 0, sizeof(FF_DATA));

	VERIFY_PTR(szDataFileName);

	if ((retValue = ff_freeList_Construct(&pThis->bfFreeList, szFreeListFileName, Parameters)) != CSSM_OK)
		return retValue;

	FileHeader.HeaderVersion = FIX_BYTE_SEX(0x100);
	FileHeader.NumFields = FIX_BYTE_SEX(NumFields);

	dataFileHeader.Length = sizeof(FileHeader);
	dataFileHeader.Data = (uint8 *)&FileHeader;

	if ((retValue = ffport_mmf_Construct(&pThis->bfPortDataFile, szDataFileName,
		DATA_FILE_IDENTIFIER, &dataFileHeader, Parameters)) != CSSM_OK)
	{
		return retValue;
	}

	pThis->NumFields = NumFields;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ff_data_Deconstruct
 *
 * Description:
 * Deconstructs a FF_DATA, but does not free the FF_DATA buffer
 *
 * Parameters:
 * hData (input) : Handle of FF_DATA structure to deconstruct
 *
 * Return value:
 * None.
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
void ff_data_Deconstruct(FF_DATA_HANDLE hData)
{
	CAST_HANDLE(hData);
	/* Write changes to the freelist first. This will maintain file integrity if a critical failure
	   occurs durring shutdown */
	ff_freeList_Deconstruct(&pThis->bfFreeList);
	ffport_mmf_Deconstruct(&pThis->bfPortDataFile);
}

static FF_ERROR ffd_static_eSkipFields(
	FF_DATA * pThis,
	uint32 NumFields,
	uint32 *pPosition,
    hLock_t *p_hLock);

/*------------------------------------------------------------------------------ Operations --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ff_data_eDeleteData																		  *
 * ff_data_eInsertData																		  *
 * ff_data_eGetData																			  *
 *--------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Name: ff_data_eDeleteData
 *
 * Description:
 * Deletes the data located at FilePosition
 *
 * Parameters:
 * hData (input) : Handle of the FF_DATA structure
 * FilePosition (input) : Position in the file corrisponding to a block of
 * data that was inserted with ff_data_eInsertData
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ff_data_eDeleteData(
	FF_DATA_HANDLE hData,
	uint32 FilePosition)
{
	FF_BLOCK_INFO Block;
	uint32 NewPosition;
	CAST_HANDLE(hData);

	/* check the validity of FilePosition */
	if (FilePosition == 0 || FilePosition == DAL_OOB ||
	   (FilePosition - DATA_START_POSITION) % BLOCK_SIZE != 0)
	{
		THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
	}

	/* First get the number of blocks in the record */
	THROW_FAILURE(ffport_mmf_eRead(&pThis->bfPortDataFile, FilePosition, sizeof(Block.Size),
		(uint8 *)&Block.Size, NULL));

	Block.Size = FIX_BYTE_SEX(Block.Size);

	ASSERT(Block.Size);

	/* Verify the integrity of the database by calculating the number of blocks that
	   should be there */
	NewPosition = FilePosition + sizeof(Block.Size);
	CATCH_FUNCTION(ffd_static_eSkipFields(pThis, pThis->NumFields, &NewPosition, NULL))
	{
		THROW_ERROR();
	}
	/* add sizeof(uint32) bytes for Semantics */
	if ((NewPosition + sizeof(uint32) - FilePosition + BLOCK_SIZE - 1) / BLOCK_SIZE != Block.Size)
	{
		/* The number of blocks did not match what we expect */
		THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
	}

	Block.Offset = (FilePosition - DATA_START_POSITION) / BLOCK_SIZE;

	return ff_freeList_eFreeBlock(&pThis->bfFreeList, &Block);
}

/*-----------------------------------------------------------------------------
 * Name: ffd_static_eCompleteInsert
 *
 * Description:
 * Static Function that will complete an insert by adding the fields and the
 * semantics to the data file
 *
 * Parameters:
 * pThis (input) : Pointer to the FF_DATA object
 * bytesFileStartOffset (input) : The starting offset in the data file for the
 * insert
 * bytesPadding (input) : Number of padding bytes to add after the Semantics
 * bytesSizeExpected (input) : Debug information to determine if we counted the bytes
 * we are going to insert correctly
 * rgFields (input) : Array of Fields to insert into the database
 * Semantics (input) : Semantics of the record to be stored
 * p_hLock (input) : Lock object that is passed to the mmf file
 *
 * Note: On failure, this function does not clean up after itself
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR ffd_static_eCompleteInsert(
	FF_DATA * pThis,
	const uint32 bytesFileStartOffset,
	const uint32 bytesPadding,
	const uint32 bytesSizeExpected,
	const DAL_CONST_DATA_PTR rgFields,
	uint32 Semantics,
    hLock_t * p_hLock)
{
	uint32 bytesInserted = sizeof(uint32); /* Number of bytes inserted at current */
	uint32 bytesCurrentPosition; /* The current position in the data file */
	uint32 i;

	ASSERT(pThis);
	ASSERT(rgFields);
	ASSERT(p_hLock);

	bytesCurrentPosition = bytesFileStartOffset + bytesInserted;

	/* Next write out the fields to the data file */
	ASSERT(pThis->NumFields); /* Needs to be > 0 so that the semantics write is not the
	  first write */
	for (i = 0; i < pThis->NumFields; i++)
	{
		/* Write the Data */
		THROW_FAILURE(ffport_mmf_eWriteCssmData(&pThis->bfPortDataFile, &bytesCurrentPosition,
			&rgFields[i], p_hLock));

		ASSERT(bytesFileStartOffset + bytesInserted == bytesCurrentPosition);
		bytesInserted += rgFields[i].Length + sizeof(uint32);

		bytesCurrentPosition = bytesFileStartOffset + bytesInserted;
	}

	/* Write the semantics */
	Semantics = FIX_BYTE_SEX(Semantics);
	THROW_FAILURE(ffport_mmf_eWrite(&pThis->bfPortDataFile, &bytesCurrentPosition,
		sizeof(Semantics), (const uint8 *)&Semantics, bytesPadding, p_hLock));

	ASSERT(bytesFileStartOffset + bytesInserted == bytesCurrentPosition);
	ASSERT(bytesInserted == bytesSizeExpected - sizeof(Semantics));

	return FF_OK;
}


/*-----------------------------------------------------------------------------
 * Name: ff_data_eInsertData
 *
 * Description:
 * Inserts data into the data file for storage
 *
 * Parameters:
 * hData (input) : Handle of the FF_DATA structure
 * rgFields (input) : Array of Fields to insert into the database
 * Semantics (input) : Semantics of the record to be stored
 * pFilePosition (output) : Location in the file that the data was stored at;
 * this is an opaque value that is used to comunicate with data_eDeleteData
 * and data_eGetData.
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ff_data_eInsertData(
	FF_DATA_HANDLE hData,
	const DAL_CONST_DATA_PTR rgFields,
	uint32 Semantics,
	uint32 *pFilePosition)
{
	FF_BLOCK_INFO Destination;
    hLock_t hLock;
	uint32 i;
	uint32 bytesSizeExpected, bytesTranslatedBlockStart, bytesFileStartOffset, bytesPadding;

	CAST_HANDLE(hData);
	ASSERT(pFilePosition);

	*pFilePosition = 0;

	/* first compute the expected size */
	for (bytesSizeExpected = 0, i = 0; i < pThis->NumFields; i++)
	{
		ASSERT(rgFields[i].Length != DAL_OOB);
		ASSERT(!rgFields[i].Length || rgFields[i].Data);

		bytesSizeExpected += rgFields[i].Length + sizeof(rgFields[i].Length);
	}
	bytesSizeExpected += 8;

	/* Calculate the number of bytes of padding we need to make SizeExpected a multiple of
	   BLOCK_SIZE */
	bytesPadding = bytesSizeExpected % BLOCK_SIZE;
	if (bytesPadding)
		bytesPadding = BLOCK_SIZE - bytesPadding;

	/* now compute the number of blocks that our byte values translates to */
	Destination.Size = (bytesPadding + bytesSizeExpected) / BLOCK_SIZE;

	ASSERT((bytesPadding + bytesSizeExpected) % BLOCK_SIZE == 0);
	ASSERT(bytesPadding >= 0 && bytesPadding < BLOCK_SIZE);

	/* Next, query the free list to get the position in the data file where we can dump, this
	   will either return DAL_OOB if there are no free blocks, or it will return the block
	   number, which we can convert to byte offset in the file */
	THROW_FAILURE(ff_freeList_eGetBlock(&pThis->bfFreeList, Destination.Size, &Destination.Offset));
	ASSERT(Destination.Offset != DAL_OOB);

	/* translate from block number to byte offset */
	bytesTranslatedBlockStart = Destination.Offset * BLOCK_SIZE + DATA_START_POSITION;

	/* Initialize the variables */
	hLock = DAL_OOB; /* indicate that we want to retrieve a lock, lock must be realeased */
	bytesFileStartOffset = bytesTranslatedBlockStart;

	/* First write the number of blocks out, this will get the File Offset if it is an append */
	Destination.Size = FIX_BYTE_SEX(Destination.Size);
	CATCH_FUNCTION(ffport_mmf_eWrite(&pThis->bfPortDataFile, &bytesFileStartOffset, sizeof(Destination.Size),
		(uint8 *)&Destination.Size, 0,	&hLock))
	{
		ffport_mmf_nrReleaseLock(&pThis->bfPortDataFile, hLock);
		ff_freeList_eFreeBlock(&pThis->bfFreeList, &Destination);
		THROW_ERROR();
	}
	/* make sure this value is not used again */
	Destination.Size = DAL_OOB;

	/* check the validity of bytesFileStartOffset */
	ASSERT(bytesTranslatedBlockStart == bytesFileStartOffset);

	/* Now complete the insert by adding the Fields and the semantics */
	CATCH_FUNCTION(ffd_static_eCompleteInsert(
		pThis, bytesFileStartOffset, bytesPadding, bytesSizeExpected, rgFields, Semantics, &hLock))
	{
		ffport_mmf_nrReleaseLock(&pThis->bfPortDataFile, hLock);
		ff_freeList_eFreeBlock(&pThis->bfFreeList, &Destination);
		THROW_ERROR();
	}

	/* Clean up, it all worked */
	ffport_mmf_nrReleaseLock(&pThis->bfPortDataFile, hLock);
	*pFilePosition = bytesFileStartOffset;
	return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ff_data_eGetData
 *
 * Description:
 * Retieves data that was inserted into the data file
 *
 * Parameters:
 * hData (input) : Handle of the FF_DATA structure
 * FilePosition (input) : Location the file where the data is stored
 * FirstField (input) : Number indicating the first field to read (0 to start at the
 * beginning of the record)
 * NumFields (input) : Number of fields in the record to read. note that this
 * number must not be greater that the number of remaining fields in the record
 * rgFields (output) : Array of fields where the reteived data will be held. All
 * data in this array will be overwritten w/o examining its contense (or freeing it).
 * pSemantics (output/optional) : Buffer to store the semantics of the record
 *
 * Note: the dl's memory functions are used to allocate the returned memory
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ff_data_eGetData(
	FF_DATA_HANDLE hData,
	uint32 FilePosition,
	uint32 FirstField,
	uint32 NumFields,
	CSSM_DATA_PTR rgFields,
	uint32 *pSemantics)
{
	uint32 CurrFieldNum = 0;
    hLock_t hLock = DAL_OOB;

	CAST_HANDLE(hData);
	ASSERT(FilePosition != DAL_OOB);
	ASSERT(NumFields + FirstField <= pThis->NumFields);

	/* check the validity of FilePosition */
	if (FilePosition == 0 || FilePosition == DAL_OOB ||
	   (FilePosition - DATA_START_POSITION) % BLOCK_SIZE != 0)
	{
		THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
	}

	/* Skip of the number of blocks field */
	FilePosition += sizeof(uint32);

	if (NumFields > 0)
	{
		uint32 i;

		ASSERT(rgFields);

		/* Skip over the first fields if we need to these are ignored */
		if (FirstField != 0)
			THROW_FAILURE(ffd_static_eSkipFields(pThis, FirstField, &FilePosition, &hLock));

		CurrFieldNum = FirstField;

		/* Read the fields */
		for (i = 0; i < NumFields; i++)
		{
			CATCH_FUNCTION(ffport_mmf_eReadCssmData(&pThis->bfPortDataFile, FilePosition,
				&rgFields[i], &hLock))
			{
				ffport_mmf_nrReleaseLock(&pThis->bfPortDataFile, hLock);
				if (Error == CSSMERR_DL_ENDOFDATA)
				{
					/* It should not be possible to read past the end of the file, this should
					   indicate that the file is corruped */
					THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
				}
				THROW_ERROR();
			}

			FilePosition += sizeof(rgFields[i].Length) + rgFields[i].Length;
			CurrFieldNum++;
		}
	}

	/* Read the semantics */
	if (pSemantics != NULL)
	{
		ASSERT(pThis->NumFields >= CurrFieldNum);

		if (pThis->NumFields > CurrFieldNum)
		{
			CATCH_FUNCTION(ffd_static_eSkipFields(pThis, pThis->NumFields - CurrFieldNum,
				&FilePosition, &hLock))
			{
				ffport_mmf_nrReleaseLock(&pThis->bfPortDataFile, hLock);
				THROW_ERROR();
			}
		}

		CATCH_FUNCTION(ffport_mmf_eRead(&pThis->bfPortDataFile, FilePosition, sizeof(*pSemantics),
			(uint8 *)pSemantics, &hLock))
		{
			ffport_mmf_nrReleaseLock(&pThis->bfPortDataFile, hLock);

			if (Error == CSSMERR_DL_ENDOFDATA)
			{
				/* It should not be possible to read past the end of the file, this should
				   indicate that the file is corruped */
				THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
			}

			THROW_ERROR();
		}
		*pSemantics = FIX_BYTE_SEX(*pSemantics);
	}

	ffport_mmf_nrReleaseLock(&pThis->bfPortDataFile, hLock);

	return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffd_static_eSkipFields
 *
 * Description:
 * Skip over fields without reading their value
 *
 * Parameters:
 * pThis (input) : Pointer to the FF_DATA structure
 * NumFields (input) : Number of fields to skip
 * pPosition (input/ouput) : Position to read from the data file, on exit, this
 * value will be the new position
 * p_hLock (input) : A handle to a lock to be filled in (see ff_port.c for more info)
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR ffd_static_eSkipFields(
	FF_DATA * pThis,
	uint32 NumFields,
	uint32 *pPosition,
    hLock_t *p_hLock)
{
	uint32 Position;
	FF_BOOL fDestroyOnFail; /* Do we need to destroy the lock if the function fails? */

	ASSERT(pThis);
	ASSERT(pPosition);

	Position = *pPosition;
	/* False if p_hLock is NULL because ffport will not return a Lock */
	fDestroyOnFail = p_hLock ? (*p_hLock == DAL_OOB) : FF_FALSE;

	/* Read the length for every field to skip, then skip over the data */
	while (NumFields)
	{
		/* Read the length, so that we know how much to skip */
		uint32 Length;
		CATCH_FUNCTION(ffport_mmf_eRead(&pThis->bfPortDataFile, Position, sizeof(Length),
			(uint8 *)&Length, p_hLock))
		{
			if (fDestroyOnFail)
			{
				/* (*p_hLock) was DAL_OOB, so the lock needs to be destroyed */
				ASSERT(p_hLock);
				ffport_mmf_nrReleaseLock(&pThis->bfPortDataFile, *p_hLock);
			}

			if (Error == CSSMERR_DL_ENDOFDATA)
			{
				/* It should not be possible to read past the end of the file, this should
				   indicate that the file is corruped */
				THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
			}

			THROW_ERROR();
		}
		Length = FIX_BYTE_SEX(Length);

		Position += sizeof(Length) + Length;
		NumFields--;
	}

	*pPosition = Position;
	return FF_OK;
}
