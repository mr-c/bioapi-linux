/*-----------------------------------------------------------------------
 * File: DAL_INTERNAL.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains all internal functions used by functions in dlsrc.cpp
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

/* DL include files */
#define DAL_SOURCE_FILE
#include "dal_classes.h"
#include "dal_internal.h"
#include "dlutil.h"
#include "ff_port.h"

#ifndef MODULE_DIRECTORY_SERVICES
#include "maf_interface.h"
#endif

#define IS_VENDOR_DEFINED_RECORDTYPE(rt) \
	((rt) >= CSSM_DB_RECORDTYPE_VENDOR_DEFINED_START && (rt) <= CSSM_DB_RECORDTYPE_VENDOR_DEFINED_END)

/*-----------------------------------------------------------------------------------
 * Name: dal_IsSupportedAttributeFormat
 *
 * Description:
 * This determines if an attribute format is supported
 *
 * Parameters:
 * AttributeFormat (input) : Attribute format to test
 *
 * RETURN:
 * CSSM_TRUE if the attribute format is supported, otherwise CSSM_FALSE
 *
 * ERROR CODES
 * None
 *-----------------------------------------------------------------------------------*/
static CSSM_BOOL dal_IsSupportedAttributeFormat(CSSM_DB_ATTRIBUTE_FORMAT AttributeFormat)
{
#if (ATTRIBUTE_FORMAT_STRING_SUPPORTED)
	if (AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_STRING)
		return CSSM_TRUE;
#endif

#if (ATTRIBUTE_FORMAT_SINT32_SUPPORTED)
	if (AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_SINT32)
		return CSSM_TRUE;
#endif

#if (ATTRIBUTE_FORMAT_UINT32_SUPPORTED)
	if (AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_UINT32)
		return CSSM_TRUE;
#endif

#if (ATTRIBUTE_FORMAT_BIG_NUM_SUPPORTED)
	if (AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_BIG_NUM)
		return CSSM_TRUE;
#endif

#if (ATTRIBUTE_FORMAT_REAL_SUPPORTED)
	if (AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_REAL)
		return CSSM_TRUE;
#endif

#if (ATTRIBUTE_FORMAT_TIME_DATE_SUPPORTED)
	if (AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_TIME_DATE)
		return CSSM_TRUE;
#endif

#if (ATTRIBUTE_FORMAT_BLOB_SUPPORTED)
	if (AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_BLOB)
		return CSSM_TRUE;
#endif

#if (ATTRIBUTE_FORMAT_MULTI_UINT32_SUPPORTED)
	if (AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32)
		return CSSM_TRUE;
#endif

#if (ATTRIBUTE_FORMAT_COMPLEX_SUPPORTED)
	if (AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_COMPLEX)
		return CSSM_TRUE;
#endif

	return CSSM_FALSE;
}

/*-----------------------------------------------------------------------------------
 * Name: dal_GetRecord
 *
 * Description:
 * This function returns information about the current record in the recordset
 *
 * Parameters:
 * pRecordset (input)
 * Attributes (output)
 * Data (output)
 *
 * RETURN:
 * A pointer to the unique record id of the current record
 *
 * ERROR CODES
 * TODO
 *-----------------------------------------------------------------------------------*/
