/*-----------------------------------------------------------------------
 * File: DAL_PREPAREFIELD.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the private function DAL_RECORD_TABLE::PrepareField
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

#define DAL_SOURCE_FILE
#ifdef WIN32
#include <float.h>
#else
#include <math.h>		/* isnan() */
#endif
#include "dal_classes.h"
#include "maf_collectn.h"
#include "maf_util.h"
#include "bioapi_util.h"

/* Added to prevent compilation error if CssmTrace.h is not available */
#ifndef CSSMTRACE0
#define CSSMTRACE0
#endif

/* Prototype for a prep function */
typedef CSSM_RETURN (*DAL_FIELD_PREP_FUNC)(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData);

/* Prototypes for functions to conver a particular datatype */
#if (ATTRIBUTE_FORMAT_STRING_SUPPORTED)
static CSSM_RETURN dal_StringFieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData);
#else
#define dal_StringFieldPrep NULL
#endif

#if (ATTRIBUTE_FORMAT_SINT32_SUPPORTED)
static CSSM_RETURN dal_Sint32FieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData);
#else
#define dal_Sint32FieldPrep NULL
#endif

#if (ATTRIBUTE_FORMAT_UINT32_SUPPORTED)
static CSSM_RETURN dal_Uint32FieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData);
#else
#define dal_Uint32FieldPrep NULL
#endif

#if (ATTRIBUTE_FORMAT_BIG_NUM_SUPPORTED)
static CSSM_RETURN dal_BigNumFieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData);
#else
#define dal_BigNumFieldPrep NULL
#endif

#if (ATTRIBUTE_FORMAT_REAL_SUPPORTED)
static CSSM_RETURN dal_RealFieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData);
#else
#define dal_RealFieldPrep NULL
#endif

#if (ATTRIBUTE_FORMAT_TIME_DATE_SUPPORTED)
static CSSM_RETURN dal_TimeDateFieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData);
#else
#define dal_TimeDateFieldPrep NULL
#endif

#if (ATTRIBUTE_FORMAT_BLOB_SUPPORTED)
static CSSM_RETURN dal_BlobFieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData);
#else
#define dal_BlobFieldPrep NULL
#endif

#if (ATTRIBUTE_FORMAT_MULTI_UINT32_SUPPORTED)
static CSSM_RETURN dal_MultiUint32FieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData);
#else
#define dal_MultiUint32FieldPrep NULL
#endif

static const DAL_FIELD_PREP_FUNC s_rgfnPrepare[] = {
	dal_StringFieldPrep, /* CSSM_DB_ATTRIBUTE_FORMAT_STRING = 0 */
	dal_Sint32FieldPrep, /* CSSM_DB_ATTRIBUTE_FORMAT_SINT32 = 1 */
	dal_Uint32FieldPrep, /* CSSM_DB_ATTRIBUTE_FORMAT_UINT32 = 2 */
	dal_BigNumFieldPrep, /* CSSM_DB_ATTRIBUTE_FORMAT_BIG_NUM = 3 */
	dal_RealFieldPrep, /* CSSM_DB_ATTRIBUTE_FORMAT_REAL = 4 */
	dal_TimeDateFieldPrep, /* CSSM_DB_ATTRIBUTE_FORMAT_TIME_DATE = 5 */
	dal_BlobFieldPrep, /* CSSM_DB_ATTRIBUTE_FORMAT_BLOB = 6 */
	dal_MultiUint32FieldPrep /* CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 = 7 */
};


