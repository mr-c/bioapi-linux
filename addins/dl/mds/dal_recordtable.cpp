/*-----------------------------------------------------------------------
 * File: DAL_RECORDTABLE.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains implentation of most of the DAL_RECORD_TABLE and
 * DAL_RECORD_TABLE_REF classes
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

#define DAL_SOURCE_FILE
#include "dal_classes.h"
#include "dlutil.h"
#include "dal_internal.h"
#include "bioapi_util.h"

#ifndef MODULE_DIRECTORY_SERVICES
#include "maf_interface.h"
#endif

/*-----------------------------------------------------------------------------
 * Name: dal_FreeData
 *
 * Description:
 * Frees an array of CSSM_DATA structures (w/ DL's mem functions)
 *
 * Parameters:
 * rgData (output) : Pointer to a CSSM_DATA_PTR to hold the beggining address of the array
 * NumDataStructs (input) : Number of CSSM_DATA structs in the array
 *
 * Return value:
 * None.
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
static void dal_FreeData(CSSM_DATA_PTR rgData, uint32 NumDataStructs)
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


/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE_REF::DAL_RECORD_TABLE_REF
 *
 * Description:
 * This is the copy contructor for the DAL_RECORD_TABLE_REF class. This
 * function exists to ensure that DAL_RECORD_TABLE_REF are always passed by
 * reference.
 *
 *---------------------------------------------------------------------------*/
