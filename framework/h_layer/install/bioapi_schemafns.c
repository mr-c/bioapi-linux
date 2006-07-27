/*-----------------------------------------------------------------------
 * File: hrsschemafns.c
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


#include "precomp_install.h"
#include "bioapi_schema.h"

#define _PRINTABLE_GUID_LENGTH	   ( 40 )
#define _PRINTABLE_VERSION_LENGTH  ( 8 )

#define __ModuleId			( 0 )
#define __ModuleName		( 1 )
#define __ProductVersion	( 2 )
#define __SpecVersion		( 3 )
#define __VendorName		( 4 )
#define __ModuleDescription ( 5 )

typedef struct bioapi_capabilities_attr_state
{
	CSSM_DB_ATTRIBUTE_DATA DbRecordAttributes[BIOAPI_H_LAYER_NUM_ATTRIBUTES];
	CSSM_DATA AttributeValues[BIOAPI_H_LAYER_NUM_ATTRIBUTES];
	uint8 PrintableGuid[_PRINTABLE_GUID_LENGTH];
	uint8 PrintableSpecVersion[_PRINTABLE_VERSION_LENGTH];
	uint8 PrintableProdVersion[_PRINTABLE_VERSION_LENGTH];
}
BIOAPI_CAPABILTIES_ATTR_STATE;

static CSSM_RETURN CSSMAPI ConstructBioAPICapabiltiesAttributes(
							MDSU_CONTEXT *pContext,
							const void *pSchemaData,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							CSSM_BOOL bFillValues,
							void **ppAttrState )
{
	CSSM_RETURN rv = CSSM_OK;
	const BioAPI_H_LEVEL_FRAMEWORK_SCHEMA *pBioAPICap =
							(const BioAPI_H_LEVEL_FRAMEWORK_SCHEMA*)pSchemaData;
	BIOAPI_CAPABILTIES_ATTR_STATE *pState = NULL;
	CSSM_DB_ATTRIBUTE_DATA *DbRecordAttributes;
	uint32 i;

	assert( pContext && DbRecord && ppAttrState );
	assert( ( bFillValues == CSSM_FALSE ) ||
			( ( bFillValues == CSSM_TRUE ) && pSchemaData ) );

	/* Allocate the memory for the attributes */
	pState = MDSU_malloc( pContext, sizeof(BIOAPI_CAPABILTIES_ATTR_STATE) );
	if ( pState == NULL )
	{
		ERR( rv = CSSMERR_CSSM_MEMORY_ERROR );
	}
	else
	{
		/* Clear the structure */
		memset( pState, 0, sizeof(BIOAPI_CAPABILTIES_ATTR_STATE) );
		DbRecordAttributes = pState->DbRecordAttributes;

		/* Fill in the record information */
		DbRecord->DataRecordType = BIOAPI_H_LAYER_RECORDTYPE;
		DbRecord->SemanticInformation = 0;
		DbRecord->NumberOfAttributes = BIOAPI_H_LAYER_NUM_ATTRIBUTES;
		DbRecord->AttributeData = DbRecordAttributes;


		/* Fill in attribute info */
		DbRecordAttributes[__ModuleId].Info = s_BioApiAttrInfo_ModuleId;
		DbRecordAttributes[__ModuleName].Info = s_BioApiAttrInfo_ModuleName;
		DbRecordAttributes[__ProductVersion].Info = s_BioApiAttrInfo_ProductVersion;
		DbRecordAttributes[__SpecVersion].Info = s_BioApiAttrInfo_SpecVersion;
		DbRecordAttributes[__VendorName].Info = s_BioApiAttrInfo_Vendor;
		DbRecordAttributes[__ModuleDescription].Info = s_BioApiAttrInfo_Description;
		if ( bFillValues == CSSM_TRUE )
		{
			/* Connect the attribute value array to the attribute array */
			for ( i = 0; i < DbRecord->NumberOfAttributes; i++ )
			{
				DbRecordAttributes[i].Value = &pState->AttributeValues[i];
				DbRecordAttributes[i].NumberOfValues = 1;
			}

			/* Fill in the record contents */
			/* ModuleID */
			BioAPI_GetPrintableUUID( &pBioAPICap->ModuleId,
									(char*)pState->PrintableGuid );
			DbRecordAttributes[__ModuleId].Value->Data =
													pState->PrintableGuid;
			DbRecordAttributes[__ModuleId].Value->Length =
								strlen( (char*)pState->PrintableGuid ) + 1;

			/* ModuleName */
			DbRecordAttributes[__ModuleName].Value->Length =
										strlen( pBioAPICap->ModuleName ) + 1 ;
			DbRecordAttributes[__ModuleName].Value->Data =
												(uint8*)pBioAPICap->ModuleName;


			/* SpecVersion */
			BioAPI_GetPrintableVersion( &pBioAPICap->SpecVersion,
									   (char*)pState->PrintableSpecVersion );
			DbRecordAttributes[__SpecVersion].Value->Data =
													pState->PrintableSpecVersion;
			DbRecordAttributes[__SpecVersion].Value->Length =
								strlen( (char*)pState->PrintableSpecVersion ) + 1;

			/* ProdVersion */
			BioAPI_GetPrintableVersion( &pBioAPICap->ProdVersion,
									   (char*)pState->PrintableProdVersion );
			DbRecordAttributes[__ProductVersion].Value->Data =
													pState->PrintableProdVersion;
			DbRecordAttributes[__ProductVersion].Value->Length =
								strlen( (char*)pState->PrintableProdVersion ) + 1;
			/* Vendor */
			DbRecordAttributes[__VendorName].Value->Length =
										strlen( pBioAPICap->Vendor ) + 1 ;
			DbRecordAttributes[__VendorName].Value->Data =
												(uint8*)pBioAPICap->Vendor;


			/* Description */
			DbRecordAttributes[__ModuleDescription].Value->Length =
										strlen( pBioAPICap->Description ) + 1 ;
			DbRecordAttributes[__ModuleDescription].Value->Data =
												(uint8*)pBioAPICap->Description;



		} /* bFillValues */
		else
		{
			for ( i = 0; i < DbRecord->NumberOfAttributes; i++ )
			{
				DbRecordAttributes[i].Value = NULL;
				DbRecordAttributes[i].NumberOfValues = 0;
			}
		}

		if ( rv != CSSM_OK )
		{
			/* Deallocate memory if there was an error */
			MDSU_free( pContext,
					   pState );
			pState = NULL;
		}
	} /* allocate DbRecordAttributes */

	*ppAttrState = (void*)pState;

	return rv;
}