/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::PrepareField
 *
 * Description:
 * This functions checks a field's value and ensure that the value is legal for
 * the attribute format. The function also preforms conversion so that it is simpler
 * for the backend to deal with some atttribute formats. Summary of condition of
 * data on function return (if successful):
 *
 * CSSM_DB_ATTRIBUTE_FORMAT_STRING: Ascii string with no null terminator
 * CSSM_DB_ATTRIBUTE_FORMAT_SINT32: 32-bit signed number
 * CSSM_DB_ATTRIBUTE_FORMAT_UINT32: 32-bit unsigned number
 * CSSM_DB_ATTRIBUTE_FORMAT_BIG_NUM: Type not supported
 * CSSM_DB_ATTRIBUTE_FORMAT_REAL: 64-bit double, not NAN
 * CSSM_DB_ATTRIBUTE_FORMAT_TIME_DATE: Type not supported
 * CSSM_DB_ATTRIBUTE_FORMAT_BLOB: Valid block
 * CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32: valid block of a size that is a multiple of four
 * CSSM_DB_ATTRIBUTE_FORMAT_COMPLEX: Type not supported
 *
 * Parameters:
 * IndexNum (input) : Index number of the field being prepared (only signifcant for the complex type)
 * FieldFormat (input) : Format of the field, used to determine how to prepare the data
 * FieldLength (input) : Length of the field data
 * FieldData (input) : Original data specified by the application
 * ppBuffer (output) : buffer block that is allocated using BioAPI_malloc that needs to be freed by the caller
 * after they are done with pNewData
 * pNewData (ouput) : New CSSM Data structure that will be passed to the backend
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
CSSM_RETURN
DAL_RECORD_TABLE::PrepareField(
	CSSM_DB_ATTRIBUTE_FORMAT FieldFormat,
	uint32 IndexNum,
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA *pNewData) const
{
	ASSERT(ppBuffer);
	ASSERT(pNewData);

	*ppBuffer = NULL;

	if (FieldLength == 0)
	{
		pNewData->Length = FieldLength;
		pNewData->Data = NULL;
		return CSSM_OK;
	}

	CSSM_RETURN retCode = CSSM_OK;

#if (ATTRIBUTE_FORMAT_COMPLEX_SUPPORTED)
	if (FieldFormat == CSSM_DB_ATTRIBUTE_FORMAT_COMPLEX)
	{
		retCode = PrepareComplexField(IndexNum, FieldLength, FieldData, ppBuffer, pNewData);
	}
	else
#endif
	{
		/* check the validity of Field Format, errors should have been found earlier */
		if ((unsigned)FieldFormat >= (sizeof(s_rgfnPrepare) / sizeof(DAL_FIELD_PREP_FUNC)) ||
			s_rgfnPrepare[FieldFormat] == NULL)
		{
			DEBUG_BREAK();
			return (CSSMERR_DL_INVALID_VALUE);
		}

		IndexNum; /* we will not need this until we support the complex type, in which case
			we will use it along with the translation table */

		if (FieldLength == CSSM_FIELDVALUE_COMPLEX_DATA_TYPE)
		{
			/* This is only a valid length if the attribute format is complex */
			retCode = CSSMERR_DL_INVALID_VALUE;
		}
		else
		{
			/* Call the Prepare function for the correct record type */
			ASSERT(CSSM_FALSE == port_IsBadReadPtr(FieldData, FieldLength));
			retCode = s_rgfnPrepare[FieldFormat](FieldLength, FieldData, ppBuffer, pNewData);
		}
	}

	ASSERT(retCode == CSSM_OK || *ppBuffer == NULL);
	ASSERT(*ppBuffer == NULL || port_IsBadWritePtr(*ppBuffer, 1) == CSSM_FALSE);
	ASSERT(pNewData->Data == NULL || port_IsBadReadPtr(pNewData->Data, 1) == CSSM_FALSE);

	return retCode;

}

#if (ATTRIBUTE_FORMAT_STRING_SUPPORTED)
/*-----------------------------------------------------------------------------
 * Name: dal_StringFieldPrep
 *
 * Description:
 * This functions checks a field's value and ensure that the value is legal for
 * the string attribute format. On function exit, pNewData contains an ascii
 * string with no null terminator
 *
 * Parameters:
 * FieldLength (input) : Length of the field data
 * FieldData (input) : Original data specified by the application
 * ppBuffer (output) : not used
 * pNewData (ouput) : New CSSM Data structure that will be passed to the backend
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
static CSSM_RETURN dal_StringFieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData)
{
	ASSERT(FieldLength);
	ASSERT(FieldData);
	ASSERT(!ppBuffer || *ppBuffer == NULL);
	ASSERT(pNewData);

	ASSERT (FieldData[FieldLength - 1] == 0);
	if (FieldData[FieldLength - 1] != 0)
		CSSMTRACE0 ("dal_StringFieldPrep: Insert string without terminating NULL\n");

	pNewData->Length = FieldLength;
	pNewData->Data = (uint8 *)FieldData;

	return CSSM_OK;
}
#endif /* End of String attribute format code */

