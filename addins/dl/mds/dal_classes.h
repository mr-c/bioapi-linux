/*-----------------------------------------------------------------------
 * File: DAL_CLASSES.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file declares class prototypes interal to the DAL.
 */

#ifndef DAL_CLASSES_H
#define DAL_CLASSES_H

#ifdef DAL_SOURCE_FILE

#include "bioapi_type.h"
#include "bioapi_err.h"
#include "port/bioapi_port.h"
#include "port/bioapi_lock.h"
#include "dal_common.h"
#include "dal_translation.h"
#include "dal_dlbe.h"
#include "dal_typearray.h"

/*	CSSM_DB_RECORD_PARSING_FUNCTION_TABLE  */
typedef struct cssm_db_record_parsing_fntable {

	CSSM_RETURN (CSSMAPI *RecordGetFirstFieldValue)
									(CSSM_HANDLE Handle,
									const CSSM_DATA *Data,
									const CSSM_OID *DataField,
									CSSM_HANDLE_PTR ResultsHandle,
									uint32 *NumberOfMatchedFields,
									CSSM_DATA_PTR *DataPtr);
	CSSM_RETURN (CSSMAPI *RecordGetNextFieldValue)
									(CSSM_HANDLE Handle,
									CSSM_HANDLE ResultsHandle,
									CSSM_DATA_PTR *DataPtr);
	CSSM_RETURN (CSSMAPI *RecordAbortQuery)
									(CSSM_HANDLE Handle,
									CSSM_HANDLE ResultsHandle);
} CSSM_DB_RECORD_PARSING_FNTABLE, *CSSM_DB_RECORD_PARSING_FNTABLE_PTR;


class DAL_QUERY_BUFFER
{
private:
	CSSM_HANDLE ResultsHandle;

public:
	CSSM_DATA_PTR *RecordTable;
	unsigned *dwSemantics; /* Semantics */
	void *Uid;			/* Unique identifier */
	uint32 NumberOfAttributes;
	uint32 NumberOfRecords;
	uint32 CurrentRecord;

	void *pBackendQueryBuffer;
	CSSM_QUERY_LIMITS QueryLimits;
	DAL_QUERY_BUFFER * pNext;

	DAL_QUERY_BUFFER()
	{
		ResultsHandle = DAL_OOB; /* This value is not actually invalid, but it would require
			2^16 - 1 relations in the database w/ 2^16-1 searches */
		pNext = NULL;
	}

	CSSM_HANDLE neGetResultsHandle() const
	{
		/* We don't need this assert. The caller of the nrSetResultsHandle
		 * method must compare this result to 0 and call nrSetResultsHandle
		 * again with an updated QueryNumber value to eliminate zero handles.
		ASSERT(this && ResultsHandle != 0);
		 */
		ASSERT(ResultsHandle != DAL_OOB);
		return ResultsHandle;
	}

	void nrSetResultsHandle(uint32 RecordTypeIndex, uint32 QueryNumber)
	{
		/* We don't need this assert, as Query number will just roll over
		 * after 2^16 queries.
		ASSERT(QueryNumber < (1 << 16));
		*/
		ASSERT(RecordTypeIndex < (1 << 16));

		ResultsHandle = (QueryNumber << 16) + RecordTypeIndex;
	}
};

/*-----------------------------------------------------------------------------------
 * Class Name: DAL_RECORD_TABLE
 *
 * Description:
 *	This class is a container for a Table (or relation) in the Database. There is one
 * instance of this class per Recordtype. It has one record in the table that is
 * currently selected. There are methods to modify, or receive information about
 * this record, or to change the currently selected record.
 *-----------------------------------------------------------------------------------*/

typedef uint32 URID; /* Unique Record Index Data */

