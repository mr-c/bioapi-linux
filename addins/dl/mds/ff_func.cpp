/*-----------------------------------------------------------------------
 * File: FF_FUNC.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the flat file specific backend interface. All functions here must
 * also exit in any other interface.
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif


/* DL includes */
#include "ff_util.h"
#include "ff_port.h"
#include "dlutil.h"
#include "dal_translation.h"
#include "intel/ff_passthrough.h"

/* CSSM includes */
#include "cssmerr.h"
#include "flatfile.h"

#include "bioapi_util.h"

/*-----------------------------------------------------------------------------
 * Name: dlbe_Initialize
 *
 * Description:
 * This function is called as part of DL_Initialize, and gives the Backend a
 * chance to perform any intializations.
 *
 * Parameters:
 * None.
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * CSSM_DL_INITIALIZE_FAIL
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlbe_Initialize(void)
{
#if defined(_WIN32) && defined(_DEBUG)
	/* turn outputting of memory leaks on */
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);

	ASSERT(_CrtCheckMemory());
#endif
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dlbe_Uninitialize
 *
 * Description:
 * This function is called as part of DL_Uninitialize, and gives the Backend a
 * chance to perform any unintializations.
 *
 * Parameters:
 * None.
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * CSSM_DL_INITIALIZE_FAIL
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlbe_Uninitialize(void)
{
#if defined(_WIN32) && defined(_DEBUG)
	ASSERT(_CrtCheckMemory());
#endif
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dlbe_CreateDatabase
 *
 * Description:
 * Tells flat file to create a new data store. The access file is placed in the current
 * directory.
 *
 * Parameters:
 * DbName (input): The name of the Data store to create
 * OpenParams (input): Unused.
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlbe_CreateDatabase(DATABASE_BACKEND_HANDLE *phDatabase, const char *DbName,
	DAL_MODULE_PARAMETERS * Parameters)
{
	ASSERT(phDatabase);
	ASSERT(DbName);
	ASSERT(Parameters);

	FF_FUNCTION_BEGIN(dlbe_CreateDatabase);

	if (ffutil_fIsBadDbName(DbName))
		return CSSMERR_DL_INVALID_DB_NAME;

	/* Fill in Parameters->AdditionalInformation.Data with the path start (CurrDir\DbName) */
    CATCH_FUNCTION(ffport_eGetPathStart(DbName, 
										GetPathStart(Parameters),
										Parameters->AccessPath))
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		RETURN_ERROR();
	}

	/* Find the length of additional Information */
	GetPathStartLength(Parameters) = strlen(GetPathStart(Parameters)) + 1;

	return dlbe_OpenDatabase(phDatabase, DbName, Parameters);
}


/*-----------------------------------------------------------------------------
 * Name: dlbe_OpenDatabase
 *
 * Description:
 * Will allocate memory for a database and then open it up using flat file commands
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
CSSM_RETURN dlbe_OpenDatabase(DATABASE_BACKEND_HANDLE *phDatabase, const char *DbName,
	DAL_MODULE_PARAMETERS const * Parameters)
{
	char * DbNameCopy;

	ASSERT(phDatabase);
	ASSERT(DbName);
	ASSERT(Parameters);

	FF_FUNCTION_BEGIN(dlbe_OpenDatabase);

	if (!phDatabase)
		return CSSMERR_DL_FUNCTION_FAILED;

	DbNameCopy = (char *)BioAPI_calloc(strlen(DbName)+1, 1, NULL);
	if (!DbNameCopy)
		return CSSMERR_DL_MEMORY_ERROR;

	strcpy(DbNameCopy, DbName);

	DATABASE_BACKEND* pDatabase = DbNameCopy;

	*phDatabase = (void *)pDatabase;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dlbe_DestroyDatabase
 *
 * Description:
 * Will destroy a database that was created by Create/Open Database.
 *
 * Parameters:
 * hDatabase (input) Handle to the database created by Create/Open Db
 *
 * Return value:
 * none.
 *
 * Error Codes:
 * none.
 *---------------------------------------------------------------------------*/
void dlbe_DestroyDatabase(DATABASE_BACKEND_HANDLE hDatabase)
{
	FF_FUNCTION_BEGIN(dlbe_DestroyDatabase);

	ASSERT(hDatabase);

	BioAPI_free((void *)hDatabase, NULL);
}


