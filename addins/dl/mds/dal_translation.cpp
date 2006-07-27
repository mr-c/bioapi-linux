/*-----------------------------------------------------------------------
 * File: DAL_TRANSLATION.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the implementation of DAL_TRANSLATION_TABLE. It also
 * contains the implementation of six classes that are manipulated by a
 * DAL_TRANSLATION_TABLE:
 *		DAL_TRANSLATED_ATTRIBUTE
 *		DAL_TRANSLATED_INPUT_ATTRIBUTE
 *		DAL_TRANSLATED_OUTPUT_ATTRIBUTE
 *		DAL_TRANSLATED_ATTRIBUTE_LIST
 *		DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST
 *		DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

#define DAL_SOURCE_FILE

#include "dal_translation.h"
#include "dlutil.h"
#include "dal_schema.h"

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE
 *
 * Description:
 * Initialize variables to zero, pointers to NULL
 *
 *---------------------------------------------------------------------------*/
DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE()
{
	START_CONSTRUCTOR();

	m_NumIndexesFromDataObject = 0;
	m_NumPureRecordAttributes = 0;
	m_NumRecordAttributes = 0;
	m_rgAttributeInfo = NULL;
	m_rgDataID = NULL;

	END_CONSTRUCTOR();
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE_NODE::DAL_TRANSLATION_TABLE_NODE
 *
 * Description:
 * Initializes a DAL_TRANSLATION_TABLE_NODE to default values
 *
 *---------------------------------------------------------------------------*/
DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE_NODE::DAL_TRANSLATION_TABLE_NODE()
{
	m_Name.Data = NULL;
	m_Name.Length = 0;
	m_IndexNumber = DAL_OOB;
	m_AttributeNameFormat = CSSM_DB_ATTRIBUTE_NAME_FORMAT(DAL_OOB);
	m_fUnique = CSSM_FALSE;
	m_FieldFormat = CSSM_DB_ATTRIBUTE_FORMAT(DAL_OOB);
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::~DAL_TRANSLATION_TABLE
 *
 * Description:
 * Deletes the AttributeInfo and the IndexInfo tables
 *
 *---------------------------------------------------------------------------*/
DAL_TRANSLATION_TABLE::~DAL_TRANSLATION_TABLE()
{
	AssertValid();

   if (m_rgAttributeInfo)
	  delete []m_rgAttributeInfo;

   if (m_rgDataID)
	  delete []m_rgDataID;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE_NODE::~DAL_TRANSLATION_TABLE_NODE
 *
 * Description:
 * Deletes the memory allocated for name if it is not NULL
 *
 *---------------------------------------------------------------------------*/
DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE_NODE::~DAL_TRANSLATION_TABLE_NODE()
{
	if (m_Name.Data)
		delete []m_Name.Data;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE_NODE::Initialize
 *
 * Description:
 * This initializes a node in the translation table by parsing a CSSM_DB_ATTRIBUTE_INFO
 * structure
 *
 * Parameters:
 * CSSM_DB_ATTRIBUTE_INFO (input) - Information about an index or an attribute that
 * will be used to initialize the node
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * CSSMERR_DL_MEMORY_ERROR
 * CSSMERR_DL_INTERNAL_ERROR
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE_NODE::Initialize(
	const CSSM_DB_ATTRIBUTE_INFO &AInfo)
{
	/* Insure that initialize has not been called mutiple times */
	ASSERT(!m_Name.Data && !m_Name.Length);

	m_AttributeNameFormat = AInfo.AttributeNameFormat;

	if (m_AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_STRING)
	{
		if (AInfo.Label.AttributeName)
		{
			uint32 len = strlen(AInfo.Label.AttributeName);
			if (len > 0)
			{
				m_Name.Data = new uint8[len+1];
				if (!m_Name.Data)
					return CSSMERR_DL_MEMORY_ERROR;
				m_Name.Length = len + 1;
				memcpy(m_Name.Data, AInfo.Label.AttributeName, m_Name.Length);
				m_FieldFormat = AInfo.AttributeFormat;

				return CSSM_OK;
			}
		}
	}
	else if (m_AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_OID)
	{
		if (AInfo.Label.AttributeOID.Data && AInfo.Label.AttributeOID.Length)
		{
			m_Name.Length = AInfo.Label.AttributeOID.Length;
			m_Name.Data = new uint8[m_Name.Length];
			if (!m_Name.Data)
				return CSSMERR_DL_MEMORY_ERROR;
			memcpy(m_Name.Data, AInfo.Label.AttributeOID.Data, m_Name.Length);
			m_FieldFormat = AInfo.AttributeFormat;

			return CSSM_OK;
		}
	}
	else if (m_AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_INTEGER)
	{
		m_Name.Length = sizeof(uint32);
		m_Name.Data = new uint8[m_Name.Length];
		if (!m_Name.Data)
			return CSSMERR_DL_MEMORY_ERROR;
		memcpy(m_Name.Data, &AInfo.Label.AttributeID, m_Name.Length);
		m_FieldFormat = AInfo.AttributeFormat;

		return CSSM_OK;
	}

	DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
	return CSSMERR_DL_INTERNAL_ERROR;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE_NODE::nrSetIndexInfo
 *
 * Description:
 * Sets the index information in the attributes of DataIndex table.
 *
 * Parameters:
 * IndexNum (input) : The new index number
 * type (input) : Describes what type of index the index is (unique/non-unique)
 *
 * Return value:
 * none.
 *
 * Error Codes:
 * none.
 *---------------------------------------------------------------------------*/
void DAL_TRANSLATION_TABLE::DAL_TRANSLATION_TABLE_NODE::nrSetIndexInfo(
	uint32 IndexNum,
	CSSM_DB_INDEX_TYPE type)
{
	m_IndexNumber = IndexNum;
	m_fUnique = CBOOL(type == CSSM_DB_INDEX_UNIQUE);
}


/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::Initialize
 *
 * Description:
 * Allocates internal variables by parsing a CSSM_DB_RECORD_ATTRIBUTE_INFO and a
 * CSSM_DB_RECORD_INDEX_INFO structure. These structures are assumed to be valid,
 * no pointer checking will be made. Also, the strucutures should be sorted already
 *
 * Parameters:
 * AttributeInfoPtr (input) : Information about the attributes of a datastore for
 *	  one particular recordtype
 * IndexInfoPtr (input) : Information about the indexes of a datastore for
 *	  one particular recordtype (same recordtype as AttributeInfoPtr)
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * CSSMERR_DL_INTERNAL_ERROR
 * CSSM_DL_INVALID_FIELD_NAME
 * CSSMERR_DL_MEMORY_ERROR
 * CSSM_DL_INVALID_RECORD_INDEX
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATION_TABLE::Initialize(
	CSSM_DB_RECORD_ATTRIBUTE_INFO const * AttributeInfoPtr,
	CSSM_DB_RECORD_INDEX_INFO const * IndexInfoPtr)
{
	CSSM_RETURN retValue = CSSM_OK;
	START_INITIALIZE();
	AssertValid();
	CSSM_DB_RECORD_ATTRIBUTE_INFO defaultAttributeInfo;
	CSSM_DB_RECORD_INDEX_INFO defaultIndexInfo;

	if (!AttributeInfoPtr || !IndexInfoPtr)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return (CSSMERR_DL_INTERNAL_ERROR);
	}

	memcpy((void *)&defaultAttributeInfo, (const void *)AttributeInfoPtr, sizeof(CSSM_DB_RECORD_ATTRIBUTE_INFO));
	memcpy((void *)&defaultIndexInfo, (const void *)IndexInfoPtr, sizeof(CSSM_DB_RECORD_INDEX_INFO));

	/* Make sure that Initialize is not run multiple times */
	ASSERT(!m_NumRecordAttributes && !m_NumIndexesFromDataObject);
	if (m_NumRecordAttributes || m_NumIndexesFromDataObject)
		return (CSSMERR_DL_INTERNAL_ERROR);

	ASSERT(defaultAttributeInfo.DataRecordType == defaultIndexInfo.DataRecordType);
	if (defaultAttributeInfo.DataRecordType != defaultIndexInfo.DataRecordType)
		return (CSSMERR_DL_INTERNAL_ERROR);

	uint32 MaxNumAttributes = defaultAttributeInfo.NumberOfAttributes;

	/* Check for the manditory attributes "PrintName" and "Alias" */
	if (MaxNumAttributes > 0)
	{
		MaxNumAttributes += 2;

		uint32 i;
		for (i = 0; i < defaultAttributeInfo.NumberOfAttributes; i++)
		{
			if (defaultAttributeInfo.AttributeInfo[i].AttributeNameFormat != CSSM_DB_ATTRIBUTE_NAME_AS_STRING)
				continue;

			if (strcmp(defaultAttributeInfo.AttributeInfo[i].Label.AttributeName, "Alias") == 0)
				MaxNumAttributes--;

			if (strcmp(defaultAttributeInfo.AttributeInfo[i].Label.AttributeName, "PrintName") == 0)
				MaxNumAttributes--;
		}
	}
	else
		MaxNumAttributes = 2; /* we need to add both manditory attributes */

	m_rgAttributeInfo = new DAL_TRANSLATION_TABLE_NODE[MaxNumAttributes];
	if (!m_rgAttributeInfo)
		return (CSSMERR_DL_MEMORY_ERROR);
	m_NumRecordAttributes = MaxNumAttributes;

	uint32 cRecordAttribute = 0;

	/* Now add the application defined indexes */
	uint32 i;
	for (i = 0; i < defaultAttributeInfo.NumberOfAttributes; i++)
	{
		ASSERT(cRecordAttribute <= MaxNumAttributes);
		uint32 AttributeNum, IndexNum;
		if ((retValue = LookupAttribute(&(defaultAttributeInfo.AttributeInfo[i]),
			CSSMERR_DL_INCOMPATIBLE_FIELD_FORMAT, AttributeNum, IndexNum)) != CSSM_OK)
		{
			return (retValue);
		}

		if (AttributeNum != DAL_OOB)
		{
			/* This attribute name has already been used */
			return (CSSMERR_DL_FIELD_SPECIFIED_MULTIPLE);
		}

		if ((retValue = m_rgAttributeInfo[cRecordAttribute++].Initialize(defaultAttributeInfo.AttributeInfo[i])) != CSSM_OK)
			return (retValue);
	}

	/* Add PrintName Attribute & Index */
	CSSM_DB_ATTRIBUTE_INFO PrintNameInfo;
	uint32 PrintNameArrayIndex, Junk;

	PrintNameInfo.AttributeNameFormat = CSSM_DB_ATTRIBUTE_NAME_AS_STRING;
	PrintNameInfo.Label.AttributeName = "PrintName";
	PrintNameInfo.AttributeFormat = CSSM_DB_ATTRIBUTE_FORMAT_STRING;

	if ((retValue = LookupAttribute(&PrintNameInfo, CSSM_OK, PrintNameArrayIndex, Junk)) != CSSM_OK)
		return (retValue);

	/* if "PrintName" was not added by the app, we need to add it ourself */
	if (PrintNameArrayIndex == DAL_OOB)
	{
		PrintNameArrayIndex = cRecordAttribute++;
		if ((retValue = m_rgAttributeInfo[PrintNameArrayIndex].Initialize(PrintNameInfo)) != CSSM_OK)
			return (retValue);
	}
	m_rgAttributeInfo[PrintNameArrayIndex].nrSetIndexInfo(0, CSSM_DB_INDEX_NONUNIQUE);

	/* Add Alias Attribute & Index */
	CSSM_DB_ATTRIBUTE_INFO AliasInfo;
	uint32 AliasArrayIndex;
	AliasInfo.AttributeNameFormat = CSSM_DB_ATTRIBUTE_NAME_AS_STRING;
	AliasInfo.Label.AttributeName = "Alias";
	AliasInfo.AttributeFormat = CSSM_DB_ATTRIBUTE_FORMAT_BLOB;
	if ((retValue = LookupAttribute(&AliasInfo, CSSM_OK, AliasArrayIndex, Junk)) != CSSM_OK)
		return (retValue);

	/* if "Alias" was not added by the app, we need to add it ourself */
	if (AliasArrayIndex == DAL_OOB)
	{
		AliasArrayIndex = cRecordAttribute++;
		if ((retValue = m_rgAttributeInfo[AliasArrayIndex].Initialize(AliasInfo)) != CSSM_OK)
			return (retValue);
	}
	m_rgAttributeInfo[AliasArrayIndex].nrSetIndexInfo(1, CSSM_DB_INDEX_NONUNIQUE);

	/* The first two indexes are print name / alias */
	uint32 cIndexNum = 2;

	/* We are done inserting attribute, so now we want to ignore extra attribute */
	ASSERT(cRecordAttribute <= MaxNumAttributes);
	m_NumRecordAttributes = cRecordAttribute;

	if (defaultIndexInfo.NumberOfIndexes > 0)
	{
		uint32 i;
		uint32 NumIndexesFromDataObject = 0;

		/* first count the number of indexes from the data */
		for (i = 0; i < defaultIndexInfo.NumberOfIndexes; i++)
		{
			CSSM_DB_INDEX_INFO_PTR IInfo = &(defaultIndexInfo.IndexInfo[i]);
			if (IInfo->IndexedDataLocation == CSSM_DB_INDEX_ON_RECORD)
				NumIndexesFromDataObject++;
		}

		/* now allocate structure */
		if (NumIndexesFromDataObject > 0)
		{
			m_rgDataID = new DAL_TRANSLATION_TABLE_NODE[NumIndexesFromDataObject];
			if (!m_rgDataID)
				return (CSSMERR_DL_MEMORY_ERROR);
		}
		m_NumIndexesFromDataObject = NumIndexesFromDataObject;

		uint32 cDataObjectIndex = 0, cIndexFromList;
		for (cIndexFromList = 0; cIndexFromList < defaultIndexInfo.NumberOfIndexes; cIndexFromList++)
		{
			CSSM_DB_INDEX_INFO_PTR IInfo = &(defaultIndexInfo.IndexInfo[cIndexFromList]);
			if (IInfo->IndexedDataLocation == CSSM_DB_INDEX_ON_ATTRIBUTE)
			{
				/* find the attribute */
				uint32 IndexNum, AttributeNum;
				if ((retValue = LookupAttribute(&IInfo->Info, CSSMERR_DL_INCOMPATIBLE_FIELD_FORMAT,
					AttributeNum, IndexNum)) != CSSM_OK)
				{
					return (retValue);
				}

				if (AttributeNum == DAL_OOB)
					/* If the attribute was not found in the attribute list */
					return (CSSMERR_DL_INVALID_RECORD_INDEX);

				/* Assign Index Number, uniqueness */
				if (IndexNum != DAL_OOB)
				{
					/* This attribute has already been set as an index */
					m_rgAttributeInfo[AttributeNum].m_fUnique = CBOOL(m_rgAttributeInfo[AttributeNum].m_fUnique
						|| IInfo->IndexType == CSSM_DB_INDEX_UNIQUE);
				}
				else
					m_rgAttributeInfo[AttributeNum].nrSetIndexInfo(cIndexNum++, IInfo->IndexType);
			}
			else if (IInfo->IndexedDataLocation == CSSM_DB_INDEX_ON_RECORD)
			{
				/* See if the index has already been used */
				uint32 ArrayIndex, Junk;
				if ((retValue = FindTableNode(
						&IInfo->Info, /* what to find */
						m_rgDataID, /* array to find it in */
						cDataObjectIndex, /* number of element in the array */
						CSSMERR_DL_FIELD_SPECIFIED_MULTIPLE, /* error code to set if the field formats don't match */
						ArrayIndex, /* index in the array (m_rgDataID) */
						Junk)) /* pure attribute, which we don't need */
						!= CSSM_OK)
				{
					return (retValue);
				}

				if (ArrayIndex != DAL_OOB)
				{
				/* This data index has already been added before */
					/* make sure the attribute format matches */
					ASSERT(m_rgDataID[ArrayIndex].m_FieldFormat == IInfo->Info.AttributeFormat);

					m_rgDataID[ArrayIndex].m_fUnique = CBOOL(m_rgDataID[ArrayIndex].m_fUnique ||
						IInfo->IndexType == CSSM_DB_INDEX_UNIQUE);
				}
				else
				{
				/* This data index has never been added before */
					if ((retValue = LookupAttribute(&IInfo->Info, CSSM_OK, ArrayIndex, Junk)) != CSSM_OK)
						return (retValue);

					if (ArrayIndex != DAL_OOB)
						/* This index is already an attribute, so it can not be used as an index on the
						data object */
						return (CSSMERR_DL_INVALID_RECORD_INDEX);

					if ((retValue = m_rgDataID[cDataObjectIndex].Initialize(IInfo->Info)) != CSSM_OK)
						return (retValue);
					m_rgDataID[cDataObjectIndex++].nrSetIndexInfo(cIndexNum++, IInfo->IndexType);
				}
			}

#ifdef _DEBUG
			else /* Index Data Location is not INDEX_ON_RECORD or INDEX_ON_ATTRIBUTE */
			{
				/* Should be detected earlier */
				DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
				return CSSMERR_DL_INTERNAL_ERROR;
			}
#endif
		}

		/* Inserting indexes is finished, so we want to ignore any extra indexes */
		ASSERT(m_NumIndexesFromDataObject >= cDataObjectIndex);
		m_NumIndexesFromDataObject = cDataObjectIndex;
	} /* end of if (IndexInfoPtr->NumberOfIndexes > 0) */

	/* Last, get a pure attribute count; */
	m_NumPureRecordAttributes = 0;
	for (i = 0; i < cRecordAttribute; i++)
	{
		if (m_rgAttributeInfo[i].m_IndexNumber == DAL_OOB)
			m_NumPureRecordAttributes++;
	}

	END_INITIALIZE();
	ASSERT(defaultIndexInfo.NumberOfIndexes + 2 >= cIndexNum);
	ASSERT(cIndexNum == neGetIndexCount());

	return (CSSM_OK);
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::neGetIndexCount
 *
 * Description/Return:
 * Returns the total number of indexes, counting both from attributes and the
 * data object (uint32 value)
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
uint32 DAL_TRANSLATION_TABLE::neGetIndexCount() const
{
	return m_NumRecordAttributes - m_NumPureRecordAttributes + m_NumIndexesFromDataObject;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::GetDataIndexCount
 *
 * Description/Return:
 * Returns the total number of indexes, counting only from the data object
 * (uint32 value)
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
uint32 DAL_TRANSLATION_TABLE::neGetDataIndexCount() const
{
	AssertValid();

	return m_NumIndexesFromDataObject;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::neneGetAttributeIndexCount
 *
 * Description/Return:
 * Returns the total number of indexes, counting only from attributes that are
 * used for indexing (uint32 value)
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
uint32 DAL_TRANSLATION_TABLE::neGetAttributeIndexCount() const
{
	AssertValid();

	return m_NumRecordAttributes - m_NumPureRecordAttributes;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::neGetUniqueIndexCount
 *
 * Description/Return:
 * Returns the total number of indexes, counting both from attributes and the
 * data object, that are required to be unique (uint32 value)
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
uint32 DAL_TRANSLATION_TABLE::neGetUniqueIndexCount() const
{
	AssertValid();

	return neGetUniqueDataIndexCount() + neGetUniqueAttributeIndexCount();
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::neGetUniqueDataIndexCount
 *
 * Description/Return:
 * Returns the total number of indexes, counting only indexes from the data object,
 * that are required to be unique (uint32 value)
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
uint32 DAL_TRANSLATION_TABLE::neGetUniqueDataIndexCount() const
{
	AssertValid();

	uint32 i, ret = 0;
	for (i = 0; i < m_NumIndexesFromDataObject; i++)
	{
		if ((m_rgDataID[i].m_IndexNumber != DAL_OOB) && m_rgDataID[i].m_fUnique)
			ret++;
	}

	return ret;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::neGetUniqueAttributeIndexCount
 *
 * Description/Return:
 * Returns the total number of indexes, counting only indexes from attributes,
 * that are required to be unique (uint32 value)
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
uint32 DAL_TRANSLATION_TABLE::neGetUniqueAttributeIndexCount() const
{
	AssertValid();

	uint32 i, ret = 0;
	for (i = 0; i < m_NumRecordAttributes; i++)
	{
		if ((m_rgAttributeInfo[i].m_IndexNumber != DAL_OOB) && m_rgAttributeInfo[i].m_fUnique)
			ret++;
	}

	return ret;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::neGetTotalAttributeCount
 *
 * Description/Return:
 * Returns the total number of attributes, regardless of if they are used for
 * indexing (uint32 value)
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
uint32 DAL_TRANSLATION_TABLE::neGetTotalAttributeCount() const
{
	AssertValid();

   return m_NumRecordAttributes;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::neGetPureAttributeCount
 *
 * Description/Return:
 * Returns the total number of attributes that are NOT used used for indexing
 * (these are known as pure attributes) (uint32 value)
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
uint32 DAL_TRANSLATION_TABLE::neGetPureAttributeCount() const
{
	AssertValid();

	return m_NumPureRecordAttributes;
}

/*-----------------------------------------------------------------------------
  * Name: DAL_TRANSLATION_TABLE::LookupAttribute
 *
 * Description:
 * This private function will lookup an Attribute in the attribute table and return
 * information about that attribute
 *
 * Parameters:
 * AttributePtr (input) : The attribute's name, this will be searched for in the attribute
 * NotMatchingFieldFormatError (input) Error code to set if the field format for the found element
 *	 does not match the expected
 * AttributeArrayIndex (output) : The array index of the attribute in the attribute table (DAL_OOB
 *	 if the attribute is not found
 * IndexNum (output) : The Index number for that attribute
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATION_TABLE::LookupAttribute(
	CSSM_DB_ATTRIBUTE_INFO const * AttributePtr,
	uint32 NotMatchingFieldFormatError,
	uint32 &AttributeArrayIndex,
	uint32 &IndexNum) const
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	/* Initialize Return Data */
	AttributeArrayIndex = DAL_OOB;

	/* Perform a find in the attributes array */
	uint32 Junk;
	if ((retValue = FindTableNode(AttributePtr, m_rgAttributeInfo, m_NumRecordAttributes,
		NotMatchingFieldFormatError, AttributeArrayIndex, Junk)) != CSSM_OK)
	{
		return retValue;
	}

	if (AttributeArrayIndex != DAL_OOB)
	{
		/* Attribute Found */
		IndexNum = m_rgAttributeInfo[AttributeArrayIndex].m_IndexNumber;
		return CSSM_OK;
	}
	else
	{
		/* Attribute Not Found */
		IndexNum = DAL_OOB;
		return CSSM_OK;
	}
}


/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::LookupIndex
 *
 * Description:
 * Lookup an index, if the the index is from the opaque data object and the name was
 *
 * Parameters:
 * IndexInfoPtr (input) : Key to seach for the index on
 * IndexNum (output) : index number of the index that was found (undefined if not found)
 * Location (output) : what the index is on (undefined if not found)
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATION_TABLE::LookupIndex(
		CSSM_DB_ATTRIBUTE_INFO const * IndexInfoPtr,
		uint32 &IndexNum,
		CSSM_DB_INDEXED_DATA_LOCATION &Location) const
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	/* Initialize Output */
	Location = CSSM_DB_INDEXED_DATA_LOCATION(DAL_OOB);
	IndexNum = DAL_OOB;
	uint32 ArrayIndex, Junk;

	/* First check the data object's indexes */
	if ((retValue = FindTableNode(
		IndexInfoPtr, /* what to find */
		m_rgDataID, /* array to find it in */
		m_NumIndexesFromDataObject, /* number of element in the array */
		CSSMERR_DL_INCOMPATIBLE_FIELD_FORMAT, /* error code to set if the field formats don't match */
		ArrayIndex, /* index in the array (m_rgDataID) */
		Junk)) /* pure attribute, which we don't need */
		!=CSSM_OK)
	{
		return retValue;
	}

	if (ArrayIndex != DAL_OOB)
	{
		/* The Index was found in list of indexes on the data object */
		IndexNum = m_rgDataID[ArrayIndex].m_IndexNumber;
		Location = CSSM_DB_INDEX_ON_RECORD;
		ASSERT(IndexNum != DAL_OOB);
		return CSSM_OK;
	}

	/* Now check on Attribute indexes */
	if ((retValue = FindTableNode(
		IndexInfoPtr, /* what to find */
		m_rgAttributeInfo, /* array to find it in */
		m_NumRecordAttributes, /* number of element in the array */
		CSSMERR_DL_INCOMPATIBLE_FIELD_FORMAT, /* error code to set if the field formats don't match */
		ArrayIndex, /* index in the array (m_rgAttributeInfo) */
		Junk)) /* pure attribute, which we don't need */
		!=CSSM_OK)
	{
		return retValue;
	}

	if (ArrayIndex != DAL_OOB)
	{
		IndexNum = m_rgAttributeInfo[ArrayIndex].m_IndexNumber;
		if (IndexNum != DAL_OOB)
		{
			/* The Index was found in list of attributes, and it is an index */
			Location = CSSM_DB_INDEX_ON_ATTRIBUTE;
			return CSSM_OK;
		}
		/* The Index was found in list of attributes, and it is not an index */
	}

	/* The index was not in either table */
	return CSSMERR_DL_INVALID_FIELD_NAME;
}


/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::FindTableNode
 *
 * Description:
 * This function will find an index table node in an array of index table nodes.
 * It is a private function that is used by LookupAttribute and LookupIndex
 *
 * Parameters:
 * KeyInfoPtr (input) : Information that describes the node that will be searched on
 * rgTableNode (input) : Array where we should look to find KeyInfoPtr
 * NumNodes (input) : Number of nodes in rgTableNode
 * NotMatchingFieldFormatError (input) : Error code to set if the field formats do not
 * match, if CSSM_OK, unmatching fields is not considered an error
 * ArrayIndexNum (output) : The Array index of the found node, or 0xffffffff if the
 *	 node is not found
 * PureAttributeNum (output) : The number of nodes (starting at zero) that would be
 *	 pure attributes (their IndexNumber Field == DAL_OOB)
 *
 * Return value:
 * CSSM_OK if no error occured (node not found is not an error), otherwise CSSM_FAIL
 *
 * Error Codes:
 * CSSMERR_DL_INTERNAL_ERROR
 * NotMatchingFieldFormatError
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATION_TABLE::FindTableNode(
	CSSM_DB_ATTRIBUTE_INFO const *KeyInfoPtr,
	DAL_TRANSLATION_TABLE_NODE const *rgTableNode,
	uint32 NumNodes,
	uint32 NotMatchingFieldFormatError,
	uint32 &ArrayIndexNum,
	uint32 &PureAttributeNum) const
{
	AssertValid();

	/* Initialize Return Data */
	ArrayIndexNum = DAL_OOB;
	PureAttributeNum = DAL_OOB;

	if (KeyInfoPtr == NULL)
		return CSSMERR_DL_INTERNAL_ERROR;

	CSSM_DATA key;
	if (KeyInfoPtr->AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_STRING)
	{
		key.Data = (uint8 *)KeyInfoPtr->Label.AttributeName;
		key.Length = strlen(KeyInfoPtr->Label.AttributeName) + 1;
	}
	else if (KeyInfoPtr->AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_OID)
	{
		key.Data = KeyInfoPtr->Label.AttributeOID.Data;
		key.Length = KeyInfoPtr->Label.AttributeOID.Length;
	}
	else if (KeyInfoPtr->AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_INTEGER)
	{
		key.Data = (unsigned char *)&KeyInfoPtr->Label.AttributeID;
		key.Length = sizeof(uint32);
	}
	else
	{
		/* The node is invalid, this should have been found sooner */
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	uint32 i;
	for (i = 0; i < NumNodes; i++)
	{
		if (rgTableNode[i].m_IndexNumber == DAL_OOB)
			PureAttributeNum++;

		if (rgTableNode[i].m_AttributeNameFormat != KeyInfoPtr->AttributeNameFormat)
			continue;

		uint32 len = rgTableNode[i].m_Name.Length;
		if (len == key.Length)
		{
			if (len != 0)
			{
				if (memcmp(rgTableNode[i].m_Name.Data, key.Data, len) != 0)
					continue;
			}

			if (NotMatchingFieldFormatError != CSSM_OK)
			{
				if (rgTableNode[i].m_FieldFormat != KeyInfoPtr->AttributeFormat)
					return NotMatchingFieldFormatError;
			}

			ArrayIndexNum = i;
			return CSSM_OK;
		}
	} /* End for */

	/* Node Not Found */
	return CSSM_OK;
}


/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::neGetCurrentDataInfo
 *
 * Description:
 * Returns information about the current index from that data object. If the current
 * index is not unique and a unique object is required, then the current index is
 * advanced until there are no longer indexes from data object left, or until a
 * unique index is found.
 *
 * Parameters:
 * Status(input/ouput) : indicates the position of the last find. If this is the first find
 *	 it should equal DAL_OOB.
 * UniqueRequired (input) : if TRUE, only information about a unique index will be returned
 * IndexNumber (output) : returns the index number (including attribute indexes)
 * Id (output) : returns a pointer to the internal Index Name (this should not be modified)
 *
 * Return value:
 * CSSM_OK if an index is found, otherwise CSSM_FAIL
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATION_TABLE::neGetCurrentDataInfo(
	uint32 &Status,
	CSSM_BOOL UniqueRequired,
	uint32 &IndexNumber,
	DAL_CONST_DATA_PTR &IndexName,
	CSSM_DB_ATTRIBUTE_FORMAT &IndexFormat) const
{
	AssertValid();

	if (Status == DAL_OOB)
		Status = 0;
	else
		Status++;

	while (Status < m_NumIndexesFromDataObject)
	{
		if (UniqueRequired && !m_rgDataID[Status].m_fUnique)
			Status++; /* Advance the current to the next index */
		else
		{
			IndexNumber = m_rgDataID[Status].m_IndexNumber;
			IndexName = &m_rgDataID[Status].m_Name;
			IndexFormat = m_rgDataID[Status].m_FieldFormat;
			return CSSM_OK;
		}
	}

	Status = DAL_OOB;
	IndexNumber = DAL_OOB;
	IndexName = NULL;
	IndexFormat = CSSM_DB_ATTRIBUTE_FORMAT(DAL_OOB);
	return CSSMERR_DL_INTERNAL_ERROR;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::IsIndexUnique
 *
 * Description:
 * This function will lookup an index number and see if that index is unique.
 *
 * Parameters:
 * IndexNum (input) : The number of the index (including indexes from attributes)
 *	 to lookup.
 *
 * Return value:
 * True if the index is unique, False on error or if the index is not unique
 *
 * Error Codes:
 * CSSM_DL_INVALID_FIELD_NAME
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATION_TABLE::IsIndexUnique(uint32 IndexNum,
	 uint32 *tmpbool) const
{
	AssertValid();

	uint32 i;

	if (neGetIndexCount() <= IndexNum)
		return CSSMERR_DL_INVALID_FIELD_NAME;

	for (i = 0; i < m_NumIndexesFromDataObject; i++)
	{
		if (m_rgDataID[i].m_IndexNumber > IndexNum)
			break;
		else if (m_rgDataID[i].m_IndexNumber == IndexNum)
		{
			*tmpbool = m_rgDataID[i].m_fUnique;
			return CSSM_OK;
		}
	}

	for (i = 0; i < m_NumRecordAttributes; i++)
	{
		if (m_rgAttributeInfo[i].m_IndexNumber == IndexNum)
		{
			*tmpbool = m_rgAttributeInfo[i].m_fUnique;
			return CSSM_OK;
		}
	}

	/* The index was not found (which should not be possible) */
	DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
	return CSSMERR_DL_INVALID_FIELD_NAME;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::GetIndexFormat
 *
 * Description:
 * This function will lookup an index number and return the field format (data type)
 * of that index
 *
 * Parameters:
 * IndexNum (input) : The number of the index (including indexes from attributes)
 *	 to lookup.
 *
 * Return value:
 * The field format or DAL_ATTRIBUTE_FORMAT_OOB if the index was not found
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_DB_ATTRIBUTE_FORMAT DAL_TRANSLATION_TABLE::GetIndexFormat(uint32 IndexNum) const
{
	AssertValid();

	uint32 i;

	if (neGetIndexCount() <= IndexNum)
	{
		return DAL_ATTRIBUTE_FORMAT_OOB;
	}

	for (i = 0; i < m_NumIndexesFromDataObject; i++)
	{
		if (m_rgDataID[i].m_IndexNumber > IndexNum)
			break;
		else if (m_rgDataID[i].m_IndexNumber == IndexNum)
		{
			return m_rgDataID[i].m_FieldFormat;
		}
	}

	for (i = 0; i < m_NumRecordAttributes; i++)
	{
		if (m_rgAttributeInfo[i].m_IndexNumber == IndexNum)
		{
			return m_rgAttributeInfo[i].m_FieldFormat;
		}
	}

	/* The index was not found (which should not be possible) */
	DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
	return DAL_ATTRIBUTE_FORMAT_OOB;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::GetAttributeFormat
 *
 * Description:
 * This function will lookup an attribute number and return the field format (data type)
 * of that attribute
 *
 * Parameters:
 * IndexNum (input) : The number of the attribute (not including indexes)
 *
 * Return value:
 * The field format or DAL_ATTRIBUTE_FORMAT_OOB if the index was not found
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_DB_ATTRIBUTE_FORMAT DAL_TRANSLATION_TABLE::GetAttributeFormat(uint32 AttributeNum) const
{
	AssertValid();

	uint32 index, cAttributeNum;

	if (neGetPureAttributeCount() <= AttributeNum)
	{
		return DAL_ATTRIBUTE_FORMAT_OOB;
	}

	for (index = 0, cAttributeNum = (uint32)-1; index < m_NumRecordAttributes; index++)
	{
		if (m_rgAttributeInfo[index].m_IndexNumber == DAL_OOB)
		{
			cAttributeNum++;

			if (cAttributeNum == AttributeNum)
				return m_rgAttributeInfo[index].m_FieldFormat;
		}
	}

	/* The index was not found (which should not be possible) */
	DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
	return DAL_ATTRIBUTE_FORMAT_OOB;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::TranslateAttributes
 *
 * Description:
 * This function is used to translate a CSSM_DB_RECORD_ATTRIBUTE_DATA structure
 * to a DAL_TRANSLATED_RECORD_ATTRIBUTE_DATA structure. These two structures are
 * highly similar; the difference is that in the DAL_TRANSLATED_RECORD_ATTRIBUTE_DATA
 * structure, Attribute Names are replaced by index and attribute numbers.
 *
 * Parameters:
 * Source (input/optional) : The DAL_TRANSLATED_RECORD_ATTRIBUTE_DATA structure that contains the info
 *	 to be translated
 * Destination (output): The translated data
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Note: Source == NULL is not a fail condition, also note that a pointer to the
 * Value of the index is saved for direct modification.
 *
 * Error Codes:
 * TODO
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATION_TABLE::TranslateAttributes(
	CSSM_DB_RECORD_ATTRIBUTE_DATA * Source,
	DAL_TRANSLATED_ATTRIBUTE_LIST &Destination) const
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	uint32 *pSemanticInformation = NULL;

	if (NULL != Source)
		pSemanticInformation = &Source->SemanticInformation;

	if ((retValue = Destination.Initialize(pSemanticInformation, neGetTotalAttributeCount())) != CSSM_OK)
		return retValue;

	/* Initialize each attribute's Unique, IndexNum, and AttributeNum fields */
	uint32 i, PureAttributeNum = 0;
	for (i = 0; i < neGetTotalAttributeCount(); i++)
	{
		DAL_TRANSLATED_ATTRIBUTE * pDestinationAttr = &Destination.neGetAttribute(i);
		const struct DAL_TRANSLATION_TABLE_NODE * pSourceAttr = &m_rgAttributeInfo[i];

		uint32 IndexNum = pSourceAttr->m_IndexNumber;
		if (IndexNum != DAL_OOB)
		{
			/* the i'th attribute is used as an index, so attribute num is DAL_OOB */
			pDestinationAttr->nrInitialize(IndexNum, DAL_OOB, pSourceAttr->m_fUnique,
				pSourceAttr->m_FieldFormat);
			/* Validity check: an index can not have an index number of DAL_OOB */
			ASSERT(pDestinationAttr->neGetIndexNumber() != DAL_OOB);
		}
		else
		{
			/* the i'th attribute is NOT used as an index, so index num is DAL_OOB */
			pDestinationAttr->nrInitialize(DAL_OOB, PureAttributeNum, pSourceAttr->m_fUnique,
				pSourceAttr->m_FieldFormat);
			PureAttributeNum++;

			/* Validity check: An attribute that is not used for indexing can not be unique */
			ASSERT(pDestinationAttr->neIsUnique() == CSSM_FALSE);
		}
	} /* End for */

	if (Source)
	{
		/* Translate the indexes and fill in pValue */
		for (i	= 0; i < Source->NumberOfAttributes; i++)
		{
			if ((retValue = TranslateOneAttribute(Source->AttributeData[i], Destination)) != CSSM_OK)
				return retValue;
		}
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATION_TABLE::TranslateOneAttribute
 *
 * Description:
 * This private function is a helper function for TranslateAttributes, it is used to translate
 * a CSSM_DB_ATTRIBUTE_DATA structure to a DAL_TRANSLATED_ATTRIBUTE_DATA_PTR structure.
 * These two structures are highly similar; the difference is that in the
 * DAL_TRANSLATED_ATTRIBUTE_DATA_PTR structure, Attribute Names are replaced by index and
 * attribute numbers.
 *
 * Parameters:
 * Source (input) : The DAL_TRANSLATED_ATTRIBUTE_DATA structure that contains the info
 *	 to be translated
 * rgAttributeData (output): The translated data
 *
 * Return value:
 * CSSM_OK on success, otherwise CSSM_FAIL
 *
 * Error Codes:
 * CSSM_DL_INVALID_FIELD_NAME
 * __CSSM_DL_INVALID_ATTRIBUTE
 * CSSM_DL_FIELD_SPECIFIED_MULTIPLE
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATION_TABLE::TranslateOneAttribute(
	CSSM_DB_ATTRIBUTE_DATA &Source,
	const DAL_TRANSLATED_ATTRIBUTE_LIST &DestinationList) const
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	/* Perform a find in the attributes array */
	uint32 ArrayIndex, PureAttributeNum;
	if ((retValue = FindTableNode(&Source.Info, m_rgAttributeInfo, m_NumRecordAttributes,
		CSSMERR_DL_INCOMPATIBLE_FIELD_FORMAT, ArrayIndex, PureAttributeNum)) != CSSM_OK)
	{
		return retValue;
	}

	if (ArrayIndex != DAL_OOB)
	{
		DAL_TRANSLATED_ATTRIBUTE_PTR pAttrib = &DestinationList.neGetAttribute(ArrayIndex);

		if (CSSM_OK != pAttrib->neInitializeValue(&Source.Value, &Source.NumberOfValues))
			return CSSMERR_DL_FIELD_SPECIFIED_MULTIPLE;

		ASSERT(pAttrib->neGetIndexNumber() != DAL_OOB || PureAttributeNum == pAttrib->neGetAttributeNumber());
		ASSERT(pAttrib->neGetIndexNumber() != DAL_OOB || pAttrib->neGetAttributeNumber() != DAL_OOB); /* Validity check */

		/* Validity check: An attribute that is not used for indexing can not be unique */
		ASSERT(pAttrib->neGetIndexNumber() != DAL_OOB || !pAttrib->neIsUnique());
		return CSSM_OK;
	}

	/* Attribute Not Found */
	return CSSMERR_DL_INVALID_FIELD_NAME;
}


/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_ATTRIBUTE_LIST::DAL_TRANSLATED_ATTRIBUTE_LIST
 *
 * Description:
 * Initialize variables to zero, pointers to NULL
 *
 *---------------------------------------------------------------------------*/
DAL_TRANSLATED_ATTRIBUTE_LIST::DAL_TRANSLATED_ATTRIBUTE_LIST(void)
{
	m_rgAttributes = NULL;
	m_NumberOfAttributes = 0;
	m_pSemanticInformation = NULL;
	m_fInitialized = CSSM_FALSE;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_ATTRIBUTE_LIST::~DAL_TRANSLATED_ATTRIBUTE_LIST
 *
 * Description:
 * Frees the AttributeData Array
 *
 *---------------------------------------------------------------------------*/
DAL_TRANSLATED_ATTRIBUTE_LIST::~DAL_TRANSLATED_ATTRIBUTE_LIST(void)
{
	AssertValid();

	if (m_rgAttributes)
		delete []m_rgAttributes;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_ATTRIBUTE_LIST::neGetAttribute
 *
 * Description:
 * Return's the index-th element in the array
 *
 *---------------------------------------------------------------------------*/
DAL_TRANSLATED_ATTRIBUTE& DAL_TRANSLATED_ATTRIBUTE_LIST::neGetAttribute(uint32 index) const
{
	ASSERT(neIsInitialized());
	ASSERT(index < m_NumberOfAttributes);
	if (index < m_NumberOfAttributes)
		return m_rgAttributes[index];
	else
		return m_rgAttributes[0];
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_ATTRIBUTE::DAL_TRANSLATED_ATTRIBUTE
 *
 * Description:
 * Placeholder for Initialize
 *
 * Parameters:
 * pSemanticInformation (input) : Address of the SemanticInformation in the
 * CSSM_DB_RECORD_ATTRIBUTE_DATA struct
 * NumberOfAttributes (input) : Number of Attribute in the CSSM_DB_RECORD_ATTRIBUTE_DATA
 * struct
 *
 * Return Value:
 * CSSM_FAIL if an error occurred (error code will be set), else CSSM_OK
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATED_ATTRIBUTE_LIST::Initialize(
	uint32 *SemanticInformation, uint32 NumberOfAttributes)
{
	/* This function should never be called - only placeholder */
	DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
	ASSERT(SemanticInformation);
	ASSERT(NumberOfAttributes);
	return CSSMERR_DL_INTERNAL_ERROR;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_INPUT_ATTRIBUTE::Initialize
 *
 * Description:
 * Initializes a translated input object; needed to call new operator for
 * DAL_TRANSLATED_INPUT_ATTRIBUTE
 *
 * Parameters:
 * pSemanticInformation (input) : Address of the SemanticInformation in the
 * CSSM_DB_RECORD_ATTRIBUTE_DATA struct
 * NumberOfAttributes (input) : Number of Attribute in the CSSM_DB_RECORD_ATTRIBUTE_DATA
 * struct
 *
 * Return Value:
 * CSSM_FAIL if an error occurred (error code will be set), else CSSM_OK
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST::Initialize(
	uint32 *pSemanticInformation, uint32 NumberOfAttributes)
{
	/* Insure class is not initialized twice */
	ASSERT(m_fInitialized == CSSM_FALSE);

	/*
		This was already checked in the copy constructor, which is the
		only place to call this Initialize function. So its commented out.
		if (m_fInitialized != CSSM_FALSE)
			return CSSMERR_DL_INTERNAL_ERROR;
	*/

	m_pSemanticInformation = pSemanticInformation;
	m_NumberOfAttributes = NumberOfAttributes;

	if (m_NumberOfAttributes)
	{
		m_rgAttributes = new DAL_TRANSLATED_INPUT_ATTRIBUTE[m_NumberOfAttributes];

		if (!m_rgAttributes)
			return CSSMERR_DL_MEMORY_ERROR;
	}

	m_fInitialized = CSSM_TRUE;
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_OUTPUT_ATTRIBUTE::DAL_TRANSLATED_ATTRIBUTE
 *
 * Description:
 * Initializes a translated input object; needed to call new operator for
 * DAL_TRANSLATED_OUTPUT_ATTRIBUTE
 *
 * Parameters:
 * pSemanticInformation (input) : Address of the SemanticInformation in the
 * CSSM_DB_RECORD_ATTRIBUTE_DATA struct
 * NumberOfAttributes (input) : Number of Attribute in the CSSM_DB_RECORD_ATTRIBUTE_DATA
 * struct
 *
 * Return Value:
 * CSSM_FAIL if an error occurred (error code will be set), else CSSM_OK
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST::Initialize(
	uint32 *pSemanticInformation, uint32 NumberOfAttributes)
{
	/* Insure class is not initialized twice */
	ASSERT(!m_fInitialized);
	if (m_fInitialized)
		return CSSMERR_DL_INTERNAL_ERROR;

	m_pSemanticInformation = pSemanticInformation;
	m_NumberOfAttributes = NumberOfAttributes;

	if (m_NumberOfAttributes)
	{
		m_rgAttributes = new DAL_TRANSLATED_OUTPUT_ATTRIBUTE[m_NumberOfAttributes];

		if (!m_rgAttributes)
			return CSSMERR_DL_MEMORY_ERROR;
	}

	m_fInitialized = CSSM_TRUE;
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_ATTRIBUTE::DAL_TRANSLATED_ATTRIBUTE
 *
 * Description:
 * Initializes everything to a default value
 *
 *---------------------------------------------------------------------------*/
DAL_TRANSLATED_ATTRIBUTE::DAL_TRANSLATED_ATTRIBUTE()
{
	m_fUnique = CSSM_FALSE;
	m_IndexNumber = DAL_OOB;
	m_AttributeNumber = DAL_OOB;
	m_FieldFormat = CSSM_DB_ATTRIBUTE_FORMAT(DAL_OOB);
	m_pValue = NULL;
	m_pNumberOfValues = 0;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_ATTRIBUTE::DAL_TRANSLATED_ATTRIBUTE
 *
 * Description:
 * Initializes m_IndexNumber, m_AttributeNumber, and m_fUnique
 *
 * Parameters:
 * IndexNumber (input) :  The IndexNumber to asign to the attribute
 * AttributeNumer (input) : The AttributeNumber to assign to the attribute
 * IsUnique (input) : The Uniqueness status to assign to the attribute
 *---------------------------------------------------------------------------*/
void DAL_TRANSLATED_ATTRIBUTE::nrInitialize(
	uint32 IndexNumber,
	uint32 AttributeNumber,
	CSSM_BOOL IsUnique,
	CSSM_DB_ATTRIBUTE_FORMAT FieldFormat)
{
	m_fUnique = IsUnique;
	m_IndexNumber = IndexNumber;
	m_AttributeNumber = AttributeNumber;
	m_FieldFormat = FieldFormat;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST::GetSemanticInformation
 *
 * Description:
 * Returns the Semantic Information specified in the AttributeList structure
 *
 *---------------------------------------------------------------------------*/
uint32 DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST::GetSemanticInformation() const
{
	ASSERT(m_pSemanticInformation);
	if (!m_pSemanticInformation)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return 0;
	}

	return *m_pSemanticInformation;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST::nrSetSemanticInformation
 *
 * Description:
 * Sets the Semantic Information inside the AttributeList paramater to an entry point
 *
 * Parameters:
 *	 SemanticInformation (input) :	Value of SemanticInformation to assign to the
 * SemanticInformation in the CSSM_DB_RECORD_ATTRIBUTE_DATA that was passed to
 * InitializeAttribtues
 *---------------------------------------------------------------------------*/
void DAL_TRANSLATED_OUTPUT_ATTRIBUTE_LIST::nrSetSemanticInformation(uint32 SemanticInformation)
{
	ASSERT(neIsInitialized());
	ASSERT(m_pSemanticInformation);

	if (m_pSemanticInformation)
		*m_pSemanticInformation = SemanticInformation;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST::DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST(DAL_TRANSLATED_ATTRIBUTE_LIST)
 *
 * Description:
 * Copy constructor, will be used with IsLegalDataModify
 *
 *---------------------------------------------------------------------------*/
DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST::DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST(
	const DAL_TRANSLATED_INPUT_ATTRIBUTE_LIST &Source)
{
	if ((Source.neIsInitialized() == CSSM_FALSE) ||
		CSSM_OK != Initialize(Source.m_pSemanticInformation, Source.m_NumberOfAttributes))
	{
		m_pSemanticInformation = NULL;
		m_NumberOfAttributes = 0;
		m_rgAttributes = NULL;
		m_fInitialized = CSSM_FALSE;
		return;
	}
	m_fInitialized = CSSM_TRUE;

	ASSERT(m_rgAttributes);
	ASSERT(Source.m_rgAttributes);
	memcpy(m_rgAttributes, Source.m_rgAttributes, sizeof(DAL_TRANSLATED_ATTRIBUTE) *
		neGetNumberOfAttributes());
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_INPUT_ATTRIBUTE::DAL_TRANSLATED_INPUT_ATTRIBUTE
 *
 * Description:
 * Initializes the Value paramater for an attribute; This will be filled with
 * the address of 'Value' in the CSSM_DB_ATTRIBUTE_DATA structure.
 *
 * Parameters:
 * pValue (input) : Address of the location of the Value of an attribute in
 * the CSSM_DB_RECORD_ATTRIBUTE_DATA structure, this will be stored to read from
 *
 * Return Value:
 * CSSM_FAIL if an error occurred (error code will be set), else CSSM_OK
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATED_INPUT_ATTRIBUTE::neInitializeValue(CSSM_DATA_PTR *pValue,
															  uint32 *pNumberOfValues)
{
	if (neWasInput())
	{
		/* If an attribute is specified multiple times */
		if (pValue[0]->Length != m_pValue[0]->Length ||
			0 != memcmp(pValue[0]->Data, m_pValue[0]->Data, pValue[0]->Length))
		{
			return CSSMERR_DL_INTERNAL_ERROR;
		}

		/* the attributes are the same, so ignore the error */
	}
	else {
		m_pValue = pValue;
		m_pNumberOfValues = pNumberOfValues;
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_TRANSLATED_INPUT_ATTRIBUTE::DAL_TRANSLATED_INPUT_ATTRIBUTE
 *
 * Description:
 * Initializes the Value paramater for an attribute; This will be filled with
 * the address of 'Value' in the CSSM_DB_ATTRIBUTE_DATA structure.
 *
 * Parameters:
 * pValue (input) : Address of the location of the Value of an attribute in
 * the CSSM_DB_RECORD_ATTRIBUTE_DATA structure, this will be stored to write to
 *
 * Return Value:
 * CSSM_FAIL if an error occurred (error code will be set), else CSSM_OK
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_TRANSLATED_OUTPUT_ATTRIBUTE::neInitializeValue(CSSM_DATA_PTR *pValue,
																uint32 *pNumberOfValues)
{
	if (neWasRequested()) {
		/* Attribute Duplicated - illegal */
		return CSSMERR_DL_INTERNAL_ERROR;
	} else {
		m_pValue = pValue;
		m_pNumberOfValues = pNumberOfValues;
	}
	return CSSM_OK;
}

CSSM_RETURN DAL_TRANSLATED_OUTPUT_ATTRIBUTE::neGetWriteableValuePtr(CSSM_HANDLE DlHandle,
																	CSSM_DATA_PTR *pValue)
{
	CSSM_DATA_PTR Value;

	Value = (CSSM_DATA_PTR) App_Calloc(DlHandle, sizeof(CSSM_DATA), 1);
	if (NULL == Value)
		return CSSMERR_DL_MEMORY_ERROR;
	*m_pValue = Value;
	*pValue = Value;
	*m_pNumberOfValues = 1;
	return CSSM_OK;
}

#ifdef _DEBUG

void DAL_TRANSLATED_ATTRIBUTE_LIST::AssertValid(void) const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_TRANSLATED_ATTRIBUTE_LIST)));
	if (m_NumberOfAttributes)
	{
		ASSERT_VALID_HEAP_POINTER(m_rgAttributes, sizeof(DAL_TRANSLATED_ATTRIBUTE) *
			m_NumberOfAttributes);
	}
}

void DAL_TRANSLATION_TABLE::AssertValidNotInitialized(void) const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_TRANSLATION_TABLE)));
	ASSERT(m_NumIndexesFromDataObject == 0);
	ASSERT(m_NumPureRecordAttributes == 0);
	ASSERT(m_NumRecordAttributes == 0);
	ASSERT(m_rgAttributeInfo == NULL);
	ASSERT(m_rgDataID == NULL);
}

void DAL_TRANSLATION_TABLE::AssertValid(void) const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_TRANSLATION_TABLE)));

	if (m_debug_ObjectState == Constructing)
		return;
	if (m_debug_ObjectState == NotInitialized)
	{
		AssertValidNotInitialized();
		return;
	}

	ASSERT(m_NumIndexesFromDataObject < 300);
	ASSERT(m_NumPureRecordAttributes < 300);
	ASSERT(m_NumRecordAttributes < 300);
	ASSERT(m_NumPureRecordAttributes <= m_NumRecordAttributes);

	if (m_debug_ObjectState == Initialized)
	{
		if (m_NumRecordAttributes)
		{
			ASSERT_VALID_HEAP_POINTER(m_rgAttributeInfo, sizeof(DAL_TRANSLATION_TABLE_NODE) *
				m_NumRecordAttributes);

			uint32 i;
			for (i = 0; i < m_NumRecordAttributes; i++)
			{
				ASSERT(m_rgAttributeInfo[i].m_AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_STRING ||
					m_rgAttributeInfo[i].m_AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_OID);
				ASSERT(m_rgAttributeInfo[i].m_IndexNumber != DAL_OOB || !m_rgAttributeInfo[i].m_fUnique);
				ASSERT(m_rgAttributeInfo[i].m_IndexNumber == DAL_OOB || m_rgAttributeInfo[i].m_IndexNumber < neGetIndexCount());
				ASSERT(m_rgAttributeInfo[i].m_Name.Length);
				ASSERT_VALID_HEAP_POINTER(m_rgAttributeInfo[i].m_Name.Data, m_rgAttributeInfo[i].m_Name.Length);
			}
		}
		else
			ASSERT(NULL == m_rgAttributeInfo);

		if (m_NumIndexesFromDataObject)
		{
			ASSERT_VALID_HEAP_POINTER(m_rgDataID, sizeof(DAL_TRANSLATION_TABLE_NODE) *
				m_NumIndexesFromDataObject);

			uint32 i;
			for (i = 0; i < m_NumIndexesFromDataObject; i++)
			{
				ASSERT(m_rgDataID[i].m_AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_STRING ||
					m_rgDataID[i].m_AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_OID);
				ASSERT(m_rgDataID[i].m_IndexNumber < neGetIndexCount());
				ASSERT(m_rgDataID[i].m_Name.Length);
				ASSERT_VALID_HEAP_POINTER(m_rgDataID[i].m_Name.Data, m_rgDataID[i].m_Name.Length);
			}
		}
		else
			ASSERT(NULL == m_rgDataID);
	}
	else
	{
		if (m_NumRecordAttributes)
		{
			ASSERT_VALID_HEAP_POINTER(m_rgAttributeInfo, sizeof(DAL_TRANSLATION_TABLE_NODE) *
				m_NumRecordAttributes);
		}
		else
			ASSERT(NULL == m_rgAttributeInfo);

		if (m_NumIndexesFromDataObject)
		{
			ASSERT_VALID_HEAP_POINTER(m_rgDataID, sizeof(DAL_TRANSLATION_TABLE_NODE) *
				m_NumIndexesFromDataObject);
		}
		else
			ASSERT(NULL == m_rgDataID);
	}
}

#endif /* End debug code */
