/*-----------------------------------------------------------------------
 * File: DAL_TRANSLATION.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the definition of DAL_TRANSLATION_TABLE. It also
 * contains the declaration of six classes that are manipulated by a
 * DAL_TRANSLATION_TABLE:
 *		DAL_TRANSLATED_ATTRIBUTE
 *		DAL_TRANSLATED_INPUT_ATTRIBUTE
 *		DAL_TRANSLATED_OUTPUT_ATTRIBUTE
 *		DAL_TRANSLATED_ATTRIBUTE_LIST
 *		DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST
 *		DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST
 */

#ifndef DAL_TRANSLATION_H
#define DAL_TRANSLATION_H

#include "cssmerr.h"
#include "dal_common.h"

class DAL_DATABASE_INFO;
class DAL_DATABASE_INFO_LIST;

#ifdef _DEBUG

enum DAL_OBJECT_STATE {
	Constructing, NotInitialized, Initializing, Initialized
};

#define START_CONSTRUCTOR() m_debug_ObjectState = Constructing
#define END_CONSTRUCTOR() m_debug_ObjectState = NotInitialized
#define START_INITIALIZE() m_debug_ObjectState = Initializing
#define END_INITIALIZE() m_debug_ObjectState = Initialized

#else

#define START_CONSTRUCTOR()
#define END_CONSTRUCTOR()
#define START_INITIALIZE()
#define END_INITIALIZE()
#endif

#define DAL_ATTRIBUTE_FORMAT_OOB ((CSSM_DB_ATTRIBUTE_FORMAT)(DAL_OOB))

/*-----------------------------------------------------------------------------------
 * Class Name: DAL_TRANSLATED_ATTRIBUTE
 *
 * Description:
 *	This structure is returned from the translation table, it is very similar to
 * CSSM_DB_ATTRIBUTE_DATA except that attributes are mapped to index and attribute
 * numbers. This class is only used as a base class.
 *-----------------------------------------------------------------------------------*/

class DAL_TRANSLATED_ATTRIBUTE
{
protected:
	CSSM_BOOL m_fUnique;
	uint32 m_IndexNumber;
	uint32 m_AttributeNumber;
	CSSM_DB_ATTRIBUTE_FORMAT m_FieldFormat;
	CSSM_DATA_PTR *m_pValue; /* Will be NULL only if the attribute was not specified */
	uint32 *m_pNumberOfValues; /* Specifies the number of values in m_pValue */

public:
	DAL_TRANSLATED_ATTRIBUTE();
	~DAL_TRANSLATED_ATTRIBUTE() {} /* Do not delete pValue because it is from the
		CSSM_DB_RECORD_ATTRIBUTE_DATA structure */

	CSSM_DB_ATTRIBUTE_FORMAT
	neGetFieldFormat() const			 { return m_FieldFormat; }

	CSSM_BOOL neIsUnique() const		 { return m_fUnique; }

	uint32 neGetIndexNumber() const		 { return m_IndexNumber; }
	/* return the Index Number, DAL_OOB if the attribute is not used as an index */

	uint32 neGetAttributeNumber() const	 { return m_AttributeNumber; }
	/* return the Attribute Number, DAL_OOB if the attribute is used as an index */

	virtual CSSM_RETURN neInitializeValue(CSSM_DATA_PTR *, uint32 *) { DEBUG_BREAK(); return CSSMERR_DL_INTERNAL_ERROR;}

	void nrInitialize(uint32 IndexNumber, uint32 AttributeNumber, CSSM_BOOL IsUnique,
		CSSM_DB_ATTRIBUTE_FORMAT FieldFormat);
};

typedef DAL_TRANSLATED_ATTRIBUTE * DAL_TRANSLATED_ATTRIBUTE_PTR;

/*-----------------------------------------------------------------------------------
 * Class Name: DAL_TRANSLATED_ATTRIBUTE_LIST
 *
 * Description:
 *	This structure is returned from the translation table, it is very similar to
 * CSSM_DB_RECORD_ATTRIBUTE_DATA except that attributes are mapped to index and attribute
 * numbers. This class is only used as a base class.
 *-----------------------------------------------------------------------------------*/

class DAL_TRANSLATED_ATTRIBUTE_LIST
{
protected:
	uint32* m_pSemanticInformation;
	uint32 m_NumberOfAttributes;
	DAL_TRANSLATED_ATTRIBUTE_PTR m_rgAttributes;
	CSSM_BOOL m_fInitialized;

public:
	DAL_TRANSLATED_ATTRIBUTE_LIST();
	~DAL_TRANSLATED_ATTRIBUTE_LIST();

	virtual CSSM_RETURN Initialize(uint32 *SemanticInformation, uint32 NumberOfAttributes);
	DAL_TRANSLATED_ATTRIBUTE& neGetAttribute(uint32 index) const;

	CSSM_BOOL neIsInitialized() const	   { return m_fInitialized; }
	/* Was the Attributes paramater specified on the command line NULL */
	CSSM_BOOL neWasSourceNull() const	   { ASSERT(neIsInitialized()); return CBOOL(m_pSemanticInformation == NULL); }
	uint32 neGetNumberOfAttributes() const { ASSERT(neIsInitialized()); return m_NumberOfAttributes; }

#ifdef _DEBUG /* Debug functions to check the validity of the system and the object */
	void AssertValid(void) const; /* Initialize has been called */
#endif
};

