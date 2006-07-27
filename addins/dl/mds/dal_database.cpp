/*-----------------------------------------------------------------------
 * File: DAL_DATABASE.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/*
 * This file contains inmplentation of the DAL_DATABASE_INFO
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

#define DAL_SOURCE_FILE
#include "dal_classes.h"
#include "dal_internal.h"
#include "maf_collectn.h"
#include "maf_util.h"
#include "bioapi_util.h"

typedef CSSM_DB_PARSING_MODULE_INFO const * CSSM_CONST_PARSING_MODULE_PTR;
typedef CSSM_DB_RECORD_ATTRIBUTE_INFO const * CSSM_CONST_ATTRIBUTE_LIST_PTR;
typedef CSSM_DB_RECORD_INDEX_INFO const * CSSM_CONST_INDEX_LIST_PTR;

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::DAL_DATABASE_INFO
 *
 * Description:
 * Initializes the member variables of an instance of a DAL_DATABASE_INFO class.
 * then get the reader lock on the current database
 *
 *---------------------------------------------------------------------------*/
DAL_DATABASE_INFO::DAL_DATABASE_INFO()
{
	START_CONSTRUCTOR();
	ASSERT(this);

	m_hDatabase = NULL;
	m_szDBName[0] = 0;
#ifdef _DEBUG
	m_debug_NumOpenHandles = 0;
#endif
	m_fHaveWriterLock = CSSM_FALSE;
	memset (&m_Parameters, 0, sizeof (DAL_MODULE_PARAMETERS));
	memset (&m_DbInfoLock, 0, sizeof (cssm_SWMRLock));

	END_CONSTRUCTOR();
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::~DAL_DATABASE_INFO
 *
 * Description:
 * Delete open (which closes open) RecordTables, delete translation tables,
 * close connections to the CL (or whatever record parser is) and close the
 * connection to the backend
 *---------------------------------------------------------------------------*/
DAL_DATABASE_INFO::~DAL_DATABASE_INFO()
{
	ASSERT(this);
#ifndef MODULE_DIRECTORY_SERVICES /* Turn off this feature for MDS */
	ASSERT(m_rgRecordTable.neGetNumRecordTypes() == m_rgParseUid.neGetNumRecordTypes());
#endif
	ASSERT(m_rgRecordTable.neGetNumRecordTypes() == m_rgTranslationTable.neGetNumRecordTypes());
	ASSERT(m_rgRecordTable.neGetNumRecordTypes() == m_rgParseHandle.neGetNumRecordTypes());

#ifndef MODULE_DIRECTORY_SERVICES /* Turn this feature off for MDS */
	unsigned i;

	for (i = 0; i < m_rgParseUid.neGetNumRecordTypes(); i++)
	{
		/* If the Guid != 0 && the Handle != 0, this is the original handle, free it */
		if (m_rgParseUid.neGetByPosition(i))
		{
			/* Get the recordtype for the Parsing Uid's node, use this to find element in another
			   array */
			CSSM_DB_RECORDTYPE RecordType = m_rgParseUid.neGetRecordType(i);
			ASSERT(m_rgParseHandle.neGetByRecordType(RecordType) != NULL);
			if (CSSM_OK == dal_CSSM_ModuleDetachAndUnload(
				*m_rgParseHandle.neGetByRecordType(RecordType), &m_rgParseUid.neGetByPosition(i)->Guid)
			)
			{
#ifdef _DEBUG /* Make sure we detach from all attached modules */
				m_debug_NumOpenHandles--;
#endif
			}
		}
	}
#endif /* End of functions removed for MDS */

	ASSERT(!m_debug_NumOpenHandles); /* Make sure we detach from all attached modules */

	/* Close the conection to the backend */
	if (m_hDatabase)
	{
		dlbe_DestroyDatabase(m_hDatabase);
	}

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
	if (m_Parameters.DbLocation)
		BioAPI_free ((void *)m_Parameters.DbLocation, NULL);
	if (m_Parameters.AccessCred)
		BioAPI_free ((void *)m_Parameters.AccessCred, NULL);
	if (m_Parameters.OptionalParameters)
		BioAPI_free ((void *)m_Parameters.OptionalParameters, NULL);
#endif


#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
		if (m_Parameters.AdditionalInformation.Data)
			BioAPI_free (m_Parameters.AdditionalInformation.Data, NULL);
#endif


	/*
	 * Release and delete the lock, if created.
	 */
	cssm_SWMRLock tmpLock;
	memset (&tmpLock, 0, sizeof (cssm_SWMRLock));

	if (memcmp (&m_DbInfoLock, &tmpLock, sizeof (cssm_SWMRLock)))
	{
		if (m_fHaveWriterLock)
			cssm_SWMRLockDoneWriting(&m_DbInfoLock);
		else
			cssm_SWMRLockDoneReading(&m_DbInfoLock);

	cssm_SWMRLockDelete(&m_DbInfoLock);
	}


}

/*-----------------------------------------------------------------------------
 * Name: s_dal_SortDbInfo
 *
 * Description:
 * Static function to sort the three parallel DbInfo structures by putting them
 * into DAL_TYPE_ARRAY's.
 *
 * Parameters:
 * pDbInfo (input) : DbInfo to sort
 * rgpParsingList (output) : CSSM_DB_PARSING_MODULE_INFO Array (will be filled in)
 * rgpAttributeList (output) : CSSM_DB_RECORD_ATTRIBUTE_INFO Array (will be filled in)
 * rgpIndexList (output) : CSSM_DB_RECORD_INDEX_INFO Array (will be filled in)
 *
 * Return value:
 * CSSM_OK on success, otherwise Error
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
static CSSM_RETURN s_dal_SortDbInfo(
	CSSM_DBINFO const * pDbInfo,
	DAL_TYPE_ARRAY<const CSSM_DB_PARSING_MODULE_INFO *> &rgpParsingList,
	DAL_TYPE_ARRAY<const CSSM_DB_RECORD_ATTRIBUTE_INFO *> &rgpAttributeList,
	DAL_TYPE_ARRAY<const CSSM_DB_RECORD_INDEX_INFO *> &rgpIndexList)
{
	CSSM_RETURN retValue = CSSM_OK;
	if ((retValue = rgpParsingList.Initialize(pDbInfo->NumberOfRecordTypes)) != CSSM_OK)
		return retValue;
	if ((retValue = rgpAttributeList.Initialize(pDbInfo->NumberOfRecordTypes)) != CSSM_OK)
		return retValue;
	if ((retValue = rgpIndexList.Initialize(pDbInfo->NumberOfRecordTypes)) != CSSM_OK)
		return retValue;

	uint32 i;
	for (i = 0; i < pDbInfo->NumberOfRecordTypes; i++)
	{
		/* Make sure a recordtype is not used twice */
		if (rgpParsingList.neGetByRecordType(pDbInfo->DefaultParsingModules[i].RecordType) != NULL ||
			rgpAttributeList.neGetByRecordType(pDbInfo->RecordAttributeNames[i].DataRecordType) != NULL ||
			rgpIndexList.neGetByRecordType(pDbInfo->RecordIndexes[i].DataRecordType) != NULL)
		{
			/* Recordtype can not be defined multiple times */
			return CSSMERR_DL_INVALID_RECORDTYPE;
		}

		rgpParsingList.neSetData(pDbInfo->DefaultParsingModules[i].RecordType) =
			&pDbInfo->DefaultParsingModules[i];
		rgpAttributeList.neSetData(pDbInfo->RecordAttributeNames[i].DataRecordType) =
			&pDbInfo->RecordAttributeNames[i];
		rgpIndexList.neSetData(pDbInfo->RecordIndexes[i].DataRecordType) =
			&pDbInfo->RecordIndexes[i];
	}

	for (i = 0; i < pDbInfo->NumberOfRecordTypes; i++)
	{
		ASSERT(rgpParsingList.neGetByRecordType(pDbInfo->DefaultParsingModules[i].RecordType) != NULL);

		/* If one exists, they must all exist */
		if (rgpAttributeList.neGetByRecordType(pDbInfo->DefaultParsingModules[i].RecordType) == NULL ||
			rgpIndexList.neGetByRecordType(pDbInfo->DefaultParsingModules[i].RecordType) == NULL)
		{
			return CSSMERR_DL_INVALID_RECORDTYPE;
		}
	}

	return CSSM_OK;
}