/*-----------------------------------------------------------------------------
 * Name: dlbe_DeleteDatabase
 *
 * Description:
 * This function removes a datastore from flat file and delete the access file
 *
 * Parameters:
 * DbName (input) : The name of the database to delete
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlbe_DeleteDatabase(const char *DbName, DAL_MODULE_PARAMETERS const * Parameters)
{
	FF_FUNCTION_BEGIN(dlbe_DeleteDatabase);

	ASSERT(DbName);
	ASSERT(Parameters);
	ASSERT(GetPathStart(Parameters) && GetPathStartLength(Parameters));

	RETURN_FAILURE(ffport_eDeleteDbFiles(GetPathStart(Parameters)));

	return CSSM_OK;
}

CSSM_RETURN dlbe_DeleteRelation(CSSM_DB_RECORDTYPE RelationID, DAL_MODULE_PARAMETERS const * Parameters)
{
	FF_FUNCTION_BEGIN(dlbe_DeleteDatabase);

	ASSERT(Parameters);
	ASSERT(GetPathStart(Parameters) && GetPathStartLength(Parameters));

	RETURN_FAILURE(ffport_eDeleteDbRelationFiles(RelationID, GetPathStart(Parameters)));

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: fff_static_nrCreatePathNames
 *
 * Description:
 * Creates the paths to the index, freelist, and data file
 *
 * Parameters:
 * IndexPath (output) : Buffer for the index file's path
 * DataPath (output) : Buffer for the data file's path
 * FreeListPath (output) : Buffer for the data file's path
 * PathStart (input) : Directory where the file is plus the file prefix for
 *	 files that are associates with this db.
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * CSSMERR_DL_INTERNAL_ERROR (should never be returned)
 * CSSMERR_DL_MEMORY_ERROR
 *---------------------------------------------------------------------------*/
static void fff_static_nrCreatePathNames(
	char *IndexPath,
	char *DataPath,
	char *FreeListPath,
	const char *PathStart,
	CSSM_DB_RECORDTYPE Recordtype)
{
	sprintf(IndexPath, "%s.%08x.index.ffdb", PathStart, (uint32)Recordtype);
	sprintf(FreeListPath, "%s.%08x.freelist.ffdb", PathStart, (uint32)Recordtype);
	sprintf(DataPath, "%s.%08x.data.ffdb", PathStart, (uint32)Recordtype);
}


/*-----------------------------------------------------------------------------
 * Name: dlbe_OpenTable
 *
 * Description:
 * Will allocate memory for a table and then open it up using flat file commands
 *
 * Parameters:
 * FUNCTION_ARG (input/output)
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * CSSMERR_DL_INTERNAL_ERROR (should never be returned)
 * CSSMERR_DL_MEMORY_ERROR
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlbe_OpenTable(DATABASE_BACKEND_HANDLE hDatabase, TABLE_BACKEND_REF *prTableBackend,
	CSSM_DB_RECORDTYPE Recordtype, const DAL_TRANSLATION_TABLE *pTranslationTable,
	const DAL_MODULE_PARAMETERS * Parameters)
{
	uint32 Error;
	CSSM_RETURN ret = CSSM_OK;
	CSSM_DB_ACCESS_TYPE AccessFilter = 0;
	CSSM_DATA_PTR Passphrase = NULL;

	FF_FUNCTION_BEGIN(dlbe_OpenTable);

	VERIFY_PTR(prTableBackend);
	VERIFY_PTR(hDatabase);
	VERIFY_PTR(pTranslationTable);

	/*DATABASE_BACKEND* pDatabase = (DATABASE_BACKEND*)hDatabase;*/
	FLATFILE_TABLE_BACKEND_REF rFlatfileBackend = NULL;

	const char *PathStart = GetPathStart(Parameters);
	ASSERT(PathStart);
	PATH_NAME IndexPath, DataPath, FreeListPath;
	fff_static_nrCreatePathNames(IndexPath, DataPath, FreeListPath, PathStart, Recordtype);

	if (CSSM_FALSE == ffport_neDoesFileExist(IndexPath))
		return CSSMERR_DL_DATABASE_CORRUPT;

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	/*
	if (Parameters->UserAuthentication)
		Passphrase = Parameters->UserAuthentication->Credential;
	else
	*/
		Passphrase = NULL;

	if (CSSM_FALSE == ffport_IsValidFilePermissions (
										Passphrase,
										IndexPath,
										Parameters->AccessRequest,
										AccessFilter,
										&Error))
		return CSSMERR_DL_OS_ACCESS_DENIED;
