/*-----------------------------------------------------------------------
 * File: MDS_UTIL_ATTR.C
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the mds functions.
 */
#ifdef BioAPI_MEMTRACK_ON
#define CSSM_MEMTRACK_ON
#endif

#include "bioapi.h"
#include "bioapi_schema.h"
#include "bioapi_util.h"
#include "mds_app_util.h"
#include "mds_util_intrn.h"
//#include <cssmMemTrack.h>


/*-----------------------------------------------------------------------------
							Internal Functions
-----------------------------------------------------------------------------*/
BioAPI_RETURN set_attrib_at_pos(CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
							  uint32 pos,
							  const CSSM_DB_ATTRIBUTE_INFO *AttributeInfo,
							  uint8 *Data,
							  uint32 Length);

/*---------------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
h_layer_schema_GetAttributes(bioapi_MDS_H_LAYER_PTR Record,
						  CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes)
{
	BioAPI_RETURN retValue = BioAPI_OK;

	Attributes->DataRecordType = BIOAPI_H_LAYER_RECORDTYPE;
	Attributes->SemanticInformation = 0;
	Attributes->NumberOfAttributes = BIOAPI_H_LAYER_NUM_ATTRIBUTES;

	/* ModuleId */
	if ((retValue = set_attrib_at_pos(Attributes,
									  0,
									  &s_BioApiAttrInfo_ModuleId,
									  Record->moduleid.Data,
									  Record->moduleid.Length)) != BioAPI_OK)
		return (retValue);

	/* ModuleName */
	if ((retValue = set_attrib_at_pos(Attributes,
									  1,
									  &s_BioApiAttrInfo_ModuleName,
									  Record->modulename.Data,
									  Record->modulename.Length)) != BioAPI_OK)
		return (retValue);

	/* Product Version */
	if ((retValue = set_attrib_at_pos(Attributes,
									  2,
									  &s_BioApiAttrInfo_ProductVersion,
									  Record->productversion.Data,
									  Record->productversion.Length)) != BioAPI_OK)
		return (retValue);

	/* Spec Version */
	if ((retValue = set_attrib_at_pos(Attributes,
									  3,
									  &s_BioApiAttrInfo_SpecVersion,
									  Record->specversion.Data,
									  Record->specversion.Length)) != BioAPI_OK)
		return (retValue);

	/* Vendor */
	if ((retValue = set_attrib_at_pos(Attributes,
									  4,
									  &s_BioApiAttrInfo_Vendor,
									  Record->vendor.Data,
									  Record->vendor.Length)) != BioAPI_OK)
		return (retValue);

	/* Description */
	if ((retValue = set_attrib_at_pos(Attributes,
									  5,
									  &s_BioApiAttrInfo_Description,
									  Record->description.Data,
									  Record->description.Length)) != BioAPI_OK)
		return (retValue);

	return (retValue);
}