static CSSM_RETURN
s_dal_CopyModuleParameters (
					DAL_MODULE_PARAMETERS *destParameters,
					DAL_MODULE_PARAMETERS *srcParameters)
{
	memset (destParameters, 0, sizeof (DAL_MODULE_PARAMETERS));

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS


	destParameters->AccessRequest = srcParameters->AccessRequest;

	if (srcParameters->DbLocation) {
		if ((destParameters->DbLocation = (CSSM_NET_ADDRESS *)
								BioAPI_calloc (sizeof (CSSM_NET_ADDRESS), 1, NULL)) == NULL)
		{
			return CSSMERR_DL_MEMORY_ERROR;
		}
		memcpy ((void *)destParameters->DbLocation, srcParameters->DbLocation, sizeof (CSSM_NET_ADDRESS));
	}

#endif
	return CSSM_OK;
}
/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::Initialize
 *
 * Description:
 * Initializes a new database object; This function MUST be called before any
 * member function. This allocates any variables, and then Initializes them.
 * The DbInfo is parsed by (a) Index translation table object(s)
 *
 * Parameters:
 * DLHandle (input) : Handle of the current DL instance, used to reteive App's Mem Functions
 * to pass to the CL
 * pDbInfo (input) : DbInfo passed to the DL. The DbInfo has already been swept of pointer
 * errors, however, it is unsorted
 *
 * Return value:
 * CSSM_OK on success, otherwise Error
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO::Initialize(
	CSSM_HANDLE DLHandle,
	const char *DbName,
#if DAL_USE_MODULE_PARAMATERS
	DAL_MODULE_PARAMETERS *pParameters,
#endif
	CSSM_DBINFO const * pDbInfo)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	START_INITIALIZE();
	ASSERT(DbName);
	ASSERT(DLHandle);

	if (!pDbInfo)
	{
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	strcpy(m_szDBName, DbName);

#if DAL_USE_MODULE_PARAMATERS
	if ((retValue = s_dal_CopyModuleParameters (&m_Parameters, pParameters)) != CSSM_OK)
		return retValue;
#endif

	/* Initialize all arrays with the number of recordtypes */
	if ((retValue = m_rgParseHandle.Initialize(pDbInfo->NumberOfRecordTypes)) != CSSM_OK)
		return retValue;
	if ((retValue = m_rgRecordTable.Initialize(pDbInfo->NumberOfRecordTypes)) != CSSM_OK)
		return retValue;
	if ((retValue = m_rgTranslationTable.Initialize(pDbInfo->NumberOfRecordTypes)) != CSSM_OK)
		return retValue;
#ifndef MODULE_DIRECTORY_SERVICES /* Turn this feature off for MDS */
	if ((retValue = m_rgParseUid.Initialize(pDbInfo->NumberOfRecordTypes)) != CSSM_OK)
		return retValue;
#endif

	/*
	 * Sort the arrays in the DbInfo structure by creating arrays of pointers;
	 */
	DAL_TYPE_ARRAY<const CSSM_DB_PARSING_MODULE_INFO *> rgpParsingList;
	DAL_TYPE_ARRAY<const CSSM_DB_RECORD_ATTRIBUTE_INFO *> rgpAttributeList;
	DAL_TYPE_ARRAY<const CSSM_DB_RECORD_INDEX_INFO *> rgpIndexList;

	if ((retValue = s_dal_SortDbInfo(pDbInfo, rgpParsingList, rgpAttributeList, rgpIndexList)) != CSSM_OK)
		return retValue;

	/*
	 * The DbInfo arrays are valid, now lets make the translation tables and the parse handle,
	 * this will validate the contense of the DbInfo
	 */
	uint32 i;
	for (i = 0; i < pDbInfo->NumberOfRecordTypes; i++)
	{
		CSSM_DB_RECORDTYPE RecordType = pDbInfo->DefaultParsingModules[i].RecordType;

		CSSM_DB_PARSING_MODULE_INFO const* const*ppParsingModule = rgpParsingList.neGetByRecordType(RecordType);
		CSSM_DB_RECORD_ATTRIBUTE_INFO const* const* ppAttributes = rgpAttributeList.neGetByRecordType(RecordType);
		CSSM_DB_RECORD_INDEX_INFO const* const*ppIndexes = rgpIndexList.neGetByRecordType(RecordType);
		ASSERT(ppParsingModule && ppAttributes && ppIndexes);
		ASSERT(*ppParsingModule && *ppAttributes && *ppIndexes);

		if ((retValue = m_rgTranslationTable.neSetData(RecordType).Initialize(*ppAttributes, *ppIndexes)) != CSSM_OK)
		{
			return retValue;
		}

		if ((retValue = MakeParseHandle(DLHandle, &(*ppParsingModule)->ModuleSubserviceUid, RecordType)) != CSSM_OK)
		{
			return retValue;
		}
	}

	/*
	 * The input is correct, we now need to initialize the DAL_RECORD_TABLE instances
	 */
	for (i = 0; i < m_rgTranslationTable.neGetNumRecordTypes(); i++)
	{
		CSSM_DB_RECORDTYPE RecordType = m_rgTranslationTable.neGetRecordType(i);
		const DAL_TRANSLATION_TABLE *pTTable = m_rgTranslationTable.neGetByPosition(i);

		uint32 RecordTypeIndex = DAL_OOB;
		DAL_RECORD_TABLE & RecordTable = m_rgRecordTable.neSetData(RecordType, &RecordTypeIndex);

		CSSM_HANDLE * pParseHandle = m_rgParseHandle.neGetByRecordType(RecordType);
		CSSM_HANDLE ParseHandle;
		CSSM_SERVICE_TYPE ServiceType; /* Service Type for the parsing module */

		if (pParseHandle == NULL)
		{
			/* Parsing handle is NULL, so we need to give them the handle that the caller passed to us */
			ServiceType = CSSM_SERVICE_DL;
			ParseHandle = DLHandle;
		}
		else
		{
			/* We connected to a parser, so pass a handle to that addin, get the type of that addin */
			const CSSM_DB_PARSING_MODULE_INFO ** ppParsingModule = rgpParsingList.neGetByRecordType(RecordType);
			if (ppParsingModule == NULL || *ppParsingModule == NULL)
			{
				/* That recordtype should be in the rgpParsingList array, I do not know why it is not */
				return CSSMERR_DL_INTERNAL_ERROR;
			}

			ServiceType = (*ppParsingModule)->ModuleSubserviceUid.SubserviceType;
			ParseHandle = *pParseHandle;
		}

#if (DAL_ENSURE_UNIQUENESS)
        PORT_MUTEX_HANDLE pUniquenessMutex;

		if (pTTable->neGetUniqueIndexCount() == 0)
			pUniquenessMutex = NULL;
		else
		{
			char szMutexNameBuffer[MAX_DB_NAME_LENGTH + sizeof("uniqueness-mutex-" MODULE_GUID_STRING "--") + 8 + 1];
			sprintf(szMutexNameBuffer, "uniqueness-mutex-" MODULE_GUID_STRING "-%s-%08x", DbName, i);
#ifdef POSIX
	    pUniquenessMutex = (PORT_MUTEX_HANDLE) local_malloc(sizeof(pthread_mutex_t), NULL);
#endif
			CSSM_RETURN ret = port_CreateMutex(szMutexNameBuffer, &pUniquenessMutex);

			if (CSSM_OK != ret)
			{
#ifdef POSIX
		local_free(pUniquenessMutex, NULL);
#endif
				return CSSMERR_DL_INTERNAL_ERROR;
			}
		}
#endif
		if ((retValue = RecordTable.Initialize(DLHandle, RecordType, RecordTypeIndex, ParseHandle, ServiceType,
			pTTable
#if (DAL_ENSURE_UNIQUENESS)
			,pUniquenessMutex
#endif
			)) != CSSM_OK)
		{
			return retValue;
		}
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::Open
 *
 * Description:
 * This function will open up an existing data store. It will then verify the validity
 * of this data store and lastly create and Initalize an instance of the DAL_RECORD_TABLE
 * object.
 *
 * Parameters:
 * DbName (input) : Name of the Database to open
 * Parameters (input) : Data to pass to dlbe_OpenDatabase
 *
 * Return value:
 * CSSM_OK on success, otherwise Error
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO::Open(
#if DAL_USE_MODULE_PARAMATERS
	const DAL_MODULE_PARAMETERS * pParameters
#endif
)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	if ((retValue = dlbe_OpenDatabase(&m_hDatabase, m_szDBName MODULE_PARAMETERS)) != CSSM_OK)
		return retValue;

	END_INITIALIZE();

	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	unsigned i;
	for (i = 0; i < m_rgTranslationTable.neGetNumRecordTypes(); i++)
	{
		CSSM_DB_RECORDTYPE RecordType = m_rgTranslationTable.neGetRecordType(i);
		const DAL_TRANSLATION_TABLE *pTTable = m_rgTranslationTable.neGetByPosition(i);
		TABLE_BACKEND_REF rTableBackend;

		if ((retValue = dlbe_OpenTable(m_hDatabase, &rTableBackend, RecordType,
			pTTable MODULE_PARAMETERS)) != CSSM_OK)
		{
			return retValue;
		}


		DAL_RECORD_TABLE * pRecordTable = m_rgRecordTable.neGetByRecordType(RecordType);
		ASSERT(pRecordTable);

		if ((retValue = pRecordTable->ConnectBackend(NULL, rTableBackend)) != CSSM_OK)
			return retValue;
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::Create
 *
 * Description:
 * Creates and Opens a new backend, then adds tables to the database, one
 * for each recordtype, lastly create a RecordTable object for each table.
 *
 * Parameters:
 * DbName (input) : Name of the Database
 * Parameters (input) : Data to pass to dlbe_CreateDatabase
 *
 * Return value:
 * CSSM_OK on success, otherwise Error
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO::Create(
#if DAL_USE_MODULE_PARAMATERS
	DAL_MODULE_PARAMETERS * pParameters
#endif
)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	ASSERT(m_fHaveWriterLock);

	if ((retValue = dlbe_CreateDatabase(&m_hDatabase,
										m_szDBName MODULE_PARAMETERS)) != CSSM_OK)
		return (retValue);

	END_INITIALIZE();

#ifdef _DEBUG
	/* Get Database list from thread local storage, should not fail */
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
	{
		return CSSMERR_DL_INTERNAL_ERROR;
	}

#endif

	uint32 i;
	for (i = 0; i < m_rgTranslationTable.neGetNumRecordTypes(); i++)
	{
		CSSM_DB_RECORDTYPE RecordType = m_rgTranslationTable.neGetRecordType(i);
		const DAL_TRANSLATION_TABLE *pTTable = m_rgTranslationTable.neGetByPosition(i);
		TABLE_BACKEND_REF rTableBackend;

		if ((retValue = dlbe_CreateTable(m_hDatabase, &rTableBackend, RecordType,
			pTTable MODULE_PARAMETERS)) != CSSM_OK)
		{
			return retValue;
		}

		DAL_RECORD_TABLE * pRecordTable = m_rgRecordTable.neGetByRecordType(RecordType);
		ASSERT(pRecordTable);

		if ((retValue = pRecordTable->ConnectBackend(NULL, rTableBackend)) != CSSM_OK)
			return retValue;
	}

	return CSSM_OK;
}


/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::neGetRecordTable
 *
 * Description:
 * Returns the RecordTable for a particular Recordtype
 *
 * Parameters:
 * Recordtype (input) : The recordtype (Cert, CRL, ect) that a RecordTable is needed for
 *
 * Return value:
 * A reference to the RecordTable, or NULL; this object should not be deleted.
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
DAL_RECORD_TABLE * DAL_DATABASE_INFO::neGetRecordTable(CSSM_DB_RECORDTYPE Recordtype) const
{
	AssertValid();

	return m_rgRecordTable.neGetByRecordType(Recordtype);
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::neGetRecordTypeByIndex
 *
 * Description:
 * Returns the Record Type for an index into the Record Table array
 *
 * Parameters:
 * Index (input) : An index into the record table array
 *
 * Return value:
 * DAL_RECORDTYPE_OOB if the record table has not been initialized, otherwise
 * the record type
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_DB_RECORDTYPE DAL_DATABASE_INFO::neGetRecordTypeByIndex(uint32 Index) const
{
	AssertValid();

	return m_rgRecordTable.neGetRecordType(Index);
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::MakeParseHandle
 *
 * Description:
 * This function will connect to the record parser if it needs to. If it does
 * need to connect, it will search through the list of Subservice UID's looking for
 * UidPtr, if it finds this Uid, it will make a corrspondance between
 * RecordType and a handle to an attached module. Otherwise, it will connect
 * to the module specified in UidPtr and make the association.
 *
 * Parameters:
 * DLHandle (input) : Handle of the current DL instance, this is used to get the app's
 * mem functions.
 * UidPtr (input) : Unique Identifier of the subservice used to parse the record type
 * RecordType(input) : Recordtype that the module in UidPtr is needed for
 *
 * Return value:
 * CSSM_OK on success, otherwise Error
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO::MakeParseHandle(
		CSSM_HANDLE DLHandle,
		CSSM_SUBSERVICE_UID const * UidPtr,
		CSSM_DB_RECORDTYPE RecordType)
{
	AssertValid();
	ASSERT(UidPtr);

	/* if the data in UidPtr is zero, we do not need to connect to the parsing module */
	if (dal_IsMemoryZero(UidPtr, sizeof(CSSM_SUBSERVICE_UID)))
	{
		return CSSM_OK;
	}

#ifdef MODULE_DIRECTORY_SERVICES /* Feature for MDS */

	return CSSMERR_DL_INVALID_PARSING_MODULE;

#else /* Turn this feature off for MDS */

	ASSERT(m_rgTranslationTable.neGetByRecordType(RecordType));
	if (!m_rgTranslationTable.neGetByRecordType(RecordType))
	{
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	/* There are no data indexes, so we do not need to connect to the CL */
	if (!m_rgTranslationTable.neGetByRecordType(RecordType)->neGetDataIndexCount())
	{
		return CSSM_OK;
	}

	uint32 i;
	for (i = 0; i < m_rgParseUid.neGetNumRecordTypes(); i++)
	{
		const CSSM_SUBSERVICE_UID * pCurrUid = m_rgParseUid.neGetByPosition(i);
		if (pCurrUid && memcmp(pCurrUid, UidPtr, sizeof(CSSM_SUBSERVICE_UID)) == 0)
		{
			/* We are all ready connected to the apropriate Parsing module, just copy the handle,
			   do not copy to Uid, leave the Uid as zero so that we know which handles to detach from */
			CSSM_DB_RECORDTYPE SourceRT = m_rgParseUid.neGetRecordType(i);
			const CSSM_HANDLE * pSourceHandle = m_rgParseHandle.neGetByRecordType(SourceRT);
			ASSERT(pSourceHandle && *pSourceHandle);
			m_rgParseHandle.neSetData(RecordType) = *pSourceHandle;

			return CSSM_OK;
		}
	}

	/* We need to connect to the the apropriate Parsing module */
	CSSM_API_MEMORY_FUNCS AppMemFuncs;
	CSSM_RETURN retValue;
	if ((retValue = dal_CSSM_GetAPIMemoryFunctions (DLHandle, &AppMemFuncs)) != CSSM_OK)
	{
		return retValue;
	}

	CSSM_HANDLE ParseHandle = dal_CSSM_ModuleLoadAndAttach(UidPtr, &AppMemFuncs);

    if (!ParseHandle)
	{
		return CSSMERR_DL_INVALID_PARSING_MODULE;
	}

	m_rgParseHandle.neSetData(RecordType) = ParseHandle;
	m_rgParseUid.neSetData(RecordType) = *UidPtr;
#ifdef _DEBUG /* Make sure we detach from all attached modules */
	m_debug_NumOpenHandles++;
#endif

	return CSSM_OK;
#endif /* End of functions removed for MDS */
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::neGetDbName
 *
 * Description:
 * Returns a pointer to the internal Name array. This should NOT be freed
 *
 * Parameters:
 * None.
 *
 * Return value:
 * Returns a pointer to the array, this value is never NULL
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
char const * DAL_DATABASE_INFO::neGetDbName() const
{
	AssertValid();

	return (char *)m_szDBName;
}

const DAL_MODULE_PARAMETERS *
DAL_DATABASE_INFO::neGetModuleParameters() const
{
	AssertValid();

	return &m_Parameters;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::SetDbCreateLock
 *
 * Description:
 * Get the persistant database information (not data) writer lock so that we
 * can add information to the registry.
 *
 * Parameters:
 * None.
 *
 * Return value:
 * Error if an error occurred, else CSSM_OK
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO::SetDbCreateLock(const char *DbName)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	ASSERT(!m_fHaveWriterLock);
	ASSERT(DbName);
	ASSERT(DbName[0]);

	if ((retValue = dal_CreateDbInfoLock(DbName, &m_DbInfoLock)) != CSSM_OK)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return retValue;
	}

	/* Wait for a nero-zero amount of time */
	CSSM_RETURN Result = cssm_SWMRLockWaitToWrite(&m_DbInfoLock, 5);

	if (Result == CSSM_OK)
	{

		m_fHaveWriterLock = CSSM_TRUE;
		return CSSM_OK;
	}
	else if (Result == CSSMERR_CSSM_INTERNAL_ERROR)
	{
		/* If the lock is being used, the datastore must already exist,
		   but give a little time in case we are concurrent with the end of a DbDelete */
		return CSSMERR_DL_DATASTORE_ALREADY_EXISTS;
	}
	else
	{
		return CSSMERR_DL_INTERNAL_ERROR;
	}
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::SetDbOpenLock
 *
 * Description:
 * Get the persistant database information (not data) reader lock so that we
 * can read information from the registry and prevent deletes while the Db is open.
 *
 * Parameters:
 * None.
 *
 * Return value:
 * Error if an error occurred, else CSSM_OK
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO::SetDbOpenLock(const char *DbName)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	ASSERT(!m_fHaveWriterLock);
	ASSERT(DbName);
	ASSERT(DbName[0]);

	if ((retValue = dal_CreateDbInfoLock(DbName, &m_DbInfoLock)) != CSSM_OK)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return retValue;
	}

	/* Wait .1 seconds in case we are right at the end of create/delete/open with admin */
	retValue = cssm_SWMRLockWaitToRead(&m_DbInfoLock, 100);

	if (CSSM_OK == retValue)
	{
		return CSSM_OK;
	}
	else if (CSSMERR_CSSM_INTERNAL_ERROR == retValue)
	{
		return CSSMERR_DL_DB_LOCKED;
	}
	else
	{
		return CSSMERR_DL_INTERNAL_ERROR;
	}
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO::nrDbCreateToDbOpenLock
 *
 * Description:
 * Release the persistant database information (not data) writer lock and go
 * back to the reader lock that DbOpen uses
 *
 * Parameters:
 * None.
 *
 * Return value:
 * none.
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
void DAL_DATABASE_INFO::nrDbCreateToDbOpenLock()
{
	ASSERT(m_fHaveWriterLock);
	AssertValid();

	cssm_SWMRLockWriteToRead(&m_DbInfoLock);
	m_fHaveWriterLock = CSSM_FALSE;
}

#ifdef _DEBUG

void DAL_DATABASE_INFO::AssertValidNotInitialized(void) const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_DATABASE_INFO)));

	ASSERT(m_debug_NumOpenHandles == 0);
	ASSERT(m_hDatabase == NULL);
	ASSERT(m_szDBName[0] == 0);
}

void DAL_DATABASE_INFO::AssertValid(void) const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_DATABASE_INFO)));

