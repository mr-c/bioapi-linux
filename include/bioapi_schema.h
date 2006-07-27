/*-----------------------------------------------------------------------
 * File: BIOAPI_SCHEMA.H
 *
 *-----------------------------------------------------------------------
 */

/*
 * Contains constants and #defines for schema information for BIOAPI
 */

#ifndef BIOAPI_SCHEMA_H
#define BIOAPI_SCHEMA_H

#define MDSU_MAX_VALUE_COUNT		( 16 )

#include "cssmtype.h"
#include "bioapi_type.h"
/* Industry At Large Application Name Space Range Definition */
/* These are re-defines from MDS for the App range in MDS	 */
#define BIOAPI_DB_RECORDTYPE_DEFINED_START (0x80000000)
#define BIOAPI_DB_RECORDTYPE_DEFINED_END   (0xffffffff)


/** defines for BioAPI ************************************************/
#define BIOAPI_MODULE_NUM_RELATIONS								 (3)

#define BIOAPI_H_LAYER_RECORDTYPE								 (BIOAPI_DB_RECORDTYPE_DEFINED_START)
#define BIOAPI_H_LAYER_NUM_ATTRIBUTES							 (6)
#define BIOAPI_H_LAYER_NUM_INDEXES								 (BIOAPI_H_LAYER_NUM_ATTRIBUTES)

#define BIOAPI_BSP_RECORDTYPE									 (BIOAPI_DB_RECORDTYPE_DEFINED_START + 1)
#define BIOAPI_BSP_NUM_ATTRIBUTES								 (20)
#define BIOAPI_BSP_NUM_INDEXES									 (BIOAPI_BSP_NUM_ATTRIBUTES)

#define BIOAPI_BIO_DEVICE_RECORDTYPE							 (BIOAPI_DB_RECORDTYPE_DEFINED_START + 2)
#define BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES						 (10)
#define BIOAPI_BIO_DEVICE_NUM_INDEXES							 (BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES)

#define BIOAPI_H_LAYER_CAPABILITIES		"BioAPICapabilities"

/* Linux-Port: General change, removed braces around scalar initializers 
 * (AttributeName). Some Linux gcc compilers choke (warn) on this, but the
 * {} should legal in both C and C++.
 */

/* Module's Id (UUID) is common between H Framework Schema, BSP Schema and Device Schema. */
#define BIOAPI_ATTRINFO_MODULEID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	 "ModuleID" , /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_ModuleId = BIOAPI_ATTRINFO_MODULEID;
#define MDS_ATTRINFO_UUID BIOAPI_ATTRINFO_MODULEID

/* Module's name (file name in Win32) */
#define BIOAPI_ATTRINFO_MODULENAME { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	 "ModuleName" , /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_ModuleName = BIOAPI_ATTRINFO_MODULENAME;


/* Product Version is common between the H-Framework Schema and BSP Schema */
#define BIOAPI_ATTRINFO_PRODUCTVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	 "Product Version" , /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_ProductVersion = BIOAPI_ATTRINFO_PRODUCTVERSION;


/* Spec Version is common between the H-Framework Schema and BSP Schema */
#define BIOAPI_ATTRINFO_SPECVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	 "Spec Version" , /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_SpecVersion = BIOAPI_ATTRINFO_SPECVERSION;

/* Vendor is common between the H-Framework Schema, BSP Schema and Device Schema */
#define BIOAPI_ATTRINFO_VENDOR { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	 "Vendor" , /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_Vendor = BIOAPI_ATTRINFO_VENDOR;

/* Description */
#define BIOAPI_ATTRINFO_DESCRIPTION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	 "Description" , /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_Description = BIOAPI_ATTRINFO_DESCRIPTION;

/**********************BSP Schema*******************************/
/* Module's Id (UUID) is common between H Framework Schema, BSP Schema and Device Schema.
  Commented definition here is for reference ONLY.

#define BIOAPI_ATTRINFO_MODULEID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, \
	{ "ModuleID" }, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_ModuleId = BIOAPI_ATTRINFO_MODULEID;
#define MDS_ATTRINFO_GUID BIOAPI_ATTRINFO_MODULEID
*/