#if (ATTRIBUTE_FORMAT_SINT32_SUPPORTED)
/*-----------------------------------------------------------------------------
 * Name: dal_Sint32FieldPrep
 *
 * Description:
 * This functions checks a field's value and ensure that the value is legal for
 * the SINT32 attribute format. On success, pNewData is a 32-bit signed number.
 *
 * Parameters:
 * FieldLength (input) : Length of the field data
 * FieldData (input) : Original data specified by the application
 * ppBuffer (output) : buffer block that is allocated using BioAPI_malloc that needs to be freed by the caller
 * after they are done with pNewData
 * pNewData (ouput) : New CSSM Data structure that will be passed to the backend
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
static CSSM_RETURN dal_Sint32FieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData)
{
	ASSERT(FieldLength);
	ASSERT(FieldData);
	ASSERT(ppBuffer);
	ASSERT(*ppBuffer == NULL);
	ASSERT(pNewData);

	sint32 *pIntBuffer = NULL;

	/* Convert to a signed 32 bit integer if we can */
	switch (FieldLength)
	{
	case sizeof(char) :
		*ppBuffer = pIntBuffer = (sint32 *)BioAPI_calloc(sizeof(sint32), 1, NULL);
		if (pIntBuffer == NULL)
			return CSSMERR_DL_MEMORY_ERROR;

		*pIntBuffer = *((char *)FieldData);
		pNewData->Data = (uint8 *)pIntBuffer;
		break;

	case sizeof(sint16) :
		*ppBuffer = pIntBuffer = (sint32 *)BioAPI_calloc(sizeof(sint32), 1, NULL);
		if (pIntBuffer == NULL)
			return CSSMERR_DL_MEMORY_ERROR;

		*pIntBuffer = *((sint16 *)FieldData);
		pNewData->Data = (uint8 *)pIntBuffer;
		break;

	case sizeof(sint32) :
		pNewData->Data = (uint8 *)FieldData;
		break;

	default:
		ASSERT(*ppBuffer == NULL);
		return (CSSMERR_DL_INVALID_VALUE);
	}

	/* the length is now 4 bytes */
	pNewData->Length = sizeof(sint32);
	return CSSM_OK;
}
#endif

#if (ATTRIBUTE_FORMAT_UINT32_SUPPORTED)
/*-----------------------------------------------------------------------------
 * Name: dal_Uint32FieldPrep
 *
 * Description:
 * This functions checks a field's value and ensure that the value is legal for
 * the UINT32 attribute format. On success, pNewData is a 32-bit unsigned number.
 *
 * Parameters:
 * FieldLength (input) : Length of the field data
 * FieldData (input) : Original data specified by the application
 * ppBuffer (output) : buffer block that is allocated using BioAPI_malloc that needs to be freed by the caller
 * after they are done with pNewData
 * pNewData (ouput) : New CSSM Data structure that will be passed to the backend
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
static CSSM_RETURN dal_Uint32FieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData)
{
	ASSERT(FieldLength);
	ASSERT(FieldData);
	ASSERT(ppBuffer);
	ASSERT(*ppBuffer == NULL);
	ASSERT(pNewData);

	uint32 *pIntBuffer = NULL;

	/* Convert to an unsigned 32-bit integer if we can */
	switch (FieldLength)
	{
	case sizeof(uint8) :
		*ppBuffer = pIntBuffer = (uint32 *)BioAPI_calloc(sizeof(uint32), 1, NULL);
		if (pIntBuffer == NULL)
			return CSSMERR_DL_MEMORY_ERROR;

		*pIntBuffer = *FieldData;
		pNewData->Data = (uint8 *)pIntBuffer;
		break;

	case sizeof(uint16) :
		*ppBuffer = pIntBuffer = (uint32 *)BioAPI_calloc(sizeof(uint32), 1, NULL);
		if (pIntBuffer == NULL)
			return CSSMERR_DL_MEMORY_ERROR;

		*pIntBuffer = *((uint16 *)FieldData);
		pNewData->Data = (uint8 *)pIntBuffer;
		break;

	case sizeof(uint32) :
		pNewData->Data = (uint8 *)FieldData;
		break;

	default:
		ASSERT(*ppBuffer == NULL);
		return (CSSMERR_DL_INVALID_VALUE);
	}

	/* the length is now 4 bytes */
	pNewData->Length = sizeof(uint32);
	return CSSM_OK;
}
#endif