class DAL_RECORD_TABLE
{
private:
	TABLE_BACKEND_REF m_rBackend;					/* Table Backend class instance */
	DAL_CONST_TRANSLATION_TABLE_PTR m_pTranslationTable;  /* Translation Table */
	CSSM_HANDLE m_ParseHandle;					 /* Handle to record parser */
	CSSM_DB_RECORD_PARSING_FNTABLE m_ParseTable;	/* Table of record parsing functions */
	CSSM_DL_HANDLE m_DLHandle;

#ifndef MODULE_DIRECTORY_SERVICES /* Turn this feature off for MDS */
	CSSM_SERVICE_TYPE m_ParserType;
#endif

#if (DAL_ENSURE_UNIQUENESS)
    PORT_MUTEX_HANDLE m_pUniquenessMutex; /* Mutex used to ensure that multiple inserts/modifies that happen
	   at the same time do not produce duplicate records, will be NULL if no unique index */
	CSSM_BOOL m_fIsUniquenessMutexLocked;
#endif

	DAL_QUERY_BUFFER * m_pQueryRoot;
	uint32 m_QueryCount; /* Number of Queries that have been executed */
	uint32 m_RecordTypeIndex;

	CSSM_RETURN PrepareField(
		CSSM_DB_ATTRIBUTE_FORMAT FieldFormat,
		uint32 IndexNum,
		const uint32 FieldLength,
		const uint8 *FieldData,
		void **ppBuffer,
		CSSM_DATA *pNewData) const;

#if (ATTRIBUTE_FORMAT_COMPLEX_SUPPORTED)
	CSSM_RETURN PrepareComplexField(
		uint32 IndexNum,
		const uint32 FieldLength,
		const uint8 *FieldData,
		void **ppBuffer,
		CSSM_DATA *pNewData) const;
#endif

	CSSM_RETURN AddSelectionPredicate(
		void *beQueryBuffer,
		CSSM_DB_OPERATOR DbOperator,
		CSSM_DB_ATTRIBUTE_FORMAT FieldFormat,
		uint32 IndexNum,
		DAL_CONST_DATA_PTR pValue) const;
public:

#ifdef _DEBUG /* Debug functions to check the validity of the system and the object */
	DAL_OBJECT_STATE m_debug_ObjectState;
	void AssertValidNotInitialized(void) const; /* Initialize has NOT been called */
	void AssertValid(void) const; /* Initialize has been called */
#endif

	/* Initialization and deconstruction */
	DAL_RECORD_TABLE(void);
	~DAL_RECORD_TABLE();

	CSSM_RETURN Initialize(
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
	);

	CSSM_RETURN ConnectBackend(
		CSSM_DB_RECORD_PARSING_FNTABLE const * ParsingTablePtr,
		TABLE_BACKEND_REF rTableBackend);

	/* For Find First, Find Next, and AbortQuery */
	CSSM_RETURN FindRecords(
		CSSM_QUERY const * QueryPtr,
		CSSM_HANDLE_PTR ResultsHandle);
	CSSM_RETURN ContinueQuery(CSSM_HANDLE ResultsHandle) const;
	CSSM_RETURN DeleteQuery(CSSM_HANDLE ResultsHandle);

#if (DAL_ENSURE_UNIQUENESS)
	/* Functions to insure that Unique indexes stay unique */
	CSSM_RETURN FindRecordWithUniqueIndexes(
		const DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST &,
		DAL_CONST_DATA_PTR, CSSM_BOOL &NoRecordsFound);
	CSSM_RETURN IsModifyLegal(
		CSSM_DL_HANDLE DLHandle,
		CSSM_DB_UNIQUE_RECORD const * pUniqueId,
		DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST,
		DAL_CONST_DATA_PTR DataPtr);
#endif

	/* Functions to modify Attributes, Semantics and the Opaque Data Object */
	CSSM_RETURN SetAttributes(const DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST &) const;
	CSSM_RETURN SetData(DAL_CONST_DATA_PTR) const;

	/* Functions to get data back from the datastore to return to the app */
	CSSM_RETURN GetAttributes(
		CSSM_HANDLE DlHandle,
		const DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST &) const;

