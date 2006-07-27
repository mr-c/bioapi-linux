/*-----------------------------------------------------------------------
 * File: DAL_SRC.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/*
 * This file contains implementations of the DL_XXX functions
 * prototyped in dlsrc.h
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

#define CSSM_INITGUID
#define DAL_SOURCE_FILE
#define DEFINE_FAILURE_ERROR_CODES
#include "dal_classes.h"
#include "dlutil.h"
#include "dal_internal.h"
#include "dal_src.h"
#include "port/bioapi_port.h"
#include "bioapi_util.h"

/* Added to prevent compilation error if CssmTrace.h is not available */
#ifndef CSSMTRACE0
#define CSSMTRACE0
#endif

#ifdef MODULE_DIRECTORY_SERVICES
#include "maf_util.h"
#else
#include "maf_config.h"
#include "maf_interface.h"
#endif

/*
 * Global Variables
 */

/* input checking static functions */

static CSSM_RETURN dal_CreateOpenArgCheck(
	const CSSM_NET_ADDRESS * DbLocation,
	CSSM_DB_ACCESS_TYPE AccessRequest,
	const CSSM_ACCESS_CREDENTIALS * AccessCred,
	const void *OpenParameters,
	CSSM_DB_HANDLE *DbHandle);

/* RETURN_ERROR and begin macros */
#define DL_API_FUNCTION_BEGIN(Name, DLHandle)

#define RETURN(x)	return(x);
#define RETURN_ERROR(error_code) { \
	if ((error_code) == CSSMERR_DL_FUNCTION_FAILED) { \
		RETURN(FAILURE_ERROR) \
	} else { \
		RETURN(error_code) \
	} \
}

#define WRAPPER_RETURN(error_code)	RETURN(error_code)

#define WRAPPER_RETURN_ERROR(error_code) { \
		RETURN(error_code) \
}

#ifdef _WIN32
/* turn off warning C4127: conditional expression is constant because of the RETURN_ERROR_ERROR macro */
#pragma warning(disable: 4127)
#endif

/* input checking macros */
#define CHECK_DL_HANDLE(hDl) if (hDl == 0) RETURN(CSSMERR_CSSM_INVALID_ADDIN_HANDLE);
#define CHECK_DB_HANDLE(hDb) if (hDb == 0) RETURN(CSSMERR_DL_INVALID_DB_HANDLE);
#define CHECK_DL_DB_HANDLE(hDlDb) \
	CHECK_DL_HANDLE(hDlDb.DLHandle); \
	CHECK_DB_HANDLE(hDlDb.DBHandle);

#define CHECK_OUTPUT_POINTER(p, type) \
	if (!p || port_IsBadWritePtr(p, sizeof(type))) RETURN(CSSMERR_DL_INVALID_OUTPUT_POINTER)

#define CHECK_INPUT_POINTER(p, type) \
	if (!p || port_IsBadReadPtr(p, sizeof(type))) RETURN(CSSMERR_DL_INVALID_INPUT_POINTER)

#define CHECK_RESOURCE_CONTROL_CONTEXT(CredAndAcl) if (CredAndAcl) \
		CHECK_INPUT_POINTER(CredAndAcl, CSSM_RESOURCE_CONTROL_CONTEXT);\

/* Define SET_DB_NAME_DEFAULT, macro to set DbName as the default DbName */
#if USE_DEFAULT_DB_NAME /* There is a default DbName */
	#if CASE_SENSITIVE_DB_NAMES
		#define SET_DB_NAME_DEFAULT(DbName) DbName = DEFAULT_DB_NAME
	#else
		#define SET_DB_NAME_DEFAULT(DbName) strcpy(DbName, DEFAULT_DB_NAME)
	#endif
#else /* Default DbName == FALSE, no default DB Name */
	#define SET_DB_NAME_DEFAULT(DbName) RETURN(CSSMERR_DL_INVALID_DB_NAME)
#endif

/* Define CHECK_DB_NAME, macro to check that DbName is valid */
#if CASE_SENSITIVE_DB_NAMES /* If DbNames are case sensitive */
	#define DB_NAME_PARAMATER DbName
	#define CHECK_DB_NAME() if (DB_NAME_PARAMATER) { \
			CSSM_RETURN retValue;\
			if ((retValue = dal_IsBadDbName(DB_NAME_PARAMATER)) != CSSM_OK) RETURN(retValue) \
			if (!DB_NAME_PARAMATER[0]) SET_DB_NAME_DEFAULT(DbName) \
		} else \
			SET_DB_NAME_DEFAULT(DbName);
#else /* If Db Names are case insensitive */
	#define DB_NAME_PARAMATER MixedCaseDbName
	#define CHECK_DB_NAME() \
		char DbName[MAX_DB_NAME_LENGTH+1]; \
		if (DB_NAME_PARAMATER) { \
			CSSM_RETURN retValue;\
			if ((retValue = dal_IsBadDbName(DB_NAME_PARAMATER)) != CSSM_OK) RETURN(retValue) \
			if (!DB_NAME_PARAMATER[0]) { \
				SET_DB_NAME_DEFAULT(DbName) \
			} else { \
				strcpy(DbName, DB_NAME_PARAMATER); \
				_strlwr(DbName); \
			} \
		} else \
			SET_DB_NAME_DEFAULT(DbName);
#endif

#define CHECK_DB_LOCATION(DbLocation) \
	if (DbLocation) { \
		CHECK_INPUT_POINTER(DbLocation, CSSM_NET_ADDRESS); \
		if (DbLocation->Address.Length != 0) \
			RETURN(CSSMERR_DL_INVALID_DB_LOCATION) \
	}

#define CHECK_RESULTS_HANDLE(ResultsHandle) \
	if (ResultsHandle == 0) { \
		RETURN(CSSMERR_DL_INVALID_RESULTS_HANDLE) \
	}

#define CHECK_INPUT_RECORD_DATA(Data, Attributes) \
	{\
	CSSM_RETURN ret;\
	if (dl_IsBadCssmDataPtr(Data)) RETURN(CSSMERR_DL_INVALID_INPUT_POINTER) \
	if ((ret = dl_IsInputRecordAttributeDataOk(Attributes)) != CSSM_OK) RETURN(ret)\
	}

#define CHECK_OUTPUT_RECORD_DATA(Data, Attributes) \
	if (Data) {	 \
		CHECK_OUTPUT_POINTER(Data, CSSM_DATA); \
		Data->Data = NULL; \
		Data->Length = 0; \
	} \
	if (Attributes) { \
		CSSM_RETURN ret;\
		if ((ret = dl_IsOutputRecordAttributeDataOk(Attributes)) != CSSM_OK) RETURN(ret) \
		unsigned attribute_counter; \
		Attributes->SemanticInformation = 0; \
		for (attribute_counter=0; attribute_counter < Attributes->NumberOfAttributes; attribute_counter++) \
			Attributes->AttributeData[attribute_counter].Value = NULL; \
	}


/*-----------------------------------------------------------------------------
 * Name: DL_DbOpen & dal_DbOpen
 *
 * Description:
 * This function opens a database created with DbCreate. More information about
 * this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED						  Open caused an exception
 * CSSMERR_DL_INVALID_ACCESS_REQUEST			   Unrecognized access type
 * CSSM_DL_INVALID_AUTHENTICATION				Unrecognized or invalid authentication credential
 * CSSM_DL_INVALID_DB_LOCATION					The DB Location is not valid
 * CSSM_DL_INVALID_DB_NAME						There is no database by name DbName
 * CSSMERR_DL_INVALID_PARSING_MODULE			   Connecting to the parsing module failed
 * CSSM_DL_INVALID_OPEN_PARAMETERS				The open parameters are not valid
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSM_DL_INVALID_RECORD_SIGNING_INFO			A connection to the CSP could not be made
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 * CSSMERR_DL_INTERNAL_ERROR					   Error occurred while reading from registry, or the value RETURN_ERRORed is invalid
 *---------------------------------------------------------------------------*/