/* Device Id is common between BSP Schema and Device Schema */
#define BIOAPI_ATTRINFO_DEVICEID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "DeviceID" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DeviceId = BIOAPI_ATTRINFO_DEVICEID;

/* BSPName	*/
#define BIOAPI_ATTRINFO_BSPNAME { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "BSPName" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_BspName = BIOAPI_ATTRINFO_BSPNAME;

/* Product Version is common between the H-Framework Schema and BSP Schema.	 Commented
   definition here is for reference ONLY.
#define BIOAPI_ATTRINFO_PRODUCTVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING,  \
	{ "Product Version" },	\
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_ProductVersion = BIOAPI_ATTRINFO_PRODUCTVERSION;

*/

/* Spec Version is common between the H-Framework Schema and BSP Schema Commented
   definition here is for reference ONLY.
#define BIOAPI_ATTRINFO_SPECVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING,  \
	{ "Spec Version" }, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_SpecVersion = BIOAPI_ATTRINFO_SPECVERSION;
*/
/* Supported Formats  */
#define BIOAPI_ATTRINFO_SUPPORTEDFORMATS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "BSP Supported Formats" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_BspSupportedFormats = BIOAPI_ATTRINFO_SUPPORTEDFORMATS;

/* Vendor  is common between the H-Framework Schema and BSP Schema Commented
   definition here is for reference ONLY.
#define BIOAPI_ATTRINFO_VENDOR { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING,  \
	{ "Vendor" },  \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_Vendor = BIOAPI_ATTRINFO_VENDOR;
*/

/* Factors Mask	 */
#define BIOAPI_ATTRINFO_FACTORSMASK { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Factors Mask" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_FactorsMask = BIOAPI_ATTRINFO_FACTORSMASK;


/* Operations  */
#define BIOAPI_ATTRINFO_OPERATIONS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Operations" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_Operations = BIOAPI_ATTRINFO_OPERATIONS;

/* Options	*/
#define BIOAPI_ATTRINFO_OPTIONS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Options" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_Options = BIOAPI_ATTRINFO_OPTIONS;


/* Payload Policy  */
#define BIOAPI_ATTRINFO_PAYLOADPOLICY { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Payload Policy" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_PayloadPolicy = BIOAPI_ATTRINFO_PAYLOADPOLICY;

/* Max Payload Size	 */
#define BIOAPI_ATTRINFO_MAXPAYLOADSIZE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Max Payload Size" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_MaxPayloadSize = BIOAPI_ATTRINFO_MAXPAYLOADSIZE;

/* Verify Timeout  */
#define BIOAPI_ATTRINFO_DEFAULTVERIFYTIMEOUT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Default Verify Timeout" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DefaultVerifyTimeout = BIOAPI_ATTRINFO_DEFAULTVERIFYTIMEOUT;

/* Identify Timeout	 */
#define BIOAPI_ATTRINFO_DEFAULTIDENTIFYTIMEOUT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Default Identify Timeout" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DefaultIdentifyTimeout = BIOAPI_ATTRINFO_DEFAULTIDENTIFYTIMEOUT;

/* Capture Timeout */
#define BIOAPI_ATTRINFO_DEFAULTCAPTURETIMEOUT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Default Capture Timeout" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DefaultCaptureTimeout = BIOAPI_ATTRINFO_DEFAULTCAPTURETIMEOUT;

/* Enroll Timeout */
#define BIOAPI_ATTRINFO_DEFAULTENROLLTIMEOUT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Default Enroll Timeout" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DefaultEnrollTimeout = BIOAPI_ATTRINFO_DEFAULTENROLLTIMEOUT;

/* MAX BSP DB size */
#define BIOAPI_ATTRINFO_MAXBSPSDBSIZE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Max BSP DB size" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_MaxBspDbSize = BIOAPI_ATTRINFO_MAXBSPSDBSIZE;

/* MAX Identify */
#define BIOAPI_ATTRINFO_MAXIDENTIFY { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Max Identify" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_MaxIdentify = BIOAPI_ATTRINFO_MAXIDENTIFY;

/* Description is common between H Framework Schema and BSP Schema.
  Commented definition here is for reference ONLY.
#define BIOAPI_ATTRINFO_DESCRIPTION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, \
	{ "Description" }, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_Description = BIOAPI_ATTRINFO_DESCRIPTION;
*/