	/* Wrapper arround DAL_TRANSLATION_TABLE function */
	CSSM_RETURN TranslateAttributes(
		CSSM_DB_RECORD_ATTRIBUTE_DATA * Source,
		DAL_TRANSLATED_ATTRIBUTE_LIST &Destination) const;

	/* Functions for parsing the data object */
	CSSM_RETURN RetrieveDataIndex(DAL_CONST_DATA_PTR ValuePtr, const CSSM_OID * IndexOIDPtr, CSSM_DATA_PTR *DataPtr) const;
	CSSM_RETURN ReleaseDataIndex(CSSM_DATA_PTR DataPtr, const CSSM_OID * IndexOIDPtr) const;

	/* Functions to get and set the parsing functions */
	void nrSetParsingFunctions(CSSM_DB_RECORD_PARSING_FNTABLE const* FunctionTable);

	/* Inline wrappers Arround Backend */
	inline CSSM_RETURN NewRecord() const
		{ VERIFY_PTR(m_rBackend); return m_rBackend->NewRecord(); }
	inline CSSM_RETURN DeleteRecord() const
		{ VERIFY_PTR(m_rBackend); return m_rBackend->DeleteRecord(); }
	inline CSSM_RETURN GetUniqueRecordId(CSSM_DB_UNIQUE_RECORD_PTR UidPtr) const
		{ VERIFY_PTR(m_rBackend); return m_rBackend->GetUniqueRecordId(UidPtr); }
	inline CSSM_RETURN MoveToRecord(CSSM_DB_UNIQUE_RECORD const * UidPtr) const
		{ VERIFY_PTR(m_rBackend); return m_rBackend->MoveToRecord(UidPtr); }
	inline CSSM_RETURN SetSemantics(unsigned sem) const
		{ VERIFY_PTR(m_rBackend); return m_rBackend->SetSemantics(sem); }
	inline CSSM_RETURN GetSemantics(uint32 &Semantics) const
		{ VERIFY_PTR(m_rBackend); return m_rBackend->GetSemantics(Semantics); }
	inline CSSM_RETURN GetData(CSSM_DL_HANDLE DLHandle, CSSM_DATA_PTR DataPtr) const
		{ VERIFY_PTR(m_rBackend); return m_rBackend->GetData(DLHandle,DataPtr); }
	CSSM_RETURN UpdateRecord();
	void ReleaseRecordTable();
};

typedef DAL_RECORD_TABLE * DAL_RECORD_TABLE_PTR;

/*-----------------------------------------------------------------------------------
 * Class Name: DAL_DATABASE_INFO
 *
 * Description:
 *	This class is a container for a database. Each DB Handle coresponds to exactly
 * one instance of this class. It contains one or more DAL_RECORD_TABLE classes
 * (one for each CSSM_DB_RECORDTYPE supported).
 *-----------------------------------------------------------------------------------*/

class DAL_DATABASE_INFO
{
private:
	DATABASE_BACKEND_HANDLE m_hDatabase;  /* Handle to the database backend */

	char m_szDBName[MAX_DB_NAME_LENGTH+1]; /* Database Name */
	DAL_MODULE_PARAMETERS m_Parameters; /* Database open parameters */

	/* Array of Record Tables and Translation Tables (one for each type) */
	DAL_TYPE_ARRAY<DAL_RECORD_TABLE> m_rgRecordTable;
	DAL_TYPE_ARRAY<DAL_TRANSLATION_TABLE> m_rgTranslationTable;

	/* Array of Handles to the data parser modules (one for each type) */
	DAL_TYPE_ARRAY<CSSM_HANDLE> m_rgParseHandle;

#ifndef MODULE_DIRECTORY_SERVICES /* Turn this feature off for MDS */
	/* Array of UIDs to the data parser modules (one for each type) */
	DAL_TYPE_ARRAY<CSSM_SUBSERVICE_UID> m_rgParseUid;
#endif

#ifdef _DEBUG
	uint32 m_debug_NumOpenHandles; /* Ensure all open handles are closed */
#endif
	CSSM_BOOL m_fHaveWriterLock; /* Ensure we do not get the writer lock twice */
	cssm_SWMRLock m_DbInfoLock; /* Lock the current database's persistant information (not data) */

