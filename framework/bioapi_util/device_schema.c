/*-----------------------------------------------------------------------
 * File: device_schema.c
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

/* BioAPI device schema header */
#include "device_schema.h"


#define __ModuleId				 ( 0 )
#define __DeviceId				 ( 1 )
#define __DeviceSupportedFormats ( 2 )
#define __SupportedEvents		 ( 3 )
#define __DeviceVendor			 ( 4 )
#define __DeviceDescription		 ( 5 )
#define __DeviceSerialNumber	 ( 6 )
#define __DeviceHardwareVersion	 ( 7 )
#define __DeviceFirmwareVersion	 ( 8 )
#define __AuthenticatedDevice	 ( 9 )

typedef struct bioapi_dynamic_attr_state
{
	CSSM_DB_ATTRIBUTE_DATA DbRecordAttributes[BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES];
	BioAPI_DATA AttributeValues[BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES];
	uint8 PrintableUuid[BioAPI_PRINTABLE_UUID_LENGTH];
	uint8 PrintableDeviceHardwareVersion[BioAPI_PRINTABLE_VERSION_LENGTH];
	uint8 PrintableDeviceFirmwareVersion[BioAPI_PRINTABLE_VERSION_LENGTH];
}
BIOAPI_DEVICE_ATTR_STATE;