static CSSM_RETURN CSSMAPI DestructBioAPICapabiltiesAttributes(
							MDSU_CONTEXT *pContext,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							CSSM_BOOL bFreeAll,
							void *pAttrState )
{
	CSSM_DB_ATTRIBUTE_DATA *DbRecordAttributes;
	uint32 i;

	assert( pContext && DbRecord && pAttrState );

	/* Free the buffers allocated for the attribute values */
	DbRecordAttributes = DbRecord->AttributeData;
	if ( bFreeAll == CSSM_TRUE )
	{
		for ( i = 0; i < DbRecord->NumberOfAttributes; i++ )
		{
			if ( DbRecordAttributes[i].Value )
			{
				if ( DbRecordAttributes[i].Value->Data )
				{
					MDSU_free( pContext,
							   DbRecordAttributes[i].Value->Data );
				}
				MDSU_free( pContext,
						   DbRecordAttributes[i].Value );
			}
		}
	}

	/* Free the state structure */
	MDSU_free( pContext,
			   pAttrState );
	DbRecord->AttributeData = NULL;

	return CSSM_OK;
}


typedef struct bioapi_capabilties_pred_state
{
	CSSM_SELECTION_PREDICATE Predicates[BIOAPI_H_LAYER_NUM_ATTRIBUTES];
	CSSM_DATA PredicateValues[BIOAPI_H_LAYER_NUM_ATTRIBUTES];
	uint8 PrintableGuid[_PRINTABLE_GUID_LENGTH];
	uint8 PrintableSpecVersion[_PRINTABLE_VERSION_LENGTH];
	uint8 PrintableProdVersion[_PRINTABLE_VERSION_LENGTH];
}
BIOAPI_CAPABILTIES_PRED_STATE;