CSSM_RETURN dal_DbOpen (
	CSSM_DL_HANDLE DLHandle,
	const char *DB_NAME_PARAMATER,
	const CSSM_NET_ADDRESS *DbLocation,
	CSSM_DB_ACCESS_TYPE AccessRequest,
	const CSSM_ACCESS_CREDENTIALS *AccessCred,
	const void *OpenParameters,
	CSSM_DB_HANDLE *DbHandle)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;
	DAL_DATABASE_INFO *NewDatabase = NULL;
	DL_API_FUNCTION_BEGIN(DL_DbOpen, DLHandle);
	ASSERT_DAL_VALID();

	CHECK_DL_HANDLE(DLHandle);
	CHECK_DB_NAME();
	if ((retValue = dal_CreateOpenArgCheck(DbLocation, AccessRequest, AccessCred, OpenParameters, DbHandle)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		RETURN(FAILURE_ERROR);
	}

	/* it's a go! */
	if ((retValue = pDatabaseList->AllocateNewDatabase(&NewDatabase)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if (AccessRequest & CSSM_DB_ACCESS_PRIVILEGED)
	{
		/*
		 * Lock down this DbName so that we can not have concurent calls to DbOpen
		 */
		if ((retValue = NewDatabase->SetDbCreateLock(DbName)) != CSSM_OK)
		{
			pDatabaseList->nrDeleteDatabase(NewDatabase);
			RETURN_ERROR(retValue)
		}
	} else {
		/*
		 * Lock down this DbName so that we can have concurent calls to DbOpen
		 */
		if ((retValue = NewDatabase->SetDbOpenLock(DbName)) != CSSM_OK)
		{
			pDatabaseList->nrDeleteDatabase(NewDatabase);
			RETURN_ERROR(retValue)
		}
	}


#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS | MODULE_PARAMETERS_USE_DB_INFO | MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	/* Declare Module Parameters variable */
	DAL_MODULE_PARAMETERS Parameters;
	DAL_MODULE_PARAMETERS_PTR pParameters = &Parameters;
#endif

#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	/* Declare AdditionalInformation */
	uint8 AdditionalInformationBlock[ADDITIONAL_INFORMATION_MAX_LENGTH];
	Parameters.AdditionalInformation.Data = AdditionalInformationBlock;
	Parameters.AdditionalInformation.Length = ADDITIONAL_INFORMATION_MAX_LENGTH;
#endif

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	Parameters.DbLocation = DbLocation;
	Parameters.AccessRequest = AccessRequest;
	Parameters.AccessCred = AccessCred;
	Parameters.OptionalParameters = OpenParameters;
#endif

	/* Get the Module Info, do not free it on success */
	CSSM_DBINFO DbInfo;
	if ((retValue = dal_ReadDbInfo(DbName, &DbInfo, ADDITIONAL_INFORMATION)) != CSSM_OK)
	{
		pDatabaseList->nrDeleteDatabase(NewDatabase);
		RETURN_ERROR(retValue)
	}

	if ((retValue = NewDatabase->Initialize(DLHandle, DbName,
#if DAL_USE_MODULE_PARAMATERS
		pParameters,
#endif
		&DbInfo)) != CSSM_OK)
	{
		dlnr_FreeDbInfo(&DbInfo);
		pDatabaseList->nrDeleteDatabase(NewDatabase);

		if (retValue == CSSMERR_DL_INVALID_RECORD_INDEX || retValue == CSSMERR_DL_INVALID_RECORDTYPE)
			RETURN(CSSMERR_DL_INTERNAL_ERROR)

		RETURN_ERROR(retValue);
	}

	if ((retValue = NewDatabase->Open(
#if DAL_USE_MODULE_PARAMATERS
		pParameters
#endif
		)) != CSSM_OK)
	{
		dlnr_FreeDbInfo(&DbInfo);
		pDatabaseList->nrDeleteDatabase(NewDatabase);
		RETURN_ERROR(retValue)
	}

	dlnr_FreeDbInfo(&DbInfo);

	if ((retValue = pDatabaseList->AddDatabase(NewDatabase, &DbHandle)) != CSSM_OK)
	{
		pDatabaseList->nrDeleteDatabase(NewDatabase);
		RETURN_ERROR(retValue)
	}

	RETURN(retValue)
}

/* wrapper around dal_DbOpen */
CSSM_RETURN CSSMDLI DL_DbOpen (
	CSSM_DL_HANDLE DLHandle,
	const char *DbName,
	const CSSM_NET_ADDRESS *DbLocation,
	CSSM_DB_ACCESS_TYPE AccessRequest,
	const CSSM_ACCESS_CREDENTIALS *AccessCred,
	const void *OpenParameters,
	CSSM_DB_HANDLE *DbHandle)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL)) {
		WRAPPER_RETURN(FAILURE_ERROR)
	}

	/* Clear the output handle. */
	if (DbHandle)
		*DbHandle = 0;

	retValue = dal_DbOpen(DLHandle, DbName, DbLocation, AccessRequest, AccessCred,
						  OpenParameters, DbHandle);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN (retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_DbClose & DL_DbClose
 *
 * Description:
 * This function closes an opened database. More information about this function
 * can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED					Close caused an exception
 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DbClose (CSSM_DL_DB_HANDLE DLDBHandle)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_DbClose, DLDBHandle.DLHandle);
	ASSERT_DAL_VALID();

	/* Check inputs */
	CHECK_DL_DB_HANDLE(DLDBHandle);

	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		RETURN(FAILURE_ERROR);
	}

	if ((retValue = pDatabaseList->RemoveDatabase(DLDBHandle.DBHandle)) != CSSM_OK)
		RETURN_ERROR(retValue)

	RETURN(retValue)
}  /* End of DL_DbClose */

/* wrapper around dal_DbClose */
CSSM_RETURN CSSMDLI DL_DbClose (CSSM_DL_DB_HANDLE DLDBHandle)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DbClose (DLDBHandle);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_DbCreate & DL_DbCreate
 *
 * Description:
 * This function creates a new database if it does not already exist. More
 * information about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSM_DL_DATASTORE_ALREADY_EXISTS				A database by name DbName already exists
 * CSSMERR_DL_FUNCTION_FAILED					Create caused an exception
 * CSSMERR_DL_FIELD_SPECIFIED_MULTIPLE			A record attribute or index was specified multiple times with differing information
 * CSSM_DL_INCOMPATIBLE_FIELD_FORMAT			The field format for an index from an attribute is different then the field format of that attribute

 * CSSMERR_DL_INVALID_ACCESS_REQUEST			Invalid requested access type
 * CSSM_DL_INVALID_AUTHENTICATION				Invalid authentication credential
 * CSSM_DL_INVALID_DB_INFO						Error in first level of DbInfo
 * CSSM_DL_INVALID_DB_LOCATION					The DB location is not valid
 * CSSM_DL_INVALID_DB_NAME						Invalid Database Name
 * CSSM_DL_INVALID_FIELD_NAME					Attribute or index name is an illegal name
 * CSSM_DL_INVALID_OPEN_PARAMETERS				The open parameters are not valid
 * CSSMERR_DL_INVALID_PARSING_MODULE			A parsing module in the DB Info is invalid
 * CSSMERR_DL_INVALID_POINTER					Invalid pointer passed to function
 * CSSM_DL_INVALID_RECORD_INDEX					A record index in the DbInfo structure is invalid
 * CSSM_DL_INVALID_RECORDTYPE					Recordtype specified DbInfo's arrays do not match
 * CSSM_DL_INVALID_RECORD_SIGNING_INFO			The record signing info in the DbInfo is invalid
 * CSSM_DL_UNSUPPORTED_AUTHENTICATION_MECHANISM Unsupported authentication mechanism
 * CSSM_DL_UNSUPPORTED_FIELD_FORMAT				A field's format (data type) is not supported
 * CSSM_DL_UNSUPPORTED_INDEX_INFO				Requested IndexInfo struct is not supported
 * CSSM_DL_UNSUPPORTED_LOCALITY					The value of DbInfo->IsLocal is not supported
 * CSSM_DL_UNSUPPORTED_NUM_ATTRIBUTES			Unsupported number of attributes specified
 * CSSM_DL_UNSUPPORTED_NUM_INDEXES				Unsupported number of indexes
 * CSSM_DL_UNSUPPORTED_NUM_RECORDTYPES			Unsupported number of record types
 * CSSM_DL_UNSUPPORTED_RECORD_SIGNING			Record signing request is not supported
 * CSSM_DL_UNSUPPORTED_RECORDTYPE				Requested record signing is not supported
 * CSSMERR_DL_INTERNAL_ERROR					   Error occurred while interacting with the registry
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DbCreate (
	CSSM_DL_HANDLE DLHandle,
	const char *DB_NAME_PARAMATER,
	const CSSM_NET_ADDRESS *DbLocation,
	const CSSM_DBINFO *DbInfo,
	CSSM_DB_ACCESS_TYPE AccessRequest,
	const CSSM_RESOURCE_CONTROL_CONTEXT *CredAndAclEntry,
	const void *OpenParameters,
	CSSM_DB_HANDLE *DbHandle)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;
	DAL_DATABASE_INFO_PTR NewDatabase = NULL;
	CSSM_ACCESS_CREDENTIALS *pCred = NULL;

	DL_API_FUNCTION_BEGIN(DL_DbCreate, DLHandle);
	ASSERT_DAL_VALID();

	CHECK_DL_HANDLE(DLHandle);
	CHECK_DB_NAME();

	if ((retValue = dal_IsInvalidDbInfo(DbInfo)) != CSSM_OK)
		RETURN_ERROR(retValue)

	CHECK_RESOURCE_CONTROL_CONTEXT(CredAndAclEntry);

	if (CredAndAclEntry)
		pCred = CredAndAclEntry->AccessCred;

	/* We cannot create a Db with READ only access request */
	if (AccessRequest == CSSM_DB_ACCESS_READ)
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_REQUEST);
	}

	if ((retValue = dal_CreateOpenArgCheck(DbLocation, AccessRequest, pCred, OpenParameters, DbHandle)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* it's a go! */
	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		RETURN(FAILURE_ERROR);
	}

	if ((retValue = pDatabaseList->AllocateNewDatabase(&NewDatabase)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Lock down this DbName so that we can not have concurent calls to DbCreate */
	if ((retValue = NewDatabase->SetDbCreateLock(DbName)) != CSSM_OK)
	{
		pDatabaseList->nrDeleteDatabase(NewDatabase);
		RETURN_ERROR(retValue)
	}

	/* Make sure database does not exist */
	if (dal_DoesDbExist(DbName) != CSSM_FALSE)
	{
		pDatabaseList->nrDeleteDatabase(NewDatabase);
		RETURN(CSSMERR_DL_DATASTORE_ALREADY_EXISTS)
	}


#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS | MODULE_PARAMETERS_USE_DB_INFO | MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	/* Declare Module Parameters variable */
	DAL_MODULE_PARAMETERS Parameters;
	DAL_MODULE_PARAMETERS_PTR pParameters = &Parameters;
#endif

#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	/* Declare AdditionalInformation */
	uint8 AdditionalInformationBlock[ADDITIONAL_INFORMATION_MAX_LENGTH];
	Parameters.AdditionalInformation.Data = AdditionalInformationBlock;
	Parameters.AdditionalInformation.Length = ADDITIONAL_INFORMATION_MAX_LENGTH;
#endif

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	Parameters.DbLocation = DbLocation;
	Parameters.AccessRequest = AccessRequest;
	if (CredAndAclEntry)
		Parameters.AccessCred = CredAndAclEntry->AccessCred;
	else
		Parameters.AccessCred = NULL;
	Parameters.AccessPath = DbInfo->AccessPath;
	Parameters.OptionalParameters = OpenParameters;
#endif

	if ((retValue = NewDatabase->Initialize(DLHandle, DbName,
#if DAL_USE_MODULE_PARAMATERS
		pParameters,
#endif
		DbInfo)) != CSSM_OK)
	{
		pDatabaseList->nrDeleteDatabase(NewDatabase);
		RETURN_ERROR(retValue)
	}

	if ((retValue = NewDatabase->Create(
#if DAL_USE_MODULE_PARAMATERS
		pParameters
#endif
		)) != CSSM_OK)
	{
		pDatabaseList->nrDeleteDatabase(NewDatabase);
		RETURN_ERROR(retValue)
	}


#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	if ((retValue = dl_schemaFileAddDb(MODULE_GUID_STRING,
									   DbName,
									   DbInfo,
									   &Parameters.AdditionalInformation))
									   != CSSM_OK)