DAL_RECORD_TABLE_REF::DAL_RECORD_TABLE_REF(const DAL_RECORD_TABLE_REF & Source)
{
	/* There is no reason to call the copy constructor for this object */
	DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */

	m_hMutex = NULL;
	m_pRecordTable = Source.m_pRecordTable;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE_REF::DAL_RECORD_TABLE_REF
 *
 * Description:
 * This is the contructor for the DAL_RECORD_TABLE_REF class. It initializes
 * the paramaters to NULL.
 *
 *---------------------------------------------------------------------------*/
DAL_RECORD_TABLE_REF::DAL_RECORD_TABLE_REF()
{
	m_hMutex = NULL;
	m_pRecordTable = NULL;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE_REF::DAL_RECORD_TABLE_REF
 *
 * Description:
 * This is the destructor. This function will Unlock the mutex that was locked
 * when the class was initialized
 *
 *---------------------------------------------------------------------------*/
DAL_RECORD_TABLE_REF::~DAL_RECORD_TABLE_REF()
{
	if (m_pRecordTable)
	{
		/* indicate to the record table we are no longer going to use it */
		m_pRecordTable->ReleaseRecordTable();
	}

	if (m_hMutex)
	{
		port_UnlockMutex (m_hMutex);
	}
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE_REF::Initialize
 *
 * Description:
 * This function calls the list of databases to obtain the database info that
 * corresponds to DBHandle. When this occurs, the database info structure is
 * locked, and the mutex that is used to lock the database is returned. Last,
 * one record table is opened. This record table is determined by UniqueRecordId.
 * This function also checks the validity of UniqueRecordId.
 *
 * Parmaters:
 * DBHandle (input) : DBHandle that corresponds to a database info class
 * UniqueRecordId (input) : an *UNCHECKED* Unique Record Id that is used to
 * determine the recordtype to open.
 * pRecordType (output) : On exit, *pRecordType equals the recordtype opened
 *
 * Return:
 * CSSM_FAIL if an error occurred, else CSSM_OK
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE_REF::Initialize(
	CSSM_DB_HANDLE DBHandle,
	CSSM_DB_UNIQUE_RECORD const * UniqueRecordId,
	CSSM_DB_RECORDTYPE *pRecordType)
{
	CSSM_RETURN retValue;
	ASSERT(this && pRecordType);

	if ((retValue = dlbe_GetUniqueRecordIdType(UniqueRecordId, pRecordType)) != CSSM_OK)
		return retValue;

	return Initialize(DBHandle, *pRecordType, CSSMERR_DL_INVALID_RECORD_UID);
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE_REF::Initialize
 *
 * Description:
 * This function calls the list of databases to obtain the database info that
 * corresponds to DBHandle. When this occurs, the database info structure is
 * locked, and the mutex that is used to lock the database is returned. Last,
 * one record table is opened. This record table is determined by
 *
 * Parmaters:
 * DBHandle (input) : DBHandle that corresponds to a database info class
 * ResultsHandle (input) : Results handle from a query that is used to
 * determine the recordtype to open.
 * pRecordType (output) : On exit, *pRecordType equals the recordtype opened
 *
 * Return:
 * CSSM_FAIL if an error occurred, else CSSM_OK
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE_REF::Initialize(
	CSSM_DB_HANDLE DBHandle,
	CSSM_HANDLE ResultsHandle,
	CSSM_DB_RECORDTYPE *pRecordType)
{
	CSSM_RETURN retValue;

	ASSERT(this && pRecordType && ResultsHandle);
	/* DBHandle should be valid and this function should only be called once for an instance of
	   DAL_RECORD_TABLE_REF */
	ASSERT(DBHandle && !m_hMutex && !m_pRecordTable);

	/* Get the appropriate recordset, this will return a locked mutex */
	DAL_DATABASE_INFO_REF_PTR pDatabase;
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
		return CSSMERR_DL_INTERNAL_ERROR;

	if ((retValue = pDatabaseList->FindDatabaseByHandle(DBHandle, pDatabase, m_hMutex)) != CSSM_OK)
	{
		return retValue;
	}
	ASSERT(pDatabase);

	/* Get the recordtype from the ResultsHandle */
	*pRecordType = pDatabase->neGetRecordTypeByIndex(0xffff & ResultsHandle);

	/* Get the record table from the record type */
	m_pRecordTable = pDatabase->neGetRecordTable(*pRecordType);
	if (!m_pRecordTable)
		return CSSMERR_DL_INVALID_RESULTS_HANDLE;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE_REF::Initialize
 *
 * Description:
 * This function calls the list of databases to obtain the database info that
 * corresponds to DBHandle. When this occurs, the database info structure is
 * locked, and the mutex that is used to lock the database is returned. Last,
 * one record table is opened. This record table is determined by
 *
 * Parmaters:
 * DBHandle (input) : DBHandle that corresponds to a database info class
 * RecordType (input) : RecordType of the record table to open
 *
 *
 * Return:
 * CSSM_FAIL if an error occurred, else CSSM_OK
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE_REF::Initialize(
	CSSM_DB_HANDLE DBHandle,
	CSSM_DB_RECORDTYPE RecordType)
{
	return Initialize(DBHandle, RecordType, CSSMERR_DL_INVALID_RECORDTYPE);
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE_REF::Initialize (private version)
 *
 * Description:
 * This function calls the list of databases to obtain the database info that
 * corresponds to DBHandle. When this occurs, the database info structure is
 * locked, and the mutex that is used to lock the database is returned. Last,
 * one record table is opened. This record table is determined by
 *
 * Parmaters:
 * DBHandle (input) : DBHandle that corresponds to a database info class
 * RecordType (input) : RecordType of the record table to open
 * MissingInfoError (input) : Error code to return if RecordType does not
 * corrispond to a valid record table in the database.
 *
 * Return:
 * CSSM_FAIL if an error occurred, else CSSM_OK
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE_REF::Initialize(
	CSSM_DB_HANDLE DBHandle,
	CSSM_DB_RECORDTYPE RecordType,
	uint32 MissingInfoError)
{
	CSSM_RETURN retValue;
	/* DBHandle should be valid and this function should only be called once for an instance of
	   DAL_RECORD_TABLE_REF */
	ASSERT(DBHandle && !m_hMutex && !m_pRecordTable);

	/* Get the appropriate recordset, this will return a locked mutex */
	DAL_DATABASE_INFO_REF_PTR pDatabase;
	DAL_DATABASE_INFO_LIST *pDatabaseList = dal_GetDatabaseList();
	if (pDatabaseList == NULL)
		return CSSMERR_DL_INTERNAL_ERROR;

	if ((retValue = pDatabaseList->FindDatabaseByHandle(DBHandle, pDatabase, m_hMutex)) != CSSM_OK)
	{
		return retValue;
	}
	ASSERT(pDatabase);

	/* Get the record table from the record type */
	m_pRecordTable = pDatabase->neGetRecordTable(RecordType);
	if (!m_pRecordTable)
		return MissingInfoError;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::DAL_RECORD_TABLE
 *
 * Description:
 * Initialize all member variables with NULL or 0
 *
 *---------------------------------------------------------------------------*/
DAL_RECORD_TABLE::DAL_RECORD_TABLE(void)
{
	START_CONSTRUCTOR();

	m_QueryCount = 1;
	m_RecordTypeIndex = DAL_OOB;
    m_ParseHandle = CSSM_INVALID_HANDLE;
	m_DLHandle = 0;
#ifndef MODULE_DIRECTORY_SERVICES /* Turn this feature off for MDS */
	m_ParserType = DAL_OOB;
#endif
	m_rBackend = NULL;
	m_pTranslationTable = NULL;
	m_ParseTable.RecordGetFirstFieldValue = NULL;
	m_ParseTable.RecordGetNextFieldValue = NULL;
	m_ParseTable.RecordAbortQuery = NULL;
	m_pQueryRoot = NULL;

#if (DAL_ENSURE_UNIQUENESS)
	m_pUniquenessMutex = NULL;
	m_fIsUniquenessMutexLocked = CSSM_FALSE;
#endif

	END_CONSTRUCTOR();
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::~DAL_RECORD_TABLE
 *
 * Description:
 * Close the recordset and delete the Index and Attribute arrays
 *
 *---------------------------------------------------------------------------*/
DAL_RECORD_TABLE::~DAL_RECORD_TABLE()
{
	m_pTranslationTable = NULL; /* This may be a dangeling pointer at this point */

	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_RECORD_TABLE)));

	while (m_pQueryRoot)
	{
		DAL_QUERY_BUFFER *pTemp;
		uint32 i;

		pTemp = m_pQueryRoot;
		m_pQueryRoot = m_pQueryRoot->pNext;

		m_rBackend->nrDeleteQuery(pTemp->pBackendQueryBuffer);

		for (i = 0; i < pTemp->NumberOfRecords; i++) {
			dal_FreeData(pTemp->RecordTable[i], pTemp->NumberOfAttributes);
		}
		BioAPI_free (pTemp->RecordTable, NULL);
		BioAPI_free (pTemp->dwSemantics, NULL);
		BioAPI_free (pTemp->Uid, NULL);
		delete pTemp;
	}

	if (m_rBackend)
		dlbe_DestroyTable(m_rBackend);
	else
	{
		ASSERT(m_debug_ObjectState != Initialized);
	}

#if (DAL_ENSURE_UNIQUENESS)
	if (m_pUniquenessMutex)
	{
		port_CloseMutex(m_pUniquenessMutex);
#ifdef POSIX
	local_free(m_pUniquenessMutex, NULL);
#endif
		m_pUniquenessMutex = NULL;
	}
#endif
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::Initialize
 *
 * Description:
 * This function will Initialize a record table object; it will get the handle to the
 * record parser from it's owner, and allocate any variables that it needs.
 *
 * Parameters:
 * Recordtype (input) : The type of record that the DAL_RECORD_TABLE will work with
 * RecordTypeIndex (input) : index into the record table array owned by the table's
 *	 creator
 * ParsingHandle(input) : Handle of an addin that is used to parse the data object
 * pTable (input) : pointer to the translation table that this recordset will use
 * pUniquenessMutex (input/optional) :	a mutex that is used to guarantee that two
 *	 concurrent inserts do not cause the unique index to be violated for a relation.
 *	 This is NULL if there are unique indexes
 *
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::Initialize(
	CSSM_DL_HANDLE DLHandle,
	CSSM_DB_RECORDTYPE RecordType,
	uint32 RecordTypeIndex,
	CSSM_HANDLE ParsingHandle,
	CSSM_SERVICE_TYPE ParsingModuleServiceType,
	DAL_CONST_TRANSLATION_TABLE_PTR pTable
#if (DAL_ENSURE_UNIQUENESS)
    ,
    PORT_MUTEX_HANDLE pUniquenessMutex
#endif
	)
{
	START_INITIALIZE();
	AssertValid();
	ASSERT(RecordType != DAL_RECORDTYPE_OOB);
	ASSERT(RecordTypeIndex < (1 << 16));
	ASSERT(ParsingModuleServiceType != 0);

	/* Insure that the parameters are not NULL */
	if (!ParsingHandle || !pTable)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	m_pTranslationTable = pTable;
	m_RecordTypeIndex = RecordTypeIndex;
	m_ParseHandle = ParsingHandle;
	m_DLHandle = DLHandle;

#if (DAL_ENSURE_UNIQUENESS)
	m_pUniquenessMutex = pUniquenessMutex;
#endif

#ifndef MODULE_DIRECTORY_SERVICES /* Turn this feature off for MDS */
	m_ParserType = ParsingModuleServiceType;

	if ((RecordType == CSSM_DL_DB_RECORD_CERT || RecordType == CSSM_DL_DB_RECORD_CRL) &&
	   (ParsingModuleServiceType == CSSM_SERVICE_CL))
	{
		const ADDIN_LOAD_DATA *pLoadData = BioAPI_GetLoadData();
		if (pLoadData == NULL)
			return CSSMERR_DL_INTERNAL_ERROR;

		if (RecordType == CSSM_DL_DB_RECORD_CERT)
		{
			m_ParseTable.RecordGetFirstFieldValue = pLoadData->cl_CertGetFirstFieldValue;
			m_ParseTable.RecordGetNextFieldValue = pLoadData->cl_CertGetNextFieldValue;
			m_ParseTable.RecordAbortQuery = pLoadData->cl_CertAbortQuery;
		}
		else
		{
			ASSERT(RecordType == CSSM_DL_DB_RECORD_CRL);
			m_ParseTable.RecordGetFirstFieldValue = pLoadData->cl_CrlGetFirstFieldValue;
			m_ParseTable.RecordGetNextFieldValue = pLoadData->cl_CrlGetNextFieldValue;
			m_ParseTable.RecordAbortQuery = pLoadData->cl_CrlAbortQuery;
		}
	}
	else
#endif
	{
		/* If we didn't just assign the CL's funcs, they should be NULL */
		ASSERT(NULL == m_ParseTable.RecordGetFirstFieldValue);
		ASSERT(NULL == m_ParseTable.RecordGetNextFieldValue);
		ASSERT(NULL == m_ParseTable.RecordAbortQuery);
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::ConnectBackend
 *
 * Description:
 * This function will Initialize a record table object; it will get the handle to the
 * record parser from it's owner, and allocate any variables that it needs.
 *
 * Parameters:
 * rTableBackend (input) : Reference to the Table backend to be used by this instance
 * ParsingTablePtr (input/optional) : List of functions that this instance will use to parse
 * the data object
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::ConnectBackend(
	CSSM_DB_RECORD_PARSING_FNTABLE const * ParsingTablePtr,
	TABLE_BACKEND_REF rTableBackend)
{
	AssertValid();

	ASSERT(rTableBackend);

	m_rBackend = rTableBackend;

	if (ParsingTablePtr && ParsingTablePtr->RecordGetFirstFieldValue)
	{
		m_ParseTable = *ParsingTablePtr;
	}

	END_INITIALIZE();

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::RetrieveDataIndex
 *
 * Description:
 * Uses the Data object's parser to remove an index from the data object. The index
 *	 is then encoded in HashVal, StringVal, and Length
 *
 * Parameters:
 * ValuePtr (input/optional) : The value of the Data Pointer that contains the data to encode
 * IndexOIDPtr (input) : The OID that identifies the index; this value will be passed
 * to the parser to remove data.
 *
 * Return value:
 * NULL if an error occurred, otherwise a pointer to the index reteived from the data
 * onject.
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::RetrieveDataIndex(
	DAL_CONST_DATA_PTR ValuePtr,
	const CSSM_OID * IndexOIDPtr,
	CSSM_DATA_PTR *DataPtr) const
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	ASSERT(IndexOIDPtr && IndexOIDPtr->Length && IndexOIDPtr->Data);

	if (NULL == ValuePtr || 0 == ValuePtr->Length || NULL == ValuePtr->Data)
	{
		/* if the value is NULL, return an empty CSSM_DATA */
		*DataPtr = (CSSM_DATA_PTR)App_Malloc(m_DLHandle, sizeof(CSSM_DATA));

		if (NULL == *DataPtr)
			return CSSMERR_DL_MEMORY_ERROR;
		else
		{
			(*DataPtr)->Length = 0;
			(*DataPtr)->Data = NULL;
		}

		return CSSM_OK;
	}

	/* Call the data parser (usually the CL) */
	uint32 NumMatch;
	CSSM_HANDLE results;

	if (!m_ParseTable.RecordGetFirstFieldValue || !m_ParseTable.RecordAbortQuery)
	{
		*DataPtr = NULL;
		return CSSMERR_DL_INVALID_PARSING_MODULE;
	}

	if ((retValue = m_ParseTable.RecordGetFirstFieldValue(m_ParseHandle, (CSSM_DATA_PTR)ValuePtr,
			(CSSM_DATA_PTR)IndexOIDPtr, &results, &NumMatch, DataPtr)) != CSSM_OK)
	{
		/* We need to return an error if the function failed */

		if (retValue == CSSM_OK || retValue == CSSMERR_CSSM_INVALID_ADDIN_HANDLE
			|| retValue == CSSMERR_DL_FUNCTION_NOT_IMPLEMENTED || retValue == CSSMERR_CSSM_INVALID_HANDLE_USAGE)
		{
			/* if the CL did not return an error, or if the CSSM returned one of several errors while
			   brokering calls, then return CSSM_DL_INVALID_PARSING_FUNCTIONS, otherwise use
			   the CL's error, and the CL's guid */
			retValue = CSSMERR_DL_INVALID_PARSING_MODULE;
		}

		return retValue;
	}

	m_ParseTable.RecordAbortQuery (m_ParseHandle, results);
	retValue = CSSM_OK;

#if ATTRIBUTE_FORMAT_COMPLEX_SUPPORTED
	if ((*DataPtr)->Length == CSSM_FIELDVALUE_COMPLEX_DATA_TYPE)
	{
		return retValue;
	}
#endif

	if (port_IsBadReadPtr((*DataPtr)->Data, (*DataPtr)->Length) != CSSM_FALSE)
	{
		/* The data returned from the parsing module is invalid */
		ReleaseDataIndex(*DataPtr, IndexOIDPtr);
		retValue = CSSMERR_DL_INVALID_PARSING_MODULE;
	}

	return retValue;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::ReleaseDataIndex
 *
 * Description:
 * Use the app's memory functions to free the data object returned by RetrieveDataObject
 *
 * Parameters:
 * DataPtr (input) : The DataPtr returned by RetrieveDataObject
 * IndexOIDPtr (input) : Name of the index, was used to reteive the index
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::ReleaseDataIndex(CSSM_DATA_PTR DataPtr, const CSSM_OID * IndexOIDPtr) const
{
	AssertValid();
	ASSERT(DataPtr);
	ASSERT(IndexOIDPtr);

	if (DataPtr->Length == CSSM_FIELDVALUE_COMPLEX_DATA_TYPE)
	{
#ifndef MODULE_DIRECTORY_SERVICES
		/* The DL does not know how to free this object */
		const ADDIN_LOAD_DATA *pLoadData = BioAPI_GetLoadData();
		if (pLoadData == NULL)
			return CSSMERR_DL_INTERNAL_ERROR;

		if (m_ParserType == CSSM_SERVICE_CL)
		{
			/* The parsing module was the CL, so we can free it */
			ASSERT(port_IsBadCodePtr((CSSM_PROC_ADDR)pLoadData->cl_FreeFieldValue) == CSSM_FALSE);
			return pLoadData->cl_FreeFieldValue(m_ParseHandle, IndexOIDPtr, DataPtr);
		}
		else
		{
			/* The parsing module was not the CL, there is no way to free the data */
			return CSSMERR_DL_INVALID_PARSING_MODULE;
		}
#else
		return CSSMERR_DL_INVALID_PARSING_MODULE;
#endif
	}

	if (DataPtr->Data)
		App_Free(m_DLHandle, DataPtr->Data);
	App_Free(m_DLHandle, DataPtr);

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::SetAttributes
 *
 * Description:
 * Sets the value for the current record's attributes
 *
 * Parameters:
 * Attributes (input) : List of attribute to set the value for
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::SetAttributes(
	const DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST & Attributes) const
{
	AssertValid();

	VERIFY_PTR(m_rBackend);

	uint32 i;
	for (i = 0; i < Attributes.neGetNumberOfAttributes(); i++)
	{
		/* Skip attributes that do not need to be set */
		if (!Attributes.neGetAttribute(i).neWasInput()) continue;

		uint32 AttributeNum, IndexNum;
		CSSM_DB_ATTRIBUTE_FORMAT FieldFormat;
		DAL_CONST_DATA_PTR pValue;
		uint32 *pNumValue;

		/* get all the field's data and the field's info */
		AttributeNum = Attributes.neGetAttribute(i).neGetAttributeNumber();
		IndexNum = Attributes.neGetAttribute(i).neGetIndexNumber();
		FieldFormat = Attributes.neGetAttribute(i).neGetFieldFormat();
		pValue = Attributes.neGetAttribute(i).neGetValuePtr();
		pNumValue = Attributes.neGetAttribute(i).neGetNumValuePtr();
		ASSERT(pValue);

		CSSM_DATA PreparedValue;
		void *pFreeBuffer;

		/* Currently we support only 1 value for multivalue attributes. */
		*pNumValue = 1;

		CSSM_RETURN retCode = CSSM_OK;

		/* TODO - if the attribute format is complex, get the index's name */
		if (CSSM_OK != (retCode = PrepareField(FieldFormat, IndexNum, pValue->Length, pValue->Data,
			&pFreeBuffer, &PreparedValue)))
		{
			ASSERT(pFreeBuffer == NULL);
			return retCode;
		}

		/* Set the field */
		if (AttributeNum != DAL_OOB)
		{
			retCode = m_rBackend->SetAttribute(AttributeNum, FieldFormat, &PreparedValue);
		}
		else if (IndexNum != DAL_OOB)
		{
			retCode = m_rBackend->SetIndex(IndexNum, FieldFormat, &PreparedValue);
		}
		else
		{
			DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
			retCode = CSSMERR_DL_INTERNAL_ERROR;
		}

		if (pFreeBuffer != NULL)
			BioAPI_free(pFreeBuffer, NULL);


		if (retCode != CSSM_OK)
			return retCode;
	}
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::SetData
 *
 * Description:
 * Sets the value of the current record's data object
 *
 * Parameters:
 * DataPtr(input/optional) : new value for the data object
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::SetData(DAL_CONST_DATA_PTR DataPtr) const
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	VERIFY_PTR (m_rBackend);

	if (!DataPtr) return CSSM_OK;

	if ((retValue = m_rBackend->SetData(DataPtr)) != CSSM_OK)
	{
		return (retValue);
	}

	VERIFY_PTR(m_pTranslationTable);

	/* Now fix any indexes that need to be updated */
	uint32 Status = DAL_OOB;
	DAL_CONST_DATA_PTR IndexName;
	unsigned IndexNum;
	CSSM_DB_ATTRIBUTE_FORMAT IndexFormat;

	/* Get the all the ID's for indexes that come from the data object, use
		these ID's to encode the indexes */
	while (m_pTranslationTable->neGetCurrentDataInfo(Status, CSSM_FALSE, IndexNum, IndexName, IndexFormat)
		== CSSM_OK)
	{
		CSSM_DATA_PTR IndexValPtr;
		if ((retValue = RetrieveDataIndex(DataPtr, IndexName, &IndexValPtr)) != CSSM_OK)
			return retValue;

		void *pFreeBuff;
		CSSM_DATA PreparedIndexValue;
		if ((retValue = PrepareField(IndexFormat, IndexNum, IndexValPtr->Length, IndexValPtr->Data,
			&pFreeBuff, &PreparedIndexValue)) != CSSM_OK)
		{
			ReleaseDataIndex(IndexValPtr, IndexName);

			if (retValue == CSSMERR_DL_INVALID_VALUE)
			{
				/* since the field that we just tested is from the parsing functions, we need
				   to change the error code if we get CSSM_DL_INCOMPATIBLE_FIELD_FORMAT */
				retValue = CSSMERR_DL_INVALID_PARSING_MODULE;
			}

			ASSERT(pFreeBuff == NULL);
			return retValue;
		}

		CSSM_RETURN SetIndexRetCode;
		SetIndexRetCode = m_rBackend->SetIndex(IndexNum, IndexFormat, &PreparedIndexValue);

		if (pFreeBuff) BioAPI_free(pFreeBuff, NULL);
		CSSM_RETURN ReleaseDataRetCode;
		ReleaseDataRetCode = ReleaseDataIndex(IndexValPtr, IndexName);

		if (SetIndexRetCode != CSSM_OK)
			return SetIndexRetCode;

		if (ReleaseDataRetCode != CSSM_OK)
			return ReleaseDataRetCode;
	}

	return CSSM_OK;
}

#if (DAL_ENSURE_UNIQUENESS)
/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::IsModifyLegal
 *
 * Description:
 * This function checks to make sure that the modify will not create a condition
 * that will violate the unique record identifier (the key of the relation). This
 * would occur if an attempt was made to change the unique index to the same
 * value as the unique index for another record.
 *
 * Parameters:
 * DLHandle (input) : Handle to the DL module
 * pUniqueId (input) : Identifier to indicate the record in the record table that is
 * being modified
 * Attributes (input) : The attributes to be modified, if the attribute will
 *	 not be modified, it's value will be obtained from the database
 * DataPtr (input/optional) : The new data object, if null, the data object from the database
 *	 will be obtained.
 *
 * Return value:
 * CSSM_FALSE if the modify is illegal or an error occurred (will always set an error)
 * CSSM_TRUE if the modify is legal (will never set an error)
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::IsModifyLegal(
	CSSM_DL_HANDLE DLHandle,
	CSSM_DB_UNIQUE_RECORD const * pUniqueId,
	DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST Attributes, /* Using Copy constructor */
	DAL_CONST_DATA_PTR DataPtr)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	ASSERT(DLHandle);
	ASSERT(pUniqueId);
	ASSERT(m_rBackend);
	ASSERT(m_pTranslationTable);

	/* Make sure there was not a memory error in the copy constructor */
	if (!Attributes.neIsInitialized())
	{
		/* The copy constructor would have called Initialize, and initialize failed
		   so return the only possible error in Initialize */
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		retValue = CSSMERR_DL_MEMORY_ERROR;
		return retValue;
	}

	/* Move to the record we are modifing to get data from it */
	if ((retValue = m_rBackend->MoveToRecord(pUniqueId)) != CSSM_OK)
		return retValue;

	/* if m_pUniquenessMutex == NULL, this indicates that there were no unique indexes on this relation */
	if (m_pUniquenessMutex == NULL)
	{
		ASSERT(m_pTranslationTable->neGetUniqueIndexCount() == 0);
		return CSSM_OK;
	}

	uint32 i;
	uint32 NumTempAttributes = 0;
	/* Allocate the needed temporary memory */
	CSSM_DATA_PTR rgTempAttributeData = new CSSM_DATA[Attributes.neGetNumberOfAttributes()];
	if (NULL == rgTempAttributeData)
		return CSSMERR_DL_MEMORY_ERROR;

	/* Get a copy of the data pointer if it is not already defined */
	CSSM_DATA ExistingData = {0, NULL};
	DAL_CONST_DATA_PTR NewDataPtr = NULL;
	if (DataPtr == NULL)
	{
		/* DataPtr is NULL, so use the data from the current record, make a copy of it
		   note: this uses the app's memory functions */
		if ((retValue = m_rBackend->GetData(DLHandle, &ExistingData)) != CSSM_OK)
		{
			delete []rgTempAttributeData;
			return retValue;
		}

		NewDataPtr = &ExistingData;
	}
	else
		NewDataPtr = DataPtr;

	/* Loop through the list of all attributes in the schema, reteiving a value for those attributes
	   which are part of the unique index and were not specified in the list sent to DataModify. */
	for (i = 0; i < Attributes.neGetNumberOfAttributes(); i++)
	{
		/* Skip attributes that are not unique */
		if (!Attributes.neGetAttribute(i).neIsUnique()) continue;

		/* Skip attributes where a value was specified in the list passed to data modify */
		if (Attributes.neGetAttribute(i).neWasInput()) continue;

		/* Using the old value for this attribute: retreive it from the database */
		CSSM_DATA_PTR pRetrievedIndexVal = &rgTempAttributeData[NumTempAttributes];
		uint32 NumberOfValues = 1;
		NumTempAttributes++;

		/* We only care about Unique indexes, so it must be an index */
		uint32 IndexNum = Attributes.neGetAttribute(i).neGetIndexNumber();
		ASSERT(IndexNum != DAL_OOB); /* IndexNum is DAL_OOB if the attribute is not an index */

		/* retreive the index with the app's memory functions */
		if ((retValue = m_rBackend->GetIndex(DLHandle, IndexNum, pRetrievedIndexVal)) != CSSM_OK)
			break;

		/* Store the value in the attributes structure */
		if (CSSM_OK != Attributes.neGetAttribute(i).neInitializeValue(&pRetrievedIndexVal, &NumberOfValues))
		{
			DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
			retValue = CSSMERR_DL_INTERNAL_ERROR;
		}
	}

	if (retValue == CSSM_OK)
	{
		/* Last, see if this new attribute list and the data pointer will find a record with
		   a Unique record id that does not equal the current unique record Id */

		CSSM_BOOL NoRecordsFound;
		retValue = FindRecordWithUniqueIndexes(Attributes, NewDataPtr, NoRecordsFound);
		if (CSSM_FALSE == NoRecordsFound)
		{
			/* Records are found, make sure that the record we found is the current record */
			CSSM_BOOL AreEqual;
			retValue = m_rBackend->IsEqualCurrentUniqueRecordId(pUniqueId, &AreEqual);
			if ((CSSM_OK == retValue) && (CSSM_FALSE == AreEqual))
			{
				/* We found a record that has the same unique index, but is not the current record */
				retValue = CSSMERR_DL_INVALID_UNIQUE_INDEX_DATA;
			}
			/* else, we have a legal modify, and no error occurred; we are on the proper record */
		}
		/* No records were found, so just move to the record to modify*/
		else
			retValue = m_rBackend->MoveToRecord(pUniqueId);
		/* else, we have a legal modify, and no error occurred; we are on the proper record */
	}

/* Begin clean up */
	/* Clean up rgTempAttributeData */
	for (i = 0; i < NumTempAttributes; i++)
	{
		if (rgTempAttributeData[i].Data)
		{
			App_Free(DLHandle, rgTempAttributeData[i].Data);
			rgTempAttributeData[i].Data = NULL;
		}
	}

	delete []rgTempAttributeData;
	if (ExistingData.Data)
	{
		ASSERT(ExistingData.Length);
		App_Free(DLHandle, ExistingData.Data);
	}


	return retValue;
}
#endif

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::GetAttributes
 *
 * Description:
 * Returns the value of an attribute from the datastore to return to the app
 *
 * Parameters:
 * DlHandle (input) : Handle to the DL instance calling this function
 * Attributes (input/output) : Describes
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * Todo
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::GetAttributes(
		CSSM_HANDLE DlHandle,
		const DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST & Attributes) const
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	VERIFY_PTR(m_rBackend);

	uint32 i;
	for (i = 0; i < Attributes.neGetNumberOfAttributes(); i++)
	{
		/* Skip attributes that do not need to be returned */
		if (!Attributes.neGetAttribute(i).neWasRequested()) continue;

		CSSM_DATA_PTR pValue;
		if ((retValue = Attributes.neGetAttribute(i).neGetWriteableValuePtr(DlHandle, &pValue)) != CSSM_OK)
			return retValue;

		uint32 AttributeNum = Attributes.neGetAttribute(i).neGetAttributeNumber();
		uint32 IndexNum = Attributes.neGetAttribute(i).neGetIndexNumber();

		ASSERT(AttributeNum == DAL_OOB || IndexNum == DAL_OOB); /* Check the validity of the return */

		if (AttributeNum != DAL_OOB) /* If the Attribute is not used as an index */
		{
			if ((retValue = m_rBackend->GetAttribute(DlHandle, AttributeNum, pValue)) != CSSM_OK)
				return retValue;
		}
		else if (IndexNum != DAL_OOB) /* If the Attribute is used as an index */
		{
			if ((retValue = m_rBackend->GetIndex(DlHandle, IndexNum, pValue)) != CSSM_OK)
				return retValue;
		}
		else
		{
			DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
			return CSSMERR_DL_INTERNAL_ERROR;
		}
	}
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::TranslateAttributes
 *
 * Description:
 * This function is mearly a wrapper arround the Index Translation Table function;
 * see that function for more information
 *
 * Parameters:
 * Source (input) : The DAL_TRANSLATED_RECORD_ATTRIBUTE_DATA structure that contains the info
 *	 to be translated
 * Destination (output): The translated data
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Note: Source == NULL is not a fail condition
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::TranslateAttributes(
	CSSM_DB_RECORD_ATTRIBUTE_DATA * Source,
	DAL_TRANSLATED_ATTRIBUTE_LIST &Destination) const
{
	AssertValid();

	ASSERT(m_pTranslationTable);
	if (!m_pTranslationTable)
		return CSSMERR_DL_INTERNAL_ERROR;

   return m_pTranslationTable->TranslateAttributes(Source, Destination);
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::AddSelectionPredicate
 *
 * Description:
 * Adds a predicate to a query
 *
 * Parameters:
 * beQueryBuffer (input/output) : Query to add the predicate to
 * DbOperator (input) : Operator used between the index and the predicate
 * FieldFormat (input) : Field format of the predicate/index
 * IndexNum (input) : Index that the pedicate is compared to
 * pValue (input/optional) : Value of the predicate, can be NULL
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::AddSelectionPredicate(
	void *beQueryBuffer,
	CSSM_DB_OPERATOR DbOperator,
	CSSM_DB_ATTRIBUTE_FORMAT FieldFormat,
	uint32 IndexNum,
	DAL_CONST_DATA_PTR pValue) const
{
	CSSM_RETURN retValue = CSSM_OK;
	CSSM_DATA PreparedData = {0, NULL};
	void *pBuffer = NULL;

	if (pValue != NULL)
	{
		if ((retValue = PrepareField(FieldFormat, IndexNum, pValue->Length, pValue->Data, &pBuffer,
			&PreparedData)) != CSSM_OK)
		{
			ASSERT(pBuffer == NULL);
			m_rBackend->nrDeleteQuery(beQueryBuffer);
			return retValue;
		}

		pValue = &PreparedData;
	}

	retValue = m_rBackend->AddSelectionPredicate(beQueryBuffer, DbOperator, FieldFormat, IndexNum,
		pValue);

	if (pBuffer)
	{
		BioAPI_free(pBuffer, NULL);
		pBuffer = NULL;
	}

	if (retValue != CSSM_OK)
	{
		m_rBackend->nrDeleteQuery(beQueryBuffer);
		return retValue;
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::FindRecords
 *
 * Description:
 * Finds any records that match the query outlined in QueryPtr; returns the number
 * of elements that are found.
 *
 * Parameters:
 * QueryPtr(input) : Describes the query to be preformed
 * FindNumber (input) : This number is the results handle returned by a query; it distinguishes
 *	  one query from another. Only the last query is vallid.
 * unsigned &NumElements)
 *
 * Return value:
 * CSSM_OK if the query was completed without error, otherwise CSSM_FAIL
 *
 * Note: if no records are found, CSSM_FAIL will be returned
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::FindRecords(
		CSSM_QUERY const * QueryPtr,
		CSSM_HANDLE_PTR ResultsHandle)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	ASSERT(QueryPtr);
	ASSERT(ResultsHandle);
	VERIFY_PTR(m_pTranslationTable);
	VERIFY_PTR(m_rBackend);

    *ResultsHandle = CSSM_INVALID_HANDLE;

	void *beQueryBuffer;
	if ((retValue = m_rBackend->NewQuery(&beQueryBuffer)) != CSSM_OK)
		return retValue;

	if (QueryPtr == NULL)
		return CSSMERR_DL_INVALID_POINTER;

	if ((retValue = m_rBackend->SetQueryConjunctive(beQueryBuffer, QueryPtr->Conjunctive)) != CSSM_OK)
	{
		m_rBackend->nrDeleteQuery(beQueryBuffer);
		return retValue;
	}

	unsigned i;
	for (i = 0; i < QueryPtr->NumSelectionPredicates; i++)
	{
		CSSM_SELECTION_PREDICATE_PTR spPtr = &QueryPtr->SelectionPredicate[i];
		CSSM_DB_INDEXED_DATA_LOCATION Junk; /* will not be used */
		unsigned IndexNum;

		if ((retValue = m_pTranslationTable->LookupIndex(&spPtr->Attribute.Info, IndexNum, Junk)) != CSSM_OK)
		{
			m_rBackend->nrDeleteQuery(beQueryBuffer);
			return retValue;
		}

		if ((retValue = AddSelectionPredicate(beQueryBuffer, spPtr->DbOperator,
			spPtr->Attribute.Info.AttributeFormat, IndexNum, spPtr->Attribute.Value)) != CSSM_OK)
		{
			return retValue;
		}
	}

	DAL_QUERY_BUFFER * pQueryBuffer = new DAL_QUERY_BUFFER;
	if (!pQueryBuffer)
	{
		m_rBackend->nrDeleteQuery(beQueryBuffer);
		return CSSMERR_DL_MEMORY_ERROR;
	}

	if ((retValue = m_rBackend->ExecuteQuery(beQueryBuffer, QueryPtr->QueryLimits, QueryPtr->QueryFlags,
			pQueryBuffer->RecordTable, pQueryBuffer->dwSemantics, pQueryBuffer->Uid,
			pQueryBuffer->NumberOfRecords, pQueryBuffer->NumberOfAttributes)) != CSSM_OK)
	{
		m_rBackend->nrDeleteQuery(beQueryBuffer);
		delete pQueryBuffer;
		return retValue;
	}

	do
	{
		pQueryBuffer->nrSetResultsHandle(m_RecordTypeIndex, m_QueryCount++);
		*ResultsHandle = pQueryBuffer->neGetResultsHandle();
	}
	while ( *ResultsHandle == 0 );

	pQueryBuffer->QueryLimits = QueryPtr->QueryLimits;
	pQueryBuffer->pBackendQueryBuffer = beQueryBuffer;
	pQueryBuffer->CurrentRecord = 0;
	pQueryBuffer->pNext = m_pQueryRoot;
	m_pQueryRoot = pQueryBuffer;

	/*
	 * Setup the first record.
	 */
	m_rBackend->SetQueryData(pQueryBuffer->RecordTable[pQueryBuffer->CurrentRecord],
											pQueryBuffer->dwSemantics[pQueryBuffer->CurrentRecord],
											pQueryBuffer->Uid, pQueryBuffer->CurrentRecord);
	pQueryBuffer->CurrentRecord++;

	return CSSM_OK;
}

CSSM_RETURN DAL_RECORD_TABLE::ContinueQuery(
	CSSM_HANDLE ResultsHandle) const
{
	DAL_QUERY_BUFFER * pTempBuffer = m_pQueryRoot;

	while (pTempBuffer && pTempBuffer->neGetResultsHandle() > ResultsHandle)
	{
		pTempBuffer = pTempBuffer->pNext;
	}

	if (pTempBuffer && pTempBuffer->neGetResultsHandle() == ResultsHandle)
	{
		if (pTempBuffer->QueryLimits.SizeLimit != CSSM_QUERY_SIZELIMIT_NONE)
		{
			if (0 == --(pTempBuffer->QueryLimits.SizeLimit))
			{
				return CSSMERR_DL_ENDOFDATA;
			}
		}

		if (pTempBuffer->CurrentRecord == pTempBuffer->NumberOfRecords)
		{
			return CSSMERR_DL_ENDOFDATA;
		} else
		{
			m_rBackend->SetQueryData(pTempBuffer->RecordTable[pTempBuffer->CurrentRecord],
										pTempBuffer->dwSemantics[pTempBuffer->CurrentRecord],
										pTempBuffer->Uid, pTempBuffer->CurrentRecord);

			pTempBuffer->CurrentRecord++;
		}

		return CSSM_OK;
	}
	else
	{
		return CSSMERR_DL_INVALID_RESULTS_HANDLE;
	}
}


CSSM_RETURN DAL_RECORD_TABLE::DeleteQuery(CSSM_HANDLE ResultsHandle)
{
	uint32 i;
	DAL_QUERY_BUFFER * pPrev = NULL, * pTempBuffer = m_pQueryRoot;

	while (pTempBuffer && pTempBuffer->neGetResultsHandle() > ResultsHandle)
	{
		pPrev = pTempBuffer;
		pTempBuffer = pTempBuffer->pNext;
	}

	if (pTempBuffer && pTempBuffer->neGetResultsHandle() == ResultsHandle)
	{
		void *beQueryBuffer = pTempBuffer->pBackendQueryBuffer;

		if (pPrev == NULL)
			m_pQueryRoot = pTempBuffer->pNext;
		else
			pPrev->pNext = pTempBuffer->pNext;

		for (i = 0; i < pTempBuffer->NumberOfRecords; i++) {
			if (i != pTempBuffer->CurrentRecord - 1)
				dal_FreeData(pTempBuffer->RecordTable[i], pTempBuffer->NumberOfAttributes);
			else
				BioAPI_free (pTempBuffer->RecordTable[i], NULL);
		}
		BioAPI_free (pTempBuffer->RecordTable, NULL);
		BioAPI_free (pTempBuffer->dwSemantics, NULL);
		BioAPI_free (pTempBuffer->Uid, NULL);
		delete pTempBuffer;
		m_rBackend->nrDeleteQuery(beQueryBuffer);
		return CSSM_OK;
	}
	else
	{
		return CSSMERR_DL_INVALID_RESULTS_HANDLE;
	}
}

#if DAL_ENSURE_UNIQUENESS
/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::FindRecordWithUniqueIndexes
 *
 * Description:
 * Build a query containing the values for all indexes that were flaged as unique
 * in the relation's schema. This query is then used to determine if there are any
 * other records that have the same unique index. If a record is found, that record
 * becomes the current record. In the case of DataInsert, if a record is found, that
 * record will be modified with the Attributes and Data object that was specified in
 * the Data Insert call, otherwise, insert proceeds as normal.
 *
 * Parameters:
 * AttributeList (input) : The attributes that the find will use
 * DataPtr (input) : The Data object of the record
 * NoRecordsFound (output) : Set to true if no records were found
 *
 * Return value:
 * CSSM_OK if no errors occurr (find a record is not an error), otherwise CSSM_FAIL
 *
 * Note: NULL is treated at a zero length field, and is an expectable value for a
 * field in the unique index.
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::FindRecordWithUniqueIndexes(
	const DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST & AttributeList,
	DAL_CONST_DATA_PTR DataPtr,
	CSSM_BOOL &NoRecordsFound)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	VERIFY_PTR(m_rBackend);
	VERIFY_PTR(m_pTranslationTable);

	/* if m_pUniquenessMutex == NULL, this indicates that there were no unique indexes on this relation */
	if (m_pUniquenessMutex == NULL)
	{
		ASSERT(m_pTranslationTable->neGetUniqueIndexCount() == 0);
		NoRecordsFound = CSSM_TRUE;
		return CSSM_OK;
	}

	NoRecordsFound = CSSM_FALSE;
	void *beQueryBuffer;
	if ((retValue = m_rBackend->NewQuery(&beQueryBuffer)) != CSSM_OK)
		return retValue;

	if ((retValue = m_rBackend->SetQueryConjunctive(beQueryBuffer, CSSM_DB_AND)) != CSSM_OK)
	{
		m_rBackend->nrDeleteQuery(beQueryBuffer);
		return retValue;
	}

	uint32 Status = DAL_OOB;
	uint32 IndexNum;
	DAL_CONST_DATA_PTR IndexName;
	CSSM_BOOL QueryEmpty = CSSM_TRUE;
	CSSM_DB_ATTRIBUTE_FORMAT IndexFormat;

	/* Get the value for all unique indexes that are from the data object, add them to the query */
	while ((retValue = m_pTranslationTable->neGetCurrentDataInfo(Status, CSSM_TRUE, IndexNum, IndexName, IndexFormat)) == CSSM_OK)
	{
		CSSM_DATA_PTR pDataIndex;
		if ((retValue = RetrieveDataIndex(DataPtr, IndexName, &pDataIndex)) != CSSM_OK)
		{
			m_rBackend->nrDeleteQuery(beQueryBuffer);
			return retValue;
		}

		if ((retValue = AddSelectionPredicate(beQueryBuffer, CSSM_DB_EQUAL, IndexFormat, IndexNum,
			pDataIndex)) != CSSM_OK)
		{
			ReleaseDataIndex(pDataIndex, IndexName);

			if (retValue == CSSMERR_DL_INCOMPATIBLE_FIELD_FORMAT)
			{
				/* since the field that we just tested is from the parsing functions, we need
				   to change the error code if we get CSSM_DL_INCOMPATIBLE_FIELD_FORMAT */
				retValue = CSSMERR_DL_INVALID_PARSING_MODULE;
			}

			return CSSMERR_DL_INTERNAL_ERROR;
		}

		if ((retValue = ReleaseDataIndex(pDataIndex, IndexName)) != CSSM_OK)
		{
			m_rBackend->nrDeleteQuery(beQueryBuffer);
			return retValue;
		}

		/* ignore the return since it will be picked up by the next call to GetCurrentDataInfo */
		QueryEmpty = CSSM_FALSE;
	}

	uint32 i;
	/* Get the value for all unique indexes that are from the data object, add them to the query */
	for (i = 0; i < AttributeList.neGetNumberOfAttributes(); i++)
	{
		/* Skip non-unique indexes */
		if (!AttributeList.neGetAttribute(i).neIsUnique()) continue;

		CSSM_DB_ATTRIBUTE_FORMAT FieldFormat;
		IndexNum = AttributeList.neGetAttribute(i).neGetIndexNumber();
		DAL_CONST_DATA_PTR pAttributeIndex = AttributeList.neGetAttribute(i).neGetValuePtr();
		FieldFormat = AttributeList.neGetAttribute(i).neGetFieldFormat();

		if ((retValue = AddSelectionPredicate(beQueryBuffer, CSSM_DB_EQUAL, FieldFormat, IndexNum,
			pAttributeIndex)) != CSSM_OK)
		{
			return retValue;
		}

		QueryEmpty = CSSM_FALSE;
	}

	ASSERT(QueryEmpty == CSSM_FALSE);

	if (QueryEmpty == CSSM_FALSE)
	{
		/* Now perform the query */
		CSSM_RETURN res;
		CSSM_QUERY_LIMITS QueryLimits;
		QueryLimits.TimeLimit = CSSM_QUERY_TIMELIMIT_NONE;
		/* Limit the size to one record, since we can only find one record w/ a particular
		   UNIQUE index, so theirfor we can stop looking after 1 is found */
		QueryLimits.SizeLimit = 1;

		/* Lock the uniqueness mutex, this is released with ReleaseRecordSet */
		ASSERT(m_pUniquenessMutex != NULL);
		if ((retValue = port_LockMutex(m_pUniquenessMutex, LOCK_TIMEOUT_PERIOD)) != CSSM_OK)
			return retValue;

		m_fIsUniquenessMutexLocked = CSSM_TRUE;

		DAL_QUERY_BUFFER * pQueryBuffer = new DAL_QUERY_BUFFER;
		if (!pQueryBuffer)
		{
			m_rBackend->nrDeleteQuery(beQueryBuffer);
			return CSSMERR_DL_MEMORY_ERROR;
		}
		pQueryBuffer->CurrentRecord = 0;

		res = m_rBackend->ExecuteQuery(beQueryBuffer, QueryLimits, 0,
					pQueryBuffer->RecordTable, pQueryBuffer->dwSemantics, pQueryBuffer->Uid,
					pQueryBuffer->NumberOfRecords, pQueryBuffer->NumberOfAttributes);

		if (CSSMERR_DL_ENDOFDATA == res)
			NoRecordsFound = CSSM_TRUE;

		if (NoRecordsFound == CSSM_FALSE) {
			/*
			 * Setup the first record.
			 */
			m_rBackend->SetQueryData(pQueryBuffer->RecordTable[pQueryBuffer->CurrentRecord],
										pQueryBuffer->dwSemantics[pQueryBuffer->CurrentRecord],
										pQueryBuffer->Uid, pQueryBuffer->CurrentRecord);
			pQueryBuffer->CurrentRecord++;
		}

		for (i = 0; i < pQueryBuffer->NumberOfRecords; i++) {
			if (i != pQueryBuffer->CurrentRecord - 1)
				dal_FreeData(pQueryBuffer->RecordTable[i], pQueryBuffer->NumberOfAttributes);
			else
				BioAPI_free (pQueryBuffer->RecordTable[i], NULL);
		}
		BioAPI_free (pQueryBuffer->RecordTable, NULL);
		if( pQueryBuffer->dwSemantics )
			BioAPI_free (pQueryBuffer->dwSemantics, NULL);
		if(pQueryBuffer->Uid)
			BioAPI_free (pQueryBuffer->Uid, NULL);
		delete pQueryBuffer;
		m_rBackend->nrDeleteQuery(beQueryBuffer);
		pQueryBuffer = NULL;

		if ((res != CSSMERR_DL_ENDOFDATA) && (res != CSSM_OK))
		{
			m_fIsUniquenessMutexLocked = CSSM_FALSE;
			port_UnlockMutex(m_pUniquenessMutex);
			return CSSMERR_DL_FUNCTION_FAILED;
		}
	}
	else
	{
		/* There are no unique indexes in the schema, however, this should have been picked up
		   earlier */
		m_rBackend->nrDeleteQuery(beQueryBuffer);
		NoRecordsFound = CSSM_TRUE;
	}

	return CSSM_OK;
}
#endif

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::nrSetParsingFunctions
 *
 * Description:
 * Set the parsing functions for an instance of DAL_RECORD_TABLE
 *
 * Parameters:
 * FunctionTable (input) : New set of parsing functions for the relation
 *
 * Return value:
 * None.
 *
 * Note: This function can not be executed while other operations are occurring
 * on the current instance of DAL_RECORD_TABLE (this).
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
void DAL_RECORD_TABLE::nrSetParsingFunctions(CSSM_DB_RECORD_PARSING_FNTABLE const * FunctionTable)
{
	AssertValid();

	ASSERT(FunctionTable);
	if (!FunctionTable) return;

	m_ParseTable = *FunctionTable;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::UpdateRecord
 *
 * Description:
 * Commit changes made to the record
 *
 * Parameters:
 * None.
 *
 * Return value:
 * CSSM_OK if no error occurred, otherwise the error
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_RECORD_TABLE::UpdateRecord()
{
	CSSM_RETURN retValue;
	VERIFY_PTR(m_rBackend);
	retValue = m_rBackend->UpdateRecord();

#if (DAL_ENSURE_UNIQUENESS)
	/* unlock the uniqueness mutex if it was locked, it should only be unlocked if
	   there are no unique indexes in this relation */
	ASSERT(m_fIsUniquenessMutexLocked != CSSM_FALSE || m_pUniquenessMutex == NULL);
	if (m_fIsUniquenessMutexLocked != CSSM_FALSE)
	{
		port_UnlockMutex(m_pUniquenessMutex);
		m_fIsUniquenessMutexLocked = CSSM_FALSE;
	}
#endif

	return retValue;
}


/*-----------------------------------------------------------------------------
 * Name: DAL_RECORD_TABLE::ReleaseRecordTable
 *
 * Description:
 * Indicates that a record table instance that the thread is releasing it. This
 * call occurrs when a DAL_RECORD_TABLE_REF goes out of scope. It is used to release
 * the uniqueness mutex
 *
 * Parameters:
 * None.
 *
 * Return value:
 * None.
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
void DAL_RECORD_TABLE::ReleaseRecordTable()
{
#if (DAL_ENSURE_UNIQUENESS)
	if (m_fIsUniquenessMutexLocked != CSSM_FALSE)
	{
		port_UnlockMutex(m_pUniquenessMutex);
		m_fIsUniquenessMutexLocked = CSSM_FALSE;
	}
#endif
}

#ifdef _DEBUG

void DAL_RECORD_TABLE::AssertValidNotInitialized(void) const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_RECORD_TABLE)));
	ASSERT(m_ParseHandle == NULL);
	ASSERT(m_ParseTable.RecordGetFirstFieldValue == NULL);
	ASSERT(m_ParseTable.RecordGetNextFieldValue == NULL);
	ASSERT(m_ParseTable.RecordAbortQuery == NULL);
	ASSERT(m_rBackend == NULL);
	ASSERT(m_pTranslationTable == NULL);
	ASSERT(m_RecordTypeIndex == DAL_OOB);
}

void DAL_RECORD_TABLE::AssertValid(void) const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_RECORD_TABLE)));

	if (m_debug_ObjectState == Constructing || m_debug_ObjectState == Initializing)
		return;
	if (m_debug_ObjectState == NotInitialized)
	{
		AssertValidNotInitialized();
		return;
	}

	ASSERT(m_RecordTypeIndex != DAL_OOB);

	if (m_ParseTable.RecordGetFirstFieldValue) {
		ASSERT(!port_IsBadCodePtr((CSSM_PROC_ADDR)m_ParseTable.RecordGetFirstFieldValue));
		ASSERT(!port_IsBadCodePtr((CSSM_PROC_ADDR)m_ParseTable.RecordAbortQuery));
		if (m_ParseTable.RecordGetNextFieldValue) {
			ASSERT(!port_IsBadCodePtr((CSSM_PROC_ADDR)m_ParseTable.RecordGetNextFieldValue));
		}
	}
	else
	{
		ASSERT(!m_ParseTable.RecordAbortQuery);
		ASSERT(!m_ParseTable.RecordGetNextFieldValue);
	}

	ASSERT(m_ParseHandle);

	ASSERT(m_rBackend);
	ASSERT_VALID_HEAP_POINTER(m_rBackend, 1);
	ASSERT(m_pTranslationTable);
}

#endif /* End debug code */