static CSSM_RETURN CSSMAPI ConstructBioAPICapabiltiesPredicate(
							MDSU_CONTEXT *pContext,
							const void *pSchemaTemplate,
							MDSU_TEMPLATE_MASK ValidTemplates,
							CSSM_QUERY *pQuery,
							void **ppPredState )
{
	CSSM_RETURN rv = CSSM_OK;
	const BioAPI_H_LEVEL_FRAMEWORK_SCHEMA *pBioAPICap =
							(const BioAPI_H_LEVEL_FRAMEWORK_SCHEMA*)pSchemaTemplate;
	BIOAPI_CAPABILTIES_PRED_STATE *pState = NULL;
	CSSM_SELECTION_PREDICATE *pPredicates = NULL;
	uint32 uNumPredicates;
	uint32 uIndex = 0;
	uint32 i;

	assert( pContext &&
			pQuery &&
			ppPredState &&
			( ( pSchemaTemplate && ValidTemplates ) ||
			  ( !pSchemaTemplate && !ValidTemplates ) ) );

	/* Count the predicates */
	_CountBits( ValidTemplates, uNumPredicates );

	/* Allocate the predicate state and link the value structures */
	if ( uNumPredicates != 0 )
	{
		pState = MDSU_malloc( pContext, sizeof(BIOAPI_CAPABILTIES_PRED_STATE) );
		if ( pState == NULL )
		{
			ERR( rv = CSSMERR_CSSM_MEMORY_ERROR );
		}
		else
		{
			memset( pState,
					0,
					sizeof(BIOAPI_CAPABILTIES_PRED_STATE) );
			pPredicates = pState->Predicates;

			/* Allocate a single attribute value struct per attribute */
			for ( i = 0; i < uNumPredicates; i++ )
			{
				pPredicates[i].Attribute.Value = &pState->PredicateValues[i];
				pPredicates[i].Attribute.NumberOfValues = 1;
			}
		} /* Allocate and link predicate values */
	} /* uNumPredicates != 0 */

	if ( rv == CSSM_OK )
	{
		/* Fill in the query parameters */
		pQuery->RecordType = BIOAPI_H_LAYER_RECORDTYPE;
		pQuery->Conjunctive = CSSM_DB_AND;
		pQuery->NumSelectionPredicates = uNumPredicates;
		pQuery->QueryLimits.TimeLimit = CSSM_QUERY_TIMELIMIT_NONE;
		pQuery->QueryLimits.SizeLimit = CSSM_QUERY_SIZELIMIT_NONE;
		pQuery->QueryFlags = 0;
		pQuery->SelectionPredicate = pPredicates;
		for ( uIndex = 0; uIndex < uNumPredicates; uIndex++ )
		{
			pPredicates[uIndex].DbOperator = CSSM_DB_EQUAL;
		}


		/* Fill in the predicate data */
		uIndex = 0;
		if ( ValidTemplates & MDSU_BIOAPI_CAP_MODULE_ID )
		{
			/* ModuleID */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_ModuleId;
			BioAPI_GetPrintableUUID( &pBioAPICap->ModuleId,
									(char*)pState->PrintableGuid );
			pPredicates[uIndex].Attribute.Value->Data = pState->PrintableGuid;
			pPredicates[uIndex].Attribute.Value->Length =
								strlen( (char*)pState->PrintableGuid ) + 1;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_CAP_MODULE_NAME )
		{
			/* ModuleName */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_ModuleName;
			pPredicates[uIndex].Attribute.Value->Length = strlen( pBioAPICap->ModuleName ) + 1;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPICap->ModuleName;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_CAP_SPEC_VERSION )
		{
			/* SpecVersion */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_SpecVersion;
			BioAPI_GetPrintableVersion( &pBioAPICap->SpecVersion,
									   (char*)pState->PrintableSpecVersion );
			pPredicates[uIndex].Attribute.Value->Data =
												pState->PrintableSpecVersion;
			pPredicates[uIndex].Attribute.Value->Length =
							strlen( (char*)pState->PrintableSpecVersion ) + 1;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_CAP_PROD_VERSION )
		{
			/* ProdVersion */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_ProductVersion;
			BioAPI_GetPrintableVersion( &pBioAPICap->ProdVersion,
									   (char*)pState->PrintableProdVersion );
			pPredicates[uIndex].Attribute.Value->Data =
												pState->PrintableProdVersion;
			pPredicates[uIndex].Attribute.Value->Length =
							strlen( (char*)pState->PrintableProdVersion ) + 1;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_CAP_VENDOR )
		{
			/* VendorNAme */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_Vendor;
			pPredicates[uIndex].Attribute.Value->Length =
										strlen( pBioAPICap->Vendor ) + 1 ;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)pBioAPICap->Vendor;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_CAP_DESCRIPTION )
		{
			/* VendorNAme */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_Description;
			pPredicates[uIndex].Attribute.Value->Length =
										strlen( pBioAPICap->Description ) + 1 ;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)pBioAPICap->Description;
			uIndex++;
		}

	} /* Initial allocation successful */


	*ppPredState = (void*)pState;
	return rv;
}