#else
	if ((retValue = dl_schemaFileAddDb(MODULE_GUID_STRING,
									   DbName,
									   DbInfo,
									   NULL)) != CSSM_OK)
#endif
	{
		pDatabaseList->nrDeleteDatabase(NewDatabase);
		RETURN_ERROR(retValue)
	}

	*DbHandle = 0;

	if ((retValue = pDatabaseList->AddDatabase(NewDatabase, &DbHandle)) != CSSM_OK)
	{
		dl_schemaFileDeleteDb(MODULE_GUID_STRING, DbName);
		dl_dlregDeleteDb(MODULE_GUID_STRING, DbName);
		pDatabaseList->nrDeleteDatabase(NewDatabase);
		RETURN_ERROR(retValue)
	}

	/* DbCreate is done, so we can go to a reader lock if not PRIVILEGED mode*/
	if (!(AccessRequest & CSSM_DB_ACCESS_PRIVILEGED))
		NewDatabase->nrDbCreateToDbOpenLock();

	RETURN(retValue)
}

CSSM_RETURN CSSMDLI DL_DbCreate (
	CSSM_DL_HANDLE DLHandle,
	const char * DbName,
	const CSSM_NET_ADDRESS *DbLocation,
	const CSSM_DBINFO *DbInfo,
	CSSM_DB_ACCESS_TYPE AccessRequest,
	const CSSM_RESOURCE_CONTROL_CONTEXT *CredAndAclEntry,
	const void *OpenParameters,
	CSSM_DB_HANDLE *DbHandle)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DbCreate(DLHandle, DbName, DbLocation, DbInfo, AccessRequest,
							CredAndAclEntry, OpenParameters, DbHandle);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}


/*-----------------------------------------------------------------------------
 * Name: dal_DbDelete & DL_DbDelete
 *
 * Description:
 * This function deletes a database that was created with DbCreate. The database must
 * not be open by any process. More information about this function can be found
 * in the Spec.
 *
 * Error Codes:
 * CSSM_DL_DATASTORE_IS_OPEN					The database by name DbName is currently open
 * CSSMERR_DL_FUNCTION_FAILED						Delete caused an exception

 * CSSM_DL_INVALID_AUTHENTICATION				Unrecognized or invalid authentication credential
 * CSSM_DL_INVALID_DB_LOCATION					The DB location is not valid
 * CSSM_DL_INVALID_DB_NAME						There is no database by name DbName
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSMERR_DL_INTERNAL_ERROR					   Error occurred while interacting with the registry
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DbDelete (
	CSSM_DL_HANDLE DLHandle,
	const char * DB_NAME_PARAMATER,
	const CSSM_NET_ADDRESS * DbLocation,
	const CSSM_ACCESS_CREDENTIALS * AccessCred)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_DbDelete, DLHandle);
	ASSERT_DAL_VALID();

	/* Check inputs */
	CHECK_DL_HANDLE(DLHandle);
	CHECK_DB_NAME();
	CHECK_DB_LOCATION(DbLocation);

	/* Check for Null AccessCred as we don't support ACLs */
	if (AccessCred)
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_CREDENTIALS)
	}

	/* Lock the presistant database information for the Db */
	cssm_SWMRLock DbInfoLock;
	if ((retValue = dal_CreateDbInfoLock(DbName, &DbInfoLock)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Wait for .1 seconds in case we are just finishing up a DbClose */
	retValue = cssm_SWMRLockWaitToWrite(&DbInfoLock, 100);

	/* Make sure that the database is not currently open */
	if (CSSMERR_CSSM_INTERNAL_ERROR == retValue)
	{
		/* Remove the Locks */
		cssm_SWMRLockDelete(&DbInfoLock);
		RETURN(CSSMERR_DL_DATASTORE_IS_OPEN)
	}
	if (CSSM_OK != retValue)
	{
		/* Remove the Locks */
		cssm_SWMRLockDelete(&DbInfoLock);
		RETURN(CSSMERR_DL_FUNCTION_FAILED)
	}

	/* Verify that the data store exists */
	if (dl_dlregDoesDbExist(MODULE_GUID_STRING, DbName) == CSSM_FALSE)
	{
        cssm_SWMRLockDoneWriting(&DbInfoLock);
		cssm_SWMRLockDelete(&DbInfoLock);
		RETURN(CSSMERR_DL_DATASTORE_DOESNOT_EXIST)
	}

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS | MODULE_PARAMETERS_USE_DB_INFO | MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	/* Declare Module Parameters variable */
	DAL_MODULE_PARAMETERS Parameters;
	DAL_MODULE_PARAMETERS_PTR pParameters = &Parameters;
#endif

#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	/* Declare AdditionalInformation */
	uint8 AdditionalInformationBlock[ADDITIONAL_INFORMATION_MAX_LENGTH];
	Parameters.AdditionalInformation.Data = AdditionalInformationBlock;
	Parameters.AdditionalInformation.Length = ADDITIONAL_INFORMATION_MAX_LENGTH;
#endif

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	Parameters.DbLocation = DbLocation;
    Parameters.AccessRequest = 0;
	Parameters.AccessCred = AccessCred;
	Parameters.OptionalParameters = NULL;
#endif

	/* Declare pDbInfo if we need to get either the DbInfo or the Additional Paramaters */
#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION && !MODULE_PARAMETERS_USE_DB_INFO
	const CSSM_DBINFO_PTR pDbInfo = NULL;
#endif

#if MODULE_PARAMETERS_USE_DB_INFO
	const CSSM_DBINFO_PTR pDbInfo = &Parameters.DbInfo;
#endif

#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION || MODULE_PARAMETERS_USE_DB_INFO
	if ((retValue = dal_ReadDbInfo(DbName, pDbInfo, ADDITIONAL_INFORMATION)) != CSSM_OK)
	{
		/* Remove the Locks */
        cssm_SWMRLockDoneWriting(&DbInfoLock);
		cssm_SWMRLockDelete(&DbInfoLock);
		RETURN_ERROR(retValue)
	}
#endif

	/* Remove the Registry Entry */
	if ((retValue = dl_dlregDeleteDb(MODULE_GUID_STRING, DbName)) != CSSM_OK)
	{
		/* Remove the Locks */
        cssm_SWMRLockDoneWriting(&DbInfoLock);
		cssm_SWMRLockDelete(&DbInfoLock);
		RETURN(CSSMERR_DL_INTERNAL_ERROR)
	}

	/* Remove the data store from the system */
	if ((retValue = dlbe_DeleteDatabase(DbName MODULE_PARAMETERS)) != CSSM_OK)
	{
		/* Remove the Locks */
        cssm_SWMRLockDoneWriting(&DbInfoLock);
		cssm_SWMRLockDelete(&DbInfoLock);

#if MODULE_PARAMETERS_USE_DB_INFO
		/* Free the DbInfo */
		dlnr_FreeDbInfo(&Parameters.DbInfo);
#endif
		RETURN_ERROR(retValue)
	}

#if MODULE_PARAMETERS_USE_DB_INFO
	/* Free the DbInfo */
	dlnr_FreeDbInfo(&Parameters.DbInfo);
#endif

	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		RETURN(FAILURE_ERROR);
	}

	/* Remove the Locks */
    cssm_SWMRLockDoneWriting(&DbInfoLock);
	cssm_SWMRLockDelete(&DbInfoLock);

	RETURN(retValue)
}

