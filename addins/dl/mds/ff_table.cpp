/*-----------------------------------------------------------------------
 * File: FF_TABLE.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the flat file specific FLATFILE_TABLE_BACKEND member functions.
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

/* DL includes */
#include "ff_util.h"
#include "flatfile.h"
#include "dal_translation.h"

#include "bioapi_util.h"

static FF_BOOL fftable_fCompareHashes(
	const QUERY_PREDICATE* Predicates,
	const uint32 * rgIndexes,
	uint32 NumIndexes,
	FF_CONJUNCTIVE Conjunctive);
static FF_BOOL fftable_fCompareIndexes(
	const QUERY_PREDICATE* Predicates,
	DAL_CONST_DATA_PTR rgIndexes,
	uint32 NumIndexes,
	FF_CONJUNCTIVE Conjunctive);

/* Macros to return Attributes and Indexes from the fields array */

#define GET_NONINDEX_ARRAY() (&m_rgFields[m_NumIndexes])
#define GET_INDEX_ARRAY() (m_rgFields)

#ifdef _DEBUG /* Debug mode (w/ range checking) */
CSSM_DATA EmptyData = { 0, NULL };

	#define GET_DATA_OBJECT() \
		((m_rgFields) ? m_rgFields[m_NumIndexes + m_NumAttributes] : EmptyData)

	#define GET_INDEX(index) \
		((index < m_NumIndexes && m_rgFields) ? m_rgFields[index] : EmptyData)

	#define GET_ATTRIBUTE(attribute) \
		((attribute < m_NumAttributes && m_rgFields) ? m_rgFields[m_NumIndexes+attribute] : EmptyData)

	CSSM_DATA fftable_debug_BreakHere(void)
	{
		CSSM_DATA EmptyData = { 0, NULL };
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return EmptyData;
	}

#else /* Release mode (w/o range checking since we can not assert) */
	#define GET_INDEX(index) (m_rgFields[index])
	#define GET_ATTRIBUTE(attribute) (m_rgFields[m_NumIndexes+attribute])
	#define GET_DATA_OBJECT() (m_rgFields[m_NumIndexes + m_NumAttributes])
#endif

QUERY_PREDICATE::QUERY_PREDICATE()
{
	IndexNum = DAL_OOB;
	fnCompare = NULL;
	CompareRef = 0;
	HashValue = DAL_OOB;
	FullValue.Data = NULL;
	FullValue.Length = 0;
	pNext = NULL;
}

QUERY_PREDICATE::~QUERY_PREDICATE()
{
	if (pNext)
		delete pNext;

	if (FullValue.Data)
	{
		BioAPI_free(FullValue.Data, NULL);
		FullValue.Data = NULL;
	}
}


/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::FLATFILE_TABLE_BACKEND (not exposed to front end)
 *
 * Description:
 * Initialize all member variables with NULL or 0
 *
 *---------------------------------------------------------------------------*/