static BioAPI_RETURN BioAPI ConstructBioAPIDeviceSchemaAttributes(
							MDSU_CONTEXT *pContext,
							const void *pSchemaData,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							BioAPI_BOOL bFillValues,
							void **ppAttrState )
{
	BioAPI_RETURN rv = CSSM_OK;
	const BioAPI_DEVICE_SCHEMA *pBioAPIDevice =
							(const BioAPI_DEVICE_SCHEMA*)pSchemaData;
	BIOAPI_DEVICE_ATTR_STATE *pState = NULL;
	CSSM_DB_ATTRIBUTE_DATA *DbRecordAttributes;
	uint32 i;

	assert( pContext && DbRecord && ppAttrState );
	assert( ( bFillValues == CSSM_FALSE ) ||
			( ( bFillValues == CSSM_TRUE ) && pSchemaData ) );

	/* Allocate the memory for the attributes */
	pState = MDSU_malloc( pContext, sizeof(BIOAPI_DEVICE_ATTR_STATE) );
	if ( pState == NULL )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_MEMORY_ERROR );
	}
	else
	{
		/* Clear the structure */
		memset( pState, 0, sizeof(BIOAPI_DEVICE_ATTR_STATE) );
		DbRecordAttributes = pState->DbRecordAttributes;

		/* Fill in the record information */
		DbRecord->DataRecordType = BIOAPI_BIO_DEVICE_RECORDTYPE;
		DbRecord->SemanticInformation = 0;
		DbRecord->NumberOfAttributes = BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES;
		DbRecord->AttributeData = DbRecordAttributes;


		/* Fill in attribute info */
		DbRecordAttributes[__ModuleId].Info = s_BioApiAttrInfo_ModuleId;
		DbRecordAttributes[__DeviceId].Info = s_BioApiAttrInfo_DeviceId;
		DbRecordAttributes[__DeviceSupportedFormats].Info = s_BioApiAttrInfo_DeviceSupportedFormats;
		DbRecordAttributes[__SupportedEvents].Info = s_BioApiAttrInfo_SupportedEvents;
		DbRecordAttributes[__DeviceVendor].Info = s_BioApiAttrInfo_DeviceVendor;
		DbRecordAttributes[__DeviceDescription].Info = s_BioApiAttrInfo_DeviceDescription;
		DbRecordAttributes[__DeviceSerialNumber].Info = s_BioApiAttrInfo_DeviceSerialNumber;
		DbRecordAttributes[__DeviceHardwareVersion].Info = s_BioApiAttrInfo_DeviceHardwareVersion;
		DbRecordAttributes[__DeviceFirmwareVersion].Info = s_BioApiAttrInfo_DeviceFirmwareVersion;
		DbRecordAttributes[__AuthenticatedDevice].Info = s_BioApiAttrInfo_AuthenticatedDevice;

		if ( bFillValues == CSSM_TRUE )
		{
			/* Connect the attribute value array to the attribute array */
			for ( i = 0; i < DbRecord->NumberOfAttributes; i++ )
			{
				DbRecordAttributes[i].Value = (CSSM_DATA *) &pState->AttributeValues[i];
				DbRecordAttributes[i].NumberOfValues = 1;
			}

			/* Fill in the record contents */
			/* ModuleId */
			BioAPI_GetPrintableUUID( &pBioAPIDevice->ModuleId,
									(char*)pState->PrintableUuid );
			DbRecordAttributes[__ModuleId].Value->Data =
													pState->PrintableUuid;
			DbRecordAttributes[__ModuleId].Value->Length =
								strlen( (char*)pState->PrintableUuid ) + 1;

			/* DeviceId */
			DbRecordAttributes[__DeviceId].Value->Length = sizeof(uint32);
			DbRecordAttributes[__DeviceId].Value->Data = (uint8*)&pBioAPIDevice->DeviceId;

			/* SupportedFormats */
			DbRecordAttributes[__DeviceSupportedFormats].Value->Length =
											sizeof(uint32) * pBioAPIDevice->NumSupportedFormats;
			DbRecordAttributes[__DeviceSupportedFormats].Value->Data = (uint8*)&pBioAPIDevice->DeviceSupportedFormats;

			/* SupportedEvents */
			DbRecordAttributes[__SupportedEvents].Value->Length =
											sizeof(uint32);
			DbRecordAttributes[__SupportedEvents].Value->Data = (uint8*)&pBioAPIDevice->SupportedEvents;

			/* DeviceVendor */
			DbRecordAttributes[__DeviceVendor].Value->Length =
										strlen( pBioAPIDevice->DeviceVendor ) + 1 ;
			DbRecordAttributes[__DeviceVendor].Value->Data =
												(uint8*)pBioAPIDevice->DeviceVendor;

			/* DeviceDescription */
			DbRecordAttributes[__DeviceDescription].Value->Length =
										strlen( pBioAPIDevice->DeviceDescription ) + 1 ;
			DbRecordAttributes[__DeviceDescription].Value->Data =
												(uint8*)pBioAPIDevice->DeviceDescription;

			/* DeviceSerialNumber */
			DbRecordAttributes[__DeviceSerialNumber].Value->Length =
										strlen( pBioAPIDevice->DeviceSerialNumber ) + 1 ;
			DbRecordAttributes[__DeviceSerialNumber].Value->Data =
												(uint8*)pBioAPIDevice->DeviceSerialNumber;

			/* DeviceHardwareVersion */
			BioAPI_GetPrintableVersion( &pBioAPIDevice->DeviceHardwareVersion,
									   (char*)pState->PrintableDeviceHardwareVersion );
			DbRecordAttributes[__DeviceHardwareVersion].Value->Data =
													pState->PrintableDeviceHardwareVersion;
			DbRecordAttributes[__DeviceHardwareVersion].Value->Length =
								strlen( (char*)pState->PrintableDeviceHardwareVersion ) + 1;

			/* DeviceFirmwareVersion */
			BioAPI_GetPrintableVersion( &pBioAPIDevice->DeviceFirmwareVersion,
									   (char*)pState->PrintableDeviceFirmwareVersion );
			DbRecordAttributes[__DeviceFirmwareVersion].Value->Data =
													pState->PrintableDeviceFirmwareVersion;
			DbRecordAttributes[__DeviceFirmwareVersion].Value->Length =
								strlen( (char*)pState->PrintableDeviceFirmwareVersion ) + 1;


			/* AuthenticatedDevice */
			DbRecordAttributes[__AuthenticatedDevice].Value->Length = sizeof(uint32);
			DbRecordAttributes[__AuthenticatedDevice].Value->Data = (uint8*)&pBioAPIDevice->AuthenticatedDevice;



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

static CSSM_RETURN CSSMAPI DestructBioAPIDeviceSchemaAttributes(
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


typedef struct bioapi_device_pred_state
{
	CSSM_SELECTION_PREDICATE Predicates[BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES];
	CSSM_DATA PredicateValues[BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES];
	uint8 PrintableUuid[BioAPI_PRINTABLE_UUID_LENGTH];
	uint8 PrintableDeviceHardwareVersion[BioAPI_PRINTABLE_VERSION_LENGTH];
	uint8 PrintableDeviceFirmwareVersion[BioAPI_PRINTABLE_VERSION_LENGTH];
}
BIOAPI_DEVICE_PRED_STATE;



static BioAPI_RETURN BioAPI ConstructBioAPIDevicePredicate(
							MDSU_CONTEXT *pContext,
							const void *pSchemaTemplate,
							MDSU_TEMPLATE_MASK ValidTemplates,
							CSSM_QUERY *pQuery,
							void **ppPredState )
{
	BioAPI_RETURN rv = CSSM_OK;
	const BioAPI_DEVICE_SCHEMA *pBioAPIDevice=
							(const BioAPI_DEVICE_SCHEMA*)pSchemaTemplate;
	BIOAPI_DEVICE_PRED_STATE *pState = NULL;
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
		pState = MDSU_malloc( pContext, sizeof(BIOAPI_DEVICE_PRED_STATE) );
		if ( pState == NULL )
		{
			ERR( rv = BioAPIERR_H_FRAMEWORK_MEMORY_ERROR );
		}
		else
		{
			memset( pState,
					0,
					sizeof(BIOAPI_DEVICE_PRED_STATE) );
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
		pQuery->RecordType = BIOAPI_BIO_DEVICE_RECORDTYPE;
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
		if ( ValidTemplates & MDSU_BIOAPI_DEV_MODULEID )
		{
			/* ModuleId */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_ModuleId;
			BioAPI_GetPrintableUUID( &pBioAPIDevice->ModuleId,
									(char*)pState->PrintableUuid );
			pPredicates[uIndex].Attribute.Value->Data = pState->PrintableUuid;
			pPredicates[uIndex].Attribute.Value->Length =
								strlen( (char*)pState->PrintableUuid ) + 1;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_DEV_DEVICEID )
		{
			/* DeviceId */
			pPredicates[uIndex].Attribute.Info =
												s_BioApiAttrInfo_DeviceId;
			pPredicates[uIndex].Attribute.Value->Length = sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIDevice->DeviceId;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_DEV_SUPPORTEDFORMATS )
		{
			/* SampleTypes */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DeviceSupportedFormats;
			pPredicates[uIndex].Attribute.Value->Length =
										sizeof(uint32) * pBioAPIDevice->NumSupportedFormats;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIDevice->DeviceSupportedFormats;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_DEV_SUPPORTEDEVENTS )
		{
			/* DeviceId */
			pPredicates[uIndex].Attribute.Info =
												s_BioApiAttrInfo_SupportedEvents;
			pPredicates[uIndex].Attribute.Value->Length = sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIDevice->SupportedEvents;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_DEV_DEVICEVENDOR )
		{
			/* Vendor */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DeviceVendor;
			pPredicates[uIndex].Attribute.Value->Length =
										strlen( pBioAPIDevice->DeviceVendor ) + 1 ;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)pBioAPIDevice->DeviceVendor;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_DEV_DEVICEDESCRIPTION )
		{
			/* Vendor */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DeviceDescription;
			pPredicates[uIndex].Attribute.Value->Length =
										strlen( pBioAPIDevice->DeviceDescription ) + 1 ;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)pBioAPIDevice->DeviceDescription;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_DEV_DEVICESERALNUMBER )
		{
			/* Vendor */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DeviceSerialNumber;
			pPredicates[uIndex].Attribute.Value->Length =
										strlen( pBioAPIDevice->DeviceSerialNumber ) + 1 ;
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)pBioAPIDevice->DeviceSerialNumber;
			uIndex++;
		}

		if ( ValidTemplates & MDSU_BIOAPI_DEV_DEVICEHARDWAREVERSION )
		{
			/* Version */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DeviceHardwareVersion;
			BioAPI_GetPrintableVersion( &pBioAPIDevice->DeviceHardwareVersion,
									   (char*)pState->PrintableDeviceHardwareVersion );
			pPredicates[uIndex].Attribute.Value->Data =
												pState->PrintableDeviceHardwareVersion;
			pPredicates[uIndex].Attribute.Value->Length =
							strlen( (char*)pState->PrintableDeviceHardwareVersion ) + 1;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_DEV_DEVICEFIRMWAREVERSION )
		{
			/* Version */
			pPredicates[uIndex].Attribute.Info = s_BioApiAttrInfo_DeviceFirmwareVersion;
			BioAPI_GetPrintableVersion( &pBioAPIDevice->DeviceFirmwareVersion ,
									   (char*)pState->PrintableDeviceFirmwareVersion );
			pPredicates[uIndex].Attribute.Value->Data =
												pState->PrintableDeviceFirmwareVersion;
			pPredicates[uIndex].Attribute.Value->Length =
							strlen( (char*)pState->PrintableDeviceFirmwareVersion ) + 1;
			uIndex++;
		}
		if ( ValidTemplates & MDSU_BIOAPI_DEV_AUTHENTICATEDDEVICE )
		{
			/* AuthenticatedDevice */
			pPredicates[uIndex].Attribute.Info =
												s_BioApiAttrInfo_AuthenticatedDevice;
			pPredicates[uIndex].Attribute.Value->Length = sizeof(uint32);
			pPredicates[uIndex].Attribute.Value->Data =
												(uint8*)&pBioAPIDevice->AuthenticatedDevice;
			uIndex++;
		}


	} /* Initial allocation successful */

	*ppPredState = (void*)pState;

	return rv;
}

