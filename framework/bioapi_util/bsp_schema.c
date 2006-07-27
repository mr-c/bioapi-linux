/*-----------------------------------------------------------------------
 * File: bsp_schema.c
 *-----------------------------------------------------------------------
 */

/* General system includes */
#include <assert.h>

/* Public BioAPI headers that we'll need */
#include "bioapi.h"
#include "bioapi_util.h"
#include "port/bioapi_port.h"
#include "cssmtype.h"

/* MDS headers */
#include "mds.h"
#include "mds_util.h"

/* BioAPI addin schema header */
#include "bsp_schema.h"


#define __ModuleId					( 0 )
#define __DeviceId					( 1 )
#define __BspName					( 2 )
#define __SpecVersion				( 3 )
#define __ProductVersion			( 4 )
#define __Vendor					( 5 )
#define __BspSupportedFormats		( 6 )
#define __FactorsMask				( 7 )
#define __Operations				( 8 )
#define __Options					( 9 )
#define __PayloadPolicy				( 10 )
#define __MaxPayloadSize			( 11 )
#define __DefaultVerifyTimeout		( 12 )
#define __DefaultIdentifyTimeout	( 13 )
#define __DefaultCaptureTimeout		( 14 )
#define __DefaultEnrollTimeout		( 15 )
#define __MaxBspDbSize				( 16 )
#define __MaxIdentify				( 17 )
#define __Description				( 18 )
#define __Path						( 19 )


typedef struct bioapi_bsp_attr_state
{
	CSSM_DB_ATTRIBUTE_DATA DbRecordAttributes[BIOAPI_BSP_NUM_ATTRIBUTES];
	CSSM_DATA AttributeValues[BIOAPI_BSP_NUM_ATTRIBUTES];
	uint8 PrintableUuid[BioAPI_PRINTABLE_UUID_LENGTH];
	uint8 PrintableSpecVersion[BioAPI_PRINTABLE_VERSION_LENGTH];
	uint8 PrintableProductVersion[BioAPI_PRINTABLE_VERSION_LENGTH];
}
BIOAPI_BSP_ATTR_STATE;