BioAPI_RETURN BioAPI
bsp_schema_GetAttributes(bioapi_MDS_BSP_RECORD_PTR Record,
						 CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes)
{
	BioAPI_RETURN retValue = BioAPI_OK;

	Attributes->DataRecordType = BIOAPI_BSP_RECORDTYPE;
	Attributes->SemanticInformation = 0;
	Attributes->NumberOfAttributes = BIOAPI_BSP_NUM_ATTRIBUTES;

	/* ModuleId */
	if ((retValue = set_attrib_at_pos(Attributes,
									  0,
									  &s_BioApiAttrInfo_ModuleId,
									  Record->moduleid.Data,
									  Record->moduleid.Length)) != BioAPI_OK)
		return (retValue);

	/* Device ID */
	if ((retValue = set_attrib_at_pos(Attributes,
									  1,
									  &s_BioApiAttrInfo_DeviceId,
									  Record->deviceid.Data,
									  Record->deviceid.Length)) != BioAPI_OK)
		return (retValue);

	/* BSP Name */
	if ((retValue = set_attrib_at_pos(Attributes,
									  2,
									  &s_BioApiAttrInfo_BspName,
									  Record->bspname.Data,
									  Record->bspname.Length)) != BioAPI_OK)
		return (retValue);

	/* Product version */
	if ((retValue = set_attrib_at_pos(Attributes,
									  3,
									  &s_BioApiAttrInfo_ProductVersion,
									  Record->productversion.Data,
									  Record->productversion.Length)) != BioAPI_OK)
		return (retValue);

	/* Spec version */
	if ((retValue = set_attrib_at_pos(Attributes,
									  4,
									  &s_BioApiAttrInfo_SpecVersion,
									  Record->specversion.Data,
									  Record->specversion.Length)) != BioAPI_OK)
		return (retValue);

	/* Supported formats */
	if ((retValue = set_attrib_at_pos(Attributes,
									  5,
									  &s_BioApiAttrInfo_BspSupportedFormats,
									  Record->supportedformats.Data,
									  Record->supportedformats.Length)) != BioAPI_OK)
		return (retValue);

	/* Vendor */
	if ((retValue = set_attrib_at_pos(Attributes,
									  6,
									  &s_BioApiAttrInfo_Vendor,
									  Record->vendor.Data,
									  Record->vendor.Length)) != BioAPI_OK)
		return (retValue);

	/* Factors mask */
	if ((retValue = set_attrib_at_pos(Attributes,
									  7,
									  &s_BioApiAttrInfo_FactorsMask,
									  Record->factorsmask.Data,
									  Record->factorsmask.Length)) != BioAPI_OK)
		return (retValue);

	/* Operations */
	if ((retValue = set_attrib_at_pos(Attributes,
									  8,
									  &s_BioApiAttrInfo_Operations,
									  Record->operations.Data,
									  Record->operations.Length)) != BioAPI_OK)
		return (retValue);

	/* Options */
	if ((retValue = set_attrib_at_pos(Attributes,
									  9,
									  &s_BioApiAttrInfo_Options,
									  Record->options.Data,
									  Record->options.Length)) != BioAPI_OK)
		return (retValue);

	/* Payload policy */
	if ((retValue = set_attrib_at_pos(Attributes,
									  10,
									  &s_BioApiAttrInfo_PayloadPolicy,
									  Record->payloadpolicy.Data,
									  Record->payloadpolicy.Length)) != BioAPI_OK)
		return (retValue);

	/* Max payload size */
	if ((retValue = set_attrib_at_pos(Attributes,
									  11,
									  &s_BioApiAttrInfo_MaxPayloadSize,
									  Record->maxpayloadsize.Data,
									  Record->maxpayloadsize.Length)) != BioAPI_OK)
		return (retValue);

	/* Default verify timeout */
	if ((retValue = set_attrib_at_pos(Attributes,
									  12,
									  &s_BioApiAttrInfo_DefaultVerifyTimeout,
									  Record->defaultverifytimeout.Data,
									  Record->defaultverifytimeout.Length)) != BioAPI_OK)
		return (retValue);

	/* Default identify timeout */
	if ((retValue = set_attrib_at_pos(Attributes,
									  13,
									  &s_BioApiAttrInfo_DefaultIdentifyTimeout,
									  Record->defaultidentifytimeout.Data,
									  Record->defaultidentifytimeout.Length)) != BioAPI_OK)
		return (retValue);

	/* Default capture timeout */
	if ((retValue = set_attrib_at_pos(Attributes,
									  14,
									  &s_BioApiAttrInfo_DefaultCaptureTimeout,
									  Record->defaultcapturetimeout.Data,
									  Record->defaultcapturetimeout.Length)) != BioAPI_OK)
		return (retValue);

	/* Default enroll timeout */
	if ((retValue = set_attrib_at_pos(Attributes,
									  15,
									  &s_BioApiAttrInfo_DefaultEnrollTimeout,
									  Record->defaultenrolltimeout.Data,
									  Record->defaultenrolltimeout.Length)) != BioAPI_OK)
		return (retValue);

	/* Max BSP DB size */
	if ((retValue = set_attrib_at_pos(Attributes,
									  16,
									  &s_BioApiAttrInfo_MaxBspDbSize,
									  Record->maxbspdbsize.Data,
									  Record->maxbspdbsize.Length)) != BioAPI_OK)
		return (retValue);

	/* Max identify */
	if ((retValue = set_attrib_at_pos(Attributes,
									  17,
									  &s_BioApiAttrInfo_MaxIdentify,
									  Record->maxidentify.Data,
									  Record->maxidentify.Length)) != BioAPI_OK)
		return (retValue);

	/* Description */
	if ((retValue = set_attrib_at_pos(Attributes,
									  18,
									  &s_BioApiAttrInfo_Description,
									  Record->description.Data,
									  Record->description.Length)) != BioAPI_OK)
		return (retValue);

	/* Path */
	if ((retValue = set_attrib_at_pos(Attributes,
									  19,
									  &s_BioApiAttrInfo_Path,
									  Record->path.Data,
									  Record->path.Length)) != BioAPI_OK)
		return (retValue);

	return (retValue);
}


/*
 *	SET ATTRIBute AT POSition in array of attributes
 */
BioAPI_RETURN set_attrib_at_pos(CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
							  uint32 pos,
							  const CSSM_DB_ATTRIBUTE_INFO *AttributeInfo,
							  uint8 *Data,
							  uint32 Length)
{
	CSSM_DB_ATTRIBUTE_DATA_PTR attrib = &Attributes->AttributeData[pos];

	attrib->Info = *AttributeInfo;
	attrib->NumberOfValues = 1;

	if ( (attrib->Value != NULL) && (!IsBadWritePtr( attrib->Value, sizeof(CSSM_DATA))) )
	{
		attrib->Value->Data = Data;
		attrib->Value->Length = Length;
	}

	return (BioAPI_OK);
}

/*
 *	CLEAN ATTRIBUTES (upto given position)
 */
void clean_attributes(CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
					  uint32 pos)
{
	uint32 index = 0;
	CSSM_DB_ATTRIBUTE_DATA_PTR attrib = &Attributes->AttributeData[pos];

	for (index = 0; index < pos; index++)
	{
		attrib = &Attributes->AttributeData[index];
		if (attrib->Value != NULL)
		{
			BioAPI_free(attrib->Value, NULL);
			attrib->Value = NULL;
		}
	}
}

/*
 *	ALLOCATE VALUE in an array of attributes for given NUMber of ATTRIBUTES
 *	if failure this function cleans-up allocations
 */
BioAPI_RETURN allocate_value(CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes, uint32 numAttributes)
{
	uint32 index = 0;

	for(index = 0; index < numAttributes; index++)
	{
		Attributes->AttributeData[index].NumberOfValues = 1;

		Attributes->AttributeData[index].Value = (CSSM_DATA *) BioAPI_calloc (1, Attributes->AttributeData[index].NumberOfValues * sizeof (CSSM_DATA), NULL);
		if (Attributes->AttributeData[index].Value == NULL)
		{
			/* clean up; clean all the prior allocations */
			clean_attributes(Attributes, Attributes->NumberOfAttributes);

			/* allocation error */
			return (BioAPI_ERRCODE_MEMORY_ERROR);
		}
	}

	return (BioAPI_OK);
}
