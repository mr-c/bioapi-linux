/*-----------------------------------------------------------------------
 * File: FF_INDEX.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/*
 * This file contains the interface to the flatfile index file. This file holds
 * the hash of all indexes and a pointer to the position in the data file where
 * the data for a particular record can be found.
 */

#define IS_VALID_UID(Rowsize,Uid) \
	( ( ( (Uid) - GET_DATA_START_POS(sizeof(FFI_HEADER)) ) % (Rowsize) ) == 0)

#define DEFINE_FF_INDEX
#include "ff_index.h"
#include "ff_util.h"
#include "maf_collectn.h"
#include "maf_util.h"
#include "bioapi_util.h"

#define CAST_HANDLE(hIndex) \
	FF_INDEX_PTR pThis; \
	ASSERT(hIndex); \
	pThis = (FF_INDEX_PTR)hIndex;


#define INDEX_FILE_IDENTIFIER 0xC20C20AF

struct ffi_header
{
	uint32 HeaderVersion;	 /* Version of the file header */
	uint32 NumIndexes;		 /* Describes the number of indexes in the data file */
	uint32 NumAttributes;	 /* Describes the number of attributes in the data file */
	uint32 RowSize;			 /* Length of each File Data structure for the index file (this value equals sizeof(struct ffi_info_node)+4*NumIndexes for this version) */
};

struct ffi_info_node
{
	uint32	 DataFileOffset;   /* Describes the position, relative to the beginning of the
	data file, where the record can be located, this is zero if the record is deleted */
};

struct ffi_node
{
	struct ffi_info_node Info;
	uint32 rgHash[1]; /* This is actually inaccurate because there can be more than one
		element in this array */
};

typedef struct ffi_header FFI_HEADER;
typedef FFI_HEADER * FFI_HEADER_PTR;
typedef FFI_HEADER const * FFI_CONST_HEADER_PTR;

typedef struct ffi_info_node FFI_INFO_NODE;
typedef FFI_INFO_NODE * FFI_INFO_NODE_PTR;
typedef FFI_INFO_NODE const * FFI_CONST_INFO_NODE_PTR;

typedef struct ffi_node FFI_NODE;
typedef FFI_NODE * FFI_NODE_PTR;
typedef FFI_NODE const * FFI_CONST_NODE_PTR;