	/* Private Member functions */
	CSSM_RETURN MakeParseHandle(
		CSSM_HANDLE DLHandle,
		CSSM_SUBSERVICE_UID const *,
		CSSM_DB_RECORDTYPE);

public:

#ifdef _DEBUG /* Debug functions to check the validity of the system and the object */
	DAL_OBJECT_STATE m_debug_ObjectState;
	void AssertValidNotInitialized(void) const; /* Initialize has NOT been called */
	void AssertValid(void) const; /* Initialize has been called */
#endif

	/* Initialization and Destruction functions */
	DAL_DATABASE_INFO();
	~DAL_DATABASE_INFO();
	CSSM_RETURN Initialize(CSSM_HANDLE DLHandle, const char *DbName,
#if DAL_USE_MODULE_PARAMATERS
					DAL_MODULE_PARAMETERS *pParameters,
#endif
					CSSM_DBINFO const *pDbInfo);

	/* One of these must be called */
	CSSM_RETURN Open(
#if DAL_USE_MODULE_PARAMATERS
		const DAL_MODULE_PARAMETERS * pParameters
#endif
	);

	CSSM_RETURN Create(
#if DAL_USE_MODULE_PARAMATERS
		DAL_MODULE_PARAMETERS * pParameters
#endif
	);

	CSSM_RETURN CreateRelation(
#if DAL_USE_MODULE_PARAMATERS
		DAL_MODULE_PARAMETERS * pParameters,
#endif
		CSSM_DB_RECORDTYPE RelationID,
		const char *RelationName,
		uint32 NumberOfAttributes,
		const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
		uint32 NumberOfIndexes,
		const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo);

	CSSM_RETURN DestroyRelation(
#if DAL_USE_MODULE_PARAMATERS
		DAL_MODULE_PARAMETERS * pParameters,
#endif
		CSSM_DB_RECORDTYPE RelationID);


	/* Return the RecordTable associated with the record type */
	DAL_RECORD_TABLE * neGetRecordTable(CSSM_DB_RECORDTYPE) const;
	CSSM_DB_RECORDTYPE neGetRecordTypeByIndex(uint32 Index) const;

	/* Inline Functions	 */
	char const* neGetDbName() const;

	const DAL_MODULE_PARAMETERS * neGetModuleParameters() const;

	/* Locking functions */
	CSSM_RETURN SetDbCreateLock(const char *DbName);
	CSSM_RETURN SetDbOpenLock(const char *DbName);
	void nrDbCreateToDbOpenLock();
};

typedef DAL_DATABASE_INFO * DAL_DATABASE_INFO_PTR;
typedef DAL_DATABASE_INFO * DAL_DATABASE_INFO_REF_PTR;

typedef DAL_TYPE_ARRAY<CSSM_DB_RECORD_PARSING_FNTABLE> DAL_PARSING_FUNCTION_ARRAY;

/*-----------------------------------------------------------------------------------
 * Class Name: DAL_RECORD_TABLE_REF
 *
 * Description:
 * This class is used to reference a record table. It is used to ensure that
 *-----------------------------------------------------------------------------------*/

class DAL_RECORD_TABLE_REF
{
private:
	/* Copy of data from the DAL_DATABASE_INFO_LIST */
    PORT_MUTEX_HANDLE m_hMutex;
	DAL_RECORD_TABLE *m_pRecordTable;

	/* Private Initialize function */
	CSSM_RETURN Initialize(CSSM_DB_HANDLE DBHandle,
		CSSM_DB_RECORDTYPE RecordType,
		uint32 MissingInfoError);

public:
	DAL_RECORD_TABLE_REF(const DAL_RECORD_TABLE_REF & Source);
	DAL_RECORD_TABLE_REF();
	~DAL_RECORD_TABLE_REF();