typedef DAL_TRANSLATED_ATTRIBUTE_LIST * DAL_TRANSLATED_ATTRIBUTE_LIST_PTR;

/*-----------------------------------------------------------------------------------
 * Class Name: DAL_TRANSLATED_INPUT_ATTRIBUTE
 *
 * Description:
 *	This structure is returned from the translation table, it is very similar to
 * CSSM_DB_ATTRIBUTE_DATA except that attributes are mapped to index and attribute
 * numbers. This represents one index that was input in DataInsert or DataModify.
 *-----------------------------------------------------------------------------------*/

class DAL_TRANSLATED_INPUT_ATTRIBUTE : public DAL_TRANSLATED_ATTRIBUTE
{
public:
	CSSM_BOOL neWasInput() const				{ return CBOOL(m_pValue != NULL); }
	DAL_CONST_DATA_PTR neGetValuePtr() const	{ if (m_pValue) return *m_pValue; else return NULL; }
	uint32 *neGetNumValuePtr() const			{ return m_pNumberOfValues; }

	CSSM_RETURN neInitializeValue(CSSM_DATA_PTR *, uint32 *);
};

typedef DAL_TRANSLATED_INPUT_ATTRIBUTE * DAL_TRANSLATED_INPUT_ATTRIBUTE_PTR;

/*-----------------------------------------------------------------------------------
 * Class Name: DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST
 *
 * Description:
 *	This structure is returned from the translation table, it is very similar to
 * CSSM_DB_RECORD_ATTRIBUTE_DATA except that attributes are mapped to index and attribute
 * numbers. This represents the fields input to DataInsert or DataModify
 *-----------------------------------------------------------------------------------*/

class DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST : public DAL_TRANSLATED_ATTRIBUTE_LIST
{
public:
	DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST() {}
	DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST(const DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST &);

	uint32 GetSemanticInformation() const;
	CSSM_RETURN Initialize(uint32 *SemanticInformation, uint32 NumberOfAttributes);

	inline DAL_TRANSLATED_INPUT_ATTRIBUTE& neGetAttribute(uint32 index) const
	{ return (DAL_TRANSLATED_INPUT_ATTRIBUTE&)DAL_TRANSLATED_ATTRIBUTE_LIST::neGetAttribute(index); }
};

typedef DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST * DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST_PTR;

/*-----------------------------------------------------------------------------------
 * Class Name: DAL_TRANSLATED_OUTPUT_ATTRIBUTE
 *
 * Description:
 *	This structure is returned from the translation table, it is very similar to
 * CSSM_DB_ATTRIBUTE_DATA except that attributes are mapped to index and attribute
 * numbers. This represents one index that was requested in DataGetFromUid, DataGetFirst,
 * or DataGetNext
 *-----------------------------------------------------------------------------------*/

class DAL_TRANSLATED_OUTPUT_ATTRIBUTE : public DAL_TRANSLATED_ATTRIBUTE
{
public:
	CSSM_BOOL neWasRequested() const			   { return CBOOL(m_pValue != NULL); }
	CSSM_RETURN neGetWriteableValuePtr(CSSM_HANDLE, CSSM_DATA_PTR *);

	virtual CSSM_RETURN neInitializeValue(CSSM_DATA_PTR *, uint32 *);
};

typedef DAL_TRANSLATED_OUTPUT_ATTRIBUTE * DAL_TRANSLATED_OUTPUT_ATTRIBUTE_PTR;

/*-----------------------------------------------------------------------------------
 * Class Name: DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST
 *
 * Description:
 *	This structure is returned from the translation table, it is very similar to
 * CSSM_DB_RECORD_ATTRIBUTE_DATA except that attributes are mapped to index and attribute
 * numbers. This represents the fields requested in DataGetFromUid, DataGetFirst,
 * or DataGetNext
 *-----------------------------------------------------------------------------------*/

class DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST : public DAL_TRANSLATED_ATTRIBUTE_LIST
{
public:
	virtual CSSM_RETURN Initialize(uint32 *SemanticInformation, uint32 NumberOfAttributes);
	void nrSetSemanticInformation(uint32);

	inline DAL_TRANSLATED_OUTPUT_ATTRIBUTE& neGetAttribute(uint32 index) const
	{ return (DAL_TRANSLATED_OUTPUT_ATTRIBUTE&)DAL_TRANSLATED_ATTRIBUTE_LIST::neGetAttribute(index); }
};

typedef DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST * DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST_PTR;


/*-----------------------------------------------------------------------------------
 * Class Name: DAL_TRANSLATION_TABLE
 *
 * Description:
 *	This class holds the list of attribute names and index names. Its purpose is to
 * translate from attribute/index names to attribute/index numbers. The Names/OIDs
 * of indexes from the data object can also be extracted one at a time. Basically,
 * the class is a parser for CSSM_DB_RECORD_ATTRIBUTE_INFO and CSSM_DB_RECORD_INDEX_INFO.
 *-----------------------------------------------------------------------------------*/