/*-----------------------------------------------------------------------------
 * Name: ff_index_WriteNewFile
 *
 * Description:
 * Writes an empty index file to the disk.
 *
 * Parameters:
 * szFileName (input) : Name of the index file
 * NumIndexes (input) : Number of indexes in the schema for this index file
 * NumPureAttribute (input) : Number of attributes that are not used as an index
 * in the schema for this index file
 *
 * Return value:
 * CSSM_OK on success (no errors), else CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN ff_index_WriteNewFile(
	const char *szFileName,
	uint32 NumIndexes,
	uint32 NumPureAttributes,
	const char *FileMode)
{
	CSSM_DATA indexFileHeader;
	FFI_HEADER FileHeader;
	FileHeader.HeaderVersion = FIX_BYTE_SEX(0x100);
	FileHeader.NumIndexes = FIX_BYTE_SEX(NumIndexes);
	FileHeader.NumAttributes = FIX_BYTE_SEX(NumPureAttributes);
	FileHeader.RowSize = FIX_BYTE_SEX(sizeof(FFI_INFO_NODE) + sizeof(uint32) * NumIndexes);

	indexFileHeader.Length = sizeof(FileHeader);
	indexFileHeader.Data = (uint8 *)&FileHeader;

	return ffport_mmf_CreateFile(szFileName, INDEX_FILE_IDENTIFIER, &indexFileHeader, FileMode);
}

/*-------------------------------------------------------------- Construction & Destruction --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ff_index_Construct																		  *
 * ff_index_Deconstruct																		  *
 *--------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: ff_index_Construct
 *
 * Description:
 * Constructs a FF_DATA, and returns a handle to it
 *
 * Parameters:
 * szIndexileName (input) : Name of the index file
 * szDataFileName (input) : Name of the the data file
 * szFreeListFileName (input) : Name of the free list file
 * NumIndexes (input) : Number of indexes in the schema for this index file
 * NumAttribute (input) : Number of attributes that are not used as an index
 * in the schema for this index file
 *
 * Return value:
 * CSSM_OK on success (no errors), else CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN ff_index_Construct(
	FF_INDEX_HANDLE hIndex,
	const char * szIndexFileName,
	const char * szDataFileName,
	const char * szFreeListFileName,
	uint32 NumIndexes,
	uint32 NumPureAttributes,
	const DAL_MODULE_PARAMETERS * Parameters)
{
	CSSM_RETURN retValue = CSSM_OK;
	CSSM_DATA indexFileHeader;
	FFI_HEADER FileHeader;
	CAST_HANDLE(hIndex);

	memset(pThis, 0, sizeof(FF_INDEX));

	VERIFY_PTR(szIndexFileName);

	if ((retValue = ff_data_Construct(&pThis->bfData, szDataFileName, szFreeListFileName,
		NumIndexes + NumPureAttributes + 1, Parameters)) != CSSM_OK)
	{
		return retValue;
	}

	FileHeader.HeaderVersion = FIX_BYTE_SEX(0x100);
	FileHeader.NumIndexes = FIX_BYTE_SEX(NumIndexes);
	FileHeader.NumAttributes = FIX_BYTE_SEX(NumPureAttributes);
	FileHeader.RowSize = FIX_BYTE_SEX(sizeof(FFI_INFO_NODE) + sizeof(uint32) * NumIndexes);

	indexFileHeader.Length = sizeof(FileHeader);
	indexFileHeader.Data = (uint8 *)&FileHeader;

	if ((retValue = ffport_mmf_Construct(&pThis->bfPortIndexFile, szIndexFileName,
		INDEX_FILE_IDENTIFIER, &indexFileHeader, Parameters)) != CSSM_OK)
	{
		return retValue;
	}

	pThis->NumAttributes = NumPureAttributes;
	pThis->NumIndexes = NumIndexes;
	pThis->RowSize = sizeof(FFI_INFO_NODE) + sizeof(uint32) * NumIndexes;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ff_data_Deconstruct
 *
 * Description:
 * Deconstructs a FF_INDEX, but does not free the FF_INDEX buffer
 *
 * Parameters:
 * hData (input) : Handle of FF_INDEX structure to deconstruct
 *
 * Return value:
 * None.
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
void ff_index_Deconstruct(FF_INDEX_HANDLE hIndex)
{
	CAST_HANDLE(hIndex);
	/* Write changes to the data first. This will maintain file integrity if a critical failure
	   occurs durring shutdown */
	ff_data_Deconstruct(&pThis->bfData);
	ffport_mmf_Deconstruct(&pThis->bfPortIndexFile);
}

/*------------------------------------------------------------------------------- Searching --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ff_index_eReadHashArray																	  *
 *--------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: ff_index_eReadHashArray
 *
 * Description:
 * Set the value of status to read the next entry in the index file. If this is
 * the first read, then set the value of *pStatus to DAL_OOB.
 *
 * Parameters:
 * hIndex (input) : Handle of the index file
 * pUid (input/output) : This determines the record to read first. If this is the
 * a new query, *pStatus should equal DAL_OOB. Otherwise, it should have the value from
 * the last call to ff_index_eReadHashArray.
 * rgIndexHashVals (output/optional) : Hash value of the indexes
 * p_hLock (input/output/optional) : optional lock handle that is passed to the mmf file
 * durring a read
 *
 * Return value:
 * CSSM_OK on success, CSSM_DL_NO_DATA_FOUND if no records were found,
 * otherwise the error code returned
 *---------------------------------------------------------------------------*/