/* wrapper around dal_DbDelete */
CSSM_RETURN CSSMDLI DL_DbDelete (
	CSSM_DL_HANDLE DLHandle,
	const char * DbName,
	const CSSM_NET_ADDRESS *DbLocation,
	const CSSM_ACCESS_CREDENTIALS *AccessCred)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DbDelete (DLHandle, DbName, DbLocation, AccessCred);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_GetDbName & DL_GetDbNames
 *
 * Description:
 * This function will return the list of database names. A NULL return indicates
 * an error occured, if no DbNames were found, then the return will not be NULL.
 * More information about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED					 Get DB Names failed
 * CSSMERR_DL_INTERNAL_ERROR					   Error occurred while reading from registry
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_GetDbNames(CSSM_DL_HANDLE DLHandle, CSSM_NAME_LIST_PTR *NameList)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_GetDbNames, DLHandle);

	ASSERT_DAL_VALID();

	/* Check inputs */
	CHECK_DL_HANDLE(DLHandle);
	CHECK_OUTPUT_POINTER(NameList, CSSM_NAME_LIST_PTR);

	/* Get the data store names in this module */
	CSSM_NAME_LIST DynamicNameList;

	/* Read the Db's from the DL registry */
	if ((retValue = dl_dlregReadDbNames(MODULE_GUID_STRING, &DynamicNameList)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Allocate memory for NameList */
	/* Allocate memory for the new pNewNameList */
	CSSM_NAME_LIST_PTR pNewNameList;
	pNewNameList = (CSSM_NAME_LIST_PTR) BioAPI_calloc(sizeof(CSSM_NAME_LIST), 1, NULL);

	if (!pNewNameList)
	{
		RETURN(CSSMERR_DL_MEMORY_ERROR)
	}

	/* just copy the pointer value from DynamicNameList */
	pNewNameList->NumStrings = DynamicNameList.NumStrings;
	pNewNameList->String = DynamicNameList.String;

	*NameList = pNewNameList;

	RETURN(retValue)
}

/* wrapper around dal_GetDbNames */
CSSM_RETURN CSSMDLI DL_GetDbNames(CSSM_DL_HANDLE DLHandle,
								  CSSM_NAME_LIST_PTR *NameList)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_GetDbNames(DLHandle, NameList);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_GetDbNameFromHandle & DL_GetDbNameFromHandle
 *
 * Description:
 * This function returns the name of the database corrisponding to a DB Handle.
 * More information about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED		  Exception thrown during function
 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_GetDbNameFromHandle (CSSM_DL_DB_HANDLE DLDBHandle,
									 char ** DbName)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_GetDbNameFromHandle, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/* Initializations */
	CSSM_DL_HANDLE DLHandle = DLDBHandle.DLHandle;

	/* Check inputs */
	CHECK_DL_DB_HANDLE(DLDBHandle);
	CHECK_OUTPUT_POINTER(DbName, char**);

	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
		RETURN(FAILURE_ERROR)

	/* Get a database object, this will lock the mutex, which needs to be unlocked before we exit */
    PORT_MUTEX_HANDLE hMutex;
	DAL_DATABASE_INFO_PTR pDatabaseInfo;
	if ((retValue = pDatabaseList->FindDatabaseByHandle(DLDBHandle.DBHandle, pDatabaseInfo, hMutex)) != CSSM_OK)
		RETURN_ERROR(retValue)

	ASSERT(pDatabaseInfo != NULL);

	char const * const retName = pDatabaseInfo->neGetDbName();
	if (!retName)
	{
		port_UnlockMutex(hMutex);
		RETURN(CSSMERR_DL_INTERNAL_ERROR)
	}

	*DbName = (char*)App_Calloc(DLHandle, strlen(retName)+1, 1);
	if(NULL == *DbName)
	{
		port_UnlockMutex(hMutex);
		RETURN(CSSMERR_DL_MEMORY_ERROR)
	}

	strcpy(*DbName, retName);
	port_UnlockMutex(hMutex);

	RETURN(retValue)
}

/* wrapper around dal_GetDbNameFromHandle */
CSSM_RETURN CSSMDLI DL_GetDbNameFromHandle (CSSM_DL_DB_HANDLE DLDBHandle,
											char ** DbName )
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_GetDbNameFromHandle (DLDBHandle, DbName);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_FreeNameList & DL_FreeNameList
 *
 * Description:
 * This function frees the name list RETURN_ERRORed by DL_GetDbNames. More information
 * about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED				   Exception thrown during FreeNameList
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_FreeNameList(CSSM_DL_HANDLE DLHandle, CSSM_NAME_LIST_PTR NameList)
{
	DL_API_FUNCTION_BEGIN(DL_FreeNameList, DLHandle);

	ASSERT_DAL_VALID();

	/* Check inputs */
	CHECK_DL_HANDLE(DLHandle);

	if (!NameList || IsBadReadPtr(NameList, sizeof(CSSM_NAME_LIST)))
		RETURN(CSSMERR_DL_INVALID_POINTER)

	uint32 IsOkay = 1;

	if (NameList->NumStrings)
	{
		IsOkay = NameList->String && !IsBadReadPtr(NameList->String, sizeof(char *) * NameList->NumStrings);

		if (IsOkay)
		{
			uint32 i;

			for (i = 0; i < NameList->NumStrings; i++)
			{
				if (!NameList->String[i] || IsBadReadPtr(NameList->String[i], 1))
					IsOkay = 0;
				else
					BioAPI_free(NameList->String[i], NULL);
			}

			BioAPI_free(NameList->String, NULL);
		}
	}

	BioAPI_free(NameList, NULL);

	if (IsOkay)
		RETURN(CSSM_OK)
	else
		RETURN(CSSMERR_DL_INVALID_POINTER)
}