class DAL_TRANSLATION_TABLE
{
private:
	/* Holds information about attributes stored in the data store */
	struct DAL_TRANSLATION_TABLE_NODE
	{
		CSSM_BOOL m_fUnique;
		unsigned m_IndexNumber;
		CSSM_DB_ATTRIBUTE_NAME_FORMAT m_AttributeNameFormat;
		CSSM_DATA m_Name;
		CSSM_DB_ATTRIBUTE_FORMAT m_FieldFormat;

		DAL_TRANSLATION_TABLE_NODE();
		~DAL_TRANSLATION_TABLE_NODE();
		CSSM_RETURN Initialize(const CSSM_DB_ATTRIBUTE_INFO &AInfo);
		void nrSetIndexInfo(uint32 IndexNum, CSSM_DB_INDEX_TYPE type);
	};

	/* Record Attribute Information */
	struct DAL_TRANSLATION_TABLE_NODE *m_rgAttributeInfo;
	unsigned m_NumRecordAttributes;
	unsigned m_NumPureRecordAttributes; /* Number of Record Attributes not used for indexing */

	/* Record Indexes from Data object */
	struct DAL_TRANSLATION_TABLE_NODE *m_rgDataID;
	unsigned m_NumIndexesFromDataObject;

	CSSM_RETURN FindTableNode(
		CSSM_DB_ATTRIBUTE_INFO const* KeyInfoPtr,
		DAL_TRANSLATION_TABLE_NODE const* rgTableNodes,
		uint32 NumNodes,
		uint32 NotMatchingFieldFormatError,
		uint32 &ArrayIndexNum,
		uint32 &PureAttributeNum) const;

	/* functions to get the index to modify from an attribute name */
	CSSM_RETURN LookupAttribute(CSSM_DB_ATTRIBUTE_INFO const*, uint32 NotMatchingFieldFormatError,
		unsigned &AttributeNum, unsigned &IndexNum) const;

	/* Helper functions for TranslateAttributes */
	CSSM_RETURN TranslateOneAttribute(
		CSSM_DB_ATTRIBUTE_DATA &Source,
		const DAL_TRANSLATED_ATTRIBUTE_LIST &DestinationList) const;

public:

#ifdef _DEBUG /* Debug functions to check the validity of the system and the object */
	DAL_OBJECT_STATE m_debug_ObjectState;
	void AssertValidNotInitialized(void) const; /* Initialize has NOT been called */
	void AssertValid(void) const; /* Initialize has been called */
#endif

	DAL_TRANSLATION_TABLE();
	~DAL_TRANSLATION_TABLE();
	CSSM_RETURN Initialize(CSSM_DB_RECORD_ATTRIBUTE_INFO const * AttributeInfo,
						  CSSM_DB_RECORD_INDEX_INFO const * IndexInfo);

	/* Retrieve count information */
	uint32 neGetIndexCount() const;
	uint32 neGetUniqueIndexCount() const;
	uint32 neGetDataIndexCount() const;
	uint32 neGetUniqueDataIndexCount() const;
	uint32 neGetAttributeIndexCount() const;
	uint32 neGetUniqueAttributeIndexCount() const;
	uint32 neGetTotalAttributeCount() const;
	uint32 neGetPureAttributeCount() const;

	/* This function will map a CSSM_DB_RECORD_ATTRIBUTE_DATA structure to a DAL_TRANSLATED_RECORD_ATTRIBUTE_DATA
	   structure, which contains attribute/index numbers instead of names */
	CSSM_RETURN TranslateAttributes(
		CSSM_DB_RECORD_ATTRIBUTE_DATA * Source,
		DAL_TRANSLATED_ATTRIBUTE_LIST &Destination) const;


	/* function to lookup information about an index */
	CSSM_RETURN LookupIndex(
		CSSM_DB_ATTRIBUTE_INFO const * IndexInfoPtr,
		uint32 &IndexNum,
		CSSM_DB_INDEXED_DATA_LOCATION &Location) const;

	/* function to get all oid's to pass to the parser to get index data */
	CSSM_RETURN neGetCurrentDataInfo(
		uint32 &Status,
		CSSM_BOOL UniqueRequired,
		uint32 &IndexNumber,
		DAL_CONST_DATA_PTR &IndexName,
		CSSM_DB_ATTRIBUTE_FORMAT &IndexFormat) const;

	CSSM_RETURN IsIndexUnique(unsigned IndexNum, uint32 *tmpbool) const;

	CSSM_DB_ATTRIBUTE_FORMAT GetIndexFormat(uint32 IndexNum) const;
	CSSM_DB_ATTRIBUTE_FORMAT GetAttributeFormat(uint32 AttributeNum) const;
};

typedef DAL_TRANSLATION_TABLE *DAL_TRANSLATION_TABLE_PTR;
typedef const DAL_TRANSLATION_TABLE DAL_CONST_TRANSLATION_TABLE;
typedef const DAL_TRANSLATION_TABLE *DAL_CONST_TRANSLATION_TABLE_PTR;

#endif