#ifndef MODULE_DIRECTORY_SERVICES /* Turn off this feature for MDS */
	ASSERT(m_rgRecordTable.neGetNumRecordTypes() == m_rgParseUid.neGetNumRecordTypes());
#endif
	ASSERT(m_rgRecordTable.neGetNumRecordTypes() == m_rgTranslationTable.neGetNumRecordTypes());
	ASSERT(m_rgRecordTable.neGetNumRecordTypes() == m_rgParseHandle.neGetNumRecordTypes());

	if (m_debug_ObjectState == Constructing)
		return;
	if (m_debug_ObjectState == NotInitialized)
	{
		AssertValidNotInitialized();
		return;
	}

	uint32 i;
	for (i = 0; i <= MAX_DB_NAME_LENGTH; i++)
	{
		if (!m_szDBName[i])
			break;
	}

	ASSERT(i <= MAX_DB_NAME_LENGTH);

	if (m_debug_ObjectState == Initialized)
	{
		ASSERT(m_szDBName[0]);
		for (i = 0; i < m_rgRecordTable.neGetNumRecordTypes(); i++)
		{
			ASSERT(m_rgTranslationTable.neGetByRecordType(m_rgTranslationTable.neGetRecordType(i)) != NULL);
			if (m_rgParseHandle.neGetByRecordType(m_rgParseHandle.neGetRecordType(i)) != NULL)
			{
				ASSERT(*m_rgParseHandle.neGetByRecordType(m_rgParseHandle.neGetRecordType(i)) != NULL);
			}
		}
	}
}

#endif /* End debug code */