FLATFILE_TABLE_BACKEND::FLATFILE_TABLE_BACKEND(void)
{
	m_Uid = 0;
	m_NumIndexes = 0;
	m_NumAttributes = 0;
	m_NumFields = 1;
	m_rgFields = NULL;
	m_dwSemantics = 0;
	m_fIsNew = FF_FALSE;
	m_fIsDirty = FF_FALSE;
	m_RecordType = (CSSM_DB_RECORDTYPE)0;
	memset(&m_bfIndexFile, 0, sizeof(m_bfIndexFile));


#ifdef _DEBUG
	m_debug_fEditing = FF_FALSE;
#endif
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::~FLATFILE_TABLE_BACKEND (not exposed to front end)
 *
 * Description:
 * Close the recordset and delete the Index and Attribute arrays
 *
 *---------------------------------------------------------------------------*/
FLATFILE_TABLE_BACKEND::~FLATFILE_TABLE_BACKEND()
{
	AssertValid();

	ffutil_nrFreeData(m_rgFields, m_NumFields);
	ff_index_Deconstruct(&m_bfIndexFile);
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::Initialize
 *
 * Description:
 * This function will Initialize a TABLE BACKEND object
 *
 * Parameters:
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::Initialize(
	const DAL_TRANSLATION_TABLE *pTranslationTb,
	uint32 NumIndexes,
	uint32 NumAttributes,
	CSSM_DB_RECORDTYPE Recordtype,
	const char *IndexFileName,
	const char *FreeListFileName,
	const char *DataFileName,
	const DAL_MODULE_PARAMETERS * Parameters)
{
	CSSM_RETURN retValue = CSSM_OK;
	ASSERT(pTranslationTb);
	m_pTranslationTb = pTranslationTb;

	m_RecordType = Recordtype;

	m_NumAttributes = NumAttributes;
	m_NumIndexes = NumIndexes;
	m_NumFields = NumAttributes + NumIndexes + 1;
	if ((retValue = ffutil_AllocateData(&m_rgFields, m_NumFields)) != CSSM_OK)
		return retValue;

	return ff_index_Construct(&m_bfIndexFile, IndexFileName, DataFileName,
		FreeListFileName, NumIndexes, NumAttributes, Parameters);
}

/*-----------------------------------------------------------------------------*
 *																			   *
 * Code dealing with the current record										   *
 *																			   *
 *-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::NewRecord
 *
 * Description:
 * This function calls ODBC to create a new record for an DataInsert call.
 *
 * Parameters:
 * None.
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::NewRecord()
{
	AssertValid();

	nrClearFields();
	m_fIsNew = FF_TRUE;
#ifdef _DEBUG
	m_debug_fEditing = FF_TRUE;
#endif
	m_fIsDirty = FF_FALSE;
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND:DeleteRecord
 *
 * Description:
 * Deletes the current record
 *
 * Parameters:
 * None.
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::DeleteRecord()
{
	AssertValid();

	ASSERT(!m_fIsNew);
	ASSERT(!m_debug_fEditing);

	m_fIsNew = FF_FALSE;
#ifdef _DEBUG
	m_debug_fEditing = FF_FALSE;
#endif
	m_fIsDirty = FF_FALSE;

	CATCH_FUNCTION(ff_index_eDeleteRecord(&m_bfIndexFile, m_Uid))
	{
		RETURN_ERROR();
	}

	return CSSM_OK;
}


/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::MoveToRecord
 *
 * Description:
 * Cancel's any editing currently being done and moves to a new record
 *
 * Parameters:
 * Uid (input) : A 32 bit unique record identifier describing the record to move to.
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::MoveToRecord(CSSM_DB_UNIQUE_RECORD const * RecordId)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	ASSERT(!m_fIsNew);
	ASSERT(!m_debug_fEditing);

	FF_UID Uid;
	if ((retValue = ffutil_GetUniqueRecordIdVal(RecordId, &Uid)) != CSSM_OK)
		return retValue;

	if (!Uid)
		return CSSMERR_DL_INTERNAL_ERROR;

	nrMoveToRecord(Uid);

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::nrMoveToRecord (not exposed to front end, private)
 *
 * Description:
 * Cancel's any editing currently being done and moves to a new record
 *
 * Parameters:
 * Uid (input) : A 32 bit unique record identifier describing the record to move to.
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
void FLATFILE_TABLE_BACKEND::nrMoveToRecord(uint32 Uid)
{
	AssertValid();
	ASSERT(!m_fIsNew);
	ASSERT(!m_debug_fEditing);

	m_Uid = Uid;
	m_fIsDirty = FF_TRUE;
	m_fIsNew = FF_FALSE;
}

/*-----------------------------------------------------------------------------*
 *																			   *
 * Code dealing with the record identifer									   *
 *																			   *
 *-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::GetUniqueRecordId
 *
 * Description:
 * Fills in a CSSM_DB_UNIQUE_RECORD structure with information about the current
 * record's unique record id.
 *
 * Parameters:
 * UidPtr (output) : Record Id to be filled
 *
 * Return value:
 * CSSM_FAIL if an error occurred; otherwise CSSM_OK
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::GetUniqueRecordId(CSSM_DB_UNIQUE_RECORD_PTR UidPtr)
{
	AssertValid();

	ASSERT(!m_fIsNew);

	if (UidPtr == NULL || UidPtr->RecordIdentifier.Length != 8 ||
		UidPtr->RecordIdentifier.Data == NULL)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return CSSMERR_DL_INVALID_RECORD_UID;
	}

	uint32 * UidData = (uint32 *)UidPtr->RecordIdentifier.Data;
	UidData[0] = uint32(m_RecordType);
	UidData[1] = m_Uid;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::IsEqualCurrentUniqueRecordId
 *
 * Description:
 * Compares a CSSM_DB_UNIQUE_RECORD structure to the Unique record Id of the current position
 *
 * Parameters:
 * UidPtr (output) : Record Id to be filled
 *
 * Return value:
 * CSSM_FAIL if an error occurred; otherwise CSSM_OK
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_RECORD_UID
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::IsEqualCurrentUniqueRecordId(
	CSSM_DB_UNIQUE_RECORD const * UidPtr,
	CSSM_BOOL *IsEqual)
{
	AssertValid();
	ASSERT(!m_fIsNew);

	if (UidPtr == NULL || UidPtr->RecordIdentifier.Length != 8 ||
		UidPtr->RecordIdentifier.Data == NULL)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return CSSMERR_DL_INVALID_RECORD_UID;
	}

	uint32 * UidData = (uint32 *)UidPtr->RecordIdentifier.Data;
	*IsEqual = CBOOL((UidData[0] == uint32(m_RecordType)) && (UidData[1] == m_Uid));

	return CSSM_OK;
}


/*-----------------------------------------------------------------------------*
 *																			   *
 * Functions to modify Attributes, Semantics and the Opaque Data Object		   *
 *																			   *
 *-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::UpdateRecord
 *
 * Description:
 * Commits the changes made to a record to the database; if this is a data insert,
 * this function will return the UniqueRecord Id (optionally).
 *
 * Parameters:
 * UidPtr (Output/optional) : Unique Record Identifier for the record that was inserted
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::UpdateRecord()
{
	AssertValid();

	ASSERT(m_debug_fEditing);
	ASSERT(!m_fIsDirty);

#ifdef _DEBUG
	m_debug_fEditing = FF_FALSE;
#endif

	m_fIsDirty = FF_FALSE;

	if (m_fIsNew)
	{
		/* Add a new record */
		m_fIsNew = FF_FALSE;
		ff_index_InsertRecord(&m_bfIndexFile, m_rgFields, m_dwSemantics, &m_Uid);
		if (!m_Uid)
			return CSSMERR_DL_INTERNAL_ERROR;
	}
	else
	{
		/* Replace an existing record */
		CATCH_FUNCTION(ff_index_eReplaceRecord(&m_bfIndexFile, m_Uid, m_rgFields, m_dwSemantics))
		{
			RETURN_ERROR();
		}
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::SetSemantics
 *
 * Description:
 * Set the value of the Record's semantics
 *
 * Parameters:
 * semantics (input) : new Record Semantics value
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::SetSemantics(unsigned semantics)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	if ((retValue = Refresh()) != CSSM_OK)
		return retValue;

#ifdef _DEBUG
	m_debug_fEditing = FF_TRUE;
#endif

	m_dwSemantics = semantics;
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::SetAttribute
 *
 * Description:
 * Sets the data for an attribute
 *
 * Parameters:
 * AttributeNum (input) : Ordinal value of the attribute to set
 * AttributeFormat (input) : Format (type) of the attribute's value
 * pValue (input) : New value for the attribute. This value is a prepared value.
 * The following assumptions can be made depending on AttributeFormat:
 *
 * CSSM_DB_ATTRIBUTE_FORMAT_STRING: Ascii string with no null terminator
 * CSSM_DB_ATTRIBUTE_FORMAT_SINT32: 32-bit signed number
 * CSSM_DB_ATTRIBUTE_FORMAT_UINT32: 32-bit unsigned number
 * CSSM_DB_ATTRIBUTE_FORMAT_BIG_NUM: Type not supported by the DAL
 * CSSM_DB_ATTRIBUTE_FORMAT_REAL: 64-bit double, not NAN
 * CSSM_DB_ATTRIBUTE_FORMAT_TIME_DATE: Type not supported by the DAL
 * CSSM_DB_ATTRIBUTE_FORMAT_BLOB: Valid block
 * CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32: valid block of a size that is a multiple of four
 * CSSM_DB_ATTRIBUTE_FORMAT_COMPLEX: Type not supported by the DAL
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::SetAttribute(
	uint32 AttributeNum,
	CSSM_DB_ATTRIBUTE_FORMAT AttributeFormat,
	DAL_CONST_DATA_PTR pValue)
{
	CSSM_RETURN retValue = CSSM_OK;

	AssertValid();

	AttributeFormat;

	ASSERT(pValue);
	if (!pValue)
		return CSSMERR_DL_INTERNAL_ERROR;

	ASSERT(AttributeNum < m_NumAttributes);
	if (AttributeNum >= m_NumAttributes)
		return CSSMERR_DL_INTERNAL_ERROR;

	if ((retValue = Refresh()) != CSSM_OK)
		return retValue;

#ifdef _DEBUG
	m_debug_fEditing = FF_TRUE;
#endif

	return ffutil_CopyData(&GET_ATTRIBUTE(AttributeNum), pValue);
}


/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::SetIndex
 *
 * Description:
 * Sets the data for an index
 *
 * Parameters:
 * IndexNum (input) : Ordinal value of the index to set
 * IndexFormat (input) : Format (type) of the index's value
 * pValue (input) : New value for the index. This value is a prepared value.
 * The following assumptions can be made depending on IndexFormat:
 *
 * CSSM_DB_ATTRIBUTE_FORMAT_STRING: Ascii string with no null terminator
 * CSSM_DB_ATTRIBUTE_FORMAT_SINT32: 32-bit signed number
 * CSSM_DB_ATTRIBUTE_FORMAT_UINT32: 32-bit unsigned number
 * CSSM_DB_ATTRIBUTE_FORMAT_BIG_NUM: Type not supported by the DAL
 * CSSM_DB_ATTRIBUTE_FORMAT_REAL: 64-bit double, not NAN
 * CSSM_DB_ATTRIBUTE_FORMAT_TIME_DATE: Type not supported by the DAL
 * CSSM_DB_ATTRIBUTE_FORMAT_BLOB: Valid block
 * CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32: valid block of a size that is a multiple of four
 * CSSM_DB_ATTRIBUTE_FORMAT_COMPLEX: Type not supported by the DAL
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::SetIndex(
	uint32 IndexNum,
	CSSM_DB_ATTRIBUTE_FORMAT IndexFormat,
	DAL_CONST_DATA_PTR pValue)

{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	IndexFormat;
	ASSERT(pValue);
	if (!pValue)
		return CSSMERR_DL_INTERNAL_ERROR;

	ASSERT(IndexNum < m_NumIndexes);
	if (IndexNum >= m_NumIndexes)
		return CSSMERR_DL_INTERNAL_ERROR;

	if ((retValue = Refresh()) != CSSM_OK)
		return retValue;

#ifdef _DEBUG
	m_debug_fEditing = FF_TRUE;
#endif

	return ffutil_CopyData(&GET_INDEX(IndexNum), pValue);
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::SetData
 *
 * Description:
 * Sets the value of the interal data object object
 *
 * Parameters:
 * DataPtr(input) : Value to set the data object to to
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::SetData(DAL_CONST_DATA_PTR DataPtr)
{
	CSSM_RETURN retValue = CSSM_OK;

	AssertValid();

	if ((retValue = Refresh()) != CSSM_OK)
		return retValue;

#ifdef _DEBUG
	m_debug_fEditing = FF_TRUE;
#endif

	return ffutil_CopyData(&GET_DATA_OBJECT(), DataPtr);
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::StartModify (not exposed to front end, private)
 *
 * Description:
 * If the object is not in editing mode, call ODBC function to begin modifiying
 * the data. Otherwise, simply return CSSM_OK.
 *
 * Parameters:
 * None.
 *
 * Return value:
 * CSSM_OK on success or if the recordset is already in editing mode, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::Refresh()
{
	AssertValid();

	if (m_fIsDirty)
	{
		ASSERT(!m_fIsNew);
		ASSERT(!m_debug_fEditing);

		CATCH_FUNCTION(ff_index_eGetRecord(&m_bfIndexFile, m_Uid, 0, m_NumFields, m_rgFields,
			&m_dwSemantics, NULL))
		{
			RETURN_ERROR();
		}

		m_fIsDirty = FF_FALSE;
		m_fIsNew = FF_FALSE;
	}

	return CSSM_OK;
}


/*-----------------------------------------------------------------------------*
 *																			   *
 * Functions to get data back from the datastore to return to the app		   *
 *																			   *
 *-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::GetSemantics
 *
 * Description:
 * Returns the value of the record semantics from the datastore
 *
 * Parameters:
 * None.
 *
 * Return value:
 * the value of the Semantics
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::GetSemantics(uint32 &Semantics)
{
	CSSM_RETURN retValue = CSSM_OK;

	AssertValid();

	if ((retValue = Refresh()) != CSSM_OK)
		return retValue;

	Semantics = m_dwSemantics;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::GetAttribute
 *
 * Description:
 * Returns the value of an attribute from the datastore to return to the app
 *
 * Parameters:
 * DlHandle (input) : Handle to the DL instance calling this function
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::GetAttribute(
	CSSM_DL_HANDLE DlHandle,
	uint32 AttributeNum,
	CSSM_DATA_PTR pValue)
{
	CSSM_RETURN retValue = CSSM_OK;

	AssertValid();

	ASSERT(pValue);
	if (!pValue)
		return CSSMERR_DL_INTERNAL_ERROR;

	ASSERT(AttributeNum < m_NumAttributes);
	if (AttributeNum >= m_NumAttributes)
		return CSSMERR_DL_INTERNAL_ERROR;

	if ((retValue = Refresh()) != CSSM_OK)
		return retValue;

	ASSERT(m_pTranslationTb);

	/* Copy to the app's memory functions, adding trailing zeros */
	return ffutil_CopyDataToApp(DlHandle, pValue, &GET_ATTRIBUTE(AttributeNum));
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::GetIndex
 *
 * Description:
 * Returns the value of an attribute from the datastore to return to the app
 *
 * Parameters:
 * DlHandle (input) : Handle to the DL instance calling this function
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::GetIndex(
	CSSM_DL_HANDLE DlHandle,
	uint32 IndexNum,
	CSSM_DATA_PTR pValue)
{
	CSSM_RETURN retValue = CSSM_OK;

	AssertValid();

	ASSERT(pValue);
	if (!pValue)
		return CSSMERR_DL_INVALID_POINTER;

	ASSERT(IndexNum < m_NumIndexes);
	if (IndexNum >= m_NumIndexes)
		return CSSMERR_DL_INVALID_FIELD_NAME;

	if ((retValue = Refresh()) != CSSM_OK)
		return retValue;

	ASSERT(m_pTranslationTb);

	/* Copy to the app's memory functions, adding trailing zeros */
	return ffutil_CopyDataToApp(DlHandle, pValue, &GET_INDEX(IndexNum));
}


/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::GetData
 *
 * Returns the value of the data object from the datastore to return to the app
 *
 * Parameters:
 * DlHandle (input) : Handle to the DL instance calling this function
 * DataCopy (output) :	the value of the data object from the datastore
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::GetData(CSSM_DL_HANDLE DlHandle, CSSM_DATA_PTR DataCopy)
{
	CSSM_RETURN retValue = CSSM_OK;

	AssertValid();

	VERIFY_PTR(DataCopy);

	if ((retValue = Refresh()) != CSSM_OK)
		return retValue;

	/* Copy to the app's memory functions, with no trailing zeros */
	return ffutil_CopyDataToApp(DlHandle, DataCopy, &GET_DATA_OBJECT());
}

/*-----------------------------------------------------------------------------*
 *																			   *
 * For Find First, Find Next, and AbortQuery								   *
 *																			   *
 *-----------------------------------------------------------------------------*/

CSSM_RETURN FLATFILE_TABLE_BACKEND::NewQuery(void ** tmp)
{
	AssertValid();

	QUERY_BUFFER * pBuffer = new FLATFILE_TABLE_BACKEND::QUERY_BUFFER;
	if (!pBuffer)
		return CSSMERR_DL_MEMORY_ERROR;

	pBuffer->LastUid = DAL_OOB;
	pBuffer->pPredicateRoot = NULL;
	pBuffer->Conjunctive = Unassigned;

	*tmp = pBuffer;
	return CSSM_OK;
}

CSSM_RETURN FLATFILE_TABLE_BACKEND::SetQueryConjunctive(
	void *pQueryStorageBuffer,
	CSSM_DB_CONJUNCTIVE Conjunctive)
{
	AssertValid();
	ASSERT(pQueryStorageBuffer);
	if (!pQueryStorageBuffer)
		return CSSMERR_DL_INTERNAL_ERROR;

	QUERY_BUFFER * pBuffer = (QUERY_BUFFER *)pQueryStorageBuffer;

	if (Conjunctive == CSSM_DB_AND)
		pBuffer->Conjunctive = And;
	else if (Conjunctive == CSSM_DB_OR)
		pBuffer->Conjunctive = Or;
	else if (Conjunctive == CSSM_DB_NONE)
		pBuffer->Conjunctive = None;
	else
		return CSSMERR_DL_INTERNAL_ERROR;

	return CSSM_OK;
}

CSSM_RETURN QUERY_PREDICATE::Initialize(
	CSSM_DB_OPERATOR DbOperator,
	CSSM_DB_ATTRIBUTE_FORMAT FieldFormat,
	uint32 IndexNumParam,
	DAL_CONST_DATA_PTR pValue)
{
	CSSM_RETURN retValue = CSSM_OK;

	/* A predicate should not be initialized multiple times */
	ASSERT(this->HashValue == DAL_OOB && this->FullValue.Data == NULL && this->FullValue.Length == 0);

	if	(DbOperator == CSSM_DB_CONTAINS)
	{
		this->fnCompare = ffutil_Contains;

		if (FieldFormat == CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32)
			this->CompareRef = sizeof(uint32); /* Assign WordSize paramater to ffutil_MemContains */
		else
		{
			/* make sure field format is valid - operator only defined for these two */
			ASSERT(FieldFormat == CSSM_DB_ATTRIBUTE_FORMAT_STRING ||
				FieldFormat == CSSM_DB_ATTRIBUTE_FORMAT_BLOB);
			this->CompareRef = sizeof(char); /* Assign WordSize paramater to ffutil_MemContains */
		}
	}
	else
	{
		ASSERT(DbOperator == CSSM_DB_EQUAL || DbOperator == CSSM_DB_NOT_EQUAL);
		this->fnCompare = ffutil_MemCompare;
		this->CompareRef = DbOperator == CSSM_DB_NOT_EQUAL; /* Assign CompareRef as IsNotEqualReq */
	}

	/* Calculate the HashValue, this will be DAL_OOB if the operator is not equals. This inidicates
	   that the hash value is not going to be used (since hashes are only usefull for equals) */
	if (DbOperator == CSSM_DB_EQUAL)
		this->HashValue = ffutil_Hash(pValue);
	else
		this->HashValue = DAL_OOB;

	this->IndexNum = IndexNumParam;

	if (pValue)
	{
		if ((retValue = ffutil_CopyData(&this->FullValue, pValue)) != CSSM_OK)
			return retValue;
	}

	return CSSM_OK;
}


/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::AddSelectionPredicate
 *
 * Adds a selection predicate to the Query
 *
 * Parameters:
 * pQueryStorageBuffer (input/output) : Buffer to store state info about the query
 * DbOperator (input) : DbOperator used between the index and pValue
 * FieldFormat (input) : format of the predicate (and the index on which the predicate acts)
 * IndexNum (input) : Number of the index that we are comparing with
 * pValue (input) : Data to compare with the value of the index
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::AddSelectionPredicate(
	void *pQueryStorageBuffer,
	CSSM_DB_OPERATOR DbOperator,
	CSSM_DB_ATTRIBUTE_FORMAT FieldFormat,
	uint32 IndexNum,
	DAL_CONST_DATA_PTR pValue)
{
	CSSM_RETURN retValue = CSSM_OK;

	AssertValid();
	ASSERT(pQueryStorageBuffer);
	if (!pQueryStorageBuffer)
		return CSSMERR_DL_INTERNAL_ERROR;

	ASSERT(IndexNum < m_NumIndexes);

	QUERY_BUFFER * pBuffer = (QUERY_BUFFER *)pQueryStorageBuffer;

	/* First, allocate a new predicate */
	QUERY_PREDICATE *pPredicate = new QUERY_PREDICATE;

	/* Next, set the value for its members */
	if ((retValue = pPredicate->Initialize(DbOperator, FieldFormat, IndexNum, pValue)) != CSSM_OK)
	{
		delete pPredicate;
		return retValue;
	}

	/* Last, add the predicate to the predicates list */
	pPredicate->pNext = pBuffer->pPredicateRoot;
	pBuffer->pPredicateRoot = pPredicate;

	return CSSM_OK;
}


/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::ExecuteQuery
 *
 * Description:
 * Executes the query that was build with AddSelectionPredicates calls.
 *
 * Parameters:
 * Limits (input) : Size and Time limits imposed on the Query
 * Flags (input) : Paramater retrieved from the CSSM_QUERY_FLAGS structure.
 * NoRecordsFound (output) : returns true if the query was successful, but found
 *	 no elements. Note: finding no elements is not a failure condition
 * ppQueryStorageBuffer (output/optional) : If the ppQueryStorageBuffer != NULL,
 *	 then (*ppQueryStorageBuffer) should be assigned to a buffer to be used in
 *	 ContinueQuery and DeleteQuery. If ppQueryStorageBuffer is NULL, the result
 *	 of the query should be discarded after
 *
 * Note: It is valid for AddSelectionPredicates to have been called zero times. If
 * this input should be considered invalid, please return CSSM_DL_INVALID_QUERY
 *
 * Return value:
 * CSSM_OK if no error occurred, otherwise CSSM_FAIL
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/

CSSM_RETURN FLATFILE_TABLE_BACKEND::ExecuteQuery(
	void *pQueryStorageBuffer,
	const CSSM_QUERY_LIMITS &Limits,
	const CSSM_QUERY_FLAGS Flags,
	CSSM_DATA_PTR * &QueryTable,
	uint32 * &Semantics,
	void * &Uid,
	uint32 &NumberOfRecords,
	uint32 &NumberOfAttributes)
{
	uint32 i;
	FF_FUNCTION_BEGIN(FLATFILE_TABLE_BACKEND::ExecuteQuery);
	Flags;

	ASSERT(pQueryStorageBuffer);
	if (!pQueryStorageBuffer)
		return CSSMERR_DL_INTERNAL_ERROR;

	m_fIsNew = FF_FALSE;

	QUERY_BUFFER * pBuffer = (QUERY_BUFFER *)pQueryStorageBuffer;

	CATCH_FUNCTION(eExecuteQuery(pBuffer, Limits.TimeLimit, QueryTable, Semantics, Uid,
											NumberOfRecords, NumberOfAttributes))
	{
		nrClearFields();

		if (Error != CSSMERR_DL_ENDOFDATA)
		{
			for (i = 0; i < NumberOfRecords; i++)
				ffutil_nrFreeData (QueryTable[i], NumberOfAttributes);

			RETURN_ERROR();
		}
	}

	if (NumberOfRecords == 0)
		return CSSMERR_DL_ENDOFDATA;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::ContinueQuery
 *
 * Description:
 * Advances to the next record that was found by the last FindRecords call. If
 * the last find != FindNumber, then an error is flagged. The number of elements
 * left is returned
 *
 * Parameters:
 * FindNumber (input) : The handle to the find that was last executed.
 * NoneLeft (output) : True if there are no records left from the find
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL (No records left is considered a success)
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN FLATFILE_TABLE_BACKEND::ContinueQuery(
	void *pQueryStorageBuffer,
	const CSSM_QUERY_LIMITS &Limits)
{
	FF_FUNCTION_BEGIN(FLATFILE_TABLE_BACKEND::ContinueQuery);

	ASSERT(pQueryStorageBuffer);
	if (!pQueryStorageBuffer)
		return CSSMERR_DL_INTERNAL_ERROR;

	m_fIsNew = FF_FALSE;

	return CSSM_OK;
}

#ifdef _WIN32
#pragma warning(disable:4127)
#endif

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::eExecuteQuery
 *
 * Description:
 * Finds the next (or first) record that satisfies the query that was created
 *
 * Parameters:
 * pBuffer (input) : Query Buffer containing state information about the query.
 * TimeLimit (input) : Maximum Time for query to take (in seconds)
 *
 * Return value:
 * CSSM_OK on success, CSSMERR_DL_NO_DATA_FOUND if no records were found,
 * otherwise the error code returned
 *---------------------------------------------------------------------------*/
FF_ERROR FLATFILE_TABLE_BACKEND::eExecuteQuery(
	FLATFILE_TABLE_BACKEND::QUERY_BUFFER * pBuffer,
	uint32 TimeLimit,
	CSSM_DATA_PTR * &QueryTable,
	uint32 * &Semantics,
	void * &Uid,
	uint32 &NumberOfRecords,
	uint32 &NumberOfAttributes)
{
    hLock_t hLock = DAL_OOB;
	uint32 StartTime;
	uint32 i;

	if (TimeLimit != CSSM_QUERY_TIMELIMIT_NONE)
		StartTime = ffutil_GetTime();
	else
		StartTime = 0;

	uint32 *HashValueBuffer;

	if (pBuffer->pPredicateRoot)
	{
		HashValueBuffer = new uint32[m_NumIndexes];
		if (!HashValueBuffer)
		{
			THROW_NEW_ERROR (CSSMERR_DL_MEMORY_ERROR);
		}
	}
	else
		HashValueBuffer = NULL;

	ASSERT(pBuffer->Conjunctive != Unassigned);

	CATCH_FUNCTION(ff_index_eGetReaderLock (&m_bfIndexFile, &hLock))
	{
		delete []HashValueBuffer;
		THROW_ERROR();
	}

	NumberOfAttributes = m_NumFields;
	NumberOfRecords = 0;
	QueryTable = NULL;
	Semantics = NULL;
	Uid = NULL;
	while (1)
	{
		/*
		 * Search until we find a record, run out of time, run out of records, or encounter an
		 * error
		 */
#ifndef _DEBUG
		if (TimeLimit != CSSM_QUERY_TIMELIMIT_NONE)
		{
			if (ffutil_GetTime() - StartTime > (TimeLimit*1000))
			{
				delete []HashValueBuffer;
				ff_index_eReleaseReaderLock(&m_bfIndexFile, hLock);
				THROW_NEW_ERROR (CSSMERR_DL_ENDOFDATA);
			}
		}
#endif

		/* Prepare for the next read of a record */
		CATCH_FUNCTION(ff_index_eReadHashArray(&m_bfIndexFile, &pBuffer->LastUid, HashValueBuffer, &hLock))
		{
			delete []HashValueBuffer;
			ff_index_eReleaseReaderLock (&m_bfIndexFile, hLock);
			THROW_ERROR();
		}

		if (pBuffer->pPredicateRoot)
		{
			/* If we have predicates, get their hash's and compare the values to the expected */
			if (!fftable_fCompareHashes(pBuffer->pPredicateRoot, HashValueBuffer, m_NumIndexes,
					pBuffer->Conjunctive))
				continue;
		}

		m_Uid = pBuffer->LastUid;
		m_fIsDirty = FF_TRUE;
		m_fIsNew = FF_FALSE;

		CATCH_FUNCTION(ff_index_eGetRecord(&m_bfIndexFile, m_Uid, 0, m_NumIndexes,
			GET_INDEX_ARRAY(), NULL, &hLock))
		{
			if (Error == CSSMERR_DL_INVALID_RECORD_UID)
			{
				/* Concurrency issue: if the record is deleted just after it is found we
				   want to ignore the error */
				continue;
			}

			delete []HashValueBuffer;
			ff_index_eReleaseReaderLock(&m_bfIndexFile, hLock);
			THROW_ERROR();
		}

		if (pBuffer->pPredicateRoot)
		{
			if (!fftable_fCompareIndexes(pBuffer->pPredicateRoot, GET_INDEX_ARRAY(), m_NumIndexes,
					pBuffer->Conjunctive))
				continue;
		}

		/* ASSERT: at this point we have found a record, and we have read in the indexes */
		CATCH_FUNCTION(ff_index_eGetRecord(&m_bfIndexFile, m_Uid, m_NumIndexes,
			m_NumFields - m_NumIndexes, GET_NONINDEX_ARRAY(), &m_dwSemantics, &hLock))
		{
			if (Error == CSSMERR_DL_INVALID_RECORD_UID)
			{
				/* Concurrency issue: if the record is deleted just after it is found we
				   want to ignore the error */
				continue;
			}

			delete []HashValueBuffer;
			ff_index_eReleaseReaderLock(&m_bfIndexFile, hLock);
			THROW_ERROR();
		}

		/* The record has been read in */
		m_fIsDirty = FF_FALSE;
		m_fIsNew = FF_FALSE;
#ifdef _DEBUG
		m_debug_fEditing = FF_FALSE;
#endif
		if ((QueryTable = (CSSM_DATA_PTR *)BioAPI_realloc (QueryTable, sizeof (CSSM_DATA_PTR) * (NumberOfRecords+1), NULL))
																== NULL)
			THROW_NEW_ERROR(CSSMERR_DL_MEMORY_ERROR);
		if ((Semantics = (uint32 *)BioAPI_realloc (Semantics, sizeof (uint32) * (NumberOfRecords+1), NULL))
																== NULL)
			THROW_NEW_ERROR(CSSMERR_DL_MEMORY_ERROR);


		if ((Uid = (FF_UID *)BioAPI_realloc (Uid, sizeof (FF_UID) * (NumberOfRecords+1), NULL))
																== NULL)
			THROW_NEW_ERROR(CSSMERR_DL_MEMORY_ERROR);

		if (CSSM_OK != ffutil_AllocateData(&QueryTable[NumberOfRecords], m_NumFields))
			THROW_NEW_ERROR(CSSMERR_DL_INTERNAL_ERROR);

		FF_UID * ffuid = (FF_UID *) Uid;
		ffuid[NumberOfRecords] = m_Uid;

		Semantics[NumberOfRecords] = m_dwSemantics;

		CSSM_DATA_PTR current = QueryTable[NumberOfRecords];

		for (i = 0; i < m_NumFields; i++) {
			current[i] = m_rgFields[i];
			m_rgFields[i].Data = NULL;
			m_rgFields[i].Length = 0;
		}

		NumberOfRecords++;
	}
}

#ifdef _WIN32
#pragma warning(default:4127)
#endif

/*-----------------------------------------------------------------------------
 * Name: fftable_fCompareHashes
 *
 * Description:
 * Compares the Hash values of the indexes at the currently location to that of
 * the desired value.
 *
 * Parameters:
 * Predicates (input) : Linked list of selection predicates
 * rgIndexes (input) : array of indexes
 * NumIndexes (input) : Number of indexes for the current relation (indentifies the number of
 *	 elements in each of rgIndexes1 and rgIndexes2)
 * Conjunctive (input) : Conjunctive operator used to join the predicates
 *
 * Return value:
 * non-zero if the Comparison was successful
 * 0 if the comparison was not successful
 *---------------------------------------------------------------------------*/
static FF_BOOL fftable_fCompareHashes(
	const QUERY_PREDICATE* Predicates,
	const uint32 * rgIndexes,
	uint32 NumIndexes,
	FF_CONJUNCTIVE Conjunctive)
{
	ASSERT(Predicates);
	while (Predicates)
	{
		ASSERT(Predicates->IndexNum < NumIndexes);

		/* Make sure we do not run off the end of our array */
		if (Predicates->IndexNum > NumIndexes)
			return FF_FALSE;

		FF_BOOL Result = rgIndexes[Predicates->IndexNum] == Predicates->HashValue;
		if (Predicates->HashValue == DAL_OOB)
		{
			/* The hash is not useful for the requested operator, so we cannot use the
			hash to disqualify this record */
            Result = FF_TRUE;
		}

		if (Result)
		{
			/* if the conjunctive is Or or None, success found, don't have to continue */
			if (Conjunctive != And) return FF_TRUE;
		}
		else /* Comparison Failed */
		{
			/* if the conjunctive is And or None, failure found, don't have to continue */
			if (Conjunctive != Or)	return FF_FALSE;
		}

		Predicates = Predicates->pNext;
	}

	return (Conjunctive == And);
}

/*-----------------------------------------------------------------------------
 * Name: fftable_fCompareIndexes
 *
 * Description:
 * Compares the Hash values of the indexes at the currently location to that of
 * the desired value.
 *
 * Parameters:
 * rgIndexes1 (input) : First set of index values
 * rgIndexes2 (input) : Second set of index values
 * NumIndexes (input) : Number of indexes for the current relation (indentifies the number of
 *	 elements in each of rgIndexes1 and rgIndexes2)
 * rgOperators (input) : Operator used to compare rgIndexes1 w/ rgIndexes2
 * Conjunctive (input) : Conjunctive operator used to join the predicates
 *
 * Return value:
 * non-zero if the Comparison was successful
 * zero if the comparison was not successful
 *---------------------------------------------------------------------------*/
static FF_BOOL fftable_fCompareIndexes(
	const QUERY_PREDICATE* Predicates,
	DAL_CONST_DATA_PTR rgIndexes,
	uint32 NumIndexes,
	FF_CONJUNCTIVE Conjunctive)
{
	ASSERT(Predicates);
	while (Predicates)
	{
		ASSERT(Predicates->IndexNum < NumIndexes);

		/* Make sure we do not run off the end of our array */
		if (Predicates->IndexNum > NumIndexes)
			return FF_FALSE;

		FF_BOOL Result = Predicates->DoPredicate(rgIndexes);

		if (Result)
		{
			/* if the conjunctive is Or or None, success found, don't have to continue */
			if (Conjunctive != And) return FF_TRUE;
		}
		else /* Comparison Failed */
		{
			/* if the conjunctive is And or None, failure found, don't have to continue */
			if (Conjunctive != Or)	return FF_FALSE;
		}

		Predicates = Predicates->pNext;
	}

	return (Conjunctive == And);
}


/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::nrDeleteQuery
 *
 * Description:
 * Frees the memory left over from a find.
 *
 * Parameters:
 * FindNumber (input) : The handle to the find that was last executed.
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
void FLATFILE_TABLE_BACKEND::nrDeleteQuery(void *pQueryStorageBuffer)
{
	AssertValid();
	ASSERT(pQueryStorageBuffer);
	if (pQueryStorageBuffer)
	{
		QUERY_BUFFER * pBuffer = (QUERY_BUFFER *)pQueryStorageBuffer;

		if (pBuffer->pPredicateRoot)
			delete pBuffer->pPredicateRoot;
		delete pBuffer;
	}
	else
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
	}
}

/*-----------------------------------------------------------------------------
 * Name: FLATFILE_TABLE_BACKEND::nrClearFields
 *
 * Description:
 * This function will reset the value of the fields (the Data Object, unique Id,
 * semantics, Index info, and attribute info) to zero.
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
void FLATFILE_TABLE_BACKEND::nrClearFields(void)
{
	AssertValid();

	ASSERT(m_rgFields);

	if (!m_rgFields) return;

	uint32 i;
	for (i = 0; i < m_NumFields; i++)
	{
		if (m_rgFields[i].Data)
			BioAPI_free(m_rgFields[i].Data, NULL);

		m_rgFields[i].Data = NULL;
		m_rgFields[i].Length = 0;
	}

	m_dwSemantics = 0;
}

/*
 * FLATFILE_TABLE_BACKEND diagnostics
 */

#ifdef _DEBUG
void FLATFILE_TABLE_BACKEND::AssertValid() const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(FLATFILE_TABLE_BACKEND)));
	ASSERT(m_NumFields > m_NumIndexes);
	ASSERT(m_NumFields == m_NumAttributes + m_NumIndexes + 1);
	ASSERT(GET_INDEX_ARRAY() + m_NumIndexes == GET_NONINDEX_ARRAY());

	if (m_NumIndexes || m_NumAttributes)
		ASSERT_VALID_HEAP_POINTER(GET_NONINDEX_ARRAY(), sizeof(CSSM_DATA) * (m_NumAttributes+1));

	if (m_NumIndexes)
	{
		ASSERT_VALID_HEAP_POINTER(GET_INDEX_ARRAY(), sizeof(CSSM_DATA) * m_NumIndexes);
		ASSERT(&GET_INDEX(m_NumIndexes-1)+1 == GET_NONINDEX_ARRAY());
	}
	else
	{
		ASSERT(GET_INDEX_ARRAY() == NULL);
	}


}

#endif /* _DEBUG */

/*-----------------------------------------------------------------------------
 * Name: SetQueryData
 *
 * Description:
 * Sets the current record in backend, from the current row in query table.
 * the desired value.
 *
 * Parameters:
 * CurrentRecord (input) : Current record row from query table.
 * Semantics (input) : Semantic info for current record.
 *
 * Return value:
 * none
 *---------------------------------------------------------------------------*/
void FLATFILE_TABLE_BACKEND::SetQueryData(
						CSSM_DATA_PTR CurrentRecord,
						uint32 Semantics,
						void *Uid,
						uint32 CurrentIndex)
{
	uint32 i;

	/*
	 * Set the current attributes and sematic info.
	 */
    for (i = 0; i < m_NumFields; i++) 
	{
		if (m_rgFields[i].Data)
		{
			BioAPI_free (m_rgFields[i].Data, NULL);
			m_rgFields[i].Data = NULL;
		}

		m_rgFields[i] = CurrentRecord[i];
		CurrentRecord[i].Data = NULL;
		CurrentRecord[i].Length = 0;
	}

	FF_UID * ffUid = (FF_UID *)Uid;
	m_Uid = ffUid[CurrentIndex];
	m_dwSemantics = Semantics;

	/* The record has been read in */
	m_fIsDirty = FF_FALSE;
	m_fIsNew = FF_FALSE;
}