CSSM_RETURN dal_GetCurrentRecordInfo(
	CSSM_DL_DB_HANDLE DLDBHandle,
	DAL_RECORD_TABLE_REF &refRecordTable,
	DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST &AttributeList,
	CSSM_DATA_PTR Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueRecPtr)
{
	CSSM_RETURN retValue = CSSM_OK;
	/* Allocate a set a Unique Record */
	if ((retValue = dlbe_CreateUniqueRecordId(DLDBHandle.DLHandle, UniqueRecPtr)) != CSSM_OK)
		return retValue;

	if (CSSM_OK != refRecordTable->GetUniqueRecordId(*UniqueRecPtr))
	{
		/* The UID we just created should be valid, but it is not */
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		dlbe_FreeUniqueRecordId(DLDBHandle, *UniqueRecPtr);
		*UniqueRecPtr = NULL;
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	/* Get the Data */
	if (Data) {
		if ((retValue = refRecordTable->GetData(DLDBHandle.DLHandle, Data)) != CSSM_OK)
		{
			dlbe_FreeUniqueRecordId(DLDBHandle, *UniqueRecPtr);
			*UniqueRecPtr = NULL;
			return retValue;
		}
	}

	/* Get the Attributes */
	if (!AttributeList.neWasSourceNull())
	{
		uint32 Semantics = 0;
		if ((retValue = refRecordTable->GetSemantics(Semantics)) != CSSM_OK)
		{
			dlbe_FreeUniqueRecordId(DLDBHandle, *UniqueRecPtr);
			*UniqueRecPtr = NULL;
			return retValue;
		}

		AttributeList.nrSetSemanticInformation(Semantics);
		if ((retValue = refRecordTable->GetAttributes(DLDBHandle.DLHandle, AttributeList)) != CSSM_OK)
		{
			dlbe_FreeUniqueRecordId(DLDBHandle, *UniqueRecPtr);
			*UniqueRecPtr = NULL;
			return retValue;
		}
	}

	return CSSM_OK;
}


/*-----------------------------------------------------------------------------------
 * Name: dal_DoesDbExist
 *
 * Description:
 * Determines if a database by DbName exists.
 *
 * Parameters:
 * DBName	(input) : data source name
 *
 * Retrurn:
 * CSSM_TRUE if db exists, CSSM_FALSE otherwise.
 *
 * Note:
 * if failure occurs, *pResult is undefined
 *
 * ERROR CODES:
 *-----------------------------------------------------------------------------------*/
CSSM_BOOL dal_DoesDbExist(
	const char * DbName)
{
	ASSERT(DbName);

	/* Check the list of datastores */
	return dl_dlregDoesDbExist (MODULE_GUID_STRING, DbName);
}

/*-----------------------------------------------------------------------------------
 * Name: dal_ReadDbInfo
 *
 * Description:
 * Reads a DbInfo structure.
 *
 * Parameters:
 * DBName	(input) : data source name
 * pDbInfo (output/optional) : Buffer to place the DbInfo structure
 * pAdditionalInformationLength (output/optional) : Initially, this contains the length of
 * pAdditionalInformationData, on return, this will be filled in with the amount of the
 * buffer used
 * pAdditionalInformationData (output/optional) : pointer to a buffer to hold additional
 * information
 *
 * Retrurn:
 * CSSM_OK on success, an error otherwise
 *
 * ERROR CODES:
 * TODO
 *-----------------------------------------------------------------------------------*/
CSSM_RETURN dal_ReadDbInfo(
	const char * DbName,
	CSSM_DBINFO * pDbInfo,
	uint32 * pAdditionalInformationLength,
	uint8 * pAdditionalInformationData)
{
	CSSM_RETURN retValue = CSSM_OK;

	ASSERT(DbName);

	/* check the list of datastores */
	if (dl_dlregDoesDbExist(MODULE_GUID_STRING, DbName) == CSSM_FALSE)
		return (CSSMERR_DL_DATASTORE_DOESNOT_EXIST);


	/* it is in the list of dynamic datastores */
	if ((retValue = dl_schemaFileReadDb(MODULE_GUID_STRING,
										DbName,
										pDbInfo,
										pAdditionalInformationLength,
										pAdditionalInformationData))
										!= CSSM_OK)
	{
		return retValue;
	}

	/* The Db Info was read succesfully, now make sure that it is valid */
	if (pDbInfo && (dal_IsInvalidDbInfo(pDbInfo) != CSSM_OK))
	{
		dlnr_FreeDbInfo(pDbInfo);
		return CSSMERR_DL_INTERNAL_ERROR;
	}
	else
		return CSSM_OK;
}


/*-----------------------------------------------------------------------------
 * Name: dal_IsInvalidDbInfo
 *
 * Description:
 * This function checks whether there are invalid data values in
 * the input CSSM_DBINFO struct
 *
 * Parameters:
 * DbInfoIn (input) : The CSSM_DBINFO struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether DbInfoIn contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 * CSSMERR_DL_DB_UNSUPPORTED_RECORD_TYPE
 * CSSMERR_DL_UNSUPPORTED_INDEXED_DATA_LOCATION
 * CSSMERR_DL_INVALID_AUTHENTICATION
 * CSSMERR_DL_UNSUPPORTED_RECORD_SIGNING
 * CSSMERR_DL_UNSUPPORTED_LOCALITY
 * CSSMERR_DL_INVALID_DB_INFO
 *
 * NOTE: A NULL input is considered bad
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_IsInvalidDbInfo(CSSM_DBINFO const * DbInfo)
{
	CSSM_RETURN retValue = CSSM_OK;
	uint32 i=0;

	if (!DbInfo)
		return CSSMERR_DL_INVALID_POINTER;

	if ((retValue = dl_IsDbInfoOk(DbInfo)) != CSSM_OK)
	   return retValue;

	if (!DbInfo->NumberOfRecordTypes)
		return CSSMERR_DL_INVALID_RECORDTYPE;

	if (DbInfo->NumberOfRecordTypes >= (1 << 16))
		return CSSMERR_DL_UNSUPPORTED_NUM_RECORDTYPES;

	/* Check array pointers */
	for (i = 0; i < DbInfo->NumberOfRecordTypes; i++)
	{
		/* check to make sure all IndexedDataLocations are known */
		const uint32 NumAttributes = DbInfo->RecordAttributeNames->NumberOfAttributes;
		const uint32 NumIndex = DbInfo->RecordIndexes->NumberOfIndexes;
		const CSSM_DB_ATTRIBUTE_INFO * attributeInfo_ptr = DbInfo->RecordAttributeNames->AttributeInfo;
		const CSSM_DB_INDEX_INFO * indexInfo_ptr = DbInfo->RecordIndexes->IndexInfo;

		if (NumAttributes > MAX_NUM_FIELDS)
			return CSSMERR_DL_UNSUPPORTED_NUM_ATTRIBUTES;

		if (NumIndex > MAX_NUM_FIELDS)
			return CSSMERR_DL_UNSUPPORTED_NUM_INDEXES;

		uint32 j;
		for (j = 0; j < NumAttributes; j++)
		{
			ASSERT(attributeInfo_ptr);
			if (CSSM_FALSE == dal_IsSupportedAttributeFormat(attributeInfo_ptr->AttributeFormat))
				return CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT;

			attributeInfo_ptr++;
		}

		for(j=0; j < NumIndex; j++)
		{
			ASSERT(indexInfo_ptr);
			if ((indexInfo_ptr->IndexedDataLocation != CSSM_DB_INDEX_ON_ATTRIBUTE) &&
						(indexInfo_ptr->IndexedDataLocation != CSSM_DB_INDEX_ON_RECORD))
				return CSSMERR_DL_UNSUPPORTED_INDEX_INFO;

			if (CSSM_FALSE == dal_IsSupportedAttributeFormat(indexInfo_ptr->Info.AttributeFormat))
				return CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT;

			indexInfo_ptr++;
		}
	}

	/* Check locality */
	if(DbInfo->IsLocal != (CSSM_BOOL)CSSM_TRUE)
		return CSSMERR_DL_UNSUPPORTED_LOCALITY;

	/* Check access path */
    if(DbInfo->AccessPath != NULL)
	{
		if (ffport_neDoesFileExist(DbInfo->AccessPath) == CSSM_FALSE)
		{
			return (CSSMERR_DL_INVALID_POINTER);
		}
    }

	/* Check reserved */
	if(DbInfo->Reserved != NULL)
		return CSSMERR_DL_INVALID_POINTER;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsInvalidQueryPtr
 *
 * Description:
 * This function checks whether there are any invalid/unsupported values in
 * the input CSSM_QUERY struct
 *
 * Parameters:
 * Query (input) : The CSSM_QUERY struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether Query contains any bad pointers
 *
 * Error Codes:
 * CSSM_DL_INVALID_QUERY
 * CSSMERR_DL_INVALID_POINTER
 * CSSM_DL_INVALID_SELECTION_PRED
 * CSSM_DL_DB_UNSUPPORTED_RECORD_TYPE
 * CSSM_DL_UNSUPPORTED_OPERATOR
 *
 * NOTE: A NULL input IS considered bad
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_IsInvalidQuery(CSSM_QUERY const * Query)
{
	CSSM_RETURN retValue = CSSM_OK;
	uint32 i=0;
	/* uint32 j=0; */

	if (!Query)
		return CSSMERR_DL_UNSUPPORTED_QUERY;

	if ((retValue = dl_IsQueryOk(Query)) != CSSM_OK)
		return retValue;

	for (i=0; i < Query->NumSelectionPredicates; i++)
	{
		/* Ensure that the requested operator is supported */
		if (!IS_SUPPORTED_DB_OPERATOR(Query->SelectionPredicate[i].DbOperator))
		{
			return (CSSMERR_DL_UNSUPPORTED_OPERATOR);
		}
		if (Query->SelectionPredicate[i].Attribute.NumberOfValues != 1)
		{
			return (CSSMERR_DL_INVALID_QUERY);
		}
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dal_IsBadDbName
 *
 * Description:
 * Determines if a DbName violates any rules set by the DAL. A name can still be
 * invalid even if this function passes, in which case DbCreate will fail.
 *
 * Parameters:
 * FUNCTION_ARG (input/output)
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_IsBadDbName(const char * DbName)
{
	CSSM_RETURN retValue = CSSM_OK;

	ASSERT(DbName);
	if ((retValue = dl_IsDbNameOk(DbName)) != CSSM_OK)
		return retValue;

	if (strcmp(DbName, "") == 0)
		return (CSSMERR_DL_INVALID_DB_NAME);

	if (strlen(DbName) > MAX_DB_NAME_LENGTH)
		return CSSMERR_DL_INVALID_DB_NAME;

	uint32 i;
	for (i = 0; DbName[i]; i++)
	{
		if (DbName[i] == DIRECTORY_SEPARATOR)
			return CSSMERR_DL_INVALID_DB_NAME;
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dal_DataModify
 *
 * Description:
 * Modifies the current data record with the contense stored in AttributesToBeModified,
 * and DataToBeModified; If Null, no changes will be made
 *
 * Parameters:
 * FUNCTION_ARG (input/output)
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DataModify(
	DAL_RECORD_TABLE_REF &refRecordTable,
	const DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST & AttributesToBeModified,
	DAL_CONST_DATA_PTR DataToBeModified)
{
	CSSM_RETURN retValue = CSSM_OK;
	if (DataToBeModified  && ((retValue = refRecordTable->SetData(DataToBeModified)) != CSSM_OK))
		return retValue;

	if (!AttributesToBeModified.neWasSourceNull())
	{
		if ((retValue = refRecordTable->SetSemantics(AttributesToBeModified.GetSemanticInformation())) != CSSM_OK)
			return retValue;

		if ((retValue = refRecordTable->SetAttributes(AttributesToBeModified)) != CSSM_OK)
			return retValue;
	}

	if ((retValue = refRecordTable->UpdateRecord()) != CSSM_OK)
	{
		return retValue;
	}

   return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dal_CreateDbInfoLock
 *
 * Description:
 * Creates a reader/writer lock for the DbInfo of a dl.
 *
 * Parameters:
 * DbName (input) : Name of the database we are protecting
 * pDbInfoLock (output) : Buffer to hold the created lock
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_CreateDbInfoLock(const char *DbName, cssm_SWMRLock * pDbInfoLock)
{
	CSSM_RETURN retValue = CSSM_OK;
	char szLock[MAX_DB_NAME_LENGTH + 70];

	sprintf(szLock, "dalinfo-" MODULE_GUID_STRING "-%s-lock", DbName);

	if ((retValue = cssm_SWMRLockCreate(pDbInfoLock, szLock)) != CSSM_OK)
		return retValue;
	else
		return CSSM_OK;
}

#ifndef MODULE_DIRECTORY_SERVICES /* Turn this feature off for MDS */
DAL_DATABASE_INFO_LIST * dal_GetDatabaseList()
{
	const ADDIN_ATTACH_DATA * pAttachData = Addin_GetAttachData();
	ASSERT(pAttachData != NULL);

	if (pAttachData == NULL)
	{
		return NULL;
	}
	else
	{
		DAL_DATABASE_INFO_LIST * pDatabaseList = pAttachData->pDatabaseList;
		ASSERT(CSSM_FALSE == port_IsBadWritePtr(pDatabaseList, sizeof(DAL_DATABASE_INFO_LIST)));
		return pDatabaseList;
	}
}
#endif

CSSM_BOOL dal_IsMemoryZero(void const *dataparam, uint32 length)
{
	const uint8 * data = (uint8 *)dataparam;
	ASSERT(dataparam);

	uint32 i;
	for (i = 0; i < length; i++)
	{
		if (data[i])
			return CSSM_FALSE;
	}

	return CSSM_TRUE;
}

#ifdef _DEBUG

void IsValidHeapPointer(void *p, uint32 size)
{
	ASSERT(p);
	ASSERT(!port_IsBadWritePtr(p, size));
}

#endif