FF_ERROR ff_index_eReadHashArray(
	FF_INDEX_HANDLE hIndex,
	FF_UID *pUid,
	uint32 *rgIndexHashVals,
    hLock_t *p_hLock)
{
	FF_UID Uid;
	uint32 Position;
	uint32 DataFileOffset;

	CAST_HANDLE(hIndex);
	ASSERT(pUid);

	/* pUid should always be non-NULL */
	if (!pUid)
	{
		THROW_NEW_ERROR(CSSMERR_DL_INTERNAL_ERROR);
	}

	/* See if we have a starting position */
	Uid = *pUid;
	if (Uid == DAL_OOB)
		Uid = GET_DATA_START_POS(sizeof(FFI_HEADER));
	else
	{
		if (IS_VALID_UID(pThis->RowSize, Uid))
			Uid += pThis->RowSize;
		else
		{
			THROW_NEW_ERROR (CSSMERR_DL_INVALID_RECORD_UID);
		}
	}

	/* Find a node that has not been deleted yet */
	do
	{
		*pUid = Uid;
		CATCH_FUNCTION(ffport_mmf_eRead(&pThis->bfPortIndexFile, Uid, sizeof(DataFileOffset),
			(uint8 *)&DataFileOffset, p_hLock))
		{
			THROW_ERROR();
		}
		Uid += pThis->RowSize;
	}
	while (0 == DataFileOffset);

	/* Make sure we don't use this value since we did not fix the byte sex on it */
	DataFileOffset = 0;

	Position = *pUid + sizeof(FFI_INFO_NODE);

	/* read the hash array */
	if (rgIndexHashVals)
	{
		CATCH_FUNCTION(ffport_mmf_eRead(&pThis->bfPortIndexFile, Position, pThis->NumIndexes * sizeof(uint32),
			(uint8 *)rgIndexHashVals, p_hLock))
		{
			THROW_ERROR();
		}
	}

	/* Fix the byte sex on the data we just retieved */
	if (rgIndexHashVals)
	{
		uint32 i;
		for (i = 0; i < pThis->NumIndexes; i++)
		{
			rgIndexHashVals[i] = FIX_BYTE_SEX(rgIndexHashVals[i]);
		}
	}

	return FF_OK;
}


/*-------------------------------------------------------------- Insert/Read/Delete Records --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ff_index_InsertRecord																	  *
 * ff_index_eReplaceRecord																	  *
 * ff_index_eGetRecord																		  *
 * ff_index_eDeleteRecord																	  *
 *--------------------------------------------------------------------------------------------*/

static FF_ERROR ffi_static_eWriteRecord(
	FF_INDEX_PTR pIndex,
	uint32 *pPosition,
	const DAL_CONST_DATA_PTR rgFields,
	uint32 Semantics,
    hLock_t *p_hLock);

