/*-----------------------------------------------------------------------
 * File: MDS_UTIL_INTRN.H
 *-----------------------------------------------------------------------
 */


#ifndef _MDS_UTIL_INTRN_H
#define _MDS_UTIL_INTRN_H

#include <stdio.h>
#include <stdlib.h>
#include "port/bioapi_port.h"
#include "mds.h"
#include "bioapi_schema.h"
#include "intel/mds_util_type.h"
#include "intel/mds_util_helper.h"


typedef struct bioapi_mds_h_layer
{
	CSSM_DATA moduleid;
	CSSM_DATA modulename;
	CSSM_DATA productversion;
	CSSM_DATA specversion;
	CSSM_DATA vendor;
	CSSM_DATA description;
} bioapi_MDS_H_LAYER, *bioapi_MDS_H_LAYER_PTR;

typedef struct bioapi_mds_bsp_record
{
	CSSM_DATA moduleid;
	CSSM_DATA deviceid;
	CSSM_DATA bspname;
	CSSM_DATA productversion;
	CSSM_DATA specversion;
	CSSM_DATA supportedformats;
	CSSM_DATA vendor;
	CSSM_DATA factorsmask;
	CSSM_DATA operations;
	CSSM_DATA options;
	CSSM_DATA payloadpolicy;
	CSSM_DATA maxpayloadsize;
	CSSM_DATA defaultverifytimeout;
	CSSM_DATA defaultidentifytimeout;
	CSSM_DATA defaultcapturetimeout;
	CSSM_DATA defaultenrolltimeout;
	CSSM_DATA maxbspdbsize;
	CSSM_DATA maxidentify;
	CSSM_DATA description;
	CSSM_DATA path;
} bioapi_MDS_BSP_RECORD, *bioapi_MDS_BSP_RECORD_PTR;

typedef struct bioapi_mds_device_record
{
	CSSM_DATA moduleid;
	CSSM_DATA deviceid;
	CSSM_DATA supportedformats;
	CSSM_DATA supportedevents;
	CSSM_DATA devicevendor;
	CSSM_DATA devicedescription;
	CSSM_DATA deviceserialnumber;
	CSSM_DATA devicehardwareversion;
	CSSM_DATA firmwareversion;
	CSSM_DATA authenticateddevice;
} bioapi_MDS_DEVICE_RECORD, *bioapi_MDS_DEVICE_RECORD_PTR;

/* mds related internal structures*/
typedef struct cssm_attr_type_value
{
	uint32 AttrType;
	uint32 count;
	CSSM_DATA AttrValue;
} cssm_ATTR_TYPE_VALUE, *cssm_ATTR_TYPE_VALUE_PTR;



void BioAPI MDSUTIL_Term ();


BioAPI_RETURN mdsutil_FreeModule (const MDSUTIL_MODULE_INFO_PTR ModuleDescription);

BioAPI_RETURN BioAPI mdsutil_DeleteRecordByUuid(CSSM_DL_DB_HANDLE hDLDB,
											   const BioAPI_UUID *guid,
											   CSSM_DB_RECORDTYPE RecordType);

BioAPI_RETURN BioAPI h_layer_schema_GetAttributes(bioapi_MDS_H_LAYER_PTR Record,
									   CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes);

BioAPI_RETURN BioAPI bsp_schema_GetAttributes(bioapi_MDS_BSP_RECORD_PTR Record,
									   CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes);

void BioAPI mdsutil_FreeAttributes (uint32 NumberOfAttributes,
									 CSSM_CONTEXT_ATTRIBUTE_PTR Attributes,
									 CSSM_FREE MDSUTIL_free,
									 void *mem_ref,
									 uint32 Free);

BioAPI_RETURN BioAPI mdsutil_FreeDbInfo (CSSM_DBINFO_PTR Info);

BioAPI_RETURN BioAPI mdsutil_FreeDbInfo (CSSM_DBINFO_PTR Info);

BioAPI_RETURN BioAPI mdsutil_FreeHardwareInfo (MDSUTIL_HARDWARE_CSPSUBSERVICE_INFO_PTR Info);

BioAPI_RETURN BioAPI mdsutil_FreeDbAttributeInfo(CSSM_DB_ATTRIBUTE_INFO_PTR Info);

BioAPI_RETURN mdsutil_CheckPermissions();

BioAPI_RETURN BioAPI mdsutil_GetModuleLocationfromUuid (const BioAPI_UUID_PTR ModuleUUID,
													   char *path);

BioAPI_RETURN BioAPI mdsutil_ModuleUninstall (const BioAPI_UUID *ModuleUUID);

BioAPI_RETURN BioAPI mdsutil_ModuleManagerUninstall (const BioAPI_UUID *Uuid);

BioAPI_RETURN BioAPI mdsutil_ListModules (CSSM_SERVICE_MASK UsageMask,
										 CSSM_BOOL MatchAll,
										 MDSUTIL_LIST_PTR *pList);

BioAPI_RETURN BioAPI mdsutil_mds_SetModuleInfo(const BioAPI_UUID *ModuleUUID,
											  const MDSUTIL_MODULE_INFO *ModuleInfo);

BioAPI_RETURN BioAPI mdsutil_ListInstalledEmms(BioAPI_UUID_PTR *ModuleManagerUuids,
											  uint32* NumberOfModuleManagers);

BioAPI_RETURN BioAPI mdsutil_mds_get_modulemanagerInfo (const BioAPI_UUID *ModuleUUID,
													   CSSM_SERVICE_MASK ServiceType,
													   MDSUTIL_MODULE_MANAGER_INFO_PTR *pInfo);

BioAPI_RETURN BioAPI mdsutil_mds_get_moduleInfo (const BioAPI_UUID *ModuleUUID,
												CSSM_SERVICE_MASK UsageMask,
												uint32 SubserviceID,
												CSSM_USEE_TAG USEERequest,
												MDSUTIL_MODULE_INFO_PTR *pModuleInfo);

BioAPI_RETURN BioAPI mdsutil_mds_install_module(const CSSM_DATA *guid,
											   CSSM_DATA_PTR Manifest,
											   CSSM_DATA_PTR ModuleName,
											   const CSSM_DATA *Path,
											   const MDSUTIL_MODULE_INFO *ModuleInfo);

BioAPI_RETURN BioAPI mdsutil_mds_install_emm(const CSSM_DATA* guid,
											CSSM_DATA_PTR Manifest,
											CSSM_DATA_PTR ModuleName,
											CSSM_DATA_PTR Path,
											const MDSUTIL_MODULE_MANAGER_INFO *ModuleManagerInfo);

BioAPI_RETURN
allocate_value(CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
			   uint32 numAttributes);

void
clean_attributes(CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
				 uint32 pos);

#ifdef WIN32
#define DIRECTORY_SEPARATOR '\\'
#define DIRECTORY_SEPARATOR_STRING "\\"
#else
#define DIRECTORY_SEPARATOR '/'
#define DIRECTORY_SEPARATOR_STRING "/"
#endif

#define UUID_FORMAT_STRING "{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x}"

#endif