/* wrapper around dal_FreeNameList */
CSSM_RETURN CSSMDLI DL_FreeNameList(CSSM_DL_HANDLE DLHandle, CSSM_NAME_LIST_PTR NameList)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_FreeNameList(DLHandle, NameList);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_DataInsert & DL_DataInsert
 *
 * Description:
 * This function inserts a new record into a table of the database, or it modifies
 * an existing record if a unique is already associated with a data record.
 * More information about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED					  Insert caused an exception
 * CSSMERR_DL_EXISTING_RECORD_UPDATED			   The value of the new record's primary key was found in another record, that record was updated successfully.
 * CSSMERR_DL_FIELD_SPECIFIED_MULTIPLE			   A record attribute was specified multiple times
 * CSSM_DL_INCOMPATIBLE_FIELD_FORMAT			The specified field format (data type) is not compatible with the schema or the attribute's value

 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSM_DL_INVALID_FIELD_NAME					Attribute or index name was not found in the schema
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSM_DL_INVALID_PARSING_FUNCTIONS			Parsing function table needs to be set for RecordType before inserting a record
 * CSSM_DL_INVALID_RECORDTYPE					RecordType not supported by database, or conflicts with Attributes->DataRecordType
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 * CSSM_DL_MISSING_VALUE						Missing needed attribute or data value
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DataInsert (
	CSSM_DL_DB_HANDLE DLDBHandle,
	const CSSM_DB_RECORDTYPE RecordType,
	const CSSM_DB_RECORD_ATTRIBUTE_DATA * Attributes,
	const CSSM_DATA * Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueId)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	DAL_DB_OPEN_PARAM openParam;
	DAL_MODULE_PARAMETERS Parameters;

	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_DataInsert, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/* Check Input */
	CHECK_DL_DB_HANDLE(DLDBHandle);
	CHECK_INPUT_RECORD_DATA(Data, Attributes);

	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		RETURN(FAILURE_ERROR);
	}

	/* Get the database name and parameters */
	if ((retValue = pDatabaseList->GetDBNamesAndParameters(DLDBHandle, &openParam)) != CSSM_OK)
	{
		RETURN_ERROR(retValue)
	}
	memcpy (&Parameters, openParam.dbParameters, sizeof (DAL_MODULE_PARAMETERS));

	/* Make sure that database was opened in READ | WRITE mode */
	if (!((Parameters.AccessRequest & CSSM_DB_ACCESS_READ)
		 && (Parameters.AccessRequest & CSSM_DB_ACCESS_WRITE)))
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_REQUEST)
	}

	/* Get the appropriate recordset */
	DAL_RECORD_TABLE_REF refRecordTable;
	if ((retValue = refRecordTable.Initialize(DLDBHandle.DBHandle, RecordType)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if (Attributes && Attributes->DataRecordType != RecordType)
		RETURN(CSSMERR_DL_INVALID_RECORDTYPE)

	/* Translate Indexes */
	DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST TranslatedAttributes;
	/* we need to type cast because this must work with input and output attributes */
	if ((retValue = refRecordTable->TranslateAttributes(
			(CSSM_DB_RECORD_ATTRIBUTE_DATA *)Attributes, TranslatedAttributes)) != CSSM_OK)
		RETURN_ERROR(retValue)

#if DAL_ENSURE_UNIQUENESS
	/* See if the record already exists */
	CSSM_BOOL NoRecordsFound;
	if ((retValue = refRecordTable->FindRecordWithUniqueIndexes(TranslatedAttributes, Data,
																NoRecordsFound)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if (!NoRecordsFound)
	{
		RETURN(CSSMERR_DL_INVALID_UNIQUE_INDEX_DATA)
	}
#endif

	/* Insert the data item into the database */
	if ((retValue = refRecordTable->NewRecord()) != CSSM_OK)
		RETURN_ERROR(retValue)

	if ((retValue = refRecordTable->SetData(Data)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if (Attributes)
	{
		if ((retValue = refRecordTable->SetSemantics(Attributes->SemanticInformation)) != CSSM_OK)
			RETURN_ERROR(retValue)

		if ((retValue = refRecordTable->SetAttributes(TranslatedAttributes)) != CSSM_OK)
			RETURN_ERROR(retValue)
	}

	if ((retValue = dlbe_CreateUniqueRecordId(DLDBHandle.DLHandle, UniqueId)) 
									!= CSSM_OK)
			RETURN_ERROR(retValue)

	if ((retValue = refRecordTable->UpdateRecord()) != CSSM_OK)
	{
		dlbe_FreeUniqueRecordId(DLDBHandle, *UniqueId);
		RETURN_ERROR(retValue)
	}

	if ((retValue = refRecordTable->GetUniqueRecordId(*UniqueId)) != CSSM_OK)
	{
		dlbe_FreeUniqueRecordId(DLDBHandle, *UniqueId);
		RETURN(CSSMERR_DL_FUNCTION_FAILED)
	}

	RETURN(retValue)
}

/* wrapper around dal_DataInsert */
CSSM_RETURN CSSMDLI DL_DataInsert (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_RECORDTYPE RecordType,
	const CSSM_DB_RECORD_ATTRIBUTE_DATA *Attributes,
	const CSSM_DATA *Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueId)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DataInsert(DLDBHandle, RecordType, Attributes, Data, UniqueId);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_DataDelete & DL_DataDelete
 *
 * Description:
 * This function deletes a record  in the table of the database. More information
 * about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED					  Delete caused an exception
 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSMERR_DL_INVALID_RECORD_UID				   The data inside the unique record identifier is not valid
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DataDelete (
	CSSM_DL_DB_HANDLE DLDBHandle,
	const CSSM_DB_UNIQUE_RECORD *UniqueRecordIdentifier)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	DAL_DB_OPEN_PARAM openParam;
	DAL_MODULE_PARAMETERS Parameters;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_DataDelete, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/* Check Input */
	CHECK_DL_DB_HANDLE(DLDBHandle);

	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		RETURN(FAILURE_ERROR);
	}

	/* Get the database name and parameters */
	if ((retValue = pDatabaseList->GetDBNamesAndParameters(DLDBHandle, &openParam)) != CSSM_OK)
	{
		RETURN_ERROR(retValue)
	}
	memcpy (&Parameters, openParam.dbParameters, sizeof (DAL_MODULE_PARAMETERS));

	/* Make sure that database was opened in READ | WRITE mode */
	if (!((Parameters.AccessRequest & CSSM_DB_ACCESS_READ)
		 && (Parameters.AccessRequest & CSSM_DB_ACCESS_WRITE)))
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_REQUEST)
	}

	/* Get the appropriate recordset */
	CSSM_DB_RECORDTYPE RecordType;
	DAL_RECORD_TABLE_REF refRecordTable;
	if ((retValue = refRecordTable.Initialize(DLDBHandle.DBHandle, UniqueRecordIdentifier, &RecordType)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Remove the data item into the database */
	if ((retValue = refRecordTable->MoveToRecord(UniqueRecordIdentifier)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if ((retValue = refRecordTable->DeleteRecord()) != CSSM_OK)
		RETURN_ERROR(retValue)

	RETURN(retValue)
}

/* wrapper around dal_DataDelete */
CSSM_RETURN CSSMDLI DL_DataDelete (
	CSSM_DL_DB_HANDLE DLDBHandle,
	const CSSM_DB_UNIQUE_RECORD *UniqueRecordIdentifier)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DataDelete(DLDBHandle, UniqueRecordIdentifier);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_DataModify & DL_DataModify
 *
 * Description:
 * This function modifies a record in a table of a database. More information
 * about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED					  Data Modify caused an exception to be thrown
 * CSSMERR_DL_FIELD_SPECIFIED_MULTIPLE			   A record attribute was specified multiple times
 * CSSM_DL_INCOMPATIBLE_FIELD_FORMAT			The specified field format (data type) is not compatible with the schema or the attribute's value

 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSM_DL_INVALID_FIELD_NAME					Attribute name was not found
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSM_DL_INVALID_RECORD_SIGNING_INFO			The record signature did not verify
 * CSSMERR_DL_INVALID_RECORD_UID				   The unique record identifier is not valid
 * CSSM_DL_INVALID_RECORDTYPE					RecordType not supported by database, or conflicts with AttributesToBeModified or UniqueRecordId
 * CSSM_DL_INVALID_UNIQUE_INDEX_DATA			The modification would have caused the new primary key to have a value that is already in use
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DataModify(
	CSSM_DL_DB_HANDLE DLDBHandle,
	const CSSM_DB_RECORDTYPE RecordType,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecordIdentifier,
	const CSSM_DB_RECORD_ATTRIBUTE_DATA * AttributesToBeModified,
	const CSSM_DATA * DataToBeModified,
	const CSSM_DB_MODIFY_MODE ModifyMode)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	DAL_DB_OPEN_PARAM openParam;
	DAL_MODULE_PARAMETERS Parameters;
	CSSM_RETURN retValue = CSSM_OK;

	/* Check for proper ModifyMode */
	if (ModifyMode != CSSM_DB_MODIFY_ATTRIBUTE_NONE)
	{
		return (CSSMERR_DL_MULTIPLE_VALUES_UNSUPPORTED);
	}

	DL_API_FUNCTION_BEGIN(DL_DataModify, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/* Check Input */
	CHECK_DL_DB_HANDLE(DLDBHandle);
	CHECK_INPUT_RECORD_DATA(DataToBeModified, AttributesToBeModified);

	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		RETURN(FAILURE_ERROR);
	}

	/* Get the database name and parameters */
	if ((retValue = pDatabaseList->GetDBNamesAndParameters(DLDBHandle, &openParam)) != CSSM_OK)
	{
		RETURN_ERROR(retValue)
	}
	memcpy (&Parameters, openParam.dbParameters, sizeof (DAL_MODULE_PARAMETERS));

	/* Make sure that database was opened in READ | WRITE mode */
	if (!((Parameters.AccessRequest & CSSM_DB_ACCESS_READ)
		 && (Parameters.AccessRequest & CSSM_DB_ACCESS_WRITE)))
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_REQUEST)
	}

	/* Get the appropriate recordset using information from UniqueRecordId */
	CSSM_DB_RECORDTYPE uridRecordType;
	DAL_RECORD_TABLE_REF refRecordTable;
	if ((retValue = refRecordTable.Initialize(DLDBHandle.DBHandle, UniqueRecordIdentifier, &uridRecordType)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Ensure that the recordtype embeded in AttributesToBeModified matches the recordtype
	   embeded in UniqueRecordId */
	if (AttributesToBeModified && AttributesToBeModified->DataRecordType != RecordType)
		RETURN(CSSMERR_DL_INVALID_RECORDTYPE)

	if (RecordType != uridRecordType)
		RETURN(CSSMERR_DL_INVALID_RECORD_UID)

	if ((!AttributesToBeModified || !AttributesToBeModified->NumberOfAttributes) && !DataToBeModified)
		/* If there is nothing to modify, just RETURN_ERROR okay */
		RETURN(retValue)

	/* Translate the attributes from (AttributeName, AttributeValue) pairs to (AttributeNumber,
	   AttributeValue) pairs, AttributeNum is the oridinal position of the attribute (0, 1, 2...)
	   in the list of attributes described in the DbInfo */
	DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST TranslatedAttributes;
	/* we need to type cast because this must work with input and output attributes */
	if ((retValue = refRecordTable->TranslateAttributes(
			(CSSM_DB_RECORD_ATTRIBUTE_DATA *)AttributesToBeModified, TranslatedAttributes)) != CSSM_OK)
		RETURN_ERROR(retValue)

#if DAL_ENSURE_UNIQUENESS /* The DAL is responcible for ensure uniqueness durring a modify */

	/* Verify that the modify is legal and move to the proper record */
	if ((retValue = refRecordTable->IsModifyLegal(DLDBHandle.DLHandle,
						UniqueRecordIdentifier, TranslatedAttributes, DataToBeModified)) != CSSM_OK)
		RETURN_ERROR(retValue)

#else /* The backend is responcible for ensure uniqueness durring a modify */

	/* move to the record */
	if ((retValue = refRecordTable->MoveToRecord(UniqueRecordIdentifier)) != CSSM_OK)
		RETURN_ERROR(retValue)
#endif

	/* preform the modify */
	if ((retValue = dal_DataModify(refRecordTable, TranslatedAttributes, DataToBeModified)) != CSSM_OK)
		RETURN_ERROR(retValue)

	RETURN(retValue)
}

/* wrapper around dal_DataModify */
CSSM_RETURN CSSMDLI DL_DataModify(
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_RECORDTYPE RecordType,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecordIdentifier,
	const CSSM_DB_RECORD_ATTRIBUTE_DATA *AttributesToBeModified,
	const CSSM_DATA *DataToBeModified,
    CSSM_DB_MODIFY_MODE ModifyMode)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DataModify(DLDBHandle, RecordType, UniqueRecordIdentifier,
							  AttributesToBeModified, DataToBeModified, ModifyMode);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

static void CheckQuery(CSSM_QUERY const * Query)
{
	uint32 i = 0, j = 0;
	CSSM_DB_ATTRIBUTE_DATA_PTR ptr;

	for (i=0; i < Query->NumSelectionPredicates; i++)
	{
		ptr =  &Query->SelectionPredicate[i].Attribute;
		if (ptr->Info.AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_STRING)
		{
			for (j = 0; j < ptr->NumberOfValues; j++) {
				ASSERT (ptr->Value[j].Data[ptr->Value[j].Length-1] == 0);
				if (ptr->Value[j].Data[ptr->Value[j].Length-1] != 0)
					CSSMTRACE0 ("CheckQuery: Query predicate string without terminating NULL\n");
			}
		}
	}
}

/*-----------------------------------------------------------------------------
 * Name: dal_DataGetFirst & DL_DataGetFirst
 *
 * Description:
 * This function initiates a query and returns information about the first
 * found elements or returns CSSM_DL_NO_DATA_FOUND if nothing is found.
 * More information about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED					An exception occurred while processing the query
 * CSSMERR_DL_FIELD_SPECIFIED_MULTIPLE			   A record index was specified multiple times with different values
 * CSSM_DL_INCOMPATIBLE_FIELD _FORMAT			The specified field format is not compatible with the schema, the index's value, or an operator

 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSM_DL_INVALID_FIELD_NAME					Index or attribute name was not found in the schema
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSM_DL_INVALID_QUERY						Invalid data found inside Query
 * CSSM_DL_INVALID_PARSING_FUNCTIONS			The record parsing function table needs to be set for this record type before performing a query
 * CSSM_DL_INVALID_RECORD_SIGNING_INFO			The signature or the record did not verify
 * CSSM_DL_INVALID_RECORDTYPE					RecordType not supported by database, or conflicts with Attributes or Query
 * CSSM_DL_INVALID_SELECTION_PRED				Invalid query selection predicate
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 * CSSM_DL_NO_DATA_FOUND						No data records match the selection predicates
 * CSSM_DL_UNSUPPORTED_NUM_SELECTION_PREDS		The number of selection predicates is not supported
 * CSSM_DL_UNSUPPORTED_OPERATOR					An unsupported operator was requested
 * CSSM_DL_UNSUPPORTED_QUERY_LIMITS				The requested query limits are not supported
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DataGetFirst (
	CSSM_DL_DB_HANDLE DLDBHandle,
	const CSSM_QUERY * Query,
	CSSM_HANDLE_PTR ResultsHandle,
	CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
	CSSM_DATA_PTR Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueId)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_DataGetFirst, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/* Check inputs */
	CHECK_OUTPUT_RECORD_DATA(Data, Attributes);
	CHECK_DL_DB_HANDLE(DLDBHandle);
	CHECK_OUTPUT_POINTER(ResultsHandle, CSSM_HANDLE);
	CHECK_OUTPUT_POINTER(UniqueId, CSSM_DB_UNIQUE_RECORD_PTR);

	if ((retValue = dal_IsInvalidQuery(Query)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/*
	 * Following is added just to track invalid string predicates.
	 */
	CheckQuery (Query);

	/* Clear the outputs */
	*ResultsHandle = 0;
	*UniqueId = NULL;

	/* Get the appropriate recordset */
	DAL_RECORD_TABLE_REF refRecordTable;
	if ((retValue = refRecordTable.Initialize(DLDBHandle.DBHandle, Query->RecordType)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if (Attributes && Attributes->DataRecordType != Query->RecordType)
		RETURN(CSSMERR_DL_INVALID_RECORDTYPE)

	DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST TranslatedAttributes;
	if ((retValue = refRecordTable->TranslateAttributes(Attributes, TranslatedAttributes)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Preform the actual Query */
	if ((retValue = refRecordTable->FindRecords(Query, ResultsHandle)) != CSSM_OK)
	{
		*ResultsHandle = 0;
		RETURN_ERROR(retValue)
	}

	if ((retValue = dal_GetCurrentRecordInfo(DLDBHandle, refRecordTable,
								TranslatedAttributes, Data, UniqueId)) != CSSM_OK)
	{
		refRecordTable->DeleteQuery(*ResultsHandle);
		*ResultsHandle = 0;
		RETURN(retValue)
	}

	RETURN(retValue)
}

/* wrapper around dal_DataGetFirst */
CSSM_RETURN CSSMDLI DL_DataGetFirst (
	CSSM_DL_DB_HANDLE DLDBHandle,
	const CSSM_QUERY *Query,
	CSSM_HANDLE_PTR ResultsHandle,
	CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
	CSSM_DATA_PTR Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueId)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DataGetFirst(DLDBHandle,Query,ResultsHandle, Attributes,Data, UniqueId);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_DataGetNext & DL_DataGetNext
 *
 * Description:
 * This function RETURN_ERRORs the next element from a find started with DataGetFirst.
 * More information about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED					 Opening the records caused an exception
 * CSSM_DL_INCOMPATIBLE_FIELD_FORMAT			The specified field format (data type) for an attribute is not compatible with the schema

 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSM_DL_INVALID_FIELD_NAME					Attribute name was not found in the schema
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSM_DL_INVALID_RECORDTYPE					Attribute->DataRecordType conflicts with ResultsHandle
 * CSSM_DL_INVALID_RECORD_SIGNING_INFO			The signature on the record did not verify
 * CSSM_DL_INVALID_RESULTS_HANDLE				Invalid results handle
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 * CSSM_DL_NO_DATA_FOUND					  No more records found
 * CSSMERR_DL_FIELD_SPECIFIED_MULTIPLE			   A record attribute was specified multiple times
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DataGetNext (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_HANDLE ResultsHandle,
	CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
	CSSM_DATA_PTR Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueId)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_DataGetNext, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/* Check inputs */
	CHECK_OUTPUT_RECORD_DATA(Data, Attributes);
	CHECK_DL_DB_HANDLE(DLDBHandle);
	CHECK_RESULTS_HANDLE(ResultsHandle);
	CHECK_OUTPUT_POINTER(UniqueId, CSSM_DB_UNIQUE_RECORD_PTR);

	/* Clear the outputs */
	*UniqueId = NULL;

	CSSM_DB_RECORDTYPE RecordType;
	DAL_RECORD_TABLE_REF refRecordTable;
	if ((retValue = refRecordTable.Initialize(DLDBHandle.DBHandle, ResultsHandle, &RecordType)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if (Attributes && Attributes->DataRecordType != RecordType)
		RETURN(CSSMERR_DL_INVALID_RECORDTYPE)

	DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST TranslatedAttributes;
	if ((retValue = refRecordTable->TranslateAttributes(Attributes, TranslatedAttributes)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Move to the next element */
	if ((retValue = refRecordTable->ContinueQuery(ResultsHandle)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if ((retValue = dal_GetCurrentRecordInfo(DLDBHandle, refRecordTable, TranslatedAttributes, Data, UniqueId)) != CSSM_OK)
		RETURN_ERROR(retValue)

	RETURN(retValue)
}

/* wrapper around dal_DataGetNext */
CSSM_RETURN CSSMDLI DL_DataGetNext (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_HANDLE ResultsHandle,
	CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
	CSSM_DATA_PTR Data,
	CSSM_DB_UNIQUE_RECORD_PTR *UniqueId)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DataGetNext(DLDBHandle, ResultsHandle, Attributes, Data, UniqueId);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

/*-----------------------------------------------------------------------------
 * Name: dal_DataAbortQuery & DL_AbortQuery
 *
 * Description:
 * This function aborts a query initiated using DataGetFirst. More information
 * about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED				  Unable to abort query
 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSM_DL_INVALID_RESULTS_HANDLE				Invalid results handle
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DataAbortQuery(
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_HANDLE ResultsHandle)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_DataAbortQuery, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/* Check inputs */
	CHECK_DL_DB_HANDLE(DLDBHandle);
	CHECK_RESULTS_HANDLE(ResultsHandle);

	CSSM_DB_RECORDTYPE RecordType;
	DAL_RECORD_TABLE_REF refRecordTable;
	if ((retValue = refRecordTable.Initialize(DLDBHandle.DBHandle, ResultsHandle, &RecordType)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if ((retValue = refRecordTable->DeleteQuery(ResultsHandle)) != CSSM_OK)
		RETURN_ERROR(retValue)

	RETURN(retValue)
}
/* wrapper around dal_DataAbortQuery */
CSSM_RETURN CSSMDLI DL_DataAbortQuery(
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_HANDLE ResultsHandle)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DataAbortQuery(DLDBHandle, ResultsHandle);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}


/*-----------------------------------------------------------------------------
 * Name: DL_DataGetFromUniqueRecordId
 *
 * Description:
 * This function reteives attributes and the data object from a record in a Db.
 * More information about this function can be found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED			 An exception occurred while processing the query
 * CSSM_DL_INCOMPATIBLE_FIELD_FORMAT			The specified field format (data type) for an attribute is not compatible with the schema
 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSM_DL_INVALID_FIELD_NAME					Attribute name was not found in the schema
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSM_DL_INVALID_RECORD_SIGNING_INFO			The signature on the record did not verify
 * CSSM_DL_INVALID_RECORDTYPE					Attribute->DataRecordType conflicts with UniqueRecord
 * CSSMERR_DL_INVALID_RECORD_UID				   The unique record identifier is not valid
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_DataGetFromUniqueRecordId (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord,
	CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
	CSSM_DATA_PTR Data)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_DataGetFromUniqueRecordId, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/* Check inputs */
	CHECK_DL_DB_HANDLE(DLDBHandle);
	CHECK_OUTPUT_RECORD_DATA(Data, Attributes);

	/* Get the appropriate recordset */
	CSSM_DB_RECORDTYPE RecordType;
	DAL_RECORD_TABLE_REF refRecordTable;
	if ((retValue = refRecordTable.Initialize(DLDBHandle.DBHandle, UniqueRecord, &RecordType)) != CSSM_OK)
		RETURN_ERROR(retValue)

	if (Attributes && Attributes->DataRecordType != RecordType)
		RETURN(CSSMERR_DL_INVALID_RECORD_UID)

	DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST TranslatedAttributes;
	if ((retValue = refRecordTable->TranslateAttributes(Attributes, TranslatedAttributes)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Move to the record */
	if ((retValue = refRecordTable->MoveToRecord(UniqueRecord)) != CSSM_OK)
		RETURN_ERROR(retValue)

	/* Get the Data */
	if (Data) {
		if ((retValue = refRecordTable->GetData(DLDBHandle.DLHandle, Data)) != CSSM_OK)
			RETURN_ERROR(retValue)
	}

	/* Get the Attributes */
	if (Attributes)
	{
		uint32 Semantics = 0;
		if ((retValue = refRecordTable->GetSemantics(Semantics)) != CSSM_OK)
			RETURN_ERROR(retValue)

		Attributes->SemanticInformation = Semantics;

		/* TranslatedAttributes contains a pointer to the data object in Attributes, so
		   this will fill in the data object inside Attributes */
		if ((retValue = refRecordTable->GetAttributes(DLDBHandle.DLHandle, TranslatedAttributes)) != CSSM_OK)
			RETURN_ERROR(retValue)
	}

	RETURN(retValue)
}
/* wrapper around dal_DataGetFromUniqueRecordId */
CSSM_RETURN CSSMDLI DL_DataGetFromUniqueRecordId (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord,
	CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
	CSSM_DATA_PTR Data)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DataGetFromUniqueRecordId(DLDBHandle, UniqueRecord, Attributes, Data);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}


/*-----------------------------------------------------------------------------
 * Name: dal_FreeUniqueRecord & DL_FreeUniqueRecord
 *
 * Description:
 * This function frees the Unique Record pointer RETURN_ERRORed as a part of
 * DataInsert/GetFirst/GetNext. More information about this function can be
 * found in the Spec.
 *
 * Error Codes:
 * CSSMERR_DL_FUNCTION_FAILED			   Exception thrown during FreeUniqueRecord
 * CSSMERR_DL_INVALID_DB_HANDLE					   Invalid database handle
 * CSSMERR_DL_INVALID_POINTER					   Invalid pointer passed to function
 * CSSMERR_DL_INVALID_RECORD_UID				   The unique record identifier is not valid
 * CSSMERR_DL_MEMORY_ERROR						   Error in allocating memory
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_FreeUniqueRecord(
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_FreeUniqueRecord, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/* Check inputs */
	CHECK_DL_DB_HANDLE(DLDBHandle);

	if (!UniqueRecord)
		RETURN(CSSMERR_DL_INVALID_POINTER)

	if ((retValue = dl_IsUniqueRecordIdOk (UniqueRecord)) != CSSM_OK)
		RETURN (retValue)

	if ((retValue = dlbe_FreeUniqueRecordId(DLDBHandle, UniqueRecord)) != CSSM_OK)
		RETURN_ERROR(retValue)

	RETURN(retValue)
}
/* wrapper around dal_FreeUniqueRecord */
CSSM_RETURN CSSMDLI DL_FreeUniqueRecord(
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_FreeUniqueRecord(DLDBHandle, UniqueRecord);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

#if USE_DL_PASSTHROUGH

/*-----------------------------------------------------------------------------
 * Name: DL_PassThrough
 *
 * Description:
 * PassThrough function to allow non-standard extensions to the DL's api
 *
 * Parameters:
 * DLDBHandle (input/optional) : DL Handle (required) and DB Handle (optional)
 * PassThroughId (input) : Identifies Pass through function to use
 *
 * RETURN_ERROR value:
 * NULL if an error occurred, otherwise a pointer to an implementation defined structure
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_DB_HANDLE			 Invalid database handle
 * CSSMERR_DL_INVALID_PASSTHROUGH_ID	 Invalid passthrough ID
 * CSSMERR_DL_INVALID_POINTER			 Invalid pointer passed to function
 * CSSMERR_DL_FUNCTION_FAILED		   Passthrough function caused an exception
 * CSSMERR_DL_INTERNAL_ERROR			 Error occurred while reading from DL's registry
 *---------------------------------------------------------------------------*/
CSSM_RETURN dal_PassThrough (
	CSSM_DL_DB_HANDLE DLDBHandle,
	uint32 PassThroughId,
	const void *InputParams,
	void **OutputParams)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;

	DL_API_FUNCTION_BEGIN(DL_PassThrough, DLDBHandle.DLHandle);

	CHECK_DL_HANDLE(DLDBHandle.DLHandle);

	if ((retValue = dlbe_PassThrough(DLDBHandle, PassThroughId, InputParams, OutputParams)) != CSSM_OK)
		RETURN_ERROR(retValue)

	RETURN(retValue)
}

/* wrapper around dal_PassThrough */
CSSM_RETURN CSSMDLI DL_PassThrough (
	CSSM_DL_DB_HANDLE DLDBHandle,
	uint32 PassThroughId,
	const void *InputParams,
	void **OutputParams)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);
	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_PassThrough(DLDBHandle, PassThroughId, InputParams, OutputParams);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}

#endif /* USE_DL_PASSTHROUGH */

/*-----------------------------------------------------------------------------
 * Name: dal_CreateOpenArgCheck
 *
 * Description:
 * Checks the inputs for DbCreate and DbOpen
 *
 * Parameters:
 * see DbOpen
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_ACCESS_REQUEST
 * CSSM_DL_INVALID_AUTHENTICATION
 * CSSM_DL_INVALID_DB_NAME
 * CSSMERR_DL_INVALID_POINTER
 * CSSM_DL_INVALID_DB_LOCATION
 *---------------------------------------------------------------------------*/
static CSSM_RETURN dal_CreateOpenArgCheck(
	const CSSM_NET_ADDRESS *DbLocation,
    CSSM_DB_ACCESS_TYPE AccessRequest,
	const CSSM_ACCESS_CREDENTIALS *AccessCred,
	const void *OpenParameters,
	CSSM_DB_HANDLE *DbHandle)

{
	CSSM_RETURN retValue = CSSM_OK;
	uint32 ValidAccess = CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE | CSSM_DB_ACCESS_PRIVILEGED;

	/* Check inputs */
	CHECK_DB_LOCATION(DbLocation);

	/* Check for all valid bits and also check that
	 * READ access is always provided
	 */
	if ((!AccessRequest)
		|| (AccessRequest & (~ValidAccess))
		|| !(AccessRequest & CSSM_DB_ACCESS_READ))
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_REQUEST)
	}

	/* Since we don't support Access Credentials, verify that the param is null */
	if (AccessCred)
	{
		RETURN (CSSMERR_DL_INVALID_ACCESS_CREDENTIALS)
	}

	if (OpenParameters)
		RETURN(CSSMERR_DL_INVALID_OPEN_PARAMETERS)

	CHECK_OUTPUT_POINTER(DbHandle, CSSM_DB_HANDLE);

	RETURN(retValue)
}


CSSM_RETURN dal_CreateRelation (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_RECORDTYPE RelationID,
	const char *RelationName,
	uint32 NumberOfAttributes,
	const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
	uint32 NumberOfIndexes,
	const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;
	DAL_DB_OPEN_PARAM openParam;
	DAL_MODULE_PARAMETERS Parameters;
	CSSM_DBINFO DbInfo;
	uint32 i;


	DL_API_FUNCTION_BEGIN(DL_CreateRelation, DLDBHandle.DLHandle);
	ASSERT_DAL_VALID();

	/*
	 * Validate input parameters.
	 */
	CHECK_DL_DB_HANDLE(DLDBHandle);
	if (dlutil_IsUnsupportedRecordType(RelationID))
	{
		return (CSSMERR_DL_UNSUPPORTED_RECORDTYPE);
	}
	if (NULL == RelationName)
		return CSSMERR_DL_INVALID_POINTER;

	if (NumberOfAttributes == 0)
		return CSSMERR_DL_MISSING_VALUE;
	if (NumberOfAttributes > MAX_NUM_FIELDS)
		return CSSMERR_DL_UNSUPPORTED_NUM_ATTRIBUTES;
	CHECK_INPUT_POINTER (pAttributeInfo, sizeof (CSSM_DB_SCHEMA_ATTRIBUTE_INFO)*NumberOfAttributes);

	if (NumberOfIndexes) {
		if (NumberOfIndexes > MAX_NUM_FIELDS)
			return CSSMERR_DL_UNSUPPORTED_NUM_INDEXES;
		CHECK_INPUT_POINTER (pIndexInfo, sizeof (CSSM_DB_SCHEMA_INDEX_INFO)*NumberOfIndexes);
	}

	/*
	 * Get Database list from thread local storage, should not fail
	 */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		RETURN(FAILURE_ERROR);
	}

	/*
	 * Get the database name and parameters.
	 */
	if ((retValue = pDatabaseList->GetDBNamesAndParameters(DLDBHandle, &openParam)) != CSSM_OK)
	{
		RETURN_ERROR(retValue)
	}
	memcpy (&Parameters, openParam.dbParameters, sizeof (DAL_MODULE_PARAMETERS));

	/*
	 * Make sure that database was opened in EXCLUSIVE mode.
	 */
	if (!(Parameters.AccessRequest & CSSM_DB_ACCESS_PRIVILEGED))
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_REQUEST)
	}

	/* Make sure that database was opened in READ | WRITE mode */
	if (!((Parameters.AccessRequest & CSSM_DB_ACCESS_READ)
		 && (Parameters.AccessRequest & CSSM_DB_ACCESS_WRITE)))
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_REQUEST)
	}

	/* Get a database object, this will lock the mutex, which needs to be unlocked before we exit */
    PORT_MUTEX_HANDLE hMutex;
	DAL_DATABASE_INFO_PTR pDatabaseInfo;
	if ((retValue = pDatabaseList->FindDatabaseByHandle(DLDBHandle.DBHandle, pDatabaseInfo, hMutex)) != CSSM_OK)
		RETURN_ERROR(retValue)

	ASSERT(pDatabaseInfo != NULL);

	#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	/* Declare AdditionalInformation */
	uint8 AdditionalInformationBlock[ADDITIONAL_INFORMATION_MAX_LENGTH];
	Parameters.AdditionalInformation.Data = AdditionalInformationBlock;
	Parameters.AdditionalInformation.Length = ADDITIONAL_INFORMATION_MAX_LENGTH;
#endif

	/*
	 * Get the Module Info.
	 */
	if ((retValue = dal_ReadDbInfo(openParam.dbName, &DbInfo, ADDITIONAL_INFORMATION)) != CSSM_OK)
	{
		port_UnlockMutex(hMutex);
		RETURN_ERROR(retValue)
	}

	/*
	 * Make sure that record type is not already there in the database.
	 */
	for (i = 0; i < DbInfo.NumberOfRecordTypes; i++)
	{
		if (DbInfo.RecordAttributeNames[i].DataRecordType == RelationID)
		{
			dlnr_FreeDbInfo(&DbInfo);
			port_UnlockMutex(hMutex);
			RETURN (CSSMERR_DL_INVALID_RECORDTYPE);
		}
	}

	/*
	 * Create database tables for the new relation.
	 */
	if ((retValue = pDatabaseInfo->CreateRelation(
#if DAL_USE_MODULE_PARAMATERS
		&Parameters,
#endif
		RelationID,
		RelationName,
		NumberOfAttributes,
		pAttributeInfo,
		NumberOfIndexes,
		pIndexInfo
		)) != CSSM_OK)
	{
		dlnr_FreeDbInfo(&DbInfo);
		port_UnlockMutex(hMutex);
		RETURN_ERROR(retValue)
	}


	/*
	 * Create the new dbinfo to be written into the metadata.
	 */
	if ((retValue = dal_AddRelationToDbInfo (&DbInfo,
				RelationID,
				RelationName,
				NumberOfAttributes,
				pAttributeInfo,
				NumberOfIndexes,
				pIndexInfo)) != CSSM_OK)
	{
		dlnr_FreeDbInfo(&DbInfo);
		port_UnlockMutex(hMutex);
		RETURN_ERROR(retValue)
	}


	/*
	 * Updated the Dbinfo for the relation.
	 */
#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	if ((retValue = dl_schemaFileAddDb(MODULE_GUID_STRING,
									   openParam.dbName,
									   &DbInfo,
									   &Parameters.AdditionalInformation))
									   != CSSM_OK)
#else
	if ((retValue = dl_schemaFileAddDb(MODULE_GUID_STRING,
									   openParam.dbName,
									   &DbInfo,
									   NULL)) != CSSM_OK)
#endif
	{
		dlnr_FreeDbInfo(&DbInfo);
		port_UnlockMutex(hMutex);
		RETURN_ERROR(retValue)
	}

	dlnr_FreeDbInfo(&DbInfo);
	port_UnlockMutex(hMutex);

	/*
	 * Close and open the database with the same handle.
	 */
	dal_DbClose (DLDBHandle);

	if ((retValue = dal_DbOpen (
					DLDBHandle.DLHandle,
					openParam.dbName,
					Parameters.DbLocation,
					Parameters.AccessRequest,
					Parameters.AccessCred,
					Parameters.OptionalParameters,
					&DLDBHandle.DBHandle)) != CSSM_OK)
	{
		port_UnlockMutex(hMutex);
		RETURN(retValue);
	}

	RETURN(retValue)
}

CSSM_RETURN CSSMDLI DL_CreateRelation (
	CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_RECORDTYPE RelationID,
	const char *RelationName,
	uint32 NumberOfAttributes,
	const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
	uint32 NumberOfIndexes,
	const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo)
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);

	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_CreateRelation(DLDBHandle, RelationID, RelationName, NumberOfAttributes,
							pAttributeInfo, NumberOfIndexes, pIndexInfo);

	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}