#endif

	if (CSSM_FALSE == ffport_neDoesFileExist(FreeListPath))
		return CSSMERR_DL_DATABASE_CORRUPT;

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	if (CSSM_FALSE == ffport_IsValidFilePermissions (
										Passphrase,
										FreeListPath,
										Parameters->AccessRequest,
										AccessFilter,
										&Error))
		return CSSMERR_DL_OS_ACCESS_DENIED;
#endif

	if (CSSM_FALSE == ffport_neDoesFileExist(DataPath))
		return CSSMERR_DL_DATABASE_CORRUPT;

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	if (CSSM_FALSE == ffport_IsValidFilePermissions (
										Passphrase,
										DataPath,
										Parameters->AccessRequest,
										AccessFilter,
										&Error))
		return CSSMERR_DL_OS_ACCESS_DENIED;
#endif

	rFlatfileBackend = new FLATFILE_TABLE_BACKEND;
	*prTableBackend = (TABLE_BACKEND*)rFlatfileBackend; /* Cast to base class */
	if (*prTableBackend == NULL)
		return CSSMERR_DL_MEMORY_ERROR;

	uint32 NumIndexes = pTranslationTable->neGetIndexCount();
	uint32 NumAttributes = pTranslationTable->neGetPureAttributeCount();
	ret = (rFlatfileBackend)->Initialize(pTranslationTable, NumIndexes, NumAttributes, Recordtype,
		IndexPath, FreeListPath, DataPath, Parameters);

	if (CSSM_OK != ret) {
		dlbe_DestroyTable (*prTableBackend);
		*prTableBackend = NULL;
	}
	return ret;
}

CSSM_RETURN dlbe_CreateFiles(DATABASE_BACKEND_HANDLE hDatabase, TABLE_BACKEND_REF *prTableBackend,
	CSSM_DB_RECORDTYPE Recordtype, const DAL_TRANSLATION_TABLE * pTable,
	DAL_MODULE_PARAMETERS * Parameters)
{
	CSSM_RETURN ret;
	char FileMode[8];
	void *AccessCtx = NULL;
	CSSM_DATA_PTR Passphrase = NULL;

	FF_FUNCTION_BEGIN(dlbe_CreateTable);

	VERIFY_PTR(hDatabase);
	VERIFY_PTR(prTableBackend);
	VERIFY_PTR(pTable);
	VERIFY_PTR(Parameters);

	/*DATABASE_BACKEND* pDatabase = (DATABASE_BACKEND*)hDatabase;*/

	PATH_NAME IndexPath, DataPath, FreeListPath;
	fff_static_nrCreatePathNames(IndexPath, DataPath, FreeListPath, GetPathStart(Parameters), Recordtype);

	uint32 NumIndexes = pTable->neGetIndexCount();
	uint32 NumAttributes = pTable->neGetPureAttributeCount();

	/* Write out the files */
	/*
	 * Setup permissions for index file and write it out.
	 */
	strcpy (FileMode, "wb");

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	/*
	if (Parameters->UserAuthentication)
		Passphrase = Parameters->UserAuthentication->Credential;
	else
	*/
		Passphrase = NULL;

	if ((ret = ffport_SetupFilePermissionsStart(Passphrase,
											IndexPath,
											Parameters->DbLocation,
											FileMode,
											&AccessCtx)) != CSSM_OK)
	{
		return ret;
	}
#endif

	ret = ff_index_WriteNewFile(IndexPath, NumIndexes, NumAttributes, (const char *)FileMode);


#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	if (((ret = ffport_SetupFilePermissionsEnd (AccessCtx)) != CSSM_OK)
								|| (CSSM_OK != ret))
	{
		return ret;
	}
#else
	if (CSSM_OK != ret)
		return ret;
#endif

	/*
	 * Setup permissions for freelist file and write it out.
	 */
#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	if ((ret = ffport_SetupFilePermissionsStart(Passphrase,
											FreeListPath,
											Parameters->DbLocation,
											FileMode,
											&AccessCtx)) != CSSM_OK)
	{
		return ret;
	}
