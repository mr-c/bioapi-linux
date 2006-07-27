/*-----------------------------------------------------------------------
 * File: FF_UTIL.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains some flat file specific backend functions. These are private
 * functions that may not exist in other backends. These functions are
 * simple utilitiy functions
 */

#include "ff_common.h"
#include "ff_util.h"
#include "dlutil.h"
#include <stdio.h>
#if defined (LINUX) || defined (SOLARIS)
#include <ctype.h>
#endif
#include "bioapi_util.h"

/*-----------------------------------------------------------------------------
 * Name: ffutil_AllocateData
 *
 * Description:
 * Allocates an array of CSSM_DATA structures (w/ DL's mem functions)
 *
 * Parameters:
 * prgData (output) : Pointer to a CSSM_DATA_PTR to hold the beggining address of the array
 * NumDataStructs (input) : Number of CSSM_DATA structs in the array
 *
 * Return value:
 * None.
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_RETURN ffutil_AllocateData(CSSM_DATA_PTR * prgData, uint32 NumDataStructs)
{
	if (NumDataStructs == 0)
	{
		*prgData = NULL;
		return CSSM_OK;
	}

	*prgData = (CSSM_DATA_PTR)BioAPI_calloc(sizeof(CSSM_DATA), NumDataStructs, NULL);

	if (!(*prgData))
		return CSSMERR_DL_MEMORY_ERROR;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffutil_nrFreeData
 *
 * Description:
 * Frees an array of CSSM_DATA structures (w/ DL's mem functions)
 *
 * Parameters:
 * prgData (output) : Pointer to a CSSM_DATA_PTR to hold the beggining address of the array
 * NumDataStructs (input) : Number of CSSM_DATA structs in the array
 *
 * Return value:
 * None.
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
void ffutil_nrFreeData(CSSM_DATA_PTR rgData, uint32 NumDataStructs)
{
	if (rgData)
	{
		uint32 i;
		for (i = 0; i < NumDataStructs; i++)
		{
			if (rgData[i].Data)
			{
				BioAPI_free(rgData[i].Data, NULL);
				rgData[i].Data = NULL;
			}
		}

		BioAPI_free(rgData, NULL);
	}
	else
	{
		ASSERT(NumDataStructs == 0);
	}
}

CSSM_RETURN ffutil_CopyData(CSSM_DATA_PTR pDestination, DAL_CONST_DATA_PTR pSource)
{
	VERIFY_PTR(pDestination);
	VERIFY_PTR(pSource);

	if (pDestination->Data)
	{
		BioAPI_free(pDestination->Data, NULL);
		pDestination->Data = NULL;
	}
	/* Destination->Data is now NULL */

	pDestination->Length = pSource->Length;

	if (!pDestination->Length)
		return CSSM_OK;

	pDestination->Data = (uint8*)BioAPI_calloc(pDestination->Length, 1, NULL);

	if (!pDestination->Data)
	{
		pDestination->Length = 0;
		return CSSMERR_DL_MEMORY_ERROR;
	}

	memcpy(pDestination->Data, pSource->Data, pDestination->Length);

	return CSSM_OK;
}