static BioAPI_RETURN BioAPI ConstructBioAPIBspCapabiltiesAttributes(
							MDSU_CONTEXT *pContext,
							const void *pSchemaData,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							CSSM_BOOL bFillValues,
							void **ppAttrState )
{
	CSSM_RETURN rv = CSSM_OK;
	const BioAPI_BSP_SCHEMA *pBioAPIBspCap =
							(const BioAPI_BSP_SCHEMA*)pSchemaData;
	BIOAPI_BSP_ATTR_STATE *pState = NULL;
	CSSM_DB_ATTRIBUTE_DATA *DbRecordAttributes;
	uint32 i;

	assert( pContext && DbRecord && ppAttrState );
	assert( ( bFillValues == CSSM_FALSE ) ||
			( ( bFillValues == CSSM_TRUE ) && pSchemaData ) );

	/* Allocate the memory for the attributes */
	pState = MDSU_malloc( pContext, sizeof(BIOAPI_BSP_ATTR_STATE) );
	if ( pState == NULL )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_MEMORY_ERROR );
	}
	else
	{
		/* Clear the structure */
		memset( pState, 0, sizeof(BIOAPI_BSP_ATTR_STATE) );
		DbRecordAttributes = pState->DbRecordAttributes;

		/* Fill in the record information */
		DbRecord->DataRecordType = BIOAPI_BSP_RECORDTYPE;
		DbRecord->SemanticInformation = 0;
		DbRecord->NumberOfAttributes = BIOAPI_BSP_NUM_ATTRIBUTES;
		DbRecord->AttributeData = DbRecordAttributes;

		/* Fill in attribute info */
		DbRecordAttributes[__ModuleId].Info = s_BioApiAttrInfo_ModuleId;
		DbRecordAttributes[__DeviceId].Info = s_BioApiAttrInfo_DeviceId;
		DbRecordAttributes[__BspName].Info = s_BioApiAttrInfo_BspName;
		DbRecordAttributes[__SpecVersion].Info = s_BioApiAttrInfo_SpecVersion;
		DbRecordAttributes[__ProductVersion].Info = s_BioApiAttrInfo_ProductVersion;
		DbRecordAttributes[__Vendor].Info = s_BioApiAttrInfo_Vendor;
		DbRecordAttributes[__BspSupportedFormats].Info = s_BioApiAttrInfo_BspSupportedFormats;
		DbRecordAttributes[__FactorsMask].Info = s_BioApiAttrInfo_FactorsMask;
		DbRecordAttributes[__Operations].Info = s_BioApiAttrInfo_Operations;
		DbRecordAttributes[__Options].Info = s_BioApiAttrInfo_Options;
		DbRecordAttributes[__PayloadPolicy].Info = s_BioApiAttrInfo_PayloadPolicy;
		DbRecordAttributes[__MaxPayloadSize].Info = s_BioApiAttrInfo_MaxPayloadSize;
		DbRecordAttributes[__DefaultVerifyTimeout].Info = s_BioApiAttrInfo_DefaultVerifyTimeout;
		DbRecordAttributes[__DefaultIdentifyTimeout].Info = s_BioApiAttrInfo_DefaultIdentifyTimeout;
		DbRecordAttributes[__DefaultCaptureTimeout].Info = s_BioApiAttrInfo_DefaultCaptureTimeout;
		DbRecordAttributes[__DefaultEnrollTimeout].Info = s_BioApiAttrInfo_DefaultEnrollTimeout;
		DbRecordAttributes[__MaxBspDbSize].Info = s_BioApiAttrInfo_MaxBspDbSize;
		DbRecordAttributes[__MaxIdentify].Info = s_BioApiAttrInfo_MaxIdentify;
		DbRecordAttributes[__Description].Info = s_BioApiAttrInfo_Description;
		DbRecordAttributes[__Path].Info = s_BioApiAttrInfo_Path;
		if ( bFillValues == CSSM_TRUE )
		{
			/* Connect the attribute value array to the attribute array */
			for ( i = 0; i < DbRecord->NumberOfAttributes; i++ )
			{
				DbRecordAttributes[i].Value = &pState->AttributeValues[i];
				DbRecordAttributes[i].NumberOfValues = 1;
			}

			/* Fill in the record contents */
			/* ModuleId */
			BioAPI_GetPrintableUUID( &pBioAPIBspCap->ModuleId,
									(char*)pState->PrintableUuid );
			DbRecordAttributes[__ModuleId].Value->Data =
													pState->PrintableUuid;
			DbRecordAttributes[__ModuleId].Value->Length =
								strlen( (char*)pState->PrintableUuid ) + 1;

			/* DeviceId */
			DbRecordAttributes[__DeviceId].Value->Length = sizeof(uint32);
			DbRecordAttributes[__DeviceId].Value->Data = (uint8*)&pBioAPIBspCap->DeviceId;

			/* BspName */
			DbRecordAttributes[__BspName].Value->Length =
										strlen( pBioAPIBspCap->BSPName ) + 1 ;
			DbRecordAttributes[__BspName].Value->Data =
												(uint8*)pBioAPIBspCap->BSPName;

			/* SpecVersion */
			BioAPI_GetPrintableVersion( &pBioAPIBspCap->SpecVersion,
									   (char*)pState->PrintableSpecVersion );
			DbRecordAttributes[__SpecVersion].Value->Data =
													pState->PrintableSpecVersion;
			DbRecordAttributes[__SpecVersion].Value->Length =
								strlen( (char*)pState->PrintableSpecVersion ) + 1;

			/* ProductVersion */
			BioAPI_GetPrintableVersion( &pBioAPIBspCap->ProductVersion,
									   (char*)pState->PrintableProductVersion );
			DbRecordAttributes[__ProductVersion].Value->Data =
													pState->PrintableProductVersion;
			DbRecordAttributes[__ProductVersion].Value->Length =
								strlen( (char*)pState->PrintableProductVersion ) + 1;

			/* Vendor */
			DbRecordAttributes[__Vendor].Value->Length =
										strlen( pBioAPIBspCap->Vendor ) + 1 ;
			DbRecordAttributes[__Vendor].Value->Data =
												(uint8*)pBioAPIBspCap->Vendor;

			/* SupportedFormats */
			DbRecordAttributes[__BspSupportedFormats].Value->Length =
											sizeof(uint32) * pBioAPIBspCap->NumSupportedFormats;
			DbRecordAttributes[__BspSupportedFormats].Value->Data = (uint8*)&pBioAPIBspCap->BspSupportedFormats;

			/* FactorsMask */
			DbRecordAttributes[__FactorsMask].Value->Length = sizeof(uint32);
			DbRecordAttributes[__FactorsMask].Value->Data = (uint8*)&pBioAPIBspCap->FactorsMask;

			/* Operations */
			DbRecordAttributes[__Operations].Value->Length = sizeof(uint32);
			DbRecordAttributes[__Operations].Value->Data = (uint8*)&pBioAPIBspCap->Operations;

			/* Options */
			DbRecordAttributes[__Options].Value->Length = sizeof(uint32);
			DbRecordAttributes[__Options].Value->Data = (uint8*)&pBioAPIBspCap->Options;

			/* PayLoadPolicy */
			DbRecordAttributes[__PayloadPolicy].Value->Length = sizeof(uint32);
			DbRecordAttributes[__PayloadPolicy].Value->Data = (uint8*)&pBioAPIBspCap->PayloadPolicy;

			/* MaxPayloadSize */
			DbRecordAttributes[__MaxPayloadSize].Value->Length = sizeof(uint32);
			DbRecordAttributes[__MaxPayloadSize].Value->Data = (uint8*)&pBioAPIBspCap->MaxPayloadSize;

			/* DefaultVerifyTimeout */
			DbRecordAttributes[__DefaultVerifyTimeout].Value->Length = sizeof(sint32);
			DbRecordAttributes[__DefaultVerifyTimeout].Value->Data = (uint8*)&pBioAPIBspCap->DefaultVerifyTimeout;

			/* DefaultIdentifyTimeout */
			DbRecordAttributes[__DefaultIdentifyTimeout].Value->Length = sizeof(sint32);
			DbRecordAttributes[__DefaultIdentifyTimeout].Value->Data = (uint8*)&pBioAPIBspCap->DefaultIdentifyTimeout;

			/* DefaultCaptureTimeout */
			DbRecordAttributes[__DefaultCaptureTimeout].Value->Length = sizeof(sint32);
			DbRecordAttributes[__DefaultCaptureTimeout].Value->Data = (uint8*)&pBioAPIBspCap->DefaultCaptureTimeout;

			/* DefaultEnrollTimeout */
			DbRecordAttributes[__DefaultEnrollTimeout].Value->Length = sizeof(sint32);
			DbRecordAttributes[__DefaultEnrollTimeout].Value->Data = (uint8*)&pBioAPIBspCap->DefaultEnrollTimeout;

			/* MaxBspDbSize */
			DbRecordAttributes[__MaxBspDbSize].Value->Length = sizeof(uint32);
			DbRecordAttributes[__MaxBspDbSize].Value->Data = (uint8*)&pBioAPIBspCap->MaxBspDbSize;

			/* Max Identify */
			DbRecordAttributes[__MaxIdentify].Value->Length = sizeof(uint32);
			DbRecordAttributes[__MaxIdentify].Value->Data = (uint8*)&pBioAPIBspCap->MaxIdentify;

			/* Description */
			DbRecordAttributes[__Description].Value->Length =
										strlen( pBioAPIBspCap->Description ) + 1 ;
			DbRecordAttributes[__Description].Value->Data =
												(uint8*)pBioAPIBspCap->Description;

			/* Path */
			DbRecordAttributes[__Path].Value->Length =
										strlen( pBioAPIBspCap->Path ) + 1 ;
			DbRecordAttributes[__Path].Value->Data =
												(uint8*)pBioAPIBspCap->Path;

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

static BioAPI_RETURN BioAPI DestructBioAPIBspCapabiltiesAttributes(
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


typedef struct bioapi_bsp_pred_state
{
	CSSM_SELECTION_PREDICATE Predicates[BIOAPI_BSP_NUM_ATTRIBUTES];
	CSSM_DATA PredicateValues[BIOAPI_BSP_NUM_ATTRIBUTES];
	uint8 PrintableUuid[BioAPI_PRINTABLE_UUID_LENGTH];
	uint8 PrintableSpecVersion[BioAPI_PRINTABLE_VERSION_LENGTH];
	uint8 PrintableProductVersion[BioAPI_PRINTABLE_VERSION_LENGTH];
}
BIOAPI_BSP_PRED_STATE;


static BioAPI_RETURN BioAPI ConstructBioAPIBspCapabiltiesPredicate(
							MDSU_CONTEXT *pContext,
							const void *pSchemaTemplate,
							MDSU_TEMPLATE_MASK ValidTemplates,
							CSSM_QUERY *pQuery,
							void **ppPredState )
{
	CSSM_RETURN rv = CSSM_OK;
	const BioAPI_BSP_SCHEMA *pBioAPIBspCap =
							(const BioAPI_BSP_SCHEMA*)pSchemaTemplate;
	BIOAPI_BSP_PRED_STATE *pState = NULL;
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
		pState = MDSU_malloc( pContext, sizeof(BIOAPI_BSP_PRED_STATE) );
		if ( pState == NULL )
		{
			ERR( rv = BioAPIERR_H_FRAMEWORK_MEMORY_ERROR );
		}
		else
		{
			memset( pState,
					0,
					sizeof(BIOAPI_BSP_PRED_STATE) );
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
		pQuery->RecordType = BIOAPI_BSP_RECORDTYPE;
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
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_MODULEID )
		{
			/* ModuleId */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_ModuleId;
			BioAPI_GetPrintableUUID( &pBioAPIBspCap->ModuleId,
									(char*)pState->PrintableUuid );
			pPredicates[uIndex].Attribute.Value->Data = pState->PrintableUuid;
			pPredicates[uIndex].Attribute.Value->Length =
								strlen( (char*)pState->PrintableUuid ) + 1;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_DEVICEID )
		{
			/* DeviceId */
			pPredicates[uIndex].Attribute.Info =
												s_BioApiAttrInfo_DeviceId;
			pPredicates[uIndex].Attribute.Value->Length = sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->DeviceId;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_BSPNAME )
		{
			/* BSPName */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_BspName;
			pPredicates[uIndex].Attribute.Value->Length = strlen( pBioAPIBspCap->BSPName ) + 1;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->BSPName;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_SPECVERSION )
		{
			/* SpecVersion */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_SpecVersion;
			BioAPI_GetPrintableVersion( &pBioAPIBspCap->SpecVersion,
									   (char*)pState->PrintableSpecVersion );
			pPredicates[uIndex].Attribute.Value->Data =
												pState->PrintableSpecVersion;
			pPredicates[uIndex].Attribute.Value->Length =
							strlen( (char*)pState->PrintableSpecVersion ) + 1;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_PRODUCTVERSION )
		{
			/* ProductVersion */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_ProductVersion;
			BioAPI_GetPrintableVersion( &pBioAPIBspCap->ProductVersion,
									   (char*)pState->PrintableProductVersion );
			pPredicates[uIndex].Attribute.Value->Data =
												pState->PrintableProductVersion;
			pPredicates[uIndex].Attribute.Value->Length =
							strlen( (char*)pState->PrintableProductVersion ) + 1;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_VENDOR )
		{
			/* Vendor */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_Vendor;
			pPredicates[uIndex].Attribute.Value->Length =
										strlen( pBioAPIBspCap->Vendor ) + 1 ;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)pBioAPIBspCap->Vendor;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_SUPPORTEDFORMATS )
		{
			/* Supportedformats */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_BspSupportedFormats;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(uint32) * pBioAPIBspCap->NumSupportedFormats;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->BspSupportedFormats;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_FACTORSMASK)
		{
			/* FactorsMask */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_FactorsMask;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->FactorsMask;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_OPERATIONS )
		{
			/* operations */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_Operations;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->Operations;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_OPTIONS )
		{
			/* Options */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_Options;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->Options;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_PAYLOADPOLICY )
		{
			/* Payloadpolicy */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_PayloadPolicy;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->PayloadPolicy;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_MAXPAYLOADSIZE )
		{
			/* MaxPayloadSize */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_MaxPayloadSize;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->MaxPayloadSize;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_DEFAULTVERIFYTIMEOUT )
		{
			/* DefaultVerifyTimeout */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DefaultVerifyTimeout;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(sint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->DefaultVerifyTimeout;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_DEFAULTIDENTIFYTIMEOUT )
		{
			/* DefaultIdentifyTimeout */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DefaultIdentifyTimeout;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(sint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->DefaultIdentifyTimeout;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_DEFAULTCAPTURETIMEOUT )
		{
			/* DefaultCaptureTimeout */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DefaultCaptureTimeout;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(sint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->DefaultCaptureTimeout;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_DEFAULTENROLLTIMEOUT )
		{
			/* DefaultEnrollTimeout */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DefaultEnrollTimeout;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(sint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->DefaultEnrollTimeout;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_MAXBSPDBSIZE )
		{
			/* Maximum database size */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_MaxBspDbSize;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->MaxBspDbSize;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_MAXIDENTIFY )
		{
			/* Maximum identify capacity */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_MaxIdentify;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(sint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIBspCap->MaxIdentify;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_DESCRIPTION )
		{
			/* Description */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_Description;
			pPredicates[uIndex].Attribute.Value->Length =
										strlen (pBioAPIBspCap->Description) + 1;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)pBioAPIBspCap->Description;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_BSP_CAP_PATH )
		{
			/* Path */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_Path;
			pPredicates[uIndex].Attribute.Value->Length =
										strlen (pBioAPIBspCap->Path) + 1;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)pBioAPIBspCap->Path;
			uIndex++;
		}

	} /* Initial allocation successful */


	*ppPredState = (void*)pState;
	return rv;
}

static BioAPI_RETURN BioAPI DestructBioAPIBspCapabiltiesPredicate(
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

static BioAPI_RETURN BioAPI ConvertBioAPIBspCapabiltiesAttributes(
							MDSU_CONTEXT *pContext,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							void *pSchemaData )
{
	CSSM_RETURN crReturn;
	BioAPI_BSP_SCHEMA *pBioAPIBspCap = (BioAPI_BSP_SCHEMA *)pSchemaData;

	const CSSM_DB_ATTRIBUTE_DATA *DbAttr = NULL;

	assert( pSchemaData && DbRecord && pContext );

	DbAttr = DbRecord->AttributeData;


	/* ModuleId */
	crReturn = BioAPI_GetStructuredUUID(
								(char*)DbAttr[__ModuleId].Value->Data,
								&pBioAPIBspCap->ModuleId );
	if ( crReturn != CSSM_OK )
	{
		return crReturn;
	}

	/* DeviceId */
	pBioAPIBspCap->DeviceId = *(uint32*)DbAttr[__DeviceId].Value->Data;

	/* BspName */
	if (DbAttr[__BspName].Value->Length)
	{
		strcpy( pBioAPIBspCap->BSPName,
				(char*)DbAttr[__BspName].Value->Data );
	}
	else
		pBioAPIBspCap->BSPName[0] = '\0';

	/* SpecVersion */
	crReturn = BioAPI_GetStructuredVersion(
									(char*)DbAttr[__SpecVersion].Value->Data,
									&pBioAPIBspCap->SpecVersion );
	/* ProductVersion */
	crReturn = BioAPI_GetStructuredVersion(
									(char*)DbAttr[__ProductVersion].Value->Data,
									&pBioAPIBspCap->ProductVersion );

	/* Vendor */
	if (DbAttr[__Vendor].Value->Length)
	{
		strcpy( pBioAPIBspCap->Vendor,
				(char*)DbAttr[__Vendor].Value->Data );
	}
	else
		pBioAPIBspCap->Vendor[0] = '\0';

	/* SupportedFormats */
	pBioAPIBspCap->NumSupportedFormats = (DbAttr[__BspSupportedFormats].Value->Length / sizeof(uint32));
	if((0 != pBioAPIBspCap->NumSupportedFormats) &&
	   (16 >= pBioAPIBspCap->NumSupportedFormats))
	{
		memcpy(pBioAPIBspCap->BspSupportedFormats,
			   DbAttr[__BspSupportedFormats].Value->Data,
			   (sizeof(uint32) * pBioAPIBspCap->NumSupportedFormats));
	}
	else
	{
		/* we can't handle more than 16 SupportedFormats */
		if(16 < pBioAPIBspCap->NumSupportedFormats)
		{
			pBioAPIBspCap->NumSupportedFormats = 0;
			return BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR;
		}
	}

	/* FactorMask */
	pBioAPIBspCap->FactorsMask = *(uint32*)DbAttr[__FactorsMask].Value->Data;

	/* Operations */
	pBioAPIBspCap->Operations = *(uint32*)DbAttr[__Operations].Value->Data;

	/* Options */
	pBioAPIBspCap->Options = *(uint32*)DbAttr[__Options].Value->Data;

	/* PayloadPolicy */
	pBioAPIBspCap->PayloadPolicy = *(uint32*)DbAttr[__PayloadPolicy].Value->Data;

	/* MaxPayloadSize */
	pBioAPIBspCap->MaxPayloadSize = *(uint32*)DbAttr[__MaxPayloadSize].Value->Data;

	/* MaxPayloadSize */
	pBioAPIBspCap->MaxPayloadSize = *(uint32*)DbAttr[__MaxPayloadSize].Value->Data;

	/* DefaultVerifyTimeout */
	pBioAPIBspCap->DefaultVerifyTimeout = *(uint32*)DbAttr[__DefaultVerifyTimeout].Value->Data;

	/* DefaultIdentifyTimeout */
	pBioAPIBspCap->DefaultIdentifyTimeout = *(uint32*)DbAttr[__DefaultIdentifyTimeout].Value->Data;

	/* DefaultCaptureTimeout*/
	pBioAPIBspCap->DefaultCaptureTimeout = *(uint32*)DbAttr[__DefaultCaptureTimeout].Value->Data;

	/* DefaultEnrollTimeout */
	pBioAPIBspCap->DefaultEnrollTimeout = *(uint32*)DbAttr[__DefaultEnrollTimeout].Value->Data;

	/* MaxBspDbSize */
	pBioAPIBspCap->MaxBspDbSize = *(uint32*)DbAttr[__MaxBspDbSize].Value->Data;

	/* MaxIdentify */
	pBioAPIBspCap->MaxIdentify = *(uint32*)DbAttr[__MaxIdentify].Value->Data;

	/* Description */
	if (DbAttr[__Description].Value->Length)
	{
		strcpy( pBioAPIBspCap->Description,
				(char*)DbAttr[__Description].Value->Data );
	}
	else
		pBioAPIBspCap->Description[0] = '\0';

	/* Path */
	if (DbAttr[__Path].Value->Length)
	{
		strcpy( pBioAPIBspCap->Path,
				(char*)DbAttr[__Path].Value->Data );
	}
	else
		pBioAPIBspCap->Path[0] = '\0';

	return CSSM_OK;
}


const IfMdsuSchemaManipulator IfiMdsuBioAPIBspCapabilitySchema =
{
	ConstructBioAPIBspCapabiltiesAttributes,
	DestructBioAPIBspCapabiltiesAttributes,
	ConstructBioAPIBspCapabiltiesPredicate,
	DestructBioAPIBspCapabiltiesPredicate,
	ConvertBioAPIBspCapabiltiesAttributes,
	BIOAPI_BSP_RECORDTYPE
};
