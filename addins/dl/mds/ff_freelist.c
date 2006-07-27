/*-----------------------------------------------------------------------
 * File: FF_FREELIST.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the interface to the flatfile free list file. This file is
 * a list to manage the free space in the data file created by file deletes.
 */

#define DEFINE_FF_FREE_LIST
#include "ff_freelist.h"
#include "maf_collectn.h"
#include "maf_util.h"

#define CAST_HANDLE(hFreeList) \
	FF_FREE_LIST_PTR pThis; \
	ASSERT(hFreeList); \
	pThis = (FF_FREE_LIST_PTR)hFreeList;

#define FREE_LIST_FILE_IDENTIFIER 0x87A1504

#pragma
struct free_list_header {
	uint32 HeaderVersion;
};

typedef struct free_list_header FREE_LIST_HEADER;
#define DATA_START_POSITION (GET_DATA_START_POS(sizeof(FREE_LIST_HEADER)))

/*-----------------------------------------------------------------------------
 * Name: ff_freeList_WriteNewFile
 *
 * Description:
 * Writes an empty free list file to the disk.
 *
 * Parameters:
 * szFileName (input) : Name of the free list file
 *
 * Return value:
 * CSSM_OK on success (no errors), else CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN ff_freeList_WriteNewFile(const char *szFileName, const char *FileMode)
{
	CSSM_DATA dataFileHeader;
	FREE_LIST_HEADER FileHeader;

	FileHeader.HeaderVersion = FIX_BYTE_SEX(0x100);
	dataFileHeader.Length = sizeof(FileHeader);
	dataFileHeader.Data = (uint8 *)&FileHeader;

	return ffport_mmf_CreateFile(szFileName, FREE_LIST_FILE_IDENTIFIER, &dataFileHeader, FileMode);
}

/*-------------------------------------------------------------- Construction & Destruction --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ff_freeList_Construct																	  *
 * ff_freeList_Deconstruct																	  *
 *--------------------------------------------------------------------------------------------*/

 /*-----------------------------------------------------------------------------
 * Name: ff_freeList_Construct
 *
 * Description:
 * Constructs a FF_FREE_LIST object, and returns a handle to it
 *
 * Parameters:
 * szFreeListFileName (input) : Name of the free list file
 *
 * Return value:
 * CSSM_OK on success (no errors), else CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN ff_freeList_Construct(
	FF_FREE_LIST_HANDLE hFreeList,
	const char * szFileName,
	const DAL_MODULE_PARAMETERS * Parameters)
{
	CSSM_RETURN retValue = CSSM_OK;
	FREE_LIST_HEADER FileHeader;
	CSSM_DATA dataFileHeader;
	CAST_HANDLE(hFreeList);

	FileHeader.HeaderVersion = FIX_BYTE_SEX(0x100);
	dataFileHeader.Length = sizeof(FileHeader);
	dataFileHeader.Data = (uint8 *)&FileHeader;

	memset(pThis, 0, sizeof(FF_FREE_LIST));

	if ((retValue = ffport_mmf_Construct(&pThis->bfPortFreeListFile, szFileName,
		FREE_LIST_FILE_IDENTIFIER, &dataFileHeader, Parameters)) != CSSM_OK)
	{
		return retValue;
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ff_freeList_Deconstruct
 *
 * Description:
 * Deconstructs a FF_FREE_LIST, but does not free the FF_FREE_LIST buffer
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
void ff_freeList_Deconstruct(FF_FREE_LIST_HANDLE hFreeList)
{
	CAST_HANDLE(hFreeList);
	ffport_mmf_Deconstruct(&pThis->bfPortFreeListFile);
}

/*------------------------------------------------------------------------------ Operations --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ff_freeList_eInsertBlock																	  *
 * ff_freeList_eGetBlock																	  *
 *--------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: fffl_static_eSetBits
 *
 * Description:
 * Static function to set all the bit in the freelist file to either high (1)
 * or low (0). This is used to indicate if a block is used (1), or free (0)
 *
 * Parameters:
 * pThis (input) : FF_FREE_LIST object
 * StartingBlock (input) : First block to set to the new state (high or low)
 * NumBlocks (input) : Number of blocks to switch to high or low
 * SetHigh (input) : Indicates if we are inserting 1's or removing 0's
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR fffl_static_eSetBits(
	FF_FREE_LIST * pThis,
	uint32 StartingBlock,
	uint32 NumBlocks,
	FF_BOOL SetHigh,
    hLock_t *p_hLock)
{
	uint32 Offset32, FirstBit, EndOffset32, EndBit;
	ASSERT(p_hLock);
	ASSERT(*p_hLock && *p_hLock != DAL_OOB);

	Offset32 = StartingBlock / (8 * sizeof(uint32));
	FirstBit = StartingBlock % (8 * sizeof(uint32));
	EndOffset32 = (StartingBlock + NumBlocks) / (8 * sizeof(uint32));
	EndBit = (StartingBlock + NumBlocks) % (8 * sizeof(uint32));

	for (; Offset32 <= EndOffset32; Offset32++, FirstBit = 0)
	{
		uint32 Position8 = Offset32 * sizeof(uint32) + DATA_START_POSITION;
		uint32 Value, BitMask;

		CATCH_FUNCTION(ffport_mmf_eRead(&pThis->bfPortFreeListFile, Position8, sizeof(Value), (uint8 *)&Value,
			p_hLock))
		{
			if (Error != CSSMERR_DL_ENDOFDATA)
			{
				return Error;
			}
			else if (!SetHigh)
			{
				/* If we are setting stuff to low, then it should have already existed at some point,
				   so this indicates file corruption */
				THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
			}
			else
			{
				/* If we are setting stuff to high, this could be an append, so it is okay if it is not
				   a there yet */
				Value = 0;
			}

			IGNORE_ERROR();
		}
		/* Value is big endian!!! */

		/* Form a number that is all zero's from First Bit to Last Bit not including Last Bit, and 1
		   everywhere else. Then, & that onto value. Mathematically it is
			 |zero| = [FirstBit, LastBit),	 |one| = !|zero| */

		if (Offset32 == EndOffset32)
			BitMask = ((1 << (EndBit - FirstBit)) - 1) << FirstBit;
		else
			BitMask = (~0) << FirstBit;

		if (SetHigh)
		{
			/* it should have been all low before */
			ASSERT((Value & FIX_BYTE_SEX(BitMask)) == 0);
			Value |= FIX_BYTE_SEX(BitMask);
		}
		else
		{
			Value &= FIX_BYTE_SEX(~BitMask);
		}

		THROW_FAILURE(ffport_mmf_eWrite(&pThis->bfPortFreeListFile, &Position8, sizeof(Value),
			(uint8 *)&Value, 0, p_hLock));
	}

	return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ff_freeList_eFreeBlock
 *
 * Description:
 * Inserts a block into the free list. This call occurs when a block from the
 * data file is deleted.
 *
 * Parameters:
 * hFreeList (input) : Handle of the FF_FREE_LIST structure
 * dwPosition (input) : Position in the file that is now free
 * dwSize (input) : Size of the block
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ff_freeList_eFreeBlock(
	FF_FREE_LIST_HANDLE hFreeList,
	const FF_BLOCK_INFO * pInsertedBlock)
{
    hLock_t hLock = DAL_OOB;
	FF_ERROR SetBitsError;

	CAST_HANDLE(hFreeList);
	ASSERT(pInsertedBlock);

	THROW_FAILURE(ffport_mmf_eGetWriterLock(&pThis->bfPortFreeListFile, &hLock));

	SetBitsError = fffl_static_eSetBits(pThis, pInsertedBlock->Offset, pInsertedBlock->Size,
		FF_FALSE, &hLock);

	ffport_mmf_nrReleaseLock(&pThis->bfPortFreeListFile, hLock);

	return SetBitsError;
}