#endif

	ret = ff_freeList_WriteNewFile(FreeListPath, (const char *)FileMode);


#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	if (((ret = ffport_SetupFilePermissionsEnd (AccessCtx)) != CSSM_OK)
								|| (CSSM_OK != ret))
	{
		return ret;
	}
#else
	if (CSSM_OK != ret)
		return ret;
#endif

	/*
	 * Setup permissions for data file and write it out.
	 */
#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	if ((ret = ffport_SetupFilePermissionsStart(Passphrase,
											DataPath,
											Parameters->DbLocation,
											FileMode,
											&AccessCtx)) != CSSM_OK)
	{
		return ret;
	}
#endif

	ret = ff_data_WriteNewFile(DataPath, NumIndexes + NumAttributes + 1, (const char *)FileMode);

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	if (((ret = ffport_SetupFilePermissionsEnd (AccessCtx)) != CSSM_OK)
								|| (CSSM_OK != ret))
	{
		return ret;
	}
#else
	if (CSSM_OK != ret)
		return ret;
#endif

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dlbe_CreateTable
 *
 * Description:
 *	Creates the a SQL String to create a table for a particular data type.
 * The schema table format depends on the number of index, and the number of attributes.
 *
 * The following should be added for record signing:
 * SignatureCSSMLen,SignatureCSSMAlg,SignatureCSSMParamLen,SignatrueCSSMParam,
 * SignatureLenCSS,SignatureCSSM fields are also added as record signing fields
 *
 * Parameters:
 * Recordtype (input) : The type of record that the table is created for
 *
 * Return value:
 * CSSM_OK or CSSM_FAIL
 *
 * Error Codes:
 * CSSM_DL_DB_CREATE_FAIL
 * CSSMERR_DL_MEMORY_ERROR
 * CSSMERR_DL_INTERNAL_ERROR (should never be returned)
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlbe_CreateTable(DATABASE_BACKEND_HANDLE hDatabase, TABLE_BACKEND_REF *prTableBackend,
	CSSM_DB_RECORDTYPE Recordtype, const DAL_TRANSLATION_TABLE * pTable,
	DAL_MODULE_PARAMETERS * Parameters)
{
	CSSM_RETURN ret;

	if ((ret = dlbe_CreateFiles(hDatabase, prTableBackend, Recordtype, pTable, Parameters))
					!= CSSM_OK)
	{
		return ret;
	}

	/* Open the new files */
	return dlbe_OpenTable(hDatabase, prTableBackend, Recordtype, pTable, Parameters);
}

/*-----------------------------------------------------------------------------
 * Name: dlbe_DestroyTable
 *
 * Description:
 * Closes the connection to a table and destroys anything created in Create/Open
 * Table
 *
 * Parameters:
 * rTableBackend (input) : Reference to the table backend returned by Create/Open Table
 *
 * Return value:
 * none.
 *
 * Error Codes:
 * none.
 *---------------------------------------------------------------------------*/
void dlbe_DestroyTable(TABLE_BACKEND_REF rTableBackend)
{
	FLATFILE_TABLE_BACKEND_REF rFlatfileTableBackend = (FLATFILE_TABLE_BACKEND*)rTableBackend;
	ASSERT(rTableBackend);
	delete rFlatfileTableBackend;
}

/*-----------------------------------------------------------------------------------
 * Name: dlbe_CreateUniqueRecordId
 *
 * Description:
 * This function allocates and initializes a Unique Record Id
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
 * CSSMERR_DL_MEMORY_ERROR
 *-----------------------------------------------------------------------------------*/