CSSM_RETURN CSSMAPI dal_DestroyRelation(
						CSSM_DL_DB_HANDLE DLDBHandle,
						CSSM_DB_RECORDTYPE RelationID )
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	CSSM_RETURN retValue = CSSM_OK;
	DAL_DB_OPEN_PARAM openParam;
	DAL_MODULE_PARAMETERS Parameters;

	CSSM_DBINFO DbInfo;
	uint32 i;


	DL_API_FUNCTION_BEGIN(DL_CreateRelation, DLDBHandle.DLHandle);

	ASSERT_DAL_VALID();

	/*
	 * Validate input parameters.
	 */
	CHECK_DL_DB_HANDLE(DLDBHandle);
	if (dlutil_IsUnsupportedRecordType(RelationID))
	{
		return (CSSMERR_DL_UNSUPPORTED_RECORDTYPE);
	}

	/*
	 * Get Database list from thread local storage, should not fail
	 */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		RETURN(FAILURE_ERROR);
	}

	/*
	 * Get the database name and parameters.
	 */
	if ((retValue = pDatabaseList->GetDBNamesAndParameters(DLDBHandle, &openParam)) != CSSM_OK)
	{
		RETURN_ERROR(retValue)
	}
	memcpy (&Parameters, openParam.dbParameters, sizeof (DAL_MODULE_PARAMETERS));

	/*
	 * Make sure that database was opened in EXCLUSIVE mode.
	 */
	if (!(Parameters.AccessRequest & CSSM_DB_ACCESS_PRIVILEGED))
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_REQUEST)
	}

	/* Make sure that database was opened in READ | WRITE mode */
	if (!((Parameters.AccessRequest & CSSM_DB_ACCESS_READ)
		 && (Parameters.AccessRequest & CSSM_DB_ACCESS_WRITE)))
	{
		RETURN(CSSMERR_DL_INVALID_ACCESS_REQUEST)
	}

	/*
	 * Get a database object, this will lock the mutex, which needs to be
	 * unlocked before we exit
	 */
    PORT_MUTEX_HANDLE hMutex;
	DAL_DATABASE_INFO_PTR pDatabaseInfo;
	if ((retValue = pDatabaseList->FindDatabaseByHandle(DLDBHandle.DBHandle, pDatabaseInfo, hMutex)) != CSSM_OK)
		RETURN_ERROR(retValue)

	ASSERT(pDatabaseInfo != NULL);

	#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	/* Declare AdditionalInformation */
	uint8 AdditionalInformationBlock[ADDITIONAL_INFORMATION_MAX_LENGTH];
	Parameters.AdditionalInformation.Data = AdditionalInformationBlock;
	Parameters.AdditionalInformation.Length = ADDITIONAL_INFORMATION_MAX_LENGTH;