/*----------------------------------------------------------------------------
 * Name: ff_index_InsertRecord
 *
 * Description:
 * Inserts a record into the database
 *
 * Parameters:
 * hIndex (input) : Handle of the index file
 * rgFields (input) : Field values of the new record
 * Semantics (input) : Record Semantics of the new record
 *
 * Return value:
 * UID of the record that was inserted, 0 on failure
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN ff_index_InsertRecord(
	FF_INDEX_HANDLE hIndex,
	const DAL_CONST_DATA_PTR rgFields,
	uint32 Semantics,
	FF_UID *retUid)
{
    hLock_t hLock = DAL_OOB;
	uint32 Offset = DAL_OOB;

	CAST_HANDLE(hIndex);

	THROW_FAILURE(ffport_mmf_eGetWriterLock(&pThis->bfPortIndexFile, &hLock));

	CATCH_FUNCTION(ffi_static_eWriteRecord(pThis, &Offset, rgFields, Semantics, &hLock))
	{
		ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);
		retUid = 0;
		return CSSMERR_DL_INTERNAL_ERROR;
		IGNORE_ERROR();
	}

	ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);
	*retUid = Offset;
	return CSSM_OK;
}

/*----------------------------------------------------------------------------
 * Name: ff_index_eReplaceRecord
 *
 * Description:
 * Replaces the contence of one record with another (used for data modify)
 *
 * Parameters:
 * hIndex (input) : Handle of the index file
 * Uid (input) : Identifies the record to update
 * rgFields (input) : new field values that will replace the existing field values
 * Semantics (input) : Record Semantics that will replace the exising record semantics
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ff_index_eReplaceRecord(
	FF_INDEX_HANDLE hIndex,
	FF_UID Uid,
	const DAL_CONST_DATA_PTR rgFields,
	uint32 Semantics)
{
#ifdef _DEBUG
	const FF_UID debug_Uid = Uid;
#endif
    hLock_t hLock = DAL_OOB;
	FF_ERROR WriteError;
	FFI_INFO_NODE Node, ZeroNode;
	CAST_HANDLE(hIndex);

	if (!IS_VALID_UID(pThis->RowSize, Uid))
	{
		THROW_NEW_ERROR (CSSMERR_DL_INVALID_RECORD_UID);
	}

	THROW_FAILURE(ffport_mmf_eGetWriterLock(&pThis->bfPortIndexFile, &hLock));

	/* Read from the index file, keep the reader lock for the file */
	CATCH_FUNCTION(ffport_mmf_eRead(&pThis->bfPortIndexFile, Uid, sizeof(Node),
		(uint8 *)&Node, &hLock))
	{
		ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);

		if (Error == CSSMERR_DL_ENDOFDATA)
		{
			THROW_NEW_ERROR (CSSMERR_DL_INVALID_RECORD_UID);
		}

		THROW_ERROR();
	}
	Node.DataFileOffset = FIX_BYTE_SEX(Node.DataFileOffset);

	/* If the record is already deleted, indicate this by saying that the
	   record id is invalid */
	if (Node.DataFileOffset == 0)
	{
		ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);
		THROW_NEW_ERROR(CSSMERR_DL_INVALID_RECORD_UID);
	}

	/* Remove the current data file offset from the index file so that we can guarantee
	   database integrity if write back caching is off */
	ZeroNode.DataFileOffset = 0;
	CATCH_FUNCTION(ffport_mmf_eWrite(&pThis->bfPortIndexFile, &Uid, sizeof(ZeroNode),
		(uint8 *)&ZeroNode, 0, &hLock))
	{
		ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);
		THROW_ERROR();
	}
	ASSERT(debug_Uid == Uid);

	/* Delete the record from the data file */
	CATCH_FUNCTION(ff_data_eDeleteData(&pThis->bfData, Node.DataFileOffset))
	{
		ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);
		THROW_ERROR();
	}

	WriteError = ffi_static_eWriteRecord(pThis, &Uid, rgFields, Semantics, &hLock);
	ASSERT(debug_Uid == Uid);

	/* Write failed, we need to attempt to remove the record from the database */
	if (WriteError != FF_OK)
	{
		ffport_mmf_eWrite(&pThis->bfPortIndexFile, &Uid, sizeof(ZeroNode), (uint8 *)&ZeroNode, 0, &hLock);
		ASSERT(debug_Uid == Uid);
	}

	ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);


	return WriteError;
}
/*----------------------------------------------------------------------------
 * Name: ff_index_eGetRecord
 *
 * Description:
 * Reads a record from the database
 *
 * Parameters:
 * hIndex (input) : Handle of the index file
 * Record (input) : Identifers the record that we want to retreive data about
 * FirstField (input) : Number indicating the first field to read (0 to start at the
 * beginning of the record)
 * NumFields (input) : Number of fields in the record to read. note that this
 * number must not be greater that the number of remaining fields in the record
 * rgFields (output) : Array of fields where the reteived data will be held. All
 * data in this array will be overwritten w/o examining its contense (or freeing it).
 * pSemantics (output/optional) : Semantics of the record
 * p_hLock (input/output/optional) : optional lock handle that is passed to the mmf file
 * durring a read
 *
 * Return value:
 * FF_OK on success, CSSMERR_DL_INVALID_RECORD_UID if the record was deleted,
 * otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ff_index_eGetRecord(
	FF_INDEX_HANDLE hIndex,
	FF_UID Record,
	uint32 FirstField,
	uint32 NumFields,
	CSSM_DATA_PTR rgFields,
	uint32 *pSemantics,
    hLock_t *p_hLock)
{
	uint32 i;
	FFI_INFO_NODE Node;

	CAST_HANDLE(hIndex);

	if (!IS_VALID_UID(pThis->RowSize, Record))
	{
		THROW_NEW_ERROR (CSSMERR_DL_INVALID_RECORD_UID);
	}

	/* Clear out the fields */
	for (i = 0; i < NumFields; i++)
	{
		if (rgFields[i].Data)
		{
			BioAPI_free(rgFields[i].Data, NULL);
			rgFields[i].Data = NULL;
		}
		rgFields[i].Length = 0;
	}

	/* Read from the index file, keep the reader lock for the file so that it doesn't
	   get deleted while we are reading it */
	CATCH_FUNCTION(ffport_mmf_eRead(&pThis->bfPortIndexFile, Record, sizeof(Node),
		(uint8 *)&Node, p_hLock))
	{

		if (Error == CSSMERR_DL_ENDOFDATA)
		{
			/* It should not be possible to read past the end of the file, this should
			   indicate the record Uid is not valid */
			THROW_NEW_ERROR(CSSMERR_DL_INVALID_RECORD_UID);
		}
		THROW_ERROR();
	}
	Node.DataFileOffset = FIX_BYTE_SEX(Node.DataFileOffset);

	/* if the record was deleted, return invalid record uid */
	if (Node.DataFileOffset == 0)
	{
		THROW_NEW_ERROR(CSSMERR_DL_INVALID_RECORD_UID);
	}

	/* Read the actual data */
	CATCH_FUNCTION(ff_data_eGetData(&pThis->bfData, Node.DataFileOffset, FirstField, NumFields,
		rgFields, pSemantics))
	{
		THROW_ERROR();
	}

	return FF_OK;
}

