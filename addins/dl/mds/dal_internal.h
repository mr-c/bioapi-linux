/*-----------------------------------------------------------------------
 * File: DAL_INTERNAL.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file declares the function prototypes for functions used internal to the DL.
 */

#ifndef DAL_INTERNAL_H
#define DAL_INTERNAL_H

#include "cssmtype.h"

#ifdef __cplusplus

#ifdef DAL_SOURCE_FILE

#include "dal_classes.h"
#include "port/bioapi_lock.h"

/* Unique Record ID functions */
CSSM_RETURN dal_GetCurrentRecordInfo(
	CSSM_DL_DB_HANDLE DLDBHandle,
	DAL_RECORD_TABLE_REF &refRecordTable,
	DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST & Attributes,
	CSSM_DATA_PTR Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueRecPtr);

CSSM_RETURN dal_DataModify(
	DAL_RECORD_TABLE_REF &refRecordTable,
	const DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST & AttributesToBeModified,
	DAL_CONST_DATA_PTR DataToBeModified);

DAL_DATABASE_INFO_LIST * dal_GetDatabaseList();

CSSM_RETURN dal_CreateDbInfoLock(const char *DbName, cssm_SWMRLock * pDbInfoLock);

#endif

extern "C"
{

#endif

/* Free & IsBad & IsValid functions */
CSSM_RETURN dal_IsInvalidDbInfo(CSSM_DBINFO const * DbInfo);
CSSM_BOOL dal_IsRecordtypeSupported(CSSM_DB_RECORDTYPE);
CSSM_RETURN dal_IsInvalidQuery(CSSM_QUERY const * Query);

CSSM_RETURN dal_IsBadDbName(const char * DbName);

CSSM_BOOL dal_DoesDbExist(
	const char * DbName);

CSSM_RETURN dal_ReadDbInfo(
	const char * DbName,
	CSSM_DBINFO * pDbInfo,
	uint32 * pAdditionalInformationLength,
	uint8 * pAdditionalInformationData);

CSSM_BOOL dal_IsMemoryZero(const void *, uint32 size);

CSSM_RETURN dal_AddRelationToDbInfo (CSSM_DBINFO_PTR pDbInfo,
				CSSM_DB_RECORDTYPE RelationID,
				const char *RelationName,
				uint32 NumberOfAttributes,
				const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
				uint32 NumberOfIndexes,
				const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo);

CSSM_RETURN dal_RemoveRelationFromDbInfo (CSSM_DBINFO_PTR pDbInfo,
										CSSM_DB_RECORDTYPE RelationID);

#ifdef __cplusplus
}
#endif

#endif