CSSM_RETURN dlbe_CreateUniqueRecordId(CSSM_HANDLE DLHandle,
	 CSSM_DB_UNIQUE_RECORD_PTR *UR)
{
	*UR = (CSSM_DB_UNIQUE_RECORD_PTR)App_Calloc(DLHandle, sizeof(CSSM_DB_UNIQUE_RECORD), 1);
	if (!(*UR))
		return CSSMERR_DL_MEMORY_ERROR;

	/* Create & fill in the RecordIdentifer Structure */
	(*UR)->RecordIdentifier.Data = (uint8 *)App_Calloc(DLHandle, 8, 1);
	if (!(*UR)->RecordIdentifier.Data)
	{
		App_Free(DLHandle, (*UR));
		return CSSMERR_DL_MEMORY_ERROR;
	}

	(*UR)->RecordIdentifier.Length = 8;
	((uint32 *)(*UR)->RecordIdentifier.Data)[0] = uint32(DAL_RECORDTYPE_OOB); /* Recordtype */
	((uint32 *)(*UR)->RecordIdentifier.Data)[1] = DAL_OOB; /* Id */

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------------
 * Name: dlbe_FreeUniqueRecordId
 *
 * Description:
 * This function allocates and initializes a Unique Record Id
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
 * CSSMERR_DL_INTERNAL_ERROR
 * CSSMERR_DL_INVALID_RECORD_UID
 *-----------------------------------------------------------------------------------*/
CSSM_RETURN dlbe_FreeUniqueRecordId(CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecordId)
{
	if (!UniqueRecordId)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	if (UniqueRecordId->RecordIdentifier.Length != 8)
		return CSSMERR_DL_INVALID_RECORD_UID;

   App_Free(DLDBHandle.DLHandle, UniqueRecordId->RecordIdentifier.Data);
   App_Free(DLDBHandle.DLHandle, UniqueRecordId);

   return CSSM_OK;
}

/*-----------------------------------------------------------------------------------
 * Name: dlbe_GetUniqueRecordIdType
 *
 * Description:
 * This function verifies the validity of a Unique Record Id and then retrieves the
 * information from it (RecordType)
 *
 * Parameters:
 * UniqueIdPtr (input) - A Unique Record Id Ptr
 *
 * RETURN:
 * The recordtype that the unique record id coresponds to if the Unique ID is valid,
 * otherwise returns DAL_OOB.
 *
 * ERROR CODES
 * CSSMERR_DL_INVALID_RECORD_UID
 *-----------------------------------------------------------------------------------*/
CSSM_RETURN dlbe_GetUniqueRecordIdType(CSSM_DB_UNIQUE_RECORD const * UniqueIdPtr,
									   CSSM_DB_RECORDTYPE *RecordType)
{
	CSSM_RETURN retValue = CSSM_OK;

	if (!UniqueIdPtr)
		return CSSMERR_DL_INVALID_POINTER;

	if ((retValue = dl_IsUniqueRecordIdOk(UniqueIdPtr)) != CSSM_OK)
		return retValue;

	if (UniqueIdPtr->RecordIdentifier.Length != 8)
		return CSSMERR_DL_INVALID_RECORD_UID;

	uint32 *Data = (uint32 *)UniqueIdPtr->RecordIdentifier.Data;

	*RecordType = Data[0];

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------------
 * Name: dlbe_PassThrough
 *
 * Description:
 * This function verifies the validity of a Unique Record Id and then retrieves the
 * information from it (RecordType)
 *
 * Parameters:
 * UniqueIdPtr (input) - A Unique Record Id Ptr
 *
 * RETURN:
 * The recordtype that the unique record id coresponds to if the Unique ID is valid,
 * otherwise returns DAL_OOB.
 *
 * ERROR CODES
 * CSSMERR_DL_INVALID_RECORD_UID
 *-----------------------------------------------------------------------------------*/
CSSM_RETURN dlbe_PassThrough (
	CSSM_DL_DB_HANDLE DLDBHandle,
	uint32 PassThroughId,
	const void * InputParams,
	void ** OutputParams)
{
	FFDL_WRITE_BACK_CACHE_STATUS CacheStatus;

	if (PassThroughId == FFDL_PASSTHROUGH_SET_WRITE_BACK_CACHE)
	{
		if (port_IsBadReadPtr(InputParams, sizeof(FFDL_WRITE_BACK_CACHE_STATUS)))
			return CSSMERR_DL_INVALID_POINTER;

		if (DLDBHandle.DBHandle != 0)
			return CSSMERR_DL_INVALID_DB_HANDLE;

		CacheStatus = *((const FFDL_WRITE_BACK_CACHE_STATUS *)InputParams);

		if (CacheStatus != FFDL_ENABLE_CACHE && CacheStatus != FFDL_DISABLE_CACHE)
			return FFDL_CACHE_STATUS_INVALID;

		ffport_nrSetCache(CBOOL(CacheStatus == FFDL_ENABLE_CACHE));
	}
	else
		return CSSMERR_DL_INVALID_PASSTHROUGH_ID;

	return CSSM_OK;
}