/*----------------------------------------------------------------------------
 * Name: ff_index_eDeleteRecord
 *
 * Description:
 * Deletes a record from the index and data files
 *
 * Parameters:
 * hIndex (input) : Handle of the index file
 * Record (input) : Identifers the record that we want to retreive data about
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ff_index_eDeleteRecord(
	FF_INDEX_HANDLE hIndex,
	FF_UID Record)
{
    hLock_t hLock = DAL_OOB;
	uint32 DataFileOffset;
#ifdef _DEBUG
	uint32 debug_OldRecord;
#endif
	FFI_INFO_NODE Node;
	FF_ERROR WriteError;

	CAST_HANDLE(hIndex);

	if (!IS_VALID_UID(pThis->RowSize, Record))
	{
		THROW_NEW_ERROR (CSSMERR_DL_INVALID_RECORD_UID);
	}

	THROW_FAILURE(ffport_mmf_eGetWriterLock(&pThis->bfPortIndexFile, &hLock));

	/* Read from the index file, keep the reader lock for the file */
	CATCH_FUNCTION(ffport_mmf_eRead(&pThis->bfPortIndexFile, Record, sizeof(Node),
		(uint8 *)&Node, &hLock))
	{
		ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);
		if (Error == CSSMERR_DL_ENDOFDATA)
		{
			/* It should not be possible to read past the end of the file, this should
			   indicate the record Uid is not valid */
			THROW_NEW_ERROR(CSSMERR_DL_INVALID_RECORD_UID);
		}

		THROW_ERROR();
	}

	DataFileOffset = FIX_BYTE_SEX(Node.DataFileOffset);

	/* If the record is already deleted, just ignore the error */
	if (Node.DataFileOffset == 0)
	{
		ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);
		return (CSSMERR_DL_RECORD_NOT_FOUND);
	}

	Node.DataFileOffset = 0;

#ifdef _DEBUG
	debug_OldRecord = Record;
#endif

	/* Delete the record from the index file */
	WriteError = ffport_mmf_eWrite(&pThis->bfPortIndexFile, &Record, sizeof(Node),
		(uint8 *)&Node, 0, &hLock);

	/* We can release the lock now because it has been removed from the index file, so
	   it would not be possible for something to find it at this point */
	ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);

	if (WriteError != FF_OK)
		return WriteError;

	ASSERT(debug_OldRecord == Record);

	/* Delete the record from the data file */
	THROW_FAILURE(ff_data_eDeleteData(&pThis->bfData, DataFileOffset));

	return FF_OK;
}

