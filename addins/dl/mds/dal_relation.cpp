/*-----------------------------------------------------------------------
 * File: dal_create.cpp
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
#include "dlutil.h"
#include "dal_internal.h"
#include "dal_src.h"
#include "port/bioapi_port.h"
#include "bioapi_util.h"


/* Added to prevent compilation error if CssmTrace.h is not available */
#ifndef CSSMTRACE0
#define CSSMTRACE0
#endif

#ifdef MODULE_DIRECTORY_SERVICES
#include "maf_util.h"
#else
#include "maf_config.h"
#include "maf_interface.h"
#endif

CSSM_RETURN DAL_DATABASE_INFO::CreateRelation(
#if DAL_USE_MODULE_PARAMATERS
	DAL_MODULE_PARAMETERS * pParameters,
#endif
	CSSM_DB_RECORDTYPE RelationID,
	const char *RelationName,
	uint32 NumberOfAttributes,
	const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
	uint32 NumberOfIndexes,
	const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo)
{
	CSSM_RETURN retValue = CSSM_OK;
	TABLE_BACKEND_REF rTableBackend;
	const DAL_TRANSLATION_TABLE *pTTable;
	DAL_TYPE_ARRAY<DAL_TRANSLATION_TABLE> TranslationTable;
	CSSM_DB_RECORD_ATTRIBUTE_INFO NewAttributeInfo;
	CSSM_DB_RECORD_INDEX_INFO NewIndexInfo;
	uint32 i = 0;

	AssertValid();
	ASSERT(m_fHaveWriterLock);

	/*
	 * Set the CSSM_DB_RECORD_ATTRIBUTE_INFO structure.
	 */
	NewAttributeInfo.DataRecordType =	RelationID;
	NewAttributeInfo.NumberOfAttributes = NumberOfAttributes;

	if ((NewAttributeInfo.AttributeInfo = (CSSM_DB_ATTRIBUTE_INFO_PTR)
							BioAPI_calloc (sizeof (CSSM_DB_ATTRIBUTE_INFO), NewAttributeInfo.NumberOfAttributes, NULL)) == NULL) {
		return CSSMERR_DL_MEMORY_ERROR;
	}

	for (i = 0; i < NewAttributeInfo.NumberOfAttributes; i++)
	{
		if (pAttributeInfo[i].AttributeName)
		{
			NewAttributeInfo.AttributeInfo[i].AttributeNameFormat = CSSM_DB_ATTRIBUTE_NAME_AS_STRING;
			NewAttributeInfo.AttributeInfo[i].Label.AttributeName = pAttributeInfo[i].AttributeName;
		}
		else if (pAttributeInfo[i].AttributeNameID.Data)
		{
			NewAttributeInfo.AttributeInfo[i].AttributeNameFormat = CSSM_DB_ATTRIBUTE_NAME_AS_OID;
			NewAttributeInfo.AttributeInfo[i].Label.AttributeOID = pAttributeInfo[i].AttributeNameID;
		}
		else
		{
			return CSSMERR_DL_INVALID_FIELD_NAME;
		}

		NewAttributeInfo.AttributeInfo[i].AttributeFormat = pAttributeInfo[i].DataType;

		if ((retValue = dl_IsAttributeInfoOk (NewAttributeInfo.AttributeInfo)) != CSSM_OK)
			return retValue;
	}

	/*
	 * Set the CSSM_DB_RECORD_INDEX_INFO structure.
	 */

	NewIndexInfo.DataRecordType =	RelationID;
	NewIndexInfo.NumberOfIndexes = NumberOfIndexes;

	if ((NewIndexInfo.IndexInfo = (CSSM_DB_INDEX_INFO_PTR)
							BioAPI_calloc (sizeof (CSSM_DB_INDEX_INFO), NewIndexInfo.NumberOfIndexes, NULL)) == NULL) {
		return CSSMERR_DL_MEMORY_ERROR;
	}

	for (i = 0; i < NewIndexInfo.NumberOfIndexes; i++)
	{
		uint32 j;

		NewIndexInfo.IndexInfo[i].IndexType = pIndexInfo[i].IndexType;
		NewIndexInfo.IndexInfo[i].IndexedDataLocation = pIndexInfo[i].IndexedDataLocation;

		for (j = 0; j < NumberOfAttributes; j++)
		{
			if (pAttributeInfo[j].AttributeId == pIndexInfo[i].AttributeId) {
				NewIndexInfo.IndexInfo[i].Info = NewAttributeInfo.AttributeInfo[j];
				break;
			}
		}
		if (j == NumberOfAttributes)
			return CSSMERR_DL_INVALID_INDEX_INFO;

		if ((retValue = dlutil_IsIndexInfoOk (NewIndexInfo.IndexInfo)) != CSSM_OK)
			return retValue;
	}

	/*
	 * Create translation table.
	 */
	if ((retValue = TranslationTable.Initialize(1)) != CSSM_OK)
	{
		/* Free the locally allocated memory */
		BioAPI_free (NewAttributeInfo.AttributeInfo, NULL);
		BioAPI_free (NewIndexInfo.IndexInfo, NULL);
		return (retValue);
	}

	if ((retValue = TranslationTable.neSetData(RelationID).Initialize(&NewAttributeInfo, &NewIndexInfo))
										!= CSSM_OK)
	{
		/* Free the locally allocated memory */
		BioAPI_free (NewAttributeInfo.AttributeInfo, NULL);
		BioAPI_free (NewIndexInfo.IndexInfo, NULL);
		return (retValue);
	}

	/* Free the locally allocated memory */
	BioAPI_free (NewAttributeInfo.AttributeInfo, NULL);
	BioAPI_free (NewIndexInfo.IndexInfo, NULL);

	/*
	 * Create the table entries for the relation
	 */
	pTTable = TranslationTable.neGetByPosition(0);

	if ((retValue = dlbe_CreateFiles(m_hDatabase, &rTableBackend, RelationID,
		pTTable,  pParameters)) != CSSM_OK)
	{
		return retValue;
	}

	return CSSM_OK;
}