CSSM_RETURN ffutil_CopyDataToApp(
	CSSM_DL_HANDLE DLHandle,
	CSSM_DATA_PTR pDestination,
	DAL_CONST_DATA_PTR pSource)
{
	VERIFY_PTR(pDestination);
	VERIFY_PTR(pSource);

	pDestination->Data = NULL;
	pDestination->Length = pSource->Length;

	if (!pDestination->Length)
		return CSSM_OK;

	pDestination->Data = (uint8 *)App_Calloc(DLHandle, pDestination->Length, 1);

	if (!pDestination->Data)
	{
		pDestination->Length = 0;
		return CSSMERR_DL_MEMORY_ERROR;
	}

	memcpy(pDestination->Data, pSource->Data, pDestination->Length);

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffutil_Hash
 *
 * Description:
 * Returns the hash of a data block. This value is used for indexing.
 *
 * Parameters:
 * pData (input) : Data to Hash
 *
 * Return value:
 * The hash value - a 32-bit integer that is not DAL_OOB
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
uint32 ffutil_Hash(DAL_CONST_DATA_PTR pData)
{
	/* initialize hashVal to something that is about half 1's and half 0's */
	uint32 hashVal = 0x9a73c8d9, i;
	uint32 j = 0, l = 0;

	if (!pData)
		return hashVal;

	/* first initialize to length */
	hashVal ^= pData->Length;

	/* next, hash everything except the last (Length % 4) bytes */
	for (i = 0; i < pData->Length/4; i++)
	{
		/*
		 * Build the uint32 byte-by-byte to avoid unaligned
		 * access errors.  This approach also yields the same
		 * hash value on both big and little endian systems,
		 * which is necessary for sharing ffdb files between
		 * them.
		 */
		l  = ((uint8 *)pData->Data)[j++];
		l |= ((uint8 *)pData->Data)[j++] <<	 8;
		l |= ((uint8 *)pData->Data)[j++] << 16;
		l |= ((uint8 *)pData->Data)[j++] << 24;
		hashVal ^= i - l;
	}

	/* Last, hash the last (Length % 4) bytes */
	for (i *= 4; i < pData->Length; i++)
	{
		hashVal ^= (0xff & pData->Data[i]) << (8 * (3 - (i % 4)));
	}

	/* returned hash value cannot be DAL_OOB (this is a special value reserved for operators
	   such as contains, not equals) where a hash is not useful for comparison */
	if (hashVal != DAL_OOB)
		return hashVal;
	else
		return pData->Data[0];
}

/*-----------------------------------------------------------------------------------
 * Name: ff_GetUniqueRecordIdVal
 *
 * Description:
 * This function verifies the validity of a Unique Record Id and then retrieves the
 * information from it (the 32 bit unique identifier)
 *
 * Parameters:
 * UniqueIdPtr (input) - A Unique Record Id Ptr
 *
 * RETURN:
 * The ID if the Unique ID is valid, otherwise returns 0.
 *
 * ERROR CODES
 * TODO
 *-----------------------------------------------------------------------------------*/
CSSM_RETURN ffutil_GetUniqueRecordIdVal(CSSM_DB_UNIQUE_RECORD const * UniqueIdPtr,
										uint32 *pData)
{
	CSSM_RETURN retValue;
	uint32 *ptmpData;

	if (!UniqueIdPtr)
		return CSSMERR_DL_INVALID_POINTER;

	if ((retValue = dl_IsUniqueRecordIdOk (UniqueIdPtr)) != CSSM_OK)
		return retValue;

	if (UniqueIdPtr->RecordIdentifier.Length != 8)
		return CSSMERR_DL_INVALID_RECORD_UID;

	ptmpData = (uint32 *)UniqueIdPtr->RecordIdentifier.Data;

	if (!ptmpData[1])
		return CSSMERR_DL_INVALID_RECORD_UID;

	*pData = ptmpData[1];

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------------
 * Name: ffutil_fIsBadDbName
 *
 * Description:
 * Determine if the DbName contains invalid characters
 *
 * Parameters:
 * DbName (input) : Name of the database
 *
 * RETURN:
 * True if the name is invalid, otherwise false
 *
 * ERROR CODES:
 * TODO
 *-----------------------------------------------------------------------------------*/
CSSM_BOOL ffutil_fIsBadDbName(const char *DbName)
{
	uint32 i;

	if (DbName == NULL)
		return CSSM_TRUE;

	/* check every character */
	for (i = 0; DbName[i]; i++)
	{
		if (DbName[i] == '\\' || DbName[i] == '/' || DbName[i] == ':' || DbName[i] == '*' ||
			DbName[i] == '?' || DbName[i] == '"' || DbName[i] == '>' || DbName[i] == '<' ||
			DbName[i] == '|' || isspace(DbName[i]))
			return CSSM_TRUE;
	}

	if (i == 0)
		return CSSM_TRUE;

	return CSSM_FALSE;
}

/*-----------------------------------------------------------------------------------
 * Name: ffutil_Contains
 *
 * Description:
 * Determine if predicate is contained in data
 *
 * Parameters:
 * pData (input) : Data to seach in
 * pPredicate (input) : Data to search for
 * WordSize (input) : size of each word, determines the granularity of the seach. Blob's and
 * strings use 1, Multi_uint32 uses 4.
 *
 * RETURN:
 * Non-zero if the predicate equals the data, 0 if it does not equal
 *
 * ERROR CODES:
 * None.
 *-----------------------------------------------------------------------------------*/
FF_BOOL ffutil_Contains(const CSSM_DATA * pData, const CSSM_DATA *pPredicate, uint32 WordSize)
{
	ASSERT(pData);
	ASSERT(pPredicate);
	ASSERT(pData->Length % WordSize == 0);
	ASSERT(pPredicate->Length % WordSize == 0);

	if (pData->Length < pPredicate->Length)
		return 0;
	else
	{
		/* the predicate is smaller than or the same as the data, so it could succeed */
		const uint32 end = pData->Length - pPredicate->Length;
		uint32 i = 0;

		/* compare the blocks until we find a match, or determine we can't */
		for (i = 0; i <= end; i += WordSize)
		{
			if (memcmp(&pData->Data[i], pPredicate->Data, pPredicate->Length) == 0)
				return 1;
		}

		return 0;
	}
}

/*-----------------------------------------------------------------------------------
 * Name: ffutil_MemCmp
 *
 * Description:
 * Determine if two blocks of data are equal or not equal depending on IsEqualReq
 *
 * Parameters:
 * pData (input) : Data from a record
 * pPredicate (input) : Data to compare pData to
 * IsNotEqualReq (input) : Boolean value indicating if it is an equal request (false) or a not
 * equal request (true).
 *
 * RETURN:
 * Non-zero if the result is correct, otherwise zero
 *
 * ERROR CODES:
 * None.
 *-----------------------------------------------------------------------------------*/
FF_BOOL ffutil_MemCompare(const CSSM_DATA * pData, const CSSM_DATA *pPredicate, uint32 IsNotEqualReq)
{
	uint32 res;

	ASSERT(pData);
	ASSERT(pPredicate);

	if (pData->Length != pPredicate->Length)
		return IsNotEqualReq;

	ASSERT(pData->Length == 0 || (pData->Data && pPredicate->Data));
	res = memcmp(pData->Data, pPredicate->Data, pData->Length);

	/* logical not the result if this is an equals reqest */
	if (0 == IsNotEqualReq)
		res = !res;

	return res;
}

#if defined (_DEBUG) && defined(_WIN32)

/*-----------------------------------------------------------------------------------
 * Name: ffutil_debug_OutputError
 *
 * Description:
 * Debug mode utilitiy function to output an error to the debug consol
 *
 * Parameters:
 * Error (input) : Error that occurred
 * Line (input) : Line on which the error occurred
 * File (input) : File in which the error occurred
 *
 * RETURN:
 * The Error paramater that was input
 *
 * ERROR CODES:
 * TODO
 *-----------------------------------------------------------------------------------*/

FF_ERROR ffutil_debug_eOutputError(FF_ERROR Error, uint32 Line, const char * File)
{
	char szErrorBuff[512];
	sprintf(szErrorBuff, "Exception thrown in %s on line %d (error = %d)\n", File, Line, Error);
	OutputDebugString(szErrorBuff);

	return Error;
}

#endif

/*-----------------------------------------------------------------------------
 * Name: ffutil_GetTime
 *
 * Description: retrieves the system time in milliseconds. Suitable for making
 *	 elapsed time computation only.
 *
 * Parameters: None.
 *
 * Returns:
 *	  - time in milliseconds.
 *
 *---------------------------------------------------------------------------*/

uint32 ffutil_GetTime()
{
#if defined (WIN32)
	return (timeGetTime());
#elif defined (UNIX)
	struct timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
}