/*-----------------------------------------------------------------------------
 * Name: ff_freeList_eGetBlock
 *
 * Description:
 * Get a block from the free list. If no free block is large enough to hold
 * the new data block, then DAL_OOB will be returned in pPosition to indicate that
 * an append should occur.
 *
 * Parameters:
 * hFreeList (input) : Handle of the FF_FREE_LIST structure
 * dwSize (input) : Size of the Data block
 * pPosition (output) : Position to insert the data, DAL_OOB if an append should
 * occur.
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ff_freeList_eGetBlock(
	FF_FREE_LIST_HANDLE hFreeList,
	uint32 NumBlocks,
	uint32 * pFirstBlockOffset)
{
    hLock_t hLock = DAL_OOB;
	uint32 BlockNum = 0, fHaveWriterLock = FF_FALSE, Position8 = DATA_START_POSITION;
	FF_ERROR SetBitsError;

	CAST_HANDLE(hFreeList);
	ASSERT(NumBlocks);
	ASSERT(pFirstBlockOffset);

	*pFirstBlockOffset = 0;

	/* Read from the free list until we either find an open block or hit the end of the
	   file (will return FF_FAIL) */
/* turn off warning C4127: conditional expression is constant */
#pragma warning(disable: 4127)
	while (1)
#pragma warning(default: 4127)
	{
		FF_BOOL Found;
		uint32 ConsecutiveZeros;

		for (ConsecutiveZeros = 0, Found = FF_FALSE; !Found; Position8 += sizeof(uint32))
		{
			uint32 Val, Bit;
			CATCH_FUNCTION(ffport_mmf_eRead(&pThis->bfPortFreeListFile, Position8,
				sizeof(Val), (uint8 *)&Val, &hLock))
			{
				/* Release the reader lock so we can get the writer lock */
				if (Error == CSSMERR_DL_ENDOFDATA)
				{
					ConsecutiveZeros++;
					Found = FF_TRUE;
					Val = 0;
				}
				else
				{
					ffport_mmf_nrReleaseLock(&pThis->bfPortFreeListFile, hLock);
					THROW_ERROR();
				}
			}
			Val = FIX_BYTE_SEX(Val);

			/* Loop through the bits in Val that we just read */
			for (Bit = BlockNum % (sizeof(uint32) * 8);
				 !Found && Bit < (sizeof(uint32) * 8);
				 Bit++, BlockNum++)
			{
				ASSERT(ConsecutiveZeros < NumBlocks);

				if (Val & (1 << Bit))
					ConsecutiveZeros = 0;
				else
				{
					ConsecutiveZeros++;
					Found = ConsecutiveZeros == NumBlocks;
					if (Found) break;
				}
			}
		} /* end for */

		/* Move back to the start of the block we just found and/or verified */
		BlockNum = BlockNum - ConsecutiveZeros + 1;

		if (fHaveWriterLock)
		{
			/* We have found, a block, then gotten the writer lock and verified that it is correct */
			break;
		}

		/* Calculate the starting Position */
		Position8 = (BlockNum / (8 * sizeof(uint32))) * sizeof(uint32) + DATA_START_POSITION;

		/* we have found something, now we need to release the reader lock, and then make sure that
		   no one beat us to the punch */
		ffport_mmf_nrReleaseLock(&pThis->bfPortFreeListFile, hLock);
		hLock = DAL_OOB;
		THROW_FAILURE(ffport_mmf_eGetWriterLock(&pThis->bfPortFreeListFile, &hLock));
		fHaveWriterLock = FF_TRUE;
	}


	SetBitsError = fffl_static_eSetBits(pThis, BlockNum, NumBlocks, FF_TRUE, &hLock);

	ffport_mmf_nrReleaseLock(&pThis->bfPortFreeListFile, hLock);

	if (SetBitsError == FF_OK)
		*pFirstBlockOffset = BlockNum;

	return SetBitsError;
}
