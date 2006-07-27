/*-----------------------------------------------------------------------
 * File: DAL_SRC.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file declares the highest level function prototypes used by this DL.
 */

#ifndef DAL_SRC_H
#define DAL_SRC_H
#define CSSMDLI CSSMAPI

#ifdef __cplusplus
extern "C"
{
#endif

/* Exported DL Functions */
CSSM_RETURN CSSMDLI DL_DbOpen (
	CSSM_DL_HANDLE DLHandle,
	const char *DbName,
	const CSSM_NET_ADDRESS *DbLocation,
	CSSM_DB_ACCESS_TYPE AccessRequest,
	const CSSM_ACCESS_CREDENTIALS *AccessCred,
	const void *OpenParameters,
	CSSM_DB_HANDLE *DbHandle);

CSSM_RETURN CSSMDLI DL_DbClose (CSSM_DL_DB_HANDLE DLDBHandle);

CSSM_RETURN CSSMDLI DL_DbCreate (
	CSSM_DL_HANDLE DLHandle,
	const char *DbName,
	const CSSM_NET_ADDRESS *DbLocation,
	const CSSM_DBINFO * DbInfo,
	CSSM_DB_ACCESS_TYPE AccessRequest,
	const CSSM_RESOURCE_CONTROL_CONTEXT *CredAndAclEntry,
	const void *OpenParameters,
	CSSM_DB_HANDLE *DbHandle);

CSSM_RETURN CSSMDLI DL_DbDelete (
	CSSM_DL_HANDLE DLHandle,
	const char *DbName,
	const CSSM_NET_ADDRESS *DbLocation,
	const CSSM_ACCESS_CREDENTIALS *AccessCred);

CSSM_RETURN CSSMDLI DL_GetDbNameFromHandle (CSSM_DL_DB_HANDLE DLDBHandle,
											char ** DbName);

CSSM_RETURN CSSMDLI DL_DataInsert (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_RECORDTYPE RecordType,
	const CSSM_DB_RECORD_ATTRIBUTE_DATA *Attributes,
	const CSSM_DATA *Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueId);

CSSM_RETURN CSSMDLI DL_DataDelete (
	CSSM_DL_DB_HANDLE DLDBHandle,
	const CSSM_DB_UNIQUE_RECORD *UniqueRecordIdentifier);

CSSM_RETURN CSSMDLI DL_DataModify(
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_RECORDTYPE RecordType,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecordIdentifier,
	const CSSM_DB_RECORD_ATTRIBUTE_DATA *AttributesToBeModified,
	const CSSM_DATA *DataToBeModified,
	CSSM_DB_MODIFY_MODE ModifyMode);

CSSM_RETURN CSSMDLI DL_DataGetFirst (
	CSSM_DL_DB_HANDLE DLDBHandle,
	const CSSM_QUERY *Query,
	CSSM_HANDLE_PTR	 ResultsHandle,
	CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
	CSSM_DATA_PTR Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueId);

CSSM_RETURN CSSMDLI DL_DataGetNext (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_HANDLE ResultsHandle,
	CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
	CSSM_DATA_PTR Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueId);

CSSM_RETURN CSSMDLI DL_DataGetFromUniqueRecordId (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord,
	CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
	CSSM_DATA_PTR  Data);

CSSM_RETURN CSSMDLI DL_FreeUniqueRecord (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord);

CSSM_RETURN CSSMDLI DL_DataAbortQuery (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_HANDLE ResultsHandle);

CSSM_RETURN CSSMDLI DL_DbSetRecordParsingFunctions (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_RECORDTYPE RecordType,
	const CSSM_DB_RECORD_PARSING_FNTABLE *FunctionTable);

CSSM_RETURN CSSMDLI DL_DbGetRecordParsingFunctions(
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_RECORDTYPE RecordType,
	CSSM_DB_RECORD_PARSING_FNTABLE_PTR ParsingFunctions);

CSSM_RETURN CSSMDLI DL_GetDbNames(CSSM_DL_HANDLE DLHandle,
										 CSSM_NAME_LIST_PTR *NameList);
CSSM_RETURN CSSMDLI DL_FreeNameList(CSSM_DL_HANDLE DLHandle,
									CSSM_NAME_LIST_PTR NameList);

CSSM_RETURN CSSMDLI DL_Initialize (
	CSSM_MODULE_HANDLE AppHandle,
	uint32 VerMajor,
	uint32 VerMinor);

CSSM_RETURN CSSMDLI DL_Uninitialize (
	CSSM_MODULE_HANDLE AppHandle);

CSSM_RETURN CSSMDLI DL_CreateRelation (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_RECORDTYPE RelationID,
	const char *RelationName,
	uint32 NumberOfAttributes,
	const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
	uint32 NumberOfIndexes,
	const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo);

CSSM_RETURN CSSMAPI DL_DestroyRelation(
						CSSM_DL_DB_HANDLE DLDBHandle,
						CSSM_DB_RECORDTYPE RelationID );
#if USE_DL_PASSTHROUGH

CSSM_RETURN CSSMDLI DL_PassThrough (
	CSSM_DL_DB_HANDLE DLDBHandle,
	uint32 PassThroughId,
	const void * InputParams,
	void **OutputParams);

#endif /* USE_DL_PASSTHROUGH */


#ifdef __cplusplus
}
#endif

#endif