#endif

	/*
	 * Get the Module Info.
	 */
	if ((retValue = dal_ReadDbInfo(openParam.dbName, &DbInfo, ADDITIONAL_INFORMATION)) != CSSM_OK)
	{
		port_UnlockMutex(hMutex);
		RETURN_ERROR(retValue)
	}

	/*
	 * Make sure that record type is there in the database.
	 */
	for (i = 0; i < DbInfo.NumberOfRecordTypes; i++)
	{
		if (DbInfo.RecordAttributeNames[i].DataRecordType == RelationID)
		{
			break;

		}
	}
	if (i == DbInfo.NumberOfRecordTypes) {
		port_UnlockMutex(hMutex);
		RETURN (CSSMERR_DL_INVALID_RECORDTYPE);
	}

	/*
	 * Create the new dbinfo to be written into the metadata.
	 */
	if ((retValue = dal_RemoveRelationFromDbInfo (&DbInfo,
				RelationID)) != CSSM_OK)
	{
		dlnr_FreeDbInfo(&DbInfo);
		port_UnlockMutex(hMutex);
		RETURN_ERROR(retValue)
	}


	/*
	 * Updated the Dbinfo for the relation.
	 */
#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	if ((retValue = dl_schemaFileAddDb(MODULE_GUID_STRING,
									   openParam.dbName,
									   &DbInfo,
									   &Parameters.AdditionalInformation))
									   != CSSM_OK)