	CSSM_RETURN Initialize(CSSM_DB_HANDLE DBHandle,
		CSSM_DB_UNIQUE_RECORD const * UniqueRecordId,
		CSSM_DB_RECORDTYPE *pRecordType);

	CSSM_RETURN Initialize(CSSM_DB_HANDLE DBHandle,
		CSSM_HANDLE ResultsHandle,
		CSSM_DB_RECORDTYPE *pRecordType);

	CSSM_RETURN Initialize(CSSM_DB_HANDLE DBHandle,
		CSSM_DB_RECORDTYPE RecordType);

	inline DAL_RECORD_TABLE* operator->()
	{
		ASSERT(m_hMutex && m_pRecordTable);
		return m_pRecordTable;
	}
};


/*-----------------------------------------------------------------------------------
 * Class Name: DAL_DATABASE_INFO_LIST
 *
 * Description:
 * The DAL_DATABASE_INFO_LIST contains a list of databases. There is one instances of
 * each class per attach
 * to the DL.
 *-----------------------------------------------------------------------------------*/

class DAL_DATABASE_INFO_LIST
{
private:

	class DAL_DATABASE_INFO_NODE : public DAL_DATABASE_INFO
	{
	private:
		DAL_DATABASE_INFO_NODE * m_pNext;
		CSSM_DB_HANDLE m_DBHandle;
        PORT_MUTEX_HANDLE m_hMutex; /* controls acess to a datbase info, do not get while having the writer 
		  lock for the database info list */

	public:
		DAL_DATABASE_INFO_NODE(void);
		~DAL_DATABASE_INFO_NODE(void);

		CSSM_RETURN Initialize();
		DAL_DATABASE_INFO * GetDatabaseReference();
        CSSM_RETURN GetDatabase(PORT_MUTEX_HANDLE &hMutex,DAL_DATABASE_INFO **dalDbInfo);

		DAL_DATABASE_INFO_NODE * neGetNext(void) const;
		void nrSetNext(DAL_DATABASE_INFO_NODE *);

		CSSM_DB_HANDLE neGetDBHandle(void) const;
		void nrCreateDone(CSSM_DB_HANDLE InHandle);
	};

	DAL_DATABASE_INFO_NODE * m_pDbRoot;
	cssm_SWMRLock m_SWMRLock; /* controls access to the database info list */

	CSSM_RETURN neFindDatabaseNode(DAL_DATABASE_INFO_NODE const * LastFind,
		const char * DbName, DAL_DATABASE_INFO_NODE **pDatabaseInfo) const;
public:

#ifdef _DEBUG /* Debug functions to check the validity of the system and the object */
	DAL_OBJECT_STATE m_debug_ObjectState;
	void AssertValidNotInitialized(void) const; /* Initialize has NOT been called */
	void AssertValid(void) const; /* Initialize has been called */
#endif

	CSSM_RETURN AllocateNewDatabase(DAL_DATABASE_INFO_REF_PTR *ret) const;
	void nrDeleteDatabase(DAL_DATABASE_INFO_REF_PTR) const;

	/* Initialization and Destruction functions */
	DAL_DATABASE_INFO_LIST();
	~DAL_DATABASE_INFO_LIST();
	CSSM_RETURN Initialize(void);

	/* Database list management functions */
	CSSM_RETURN AddDatabase(DAL_DATABASE_INFO_REF_PTR, CSSM_DB_HANDLE **tmpDbHandle);
	CSSM_RETURN RemoveDatabase(CSSM_DB_HANDLE DbHandle);
	CSSM_RETURN FindDatabaseByHandle(CSSM_DB_HANDLE DbHandle,
		DAL_DATABASE_INFO_REF_PTR &prefDatabase,
        PORT_MUTEX_HANDLE &Mutex);

// was  CSSM_RETURN DAL_DATABASE_INFO_LIST::GetDBNamesAndParameters(
	CSSM_RETURN GetDBNamesAndParameters(
				CSSM_DL_DB_HANDLE DLDBHandle,
				DAL_DB_OPEN_PARAM_PTR pParam);
};

#endif
#endif