static CSSM_RETURN CSSMAPI DestructBioAPICapabiltiesPredicate(
							MDSU_CONTEXT *pContext,
							CSSM_QUERY *pQuery,
							void *pPredState )
{
	assert( pContext && pQuery );

	/* Free the attribute array */
	if ( pPredState != NULL )
	{
		MDSU_free( pContext, pPredState );
	}

	return CSSM_OK;
}

static CSSM_RETURN CSSMAPI ConvertBioAPICapabiltiesAttributes(
							MDSU_CONTEXT *pContext,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							void *pSchemaData )
{
	CSSM_RETURN crReturn;
	BioAPI_H_LEVEL_FRAMEWORK_SCHEMA *pBioAPICap = (BioAPI_H_LEVEL_FRAMEWORK_SCHEMA*)pSchemaData;

	const CSSM_DB_ATTRIBUTE_DATA *DbAttr = NULL;

	assert( pSchemaData && DbRecord && pContext );

	DbAttr = DbRecord->AttributeData;


	/* ModuleID */
	crReturn = BioAPI_GetStructuredUUID(
								(char*)DbAttr[__ModuleId].Value->Data,
								&pBioAPICap->ModuleId );
	if ( crReturn != CSSM_OK )
	{
		return crReturn;
	}


	/* ModuleName */
	strcpy( pBioAPICap->ModuleName,
			(char*)DbAttr[__ModuleName].Value->Data );


	/* SpecVersion */
	crReturn = BioAPI_GetStructuredVersion(
									(char*)DbAttr[__SpecVersion].Value->Data,
									&pBioAPICap->SpecVersion );
	/* ProdVersion */
	crReturn = BioAPI_GetStructuredVersion(
									(char*)DbAttr[__ProductVersion].Value->Data,
									&pBioAPICap->ProdVersion );

	/* VendorName */
	strcpy( pBioAPICap->Vendor,
			(char*)DbAttr[__VendorName].Value->Data );

	/* ModuleDescription */
	strcpy( pBioAPICap->Description,
			(char*)DbAttr[__ModuleDescription].Value->Data );


	return CSSM_OK;
}


const IfMdsuSchemaManipulator IfiMdsuBioAPICapabilitySchema =
{
	ConstructBioAPICapabiltiesAttributes,
	DestructBioAPICapabiltiesAttributes,
	ConstructBioAPICapabiltiesPredicate,
	DestructBioAPICapabiltiesPredicate,
	ConvertBioAPICapabiltiesAttributes,
	BIOAPI_H_LAYER_RECORDTYPE
};
