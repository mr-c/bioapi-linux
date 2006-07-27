/*-----------------------------------------------------------------------
 * File: FLATFILE.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file defines the public portion of the DL Flat File Backend.
 */

#ifndef _FLATFILE_H_
#define _FLATFILE_H_

#include "dal_common.h"
#include "dal_dlbe.h"
#include "ff_index.h"
#include "maf_collectn.h"
#include "maf_util.h"

/* Type declarations */
class DAL_TRANSLATION_TABLE;
typedef void * DATABASE_BACKEND_HANDLE;

typedef FF_BOOL (* FF_COMPARE_FUNC)(const CSSM_DATA * pData, const CSSM_DATA *pPredicate, uint32 CompareRef);

typedef enum { And, Or, None, Unassigned } FF_CONJUNCTIVE;

/* Holds information about one predicate for a query, this information is in a linked list */
struct QUERY_PREDICATE
{
	uint32 IndexNum;
	uint32 HashValue;
	CSSM_DATA FullValue;
	FF_COMPARE_FUNC fnCompare;
	uint32 CompareRef;
	struct QUERY_PREDICATE *pNext;

	CSSM_RETURN Initialize(
		CSSM_DB_OPERATOR DbOperator,
		CSSM_DB_ATTRIBUTE_FORMAT FieldFormat,
		uint32 IndexNumParam,
		DAL_CONST_DATA_PTR pValue);

	QUERY_PREDICATE();
	~QUERY_PREDICATE();


	FF_BOOL DoPredicate(const CSSM_DATA * rgIndexList) const
	{
		ASSERT(fnCompare);
		if (fnCompare)
			return fnCompare(&rgIndexList[IndexNum], &FullValue, CompareRef);
		else
			return 1;
	}
};

class FLATFILE_TABLE_BACKEND : public TABLE_BACKEND
{
public:
	/*
	 * Functions dealing with the current record
	 */
	/* Changing which record is selected as current */
	virtual CSSM_RETURN NewRecord();
	virtual CSSM_RETURN DeleteRecord();
	virtual CSSM_RETURN MoveToRecord(CSSM_DB_UNIQUE_RECORD const * RecordId);

	/* Code dealing with the record identifer */
	virtual CSSM_RETURN GetUniqueRecordId(CSSM_DB_UNIQUE_RECORD_PTR);
	virtual CSSM_RETURN IsEqualCurrentUniqueRecordId(CSSM_DB_UNIQUE_RECORD const *, CSSM_BOOL *IsEqual);

	/* Functions to modify Attributes, Semantics and the Opaque Data Object */
	virtual CSSM_RETURN UpdateRecord();
	virtual CSSM_RETURN SetSemantics(unsigned);
	virtual CSSM_RETURN SetAttribute(uint32 AttributeNum, CSSM_DB_ATTRIBUTE_FORMAT, DAL_CONST_DATA_PTR);
	virtual CSSM_RETURN SetIndex(uint32 AttributeNum, CSSM_DB_ATTRIBUTE_FORMAT, DAL_CONST_DATA_PTR);
	virtual CSSM_RETURN SetData(DAL_CONST_DATA_PTR);

	/* Functions to get data back from the datastore to return to the app */
	virtual CSSM_RETURN GetSemantics(uint32 &Semantics);
	virtual CSSM_RETURN GetAttribute(CSSM_DL_HANDLE DLHandle, uint32 AttributeNum, CSSM_DATA_PTR);
	virtual CSSM_RETURN GetIndex(CSSM_DL_HANDLE DLHandle, uint32 AttributeNum, CSSM_DATA_PTR);
	virtual CSSM_RETURN GetData(CSSM_DL_HANDLE DLHandle, CSSM_DATA_PTR);