CSSM_RETURN DAL_DATABASE_INFO::DestroyRelation(
#if DAL_USE_MODULE_PARAMATERS
	DAL_MODULE_PARAMETERS * pParameters,
#endif
	CSSM_DB_RECORDTYPE RelationID)
{
	/*
	DAL_RECORD_TABLE *pRecordTable = m_rgRecordTable.neGetByRecordType(RelationID);

	pRecordTable->~DAL_RECORD_TABLE();

	return dlbe_DeleteRelation (RelationID MODULE_PARAMETERS);
	*/
	return CSSM_OK;
}

CSSM_RETURN dal_AddRelationToDbInfo (CSSM_DBINFO_PTR pDbInfo,
				CSSM_DB_RECORDTYPE RelationID,
				const char *RelationName,
				uint32 NumberOfAttributes,
				const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
				uint32 NumberOfIndexes,
				const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo)
{
	uint32 i, j;
	CSSM_DB_PARSING_MODULE_INFO_PTR pDefaultParsingModules;
	CSSM_DB_RECORD_ATTRIBUTE_INFO_PTR pRecordAttributeNames;
	CSSM_DB_RECORD_INDEX_INFO_PTR pRecordIndexes;


	/*
	 * Increment the number of relations.
	 */
	pDbInfo->NumberOfRecordTypes++;

	/*
	 * Create the new dbinfo to be written into the metadata.
	 */
	if ((pDbInfo->DefaultParsingModules = (CSSM_DB_PARSING_MODULE_INFO_PTR)
										BioAPI_realloc (pDbInfo->DefaultParsingModules,
										sizeof (CSSM_DB_PARSING_MODULE_INFO) *	pDbInfo->NumberOfRecordTypes,
										NULL)) == NULL)
	{
		return(CSSMERR_DL_MEMORY_ERROR);;
	}

	if ((pDbInfo->RecordAttributeNames = (CSSM_DB_RECORD_ATTRIBUTE_INFO_PTR)
										BioAPI_realloc (pDbInfo->RecordAttributeNames,
										sizeof (CSSM_DB_RECORD_ATTRIBUTE_INFO) *  pDbInfo->NumberOfRecordTypes,
										NULL)) == NULL)
	{
		return(CSSMERR_DL_MEMORY_ERROR);
	}

	if ((pDbInfo->RecordIndexes = (CSSM_DB_RECORD_INDEX_INFO_PTR)
										BioAPI_realloc (pDbInfo->RecordIndexes,
										sizeof (CSSM_DB_RECORD_INDEX_INFO) *  pDbInfo->NumberOfRecordTypes,
										NULL)) == NULL)
	{
		return(CSSMERR_DL_MEMORY_ERROR);
	}

	pDefaultParsingModules = &pDbInfo->DefaultParsingModules[pDbInfo->NumberOfRecordTypes-1];
	pRecordAttributeNames = &pDbInfo->RecordAttributeNames[pDbInfo->NumberOfRecordTypes-1];
	pRecordIndexes = &pDbInfo->RecordIndexes[pDbInfo->NumberOfRecordTypes-1];

	/*
	 * Set the parsing module info.
	 */
	pDefaultParsingModules->RecordType = RelationID;
	memset (&pDefaultParsingModules->ModuleSubserviceUid, 0, sizeof (CSSM_SUBSERVICE_UID));

	/*
	 * Set the Attributes info.
	 */
	pRecordAttributeNames->DataRecordType = RelationID;
	pRecordAttributeNames->NumberOfAttributes = NumberOfAttributes;

	if ((pRecordAttributeNames->AttributeInfo = (CSSM_DB_ATTRIBUTE_INFO_PTR)
										BioAPI_calloc (sizeof (CSSM_DB_ATTRIBUTE_INFO) * NumberOfAttributes,
										1,
										NULL)) == NULL)
	{
		return(CSSMERR_DL_MEMORY_ERROR);
	}

	for (i = 0; i < NumberOfAttributes; i++) {
		CSSM_DB_ATTRIBUTE_INFO_PTR pAttr;

		pAttr = &pRecordAttributeNames->AttributeInfo[i];
		if (pAttributeInfo[i].AttributeName) {
			pAttr->AttributeNameFormat = CSSM_DB_ATTRIBUTE_NAME_AS_STRING;
			if ((pAttr->Label.AttributeName = (char *)
										BioAPI_calloc (strlen (pAttributeInfo[i].AttributeName)+1,
										1, NULL)) == NULL) {
					return(CSSMERR_DL_MEMORY_ERROR);
			}
			strcpy (pAttr->Label.AttributeName, pAttributeInfo[i].AttributeName);
		} else if (pAttributeInfo[i].AttributeNameID.Data) {
			pAttr->AttributeNameFormat = CSSM_DB_ATTRIBUTE_NAME_AS_OID;
				if ((pAttr->Label.AttributeOID.Data = (uint8 *)
										BioAPI_calloc (pAttributeInfo[i].AttributeNameID.Length,
										1, NULL)) == NULL) {
					return(CSSMERR_DL_MEMORY_ERROR);
			}
			memcpy (pAttr->Label.AttributeOID.Data, pAttributeInfo[i].AttributeNameID.Data,
												pAttributeInfo[i].AttributeNameID.Length);
			pAttr->Label.AttributeOID.Length = pAttributeInfo[i].AttributeNameID.Length;
		} else {
			pAttr->AttributeNameFormat = CSSM_DB_ATTRIBUTE_NAME_AS_INTEGER;
			pAttr->Label.AttributeID = pAttributeInfo[i].AttributeId;
		}

		pAttr->AttributeFormat = pAttributeInfo[i].DataType;
	}

	/*
	 * Set the Index info.
	 */
	pRecordIndexes->DataRecordType = RelationID;
	pRecordIndexes->NumberOfIndexes = NumberOfIndexes;

	if ((pRecordIndexes->IndexInfo = (CSSM_DB_INDEX_INFO_PTR)
										BioAPI_calloc (sizeof (CSSM_DB_INDEX_INFO) * NumberOfIndexes,
										1, NULL)) == NULL)
	{
		return(CSSMERR_DL_MEMORY_ERROR);
	}

	for (i = 0; i < NumberOfIndexes; i++) {
		CSSM_DB_INDEX_INFO_PTR pIndex;

		pIndex = &pRecordIndexes->IndexInfo[i];

		pIndex->IndexType = pIndexInfo[i].IndexType;
		pIndex->IndexedDataLocation = pIndexInfo[i].IndexedDataLocation;
		for (j = 0; j < NumberOfAttributes; j++) {
			if (pAttributeInfo[j].AttributeId == pIndexInfo[i].AttributeId) {

				pIndex->Info.AttributeNameFormat = pRecordAttributeNames->AttributeInfo[j].AttributeNameFormat;
				pIndex->Info.AttributeFormat = pRecordAttributeNames->AttributeInfo[j].AttributeFormat;

				if (pRecordAttributeNames->AttributeInfo[j].AttributeNameFormat
													== CSSM_DB_ATTRIBUTE_NAME_AS_STRING) {
					if ((pIndex->Info.Label.AttributeName = (char *)
						BioAPI_calloc (strlen (pRecordAttributeNames->AttributeInfo[j].Label.AttributeName)+1,
						1, NULL)) == NULL) {
							return(CSSMERR_DL_MEMORY_ERROR);
					}
					strcpy (pIndex->Info.Label.AttributeName,
										pRecordAttributeNames->AttributeInfo[j].Label.AttributeName);
				} else if (pRecordAttributeNames->AttributeInfo[j].AttributeNameFormat
													== CSSM_DB_ATTRIBUTE_NAME_AS_OID) {
					if ((pIndex->Info.Label.AttributeOID.Data = (uint8 *)
							BioAPI_calloc (pRecordAttributeNames->AttributeInfo[j].Label.AttributeOID.Length,
							1, NULL)) == NULL) {
							return(CSSMERR_DL_MEMORY_ERROR);
					}
					memcpy (pIndex->Info.Label.AttributeOID.Data,
							pRecordAttributeNames->AttributeInfo[j].Label.AttributeOID.Data,
							pRecordAttributeNames->AttributeInfo[j].Label.AttributeOID.Length);
					pIndex->Info.Label.AttributeOID.Length =
							pRecordAttributeNames->AttributeInfo[j].Label.AttributeOID.Length;
				} else {
					pIndex->Info.Label.AttributeID =
							pRecordAttributeNames->AttributeInfo[j].Label.AttributeID;
				}

				break;	/* We found the attribute that the index is pointing at. */
			}
		}
	}

	return CSSM_OK;
}

CSSM_RETURN dal_RemoveRelationFromDbInfo (CSSM_DBINFO_PTR pDbInfo,
										CSSM_DB_RECORDTYPE RelationID)
{
	uint32 i, j;
	CSSM_RETURN retValue;

	for (i = 0; i < pDbInfo->NumberOfRecordTypes; i++) {
		if (pDbInfo->RecordAttributeNames[i].DataRecordType == RelationID)
			break;
	}

	if (i == pDbInfo->NumberOfRecordTypes) {
		return (CSSMERR_DL_INVALID_RECORDTYPE);
	}
	if ((retValue = freeDbInfoRecord (pDbInfo, i)) != CSSM_OK)
		return retValue;

	pDbInfo->NumberOfRecordTypes--;

	for (j = i; j < pDbInfo->NumberOfRecordTypes; j++) {
		pDbInfo->DefaultParsingModules[j] = pDbInfo->DefaultParsingModules[j+1];
		pDbInfo->RecordAttributeNames[j] = pDbInfo->RecordAttributeNames[j+1];
		pDbInfo->RecordIndexes[j] = pDbInfo->RecordIndexes[j+1];
	}

	return CSSM_OK;
}