#if (ATTRIBUTE_FORMAT_REAL_SUPPORTED)
/*-----------------------------------------------------------------------------
 * Name: dal_RealFieldPrep
 *
 * Description:
 * This functions checks a field's value and ensure that the value is legal for
 * the REAL attribute format. On exist, pNewData is a 64-bit double, which is not NAN.
 *
 * Parameters:
 * FieldLength (input) : Length of the field data
 * FieldData (input) : Original data specified by the application
 * ppBuffer (output) : buffer block that is allocated using BioAPI_malloc that needs to be freed by the caller
 * after they are done with pNewData
 * pNewData (ouput) : New CSSM Data structure that will be passed to the backend
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
static CSSM_RETURN dal_RealFieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData)
{
	ASSERT(FieldLength);
	ASSERT(FieldData);
	ASSERT(ppBuffer);
	ASSERT(*ppBuffer == NULL);
	ASSERT(pNewData);

	double *pDouble;

	/* put the result in pDouble */
	if (FieldLength == sizeof(float))
	{
		/* convert from float to double */
		*ppBuffer = pDouble = (double *)BioAPI_calloc(sizeof(double), 1, NULL);
		if (pDouble == NULL)
			return CSSMERR_DL_MEMORY_ERROR;

		*pDouble = *(float *)FieldData;

	}
	else if (FieldLength == sizeof(double))
	{
		pDouble = (double *)FieldData;
	}
	else
	{
		return (CSSMERR_DL_INVALID_VALUE);
	}

	/* check on the value of *pDouble */
#ifdef _WIN32
	if (_isnan(*pDouble))
#elif defined (UNIX)
	if ( isnan(*pDouble))
#else
#error Method to check for not a number (nan) neeed
#endif
	{
		if (*ppBuffer)
		{
			BioAPI_free(*ppBuffer, NULL);
			*ppBuffer = NULL;
		}
		return (CSSMERR_DL_INVALID_VALUE);
	}

	/* fix the ambiguity in the binary value of 0.0 */
	if (*pDouble == 0.0)
	{
		if (*ppBuffer)
		{
			BioAPI_free(*ppBuffer, NULL);
			*ppBuffer = NULL;
		}

		static const uint8 rgZero[sizeof(double)] = { 0,0,0,0,0,0,0,0 };
		pDouble = (double *)rgZero;
	}

	pNewData->Data = (uint8 *)pDouble;
	pNewData->Length = 8;
	return CSSM_OK;
}
#endif

#if (ATTRIBUTE_FORMAT_BLOB_SUPPORTED)
/*-----------------------------------------------------------------------------
 * Name: dal_BlobFieldPrep
 *
 * Description:
 * This functions checks a field's value and ensure that the value is legal for
 * the BLOB attribute format. This function is just a passthrough which does not
 * effect the value of Blob.
 *
 * Parameters:
 * FieldLength (input) : Length of the field data
 * FieldData (input) : Original data specified by the application
 * ppBuffer (output) : not used
 * pNewData (ouput) : New CSSM Data structure that will be passed to the backend
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
static CSSM_RETURN dal_BlobFieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData)
{
	ASSERT(FieldLength);
	ASSERT(FieldData);
	ASSERT(!ppBuffer || *ppBuffer == NULL);
	ASSERT(pNewData);

	pNewData->Length = FieldLength;
	pNewData->Data = (uint8 *)FieldData;

	return CSSM_OK;
}
#endif

#if (ATTRIBUTE_FORMAT_MULTI_UINT32_SUPPORTED)
/*-----------------------------------------------------------------------------
 * Name: dal_MultiUint32FieldPrep
 *
 * Description:
 * This functions checks a field's value and ensure that the value is legal for
 * the MULTI_UINT32 attribute format. On success, pNewData is a valid block of
 * a size that is a multiple of four
 *
 * Parameters:
 * FieldLength (input) : Length of the field data
 * FieldData (input) : Original data specified by the application
 * ppBuffer (output) : not used
 * pNewData (ouput) : New CSSM Data structure that will be passed to the backend
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
static CSSM_RETURN dal_MultiUint32FieldPrep(
	const uint32 FieldLength,
	const uint8 *FieldData,
	void **ppBuffer,
	CSSM_DATA * pNewData)
{
	ASSERT(FieldLength);
	ASSERT(FieldData);
	ASSERT(!ppBuffer || *ppBuffer == NULL);
	ASSERT(pNewData);

	if (FieldLength % 4 != 0)
		return (CSSMERR_DL_INVALID_VALUE);

	pNewData->Length = FieldLength;
	pNewData->Data = (uint8 *)FieldData;

	return CSSM_OK;
}
#endif
