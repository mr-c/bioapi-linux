/*-----------------------------------------------------------------------
 * File: DAL_DLBE.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file defines the public portion of the DL Backend. Theses functions
 * should be implemented by the backend. This file is only included in the
 * DAL.
 */

//#ifdef DAL_SOURCE_FILE
#ifndef DAL_DLBE_H
#define DAL_DLBE_H

#include "dal_common.h"

/* Type declarations */
class DAL_TRANSLATION_TABLE;
typedef void * DATABASE_BACKEND_HANDLE;

/* This is a virtual base class. It contains the interface that must be
 * implemented by any database backend using the dal-dl framework.
 */
class TABLE_BACKEND
{
public:
	/*
	 * Functions dealing with the current record
	 */
	/* Changing which record is selected as current */
	virtual CSSM_RETURN NewRecord() = 0;
	virtual CSSM_RETURN DeleteRecord() = 0;
	virtual CSSM_RETURN MoveToRecord(CSSM_DB_UNIQUE_RECORD const * RecordId) = 0;

	/* Code dealing with the record identifer */
	virtual CSSM_RETURN GetUniqueRecordId(CSSM_DB_UNIQUE_RECORD_PTR) = 0;
	virtual CSSM_RETURN IsEqualCurrentUniqueRecordId(CSSM_DB_UNIQUE_RECORD const *, CSSM_BOOL *IsEqual) = 0;

	/* Functions to modify Attributes, Semantics and the Opaque Data Object */
	virtual CSSM_RETURN UpdateRecord() = 0;
	virtual CSSM_RETURN SetSemantics(unsigned) = 0;
	virtual CSSM_RETURN SetAttribute(uint32 AttributeNum, CSSM_DB_ATTRIBUTE_FORMAT AttributeFormat, DAL_CONST_DATA_PTR) = 0;
	virtual CSSM_RETURN SetIndex(uint32 IndexNum, CSSM_DB_ATTRIBUTE_FORMAT IndexFormat, DAL_CONST_DATA_PTR) = 0;
	virtual CSSM_RETURN SetData(DAL_CONST_DATA_PTR) = 0;

	/* Functions to get data back from the datastore to return to the app */
	virtual CSSM_RETURN GetSemantics(uint32 &Semantics) = 0;
	virtual CSSM_RETURN GetAttribute(CSSM_DL_HANDLE DLHandle, uint32 AttributeNum, CSSM_DATA_PTR) = 0;
	virtual CSSM_RETURN GetIndex(CSSM_DL_HANDLE DLHandle, uint32 AttributeNum, CSSM_DATA_PTR) = 0;
	virtual CSSM_RETURN GetData(CSSM_DL_HANDLE DLHandle, CSSM_DATA_PTR) = 0;

	/*
	 * For Find First, Find Next, and AbortQuery
	 */
	/* Query Construction */
	virtual CSSM_RETURN NewQuery(void **tmp) = 0;
	virtual CSSM_RETURN SetQueryConjunctive(void *pQueryStorageBuffer, CSSM_DB_CONJUNCTIVE) = 0;
	virtual CSSM_RETURN AddSelectionPredicate(
		void *pQueryStorageBuffer,
		CSSM_DB_OPERATOR DbOperator,
		CSSM_DB_ATTRIBUTE_FORMAT FieldFormat,
		uint32 IndexNum,
		DAL_CONST_DATA_PTR pValue) = 0;

	/* Query Execution */
	virtual CSSM_RETURN ExecuteQuery(void *pQueryStorageBuffer, const CSSM_QUERY_LIMITS &, const CSSM_QUERY_FLAGS,
					CSSM_DATA_PTR * &QueryTable, uint32 * &Semantics, void * &Uid, uint32 &NumberOfRecords,
					uint32 &NumberOfAttributes) = 0;
	virtual CSSM_RETURN ContinueQuery(void *pQueryStorageBuffer, const CSSM_QUERY_LIMITS &Limits) = 0;
	virtual void SetQueryData(CSSM_DATA_PTR CurrentRecord, uint32 Semantics, void *Uid, uint32 CurrentIndex) = 0;
	/* Query Destruction */
	virtual void nrDeleteQuery(void *pQueryStorageBuffer) = 0;

};
typedef TABLE_BACKEND * TABLE_BACKEND_REF;

CSSM_RETURN dlbe_Initialize(void);
CSSM_RETURN dlbe_Uninitialize(void);

/* Database Functions */
CSSM_RETURN dlbe_CreateDatabase(DATABASE_BACKEND_HANDLE *phDatabase, const char *DbName
	MODULE_PARAMETERS_DECL);

CSSM_RETURN dlbe_OpenDatabase(DATABASE_BACKEND_HANDLE *phDatabase, const char *DbName
	CONST_MODULE_PARAMETERS_DECL);

void dlbe_DestroyDatabase(DATABASE_BACKEND_HANDLE hDatabase);

CSSM_RETURN dlbe_DeleteDatabase(const char *DbName CONST_MODULE_PARAMETERS_DECL);
CSSM_RETURN dlbe_DeleteRelation(CSSM_DB_RECORDTYPE RelationID, DAL_MODULE_PARAMETERS const * Parameters);


/* Table Functions */

CSSM_RETURN dlbe_CreateFiles(DATABASE_BACKEND_HANDLE hDatabase, TABLE_BACKEND_REF *prTableBackend,
	CSSM_DB_RECORDTYPE Recordtype, const DAL_TRANSLATION_TABLE * pTable,
	DAL_MODULE_PARAMETERS * Parameters);

CSSM_RETURN dlbe_OpenTable(DATABASE_BACKEND_HANDLE hDatabase, TABLE_BACKEND_REF *prTableBackend,
	CSSM_DB_RECORDTYPE Recordtype, const DAL_TRANSLATION_TABLE *pTable
	CONST_MODULE_PARAMETERS_DECL);

CSSM_RETURN dlbe_CreateTable(DATABASE_BACKEND_HANDLE hDatabase, TABLE_BACKEND_REF *prTableBackend,
	CSSM_DB_RECORDTYPE Recordtype, const DAL_TRANSLATION_TABLE *pTable
	MODULE_PARAMETERS_DECL);

void dlbe_DestroyTable(TABLE_BACKEND_REF rTableBackend);

/* Unique Record ID functions */
CSSM_RETURN dlbe_CreateUniqueRecordId(CSSM_HANDLE DLHandle,
									  CSSM_DB_UNIQUE_RECORD_PTR *UR);

CSSM_RETURN dlbe_FreeUniqueRecordId(CSSM_DL_DB_HANDLE DLDBHandle,
	CSSM_DB_UNIQUE_RECORD_PTR UniqueRecordId);

CSSM_RETURN dlbe_GetUniqueRecordIdType(CSSM_DB_UNIQUE_RECORD const * UniqueIdPtr,
									   CSSM_DB_RECORDTYPE *RecordType);

#if USE_DL_PASSTHROUGH

CSSM_RETURN dlbe_PassThrough (
	CSSM_DL_DB_HANDLE DLDBHandle,
	uint32 PassThroughId,
	const void * InputParams,
	void ** OutputParams);

#endif /* USE_DL_PASSTHROUGH */

#endif
//#endif