/*----------------------------------------------------------------------------
 * Name: ffi_static_eWriteRecord
 *
 * Description:
 * Static function to insert or update a record.
 *
 * Parameters:
 * pThis (input) : Pointer to the index file object
 * pPosition (input/output) : Identifies the position to insert. If *pPosition is
 * DAL_OOB, then this function acts as an append. On successful return *pPosition will be the
 * location where the data was inserted.
 * rgFields (input) : Values of the fields of the record. ORDER IS SIGNIFICANT. First,
 * the indexes must be listed. There should be NumAttributes+NumIndex+1 entries in this array
 * Semantics (input) : Semantics of the record
 * p_hLock (input/output/optional) : optional lock handle that is passed to the mmf file
 * durring a write
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR ffi_static_eWriteRecord(
	FF_INDEX_PTR pThis,
	uint32 *pPosition,
	const DAL_CONST_DATA_PTR rgFields,
	uint32 Semantics,
    hLock_t *p_hLock)
{
	uint8 *Buffer;
	FFI_NODE_PTR pNode;
	uint32 i;
	ASSERT(pThis->RowSize == pThis->NumIndexes * sizeof(uint32) + sizeof(FFI_INFO_NODE));

	/* Allocate a buffer for the current row */
	Buffer = (uint8*)BioAPI_calloc(pThis->RowSize, 1, NULL);
	if (!Buffer)
	{
		THROW_NEW_ERROR (CSSMERR_DL_MEMORY_ERROR);
	}

	pNode = (FFI_NODE_PTR)Buffer;

	/* Insert data into the data file, store it's offset */
	CATCH_FUNCTION(ff_data_eInsertData(&pThis->bfData, rgFields, Semantics,
		&pNode->Info.DataFileOffset))
	{
		BioAPI_free(Buffer, NULL);
		THROW_ERROR();
	}

	pNode->Info.DataFileOffset = FIX_BYTE_SEX(pNode->Info.DataFileOffset);

	/* Compute hash of index */
	for (i = 0; i < pThis->NumIndexes; i++)
	{
		pNode->rgHash[i] = FIX_BYTE_SEX(ffutil_Hash(&rgFields[i]));
	}

	/* Write to the index file */
	CATCH_FUNCTION(ffport_mmf_eWrite(&pThis->bfPortIndexFile, pPosition, pThis->RowSize,
		Buffer, 0, p_hLock))
	{
		ff_data_eDeleteData(&pThis->bfData, pNode->Info.DataFileOffset);
		BioAPI_free(Buffer, NULL);
		THROW_ERROR();
	}

	ASSERT(IS_VALID_UID(pThis->RowSize, *pPosition));
	BioAPI_free(Buffer, NULL);

	return FF_OK;
}

/*----------------------------------------------------------------------------
 * Name: ff_index_eGetReaderLock
 *
 * Description:
 * Fills p_hLock with the reader lock.
 *
 * Parameters:
 * hIndex (input) : Handle to the index file object
 *
 * p_hLock (input/output/optional) : This is an optional method of preserving a
 * lock after the function exits, or using an already existing lock. If p_hLock
 * is Null, the paramater is ignored. If *p_hLock is DAL_OOB, then it will be
 * filled in with a handle to the lock. This handle must be released. If the
 * function fails and *p_hLock was DAL_OOB, it will remain DAL_OOB.
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ff_index_eGetReaderLock (
	FF_INDEX_HANDLE hIndex,
    hLock_t *p_hLock)
{
	CAST_HANDLE (hIndex);

	CATCH_FUNCTION(ffport_mmf_eGetReaderLock(&pThis->bfPortIndexFile, p_hLock))
	{
		THROW_ERROR ();
	}
	return CSSM_OK;
}

/*----------------------------------------------------------------------------
 * Name: ff_index_eGetReaderLock
 *
 * Description:
 * Releases a lock that was created in another ffport_mmf call
 *
 * Parameters:
 * hIndex (input) : Handle to the index file object
 * hLock (input) : Lock descripter that was returned from a ffport_mmf call
 *
 * Return value:
 * void
 *---------------------------------------------------------------------------*/

void ff_index_eReleaseReaderLock (
	FF_INDEX_HANDLE hIndex,
    hLock_t hLock)
{
	CAST_HANDLE (hIndex);

	ffport_mmf_nrReleaseLock(&pThis->bfPortIndexFile, hLock);
}