/* Module path */
#define BIOAPI_ATTRINFO_PATH { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Path" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_Path = BIOAPI_ATTRINFO_PATH;

/***************Device Schema**********************/
/* Module's Id (UUID) is common between H Framework Schema, BSP Schema and Device Schema.
  Commented definition here is for reference ONLY.

#define BIOAPI_ATTRINFO_MODULEID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, \
	{ "ModuleID" }, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_ModuleId = BIOAPI_ATTRINFO_MODULEID;
#define MDS_ATTRINFO_GUID BIOAPI_ATTRINFO_MODULEID
*/

/* Device Id is common between BSP Schema and Device Schema. Commented
   definition here is for reference ONLY.
#define BIOAPI_ATTRINFO_DEVICEID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING,  \
	{ "DeviceID" },	 \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DeviceId = BIOAPI_ATTRINFO_DEVICEID;
*/

/* Supported Formats */
#define BIOAPI_ATTRINFO_DEVICESUPPORTEDFORMATS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Device Supported Formats" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DeviceSupportedFormats = BIOAPI_ATTRINFO_DEVICESUPPORTEDFORMATS;

/* Supported Events */
#define BIOAPI_ATTRINFO_SUPPORTEDEVENTS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Supported Events" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_SupportedEvents = BIOAPI_ATTRINFO_SUPPORTEDEVENTS;

/* Device Vendor */
#define BIOAPI_ATTRINFO_DEVICEVENDOR { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Device Vendor" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DeviceVendor = BIOAPI_ATTRINFO_DEVICEVENDOR;


/* Device Description */
#define BIOAPI_ATTRINFO_DEVICEDESCRIPTION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Device Description" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DeviceDescription = BIOAPI_ATTRINFO_DEVICEDESCRIPTION;

/* Device Serial Number */
#define BIOAPI_ATTRINFO_DEVICESERIALNUMBER { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Device Serial Number" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DeviceSerialNumber = BIOAPI_ATTRINFO_DEVICESERIALNUMBER;

/* Device Hardware Version */
#define BIOAPI_ATTRINFO_DEVICEHARDWAREVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Device Hardware Version" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DeviceHardwareVersion = BIOAPI_ATTRINFO_DEVICEHARDWAREVERSION;

/* Device Firmware Version */
#define BIOAPI_ATTRINFO_DEVICEFIRMWAREVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Device Firmware Version" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_DeviceFirmwareVersion = BIOAPI_ATTRINFO_DEVICEFIRMWAREVERSION;

/* Authenticated Device */
#define BIOAPI_ATTRINFO_AUTHENTICATEDDEVICE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Authenticated Device" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_BioApiAttrInfo_AuthenticatedDevice = BIOAPI_ATTRINFO_AUTHENTICATEDDEVICE;

/*****************INDEX_SCHEMA**********************/

#define BIOAPI_INDEX_SCHEMA_FRAMEWORK_MODULEID { \
	1, \
	0, \
	CSSM_DB_INDEX_UNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Module's name (file name in Win32) */
#define BIOAPI_INDEX_SCHEMA_FRAMEWORK_MODULENAME { \
	2, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Product Version */
#define BIOAPI_INDEX_SCHEMA_FRAMEWORK_PRODUCTVERSION { \
	3, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Spec Version */
#define BIOAPI_INDEX_SCHEMA_FRAMEWORK_SPECVERSION { \
	4, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Vendor */
#define BIOAPI_INDEX_SCHEMA_FRAMEWORK_VENDOR { \
	5, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Description */
#define BIOAPI_INDEX_SCHEMA_FRAMEWORK_DESCRIPTION { \
	6, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/******************************************BSP_INDEX_SCHEMA**************/
/* Module's Id (UUID) */
#define BIOAPI_INDEX_SCHEMA_BSP_MODULEID { \
	1, \
	0, \
	CSSM_DB_INDEX_UNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}


/* Device Id is common between BSP Schema and Device Schema */
#define BIOAPI_INDEX_SCHEMA_BSP_DEVICEID { \
	2, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* BSPName	*/