	/*
	 * For Find First, Find Next, and AbortQuery
	 */
	/* Query Construction */
	virtual CSSM_RETURN NewQuery(void ** tmp);
	virtual CSSM_RETURN SetQueryConjunctive(void *pQueryStorageBuffer, CSSM_DB_CONJUNCTIVE);
	virtual CSSM_RETURN AddSelectionPredicate(
		void *pQueryStorageBuffer,
		CSSM_DB_OPERATOR DbOperator,
		CSSM_DB_ATTRIBUTE_FORMAT FieldFormat,
		uint32 IndexNum,
		DAL_CONST_DATA_PTR pValue);

	/* Query Execution */
	virtual CSSM_RETURN ExecuteQuery(void *pQueryStorageBuffer, const CSSM_QUERY_LIMITS &, const CSSM_QUERY_FLAGS,
		CSSM_DATA_PTR * &QueryTable, uint32 * &Semantics, void * &uid, uint32 &NumberOfRecords, uint32 &NumberOfAttributes);
	virtual CSSM_RETURN ContinueQuery(void *pQueryStorageBuffer, const CSSM_QUERY_LIMITS &Limits);
	virtual void SetQueryData(CSSM_DATA_PTR CurrentRecord, unsigned Semantics, void *Uid, uint32 CurrentIndex);
	/* Query Destruction */
	virtual void nrDeleteQuery(void *pQueryStorageBuffer);
public:

	CSSM_RETURN Initialize(
		const DAL_TRANSLATION_TABLE *pTranslationTb,
		uint32 NumIndexes,
		uint32 NumAttributes,
		CSSM_DB_RECORDTYPE Recordtype,
		const char *IndexFileName,
		const char *FreeListFileName,
		const char *DataFileName,
		const DAL_MODULE_PARAMETERS * Parameters);

	FLATFILE_TABLE_BACKEND();
	~FLATFILE_TABLE_BACKEND();

#ifdef _DEBUG /* Debug functions to check the validity of the system and the object */
	void AssertValid() const; /* Initialize has been called */
#endif

private:
	FF_INDEX_BUFF m_bfIndexFile; /* The Index file - interface to the persistant store */

	const DAL_TRANSLATION_TABLE *m_pTranslationTb;

	// Index information for this record
	FF_UID m_Uid;// Unique Identifier

	uint32 m_NumAttributes;
	uint32 m_NumIndexes;
	uint32 m_NumFields;

	CSSM_DATA_PTR m_rgFields;

	unsigned m_dwSemantics; // Semantics

	// misc private variables
	FF_BOOL m_fIsDirty;
	FF_BOOL m_fIsNew;
	CSSM_DB_RECORDTYPE m_RecordType;

	// find private variables & functions

	struct QUERY_BUFFER
	{
		struct QUERY_PREDICATE *pPredicateRoot;

		FF_UID LastUid;
		FF_CONJUNCTIVE Conjunctive;
	};

	FF_ERROR eExecuteQuery(
		FLATFILE_TABLE_BACKEND::QUERY_BUFFER * pBuffer,
		uint32 TimeLimit,
		CSSM_DATA_PTR * &QueryTable,
		uint32 * &Semantics,
		void * &Uid,
		uint32 &NumberOfRecords,
		uint32 &NumberOfAttributes);

	CSSM_RETURN Refresh(void);
	void nrClearFields(void);
	void nrMoveToRecord(uint32 RecordId);

#if defined(_DEBUG)
	FF_BOOL m_debug_fEditing;
#endif
};

typedef FLATFILE_TABLE_BACKEND * FLATFILE_TABLE_BACKEND_REF;

typedef const char DATABASE_BACKEND;
typedef const char * DATABASE_BACKEND_PTR;

/* Macro to get the Directory Name from a pointer to a DAL_MODULE_PARAMETERS struct */
#define GetPathStart(pParameters) ((char*)((pParameters)->AdditionalInformation.Data))
#define GetPathStartLength(pParameters) ((pParameters)->AdditionalInformation.Length)

#endif
