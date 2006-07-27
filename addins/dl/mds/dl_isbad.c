/*-----------------------------------------------------------------------
 * File: DL_ISBAD.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains functions which check that all of the pointers inside of
 * a given data structure are valid.
 */

#include <assert.h>
#include "dlutil.h"
#include "mds.h"

#define IS_OPEN_GROUP_RECORDTYPE(rt) \
	((rt) >= CSSM_DB_RECORDTYPE_OPEN_GROUP_START && (rt) <= CSSM_DB_RECORDTYPE_OPEN_GROUP_END)

#define IS_APP_DEFINED_RECORDTYPE(rt) \
	((rt) >= CSSM_DB_RECORDTYPE_APP_DEFINED_START && (rt) <= CSSM_DB_RECORDTYPE_APP_DEFINED_END)

#define IS_SCHEMA_RECORDTYPE(rt) \
	((rt) >= CSSM_DB_RECORDTYPE_SCHEMA_START && (rt) <= CSSM_DB_RECORDTYPE_SCHEMA_END)

#define IS_MDS_RECORDTYPE(rt) \
	((rt) >= CSSM_DB_RELATIONID_MDS_START && (rt) <= CSSM_DB_RELATIONID_MDS_END)

unsigned dlutil_IsUnsupportedRecordType(CSSM_DB_RECORDTYPE rt)
{
	return (!IS_OPEN_GROUP_RECORDTYPE(rt) &&
			!IS_APP_DEFINED_RECORDTYPE(rt) &&
			!IS_SCHEMA_RECORDTYPE(rt) &&
			!IS_MDS_RECORDTYPE(rt));
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsBadStrPtr
 *
 * Description:
 * This function verifies that all of the characters up to the first NULL
 * are printable and that this is a valid string buffer
 *
 * Parameters:
 * StringIn (input) : The character array to be verified
 * fInvalidChar (output) :	Were there any invalid characters
 *
 * Return value:
 * CSSM_FAIL if the string was invalid for any reason other than a bad character
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 *
 * NOTE: A NULL input is not considered bad
 *---------------------------------------------------------------------------*/
CSSM_BOOL dl_IsBadStrPtr(const char * StringIn, CSSM_BOOL *fInvalidChar)
{
	const char * i = StringIn;

	assert(fInvalidChar);
	*fInvalidChar = CSSM_FALSE;

	if (!StringIn)
	{
		return CSSM_FALSE;
	}

	/* check that all characters up to the first NULL are printable */
	if (port_IsBadReadPtr(i, 1))
	{
		return CSSM_TRUE;
	}

	while (*i != '\0')
	{
		if (!isprint(*i) &&
				*i != 0x09	&& /* Horizontal tab */
				*i != 0x0A	&& /* Line feed */
				*i != 0x0D)		/* Carriage Return */
		{
			*fInvalidChar = CSSM_TRUE;
			return CSSM_FALSE;
		}

		i++;

		if (port_IsBadReadPtr(i, 1))
		{
			return CSSM_TRUE;
		}
	}

	return CSSM_FALSE;
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsBadCssmDataPtr
 *
 * Description:
 * This function checks whether there are any bad pointers in
 * the input CSSM_DATA struct
 *
 * Parameters:
 * Data (input) : The CSSM_DATA struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether Data contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 *
 * NOTE: A NULL input is not considered bad
 *---------------------------------------------------------------------------*/
CSSM_BOOL dl_IsBadCssmDataPtr(CSSM_DATA const * Data)
{
	if (!Data)
	{
		return CSSM_FALSE;
	}

	if (port_IsBadReadPtr(Data, sizeof(CSSM_DATA)))
	{
		return CSSM_TRUE;
	}

	if (port_IsBadReadPtr(Data->Data, Data->Length))
	{
		return CSSM_TRUE;
	}

	return CSSM_FALSE;
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsDbNameOk
 *
 * Description:
 * This function checks whether there are any bad pointers or
 * bad string pointers in the input char *
 *
 * Parameters:
 * DbName (input) : The char * to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether DbName contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 * CSSMERR_DL_INVALID_DB_NAME
 *
 * NOTE: A NULL input is not considered bad
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_IsDbNameOk(const char *DbName)
{
	CSSM_BOOL fInvalidChar;

	if (!DbName)
	{
		return CSSM_OK;
	}

	if (port_IsBadReadPtr(DbName, 1))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	if (dl_IsBadStrPtr(DbName, &fInvalidChar))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	if (fInvalidChar != CSSM_FALSE)
	{
		return CSSMERR_DL_INVALID_DB_NAME;
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsAttributeInfoOk
 *
 * Description:
 * This function checks whether there are any bad pointers in
 * the input CSSM_DB_ATTRIBUTE_INFO struct
 *
 * Parameters:
 * AttributeInfo (input) : The CSSM_DB_ATTRIBUTE_INFO struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether AttributeInfo contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 * CSSMERR_DL_INVALID_FIELD_NAME
 * CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT
 *
 * NOTE: A NULL input is not considered bad, but if the input is not NULL,
 *	 the name must not be of zero length
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_IsAttributeInfoOk(CSSM_DB_ATTRIBUTE_INFO const *AttributeInfo)
{
	CSSM_BOOL fInvalidChar;

	if (!AttributeInfo)
	{
		return CSSM_OK;
	}

	switch (AttributeInfo->AttributeNameFormat)
	{
	case CSSM_DB_ATTRIBUTE_NAME_AS_STRING :
		if (!AttributeInfo->Label.AttributeName)
		{
			return CSSMERR_DL_INVALID_POINTER;
		}

		if (dl_IsBadStrPtr(AttributeInfo->Label.AttributeName, &fInvalidChar))
		{
			return CSSMERR_DL_INVALID_POINTER;
		}

		if (fInvalidChar != CSSM_FALSE)
		{
			return CSSMERR_DL_INVALID_FIELD_NAME;
		}

		/* Disallow zero length strings */
		if (!(*AttributeInfo->Label.AttributeName))
		{
			return CSSMERR_DL_INVALID_FIELD_NAME;
		}
		break;
	case CSSM_DB_ATTRIBUTE_NAME_AS_OID :
		if (!AttributeInfo->Label.AttributeOID.Length)
		{
			return CSSMERR_DL_INVALID_FIELD_NAME;
		}

		if (!AttributeInfo->Label.AttributeOID.Data)
		{
			return CSSMERR_DL_INVALID_POINTER;
		}

		if (port_IsBadReadPtr(AttributeInfo->Label.AttributeOID.Data,
			AttributeInfo->Label.AttributeOID.Length) )
		{
			return CSSMERR_DL_INVALID_POINTER;
		}
		break;
	case CSSM_DB_ATTRIBUTE_NAME_AS_INTEGER :
		break;
	default :
		return CSSMERR_DL_INVALID_FIELD_NAME;
	}

	if ((unsigned)(AttributeInfo->AttributeFormat) > (unsigned)(CSSM_DB_ATTRIBUTE_FORMAT_COMPLEX))
	{
		return CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT;
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsOutputRecordAttributeDataOk
 *
 * Description:
 * This function checks whether there are any bad pointers in
 * the input CSSM_DB_RECORD_ATTRIBUTE_DATA struct
 *
 * Parameters:
 * Attributes (input) : The CSSM_DB_RECORD_ATTRIBUTE_DATA struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether Attributes contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 * CSSMERR_DL_INVALID_FIELD_NAME
 * CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT
 *
 * NOTE: A NULL input is ot considered bad
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_IsOutputRecordAttributeDataOk(CSSM_DB_RECORD_ATTRIBUTE_DATA const * Attributes)
{
	uint32 i=0;
	CSSM_RETURN ret = CSSM_OK;

	if (!Attributes)
	{
		return CSSM_OK;
	}

	if (port_IsBadWritePtr((void *)Attributes, sizeof(CSSM_DB_RECORD_ATTRIBUTE_DATA)))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	if (port_IsBadWritePtr(Attributes->AttributeData, Attributes->NumberOfAttributes *
		sizeof(CSSM_DB_ATTRIBUTE_DATA)) )
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	for (i=0; i < Attributes->NumberOfAttributes; i++)
	{
		if ((ret = dl_IsAttributeInfoOk(&Attributes->AttributeData[i].Info)) != CSSM_OK)
			return ret;
	}

	return ret;
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsInputRecordAttributeDataOk
 *
 * Description:
 * This function checks whether there are any bad pointers in
 * the input CSSM_DB_RECORD_ATTRIBUTE_DATA struct
 *
 * Parameters:
 * Attributes (input) : The CSSM_DB_RECORD_ATTRIBUTE_DATA struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether Attributes contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 * CSSMERR_DL_INVALID_FIELD_NAME
 * CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT
 *
 * NOTE: A NULL input is ot considered bad
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_IsInputRecordAttributeDataOk(CSSM_DB_RECORD_ATTRIBUTE_DATA const * Attributes)
{
	uint32 i=0, j=0;
	CSSM_RETURN ret = CSSM_OK;

	if (!Attributes)
	{
		return CSSM_OK;
	}

	if (port_IsBadReadPtr(Attributes, sizeof(CSSM_DB_RECORD_ATTRIBUTE_DATA)))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	if (port_IsBadReadPtr(Attributes->AttributeData, Attributes->NumberOfAttributes *
		sizeof(CSSM_DB_ATTRIBUTE_DATA)) )
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	for (i=0; i < Attributes->NumberOfAttributes; i++)
	{
		if ((ret = dl_IsAttributeInfoOk(&Attributes->AttributeData[i].Info)) != CSSM_OK)
		{
			return ret;
		}

		for (j = 0; j < Attributes->AttributeData[i].NumberOfValues; j++)
		{
			if (port_IsBadReadPtr(Attributes->AttributeData[i].Value[j].Data,
				Attributes->AttributeData[i].Value[j].Length))
			{
				return CSSMERR_DL_INVALID_POINTER;
			}
		}
	}
	return ret;
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsIndexInfoOk
 *
 * Description:
 * This function checks whether there are any bad pointers in
 * the input CSSM_DB_INDEX_INFO struct
 *
 * Parameters:
 * IndexInfo (input) : The CSSM_DB_INDEX_INFO struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether IndexInfo contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 * CSSMERR_DL_INVALID_FIELD_NAME
 * CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT
 * CSSMERR_DL_UNSUPPORTED_INDEX_INFO
 *
 * NOTE: A NULL input is not considered bad, but if the input is not NULL,
 *	 the name must not be of zero length
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlutil_IsIndexInfoOk(CSSM_DB_INDEX_INFO_PTR IndexInfo)
{
	CSSM_RETURN ret = CSSM_OK;

	if (!IndexInfo)
	{
		return CSSM_OK;
	}

	if (IndexInfo->IndexType != CSSM_DB_INDEX_UNIQUE && IndexInfo->IndexType != CSSM_DB_INDEX_NONUNIQUE)
	{
		return CSSMERR_DL_UNSUPPORTED_INDEX_INFO;
	}

	if (IndexInfo->IndexedDataLocation != CSSM_DB_INDEX_ON_UNKNOWN &&
			IndexInfo->IndexedDataLocation != CSSM_DB_INDEX_ON_ATTRIBUTE &&
			IndexInfo->IndexedDataLocation != CSSM_DB_INDEX_ON_RECORD)
	{
		return CSSMERR_DL_UNSUPPORTED_INDEX_INFO;
	}

	if ((ret = dl_IsAttributeInfoOk(&IndexInfo->Info)) != CSSM_OK)
	{
		return ret;
	}

	return ret;
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsDbInfoOk
 *
 * Description:
 * This function checks whether there are any bad pointers in
 * the input CSSM_DBINFO struct
 *
 * Parameters:
 * DbInfoIn (input) : The CSSM_DBINFO struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether DbInfoIn contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_DB_INFO
 * CSSMERR_DL_INVALID_POINTER
 * CSSMERR_DL_INVALID_FIELD_NAME
 * CSSMERR_DL_UNSUPPORTED_INDEX_INFO
 * CSSMERR_DL_UNSUPPORTED_RECORDTYPE
 * CSSM_DL_UNSUPPORTED_AUTHENTICATION_MECHANISM
 * CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT
 *
 * NOTE: A NULL input is not considered bad
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_IsDbInfoOk(CSSM_DBINFO const * DbInfoIn)
{
	uint32 i=0, j=0;
	CSSM_RETURN ret = CSSM_OK;

	if (!DbInfoIn)
	{
		return CSSM_OK;
	}

	if (port_IsBadReadPtr(DbInfoIn, sizeof(CSSM_DBINFO)))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	if (DbInfoIn->NumberOfRecordTypes)
	{
		if (!DbInfoIn->RecordAttributeNames || !DbInfoIn->RecordIndexes || !DbInfoIn->DefaultParsingModules)
		{
			return CSSMERR_DL_INVALID_POINTER;
		}

		if (port_IsBadReadPtr(DbInfoIn->RecordAttributeNames, DbInfoIn->NumberOfRecordTypes * sizeof(CSSM_DB_RECORD_ATTRIBUTE_INFO)) ||
			port_IsBadReadPtr(DbInfoIn->RecordIndexes, DbInfoIn->NumberOfRecordTypes * sizeof(CSSM_DB_RECORD_INDEX_INFO)) ||
			port_IsBadReadPtr(DbInfoIn->DefaultParsingModules, DbInfoIn->NumberOfRecordTypes * sizeof(CSSM_DB_PARSING_MODULE_INFO)))
		{
			return CSSMERR_DL_INVALID_POINTER;
		}

		for(i=0; i < DbInfoIn->NumberOfRecordTypes; i++)
		{
			if (dlutil_IsUnsupportedRecordType(DbInfoIn->DefaultParsingModules[i].RecordType) ||
					dlutil_IsUnsupportedRecordType(DbInfoIn->RecordAttributeNames[i].DataRecordType) ||
					dlutil_IsUnsupportedRecordType(DbInfoIn->RecordIndexes[i].DataRecordType))
			{
				return CSSMERR_DL_UNSUPPORTED_RECORDTYPE;
			}

			if (DbInfoIn->RecordAttributeNames[i].NumberOfAttributes)
			{
				/* Verify the validity of the AttributeInfo */
				if (!DbInfoIn->RecordAttributeNames[i].AttributeInfo)
				{
					return CSSMERR_DL_INVALID_POINTER;
				}
				if (port_IsBadReadPtr(DbInfoIn->RecordAttributeNames[i].AttributeInfo,
					DbInfoIn->RecordAttributeNames[i].NumberOfAttributes * sizeof(CSSM_DB_ATTRIBUTE_INFO)))
				{
					return CSSMERR_DL_INVALID_POINTER;
				}
				for (j=0; j < DbInfoIn->RecordAttributeNames[i].NumberOfAttributes; j++)
				{
					if ((ret = dl_IsAttributeInfoOk(&DbInfoIn->RecordAttributeNames[i].AttributeInfo[j]))
						!= CSSM_OK)
					{
						return ret;
					}
				}
			}

			if (DbInfoIn->RecordIndexes[i].NumberOfIndexes)
			{
				/* Verify the validity of the IndexInfo */
				if (!DbInfoIn->RecordIndexes[i].IndexInfo)
				{
					return CSSMERR_DL_INVALID_POINTER;
				}
				if (port_IsBadReadPtr(DbInfoIn->RecordIndexes[i].IndexInfo,
					DbInfoIn->RecordIndexes[i].NumberOfIndexes * sizeof(CSSM_DB_INDEX_INFO_PTR)))
				{
					return CSSMERR_DL_INVALID_POINTER;
				}
				for (j=0; j < DbInfoIn->RecordIndexes[i].NumberOfIndexes; j++)
				{
					if ((ret = dlutil_IsIndexInfoOk(&DbInfoIn->RecordIndexes[i].IndexInfo[j]))
						!= CSSM_OK)
					{
						return ret;
					}
				}
			}
		}
	}

	if (DbInfoIn->AccessPath)
	{
		CSSM_BOOL fInvalidChar;
		if (dl_IsBadStrPtr(DbInfoIn->AccessPath, &fInvalidChar))
		{
			return CSSMERR_DL_INVALID_POINTER;
		}

		if (fInvalidChar != CSSM_FALSE)
		{
			return CSSMERR_DL_INVALID_POINTER;
		}
	}

	if (DbInfoIn->Reserved)
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	return ret;
}

/*-----------------------------------------------------------------------------
 * Name: dl_IsSelectionPredicateOk
 *
 * Description:
 * This function checks whether there are any bad pointers in
 * the input CSSM_SELECTION_PREDICATE struct
 *
 * Parameters:
 * SelectionPredicate (input) : The CSSM_SELECTION_PREDICATE struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether SelectionPredicate contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 * CSSMERR_DL_INVALID_FIELD_NAME
 * CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT
 * CSSMERR_DL_UNSUPPORTED_OPERATOR
 *
 * NOTE: A NULL input is not considered bad
 *---------------------------------------------------------------------------*/
static CSSM_RETURN dlutil_IsSelectionPredicateOk(
	CSSM_SELECTION_PREDICATE_PTR SelectionPredicate)
{
	uint32 j;
	CSSM_RETURN ret = CSSM_OK;

	if (!SelectionPredicate)
	{
		return CSSM_OK;
	}

	if (SelectionPredicate->DbOperator < CSSM_DB_EQUAL ||
		SelectionPredicate->DbOperator > CSSM_DB_CONTAINS_FINAL_SUBSTRING)
	{
		return CSSMERR_DL_UNSUPPORTED_OPERATOR;
	}

	if ((ret = dl_IsAttributeInfoOk(&SelectionPredicate->Attribute.Info)) != CSSM_OK)
	{
		return ret;
	}

	/* If the operator is a 'contains' operator, the attribute format must be some kind of list */
	if (SelectionPredicate->DbOperator >= CSSM_DB_CONTAINS &&
		SelectionPredicate->DbOperator <= CSSM_DB_CONTAINS_FINAL_SUBSTRING)
	{
		CSSM_DB_ATTRIBUTE_FORMAT Format = SelectionPredicate->Attribute.Info.AttributeFormat;
		if (Format != CSSM_DB_ATTRIBUTE_FORMAT_STRING && Format != CSSM_DB_ATTRIBUTE_FORMAT_BLOB &&
			Format != CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32)
		{
			return CSSMERR_DL_UNSUPPORTED_OPERATOR;
		}
	}

	for (j = 0; j < SelectionPredicate->Attribute.NumberOfValues; j++)
	{
		{
			if (port_IsBadReadPtr(SelectionPredicate->Attribute.Value[j].Data,
						SelectionPredicate->Attribute.Value[j].Length))
			{
				return CSSMERR_DL_INVALID_POINTER;
			}
		}
	}

	return ret;
}


/*-----------------------------------------------------------------------------
 * Name: dl_IsQueryOk
 *
 * Description:
 * This function checks whether there are any bad pointers in
 * the input CSSM_QUERY struct
 *
 * Parameters:
 * Query (input) : The CSSM_QUERY struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether Query contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 * CSSMERR_DL_INVALID_FIELD_NAME
 * CSSMERR_DL_UNSUPPORTED_RECORDTYPE
 * CSSMERR_DL_UNSUPPORTED_OPERATOR
 * CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT
 *
 * NOTE: A NULL input is not considered bad
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_IsQueryOk(CSSM_QUERY const * Query)
{
	uint32 i=0;
	CSSM_RETURN ret = CSSM_OK;

	if (!Query)
	{
		return CSSM_OK;
	}

	if (port_IsBadReadPtr(Query, sizeof(CSSM_QUERY)))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	if (dlutil_IsUnsupportedRecordType(Query->RecordType))
	{
		return CSSMERR_DL_UNSUPPORTED_RECORDTYPE;
	}

	/* If this is not a supported conjunctive or if this is NONE and there
	is more than 1 selection predicate, return an error */
	if (Query->Conjunctive < CSSM_DB_NONE || Query->Conjunctive > CSSM_DB_OR ||
	(Query->Conjunctive == CSSM_DB_NONE && Query->NumSelectionPredicates > 1 ))
	{
		return CSSMERR_DL_UNSUPPORTED_OPERATOR;
	}

	if (port_IsBadReadPtr(Query->SelectionPredicate, Query->NumSelectionPredicates * sizeof(CSSM_SELECTION_PREDICATE)) )
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	for (i=0; i< Query->NumSelectionPredicates; i++)
	{
		if ((ret = dlutil_IsSelectionPredicateOk(&Query->SelectionPredicate[i])) != CSSM_OK)
		{
			return ret;
		}
	}

	return ret;
}



/*-----------------------------------------------------------------------------
 * Name: dl_IsUniqueRecordIdOk
 *
 * Description:
 * This function checks whether there are any bad pointers in
 * the input CSSM_DB_UNIQUE_RECORD struct
 *
 * Parameters:
 * UniqueRecordIdentifier (input) : The CSSM_DB_UNIQUE_RECORD struct to be checked
 *
 * Return value:
 * A TRUE/FALSE indicator of whether UniqueRecordIdentifier contains any bad pointers
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_POINTER
 *
 * NOTE: A NULL input is not considered bad
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_IsUniqueRecordIdOk(CSSM_DB_UNIQUE_RECORD const * UniqueRecordIdentifier)
{
	if (!UniqueRecordIdentifier)
	{
		return CSSM_OK;
	}

	if (port_IsBadReadPtr(UniqueRecordIdentifier, sizeof(CSSM_DB_UNIQUE_RECORD)))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	if (UniqueRecordIdentifier->RecordIdentifier.Length)
	{
		if (!UniqueRecordIdentifier->RecordIdentifier.Data)
		{
			return CSSMERR_DL_INVALID_RECORD_UID;
		}

		if (port_IsBadReadPtr(UniqueRecordIdentifier->RecordIdentifier.Data,
			UniqueRecordIdentifier->RecordIdentifier.Length))
		{
			return CSSMERR_DL_INVALID_RECORD_UID;
		}
	}

	return CSSM_OK;
}