#define BIOAPI_INDEX_SCHEMA_BSP_BSPNAME { \
	3, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

#define BIOAPI_INDEX_SCHEMA_BSP_SPECVERSION { \
	4, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

#define BIOAPI_INDEX_SCHEMA_BSP_PRODUCTVERSION { \
	5, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Vendor  */
#define BIOAPI_INDEX_SCHEMA_BSP_VENDOR { \
	6, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Supported Formats  */
#define BIOAPI_INDEX_SCHEMA_BSP_SUPPORTEDFORMATS { \
	7, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Factors Mask	 */
#define BIOAPI_INDEX_SCHEMA_BSP_FACTORSMASK { \
	8, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}


/* Operations  */
#define BIOAPI_INDEX_SCHEMA_BSP_OPERATIONS { \
	9, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Options	*/
#define BIOAPI_INDEX_SCHEMA_BSP_OPTIONS { \
	10, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}


/* Payload Policy  */
#define BIOAPI_INDEX_SCHEMA_BSP_PAYLOADPOLICY { \
	11, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Max Payload Size	 */
#define BIOAPI_INDEX_SCHEMA_BSP_MAXPAYLOADSIZE { \
	12, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Verify Timeout  */
#define BIOAPI_INDEX_SCHEMA_BSP_DEFAULTVERIFYTIMEOUT { \
	13, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Identify Timeout	 */
#define BIOAPI_INDEX_SCHEMA_BSP_DEFAULTIDENTIFYTIMEOUT { \
	14, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Capture Timeout */
#define BIOAPI_INDEX_SCHEMA_BSP_DEFAULTCAPTURETIMEOUT { \
	15, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Enroll Timeout */
#define BIOAPI_INDEX_SCHEMA_BSP_DEFAULTENROLLTIMEOUT { \
	16, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* MAX BSP DB size */
#define BIOAPI_INDEX_SCHEMA_BSP_MAXBSPDBSIZE { \
	17, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* MAX Identify */
#define BIOAPI_INDEX_SCHEMA_BSP_MAXIDENTIFY { \
	18, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Description */
#define BIOAPI_INDEX_SCHEMA_BSP_DESCRIPTION { \
	19, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Path */
#define BIOAPI_INDEX_SCHEMA_BSP_PATH { \
	20, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/********************Biometric Device Schema Index*********/
/* Supported Events */
#define BIOAPI_INDEX_SCHEMA_DEVICE_MODULEID { \
	1,	\
	0,	\
	CSSM_DB_INDEX_UNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

#define BIOAPI_INDEX_SCHEMA_DEVICE_DEVICEID { \
	2, \
	0, \
	CSSM_DB_INDEX_UNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

#define BIOAPI_INDEX_SCHEMA_DEVICE_SUPPORTEDFORMATS { \
	3, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

#define BIOAPI_INDEX_SCHEMA_DEVICE_SUPPORTEDEVENTS { \
	4, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Device Vendor */
#define BIOAPI_INDEX_SCHEMA_DEVICE_VENDOR { \
	5, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Device Description */
#define BIOAPI_INDEX_SCHEMA_DEVICE_DESCRIPTION { \
	6, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Device Serial Number */
#define BIOAPI_INDEX_SCHEMA_DEVICE_SERIALNUMBER { \
	7, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Device Hardware Version */
#define BIOAPI_INDEX_SCHEMA_DEVICE_HARDWAREVERSION { \
	8, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Device Firmware Version */
#define BIOAPI_INDEX_SCHEMA_DEVICE_FIRMWAREVERSION { \
	9, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

/* Authenticated Device */
#define BIOAPI_INDEX_SCHEMA_DEVICE_AUTHENTICATEDDEVICE { \
	10, \
	0, \
	CSSM_DB_INDEX_NONUNIQUE, \
	CSSM_DB_INDEX_ON_ATTRIBUTE \
}

#define MDSU_BIOAPI_CAP_MODULE_ID		( 0x00000001 )
#define MDSU_BIOAPI_CAP_MODULE_NAME		( 0x00000002 )
#define MDSU_BIOAPI_CAP_SPEC_VERSION	( 0x00000004 )
#define MDSU_BIOAPI_CAP_PROD_VERSION	( 0x00000008 )
#define MDSU_BIOAPI_CAP_VENDOR			( 0x00000010 )
#define MDSU_BIOAPI_CAP_DESCRIPTION		( 0x00000020 )