static CSSM_RETURN CSSMAPI DestructBioAPIDevicePredicate(
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

static CSSM_RETURN CSSMAPI ConvertBioAPIDeviceAttributes(
							MDSU_CONTEXT *pContext,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							void *pSchemaData )
{
	CSSM_RETURN crReturn;
	BioAPI_DEVICE_SCHEMA *pBioAPIDevice = (BioAPI_DEVICE_SCHEMA*)pSchemaData;

	const CSSM_DB_ATTRIBUTE_DATA *DbAttr = NULL;

	assert( pSchemaData && DbRecord && pContext );

	DbAttr = DbRecord->AttributeData;


	 /* ModuleId */
	crReturn = BioAPI_GetStructuredUUID(
								(char*)DbAttr[__ModuleId].Value->Data,
								&pBioAPIDevice->ModuleId );
	if ( crReturn != CSSM_OK )
	{
		return crReturn;
	}

	/* DeviceId */
	pBioAPIDevice->DeviceId = *(uint32*)DbAttr[__DeviceId].Value->Data;

	/* SupportedFormats */
	pBioAPIDevice->NumSupportedFormats = (DbAttr[__DeviceSupportedFormats].Value->Length / sizeof(uint32));
	if((0 != pBioAPIDevice->NumSupportedFormats) &&
	   (16 >= pBioAPIDevice->NumSupportedFormats))
	{
		memcpy(pBioAPIDevice->DeviceSupportedFormats,
			   DbAttr[__DeviceSupportedFormats].Value->Data,
			   (sizeof(uint32) * pBioAPIDevice->NumSupportedFormats));
	}
	else
	{
		/* we can't handle more than 16 SupportedFormats */
		if(16 < pBioAPIDevice->NumSupportedFormats)
		{
			pBioAPIDevice->NumSupportedFormats = 0;
			return BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR;
		}
	}

	/* SupportedEvents */
	pBioAPIDevice->SupportedEvents = *(uint32*)DbAttr[__SupportedEvents].Value->Data;
	/* DeviceVendor */
	strcpy( pBioAPIDevice->DeviceVendor,
			(char*)DbAttr[__DeviceVendor].Value->Data );

	/* DeviceDescription */
	strcpy( pBioAPIDevice->DeviceDescription,
			(char*)DbAttr[__DeviceDescription].Value->Data );

	/* DeviceSerialNumber */
	strcpy( pBioAPIDevice->DeviceSerialNumber,
			(char*)DbAttr[__DeviceSerialNumber].Value->Data );

	/* DeviceHardwareVersion */
	crReturn = BioAPI_GetStructuredVersion(
									(char*)DbAttr[__DeviceHardwareVersion].Value->Data,
									&pBioAPIDevice->DeviceHardwareVersion );

	/* DeviceFirmwareVersion */
	crReturn = BioAPI_GetStructuredVersion(
									(char*)DbAttr[__DeviceFirmwareVersion].Value->Data,
									&pBioAPIDevice->DeviceFirmwareVersion );

	/* AuthenticatdDevice */
	pBioAPIDevice->AuthenticatedDevice = *(uint32*)DbAttr[__AuthenticatedDevice].Value->Data;

	return CSSM_OK;
}

const IfMdsuSchemaManipulator IfiMdsuBioAPIBspDeviceSchema =
{
	ConstructBioAPIDeviceSchemaAttributes,
	DestructBioAPIDeviceSchemaAttributes,
	ConstructBioAPIDevicePredicate,
	DestructBioAPIDevicePredicate,
	ConvertBioAPIDeviceAttributes,
	BIOAPI_BIO_DEVICE_RECORDTYPE
};