#else
	if ((retValue = dl_schemaFileAddDb(MODULE_GUID_STRING,
									   openParam.dbName,
									   &DbInfo,
									   NULL)) != CSSM_OK)
#endif
	{
		dlnr_FreeDbInfo(&DbInfo);
		port_UnlockMutex(hMutex);
		RETURN_ERROR(retValue)
	}

	dlnr_FreeDbInfo(&DbInfo);

	/*
	 * Destroy database tables for the relation.
	 */
	if ((retValue = pDatabaseInfo->DestroyRelation(
#if DAL_USE_MODULE_PARAMATERS
		&Parameters,
#endif
		RelationID
		)) != CSSM_OK)
	{
		dlnr_FreeDbInfo(&DbInfo);
		port_UnlockMutex(hMutex);
		RETURN_ERROR(retValue)
	}

	port_UnlockMutex(hMutex);

	/*
	 * Close and open the database with the same handle.
	 */
	dal_DbClose (DLDBHandle);

	if ((retValue = dal_DbOpen (
					DLDBHandle.DLHandle,
					openParam.dbName,
					Parameters.DbLocation,
					Parameters.AccessRequest,
					Parameters.AccessCred,
					Parameters.OptionalParameters,
					&DLDBHandle.DBHandle)) != CSSM_OK)
	{
		port_UnlockMutex(hMutex);
		RETURN(retValue);
	}

	RETURN(retValue)
}

CSSM_RETURN CSSMAPI DL_DestroyRelation(
						CSSM_DL_DB_HANDLE DLDBHandle,
						CSSM_DB_RECORDTYPE RelationID )
{
	const uint32 FAILURE_ERROR = CSSMERR_DL_FUNCTION_FAILED;
	void *pLockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, DLDBHandle.DLHandle, pLockRef);

	if ((CSSM_OK != retValue) || (pLockRef == NULL))
		WRAPPER_RETURN(FAILURE_ERROR)

	retValue = dal_DestroyRelation(DLDBHandle, RelationID);
	Addin_SPIEnd(pLockRef);

	WRAPPER_RETURN(retValue)
}