/* Linux-Port: _MAX_PATH not defined in this file but used in this file.
 * It is defined in "port/bioapi_port.h";
 */
#if defined (LINUX) || defined (UNIX) || defined (SOLARIS)
#include <limits.h>
#define _MAX_PATH (PATH_MAX + 1)
#define  MAX_PATH _MAX_PATH
/*#include <sys/param.h>
#include <port/bioapi_port.h>*/
#endif

typedef struct _bioapi_h_level_framework_schema
{
	BioAPI_UUID ModuleId;
	CSSM_STRING ModuleName;
	BioAPI_VERSION SpecVersion;
	BioAPI_VERSION ProdVersion;
	CSSM_STRING Vendor;
	CSSM_STRING Description;
}
BioAPI_H_LEVEL_FRAMEWORK_SCHEMA, *BioAPI_H_LEVEL_FRAMEWORK_SCHEMA_PTR;

typedef struct _bioapi_bsp_schema
{
	BioAPI_UUID ModuleId;
	BioAPI_DEVICE_ID DeviceId;
	CSSM_STRING BSPName;
	BioAPI_VERSION SpecVersion;
	BioAPI_VERSION ProductVersion;
	CSSM_STRING Vendor;
	BioAPI_BIR_BIOMETRIC_DATA_FORMAT BspSupportedFormats[MDSU_MAX_VALUE_COUNT];
	uint32 NumSupportedFormats;
	uint32 FactorsMask;
	uint32 Operations;
	uint32 Options;
	uint32 PayloadPolicy;
	uint32 MaxPayloadSize;
	sint32 DefaultVerifyTimeout;
	sint32 DefaultIdentifyTimeout;
	sint32 DefaultCaptureTimeout;
	sint32 DefaultEnrollTimeout;
	uint32 MaxBspDbSize;
	uint32 MaxIdentify;
	CSSM_STRING Description;
	char Path[_MAX_PATH];
}
BioAPI_BSP_SCHEMA, *BioAPI_BSP_SCHEMA_PTR;

typedef BioAPI_BSP_SCHEMA_PTR BioAPI_BSP_SCHEMA_ARRAY, *BioAPI_BSP_SCHEMA_ARRAY_PTR;

typedef struct _bioapi_device_schema
{
	BioAPI_UUID ModuleId;
	BioAPI_DEVICE_ID DeviceId;
	BioAPI_BIR_BIOMETRIC_DATA_FORMAT DeviceSupportedFormats[MDSU_MAX_VALUE_COUNT];
	uint32 NumSupportedFormats;
	uint32 SupportedEvents;
	CSSM_STRING DeviceVendor;
	CSSM_STRING DeviceDescription;
	CSSM_STRING DeviceSerialNumber;
	BioAPI_VERSION DeviceHardwareVersion;
	BioAPI_VERSION DeviceFirmwareVersion;
	BioAPI_BOOL AuthenticatedDevice;
}
BioAPI_DEVICE_SCHEMA, *BioAPI_DEVICE_SCHEMA_PTR;

typedef BioAPI_DEVICE_SCHEMA_PTR BioAPI_DEVICE_SCHEMA_ARRAY, *BioAPI_DEVICE_SCHEMA_ARRAY_PTR;


#ifdef __cplusplus
extern "C" {
#endif

BioAPI_RETURN BioAPI BioAPI_EnumModules(BioAPI_BSP_SCHEMA * BspSchemaArray,
									uint32 ArraySize,
									uint32 * ElementsNeeded,
									uint32 * NumElementsReturned);

BioAPI_RETURN BioAPI BioAPI_EnumDevices(const BioAPI_UUID *uuid,
									BioAPI_DEVICE_SCHEMA * DeviceSchemaArray,
									uint32 ArraySize,
									uint32 * ElementsNeeded,
									uint32 * NumElementsReturned);

#ifdef __cplusplus
}
#endif


#endif /**BIOAPI_SCHEMA_H**/
