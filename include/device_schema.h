/*-----------------------------------------------------------------------
 * File: DEVICE_SCHEMA.H
 *-----------------------------------------------------------------------
 */

#ifndef _DEVICE_SCHEMA_H_
#define _DEVICE_SCHEMA_H_

#include "bioapi_schema.h"

#define BIOAPI_BSP_DEVICE	   "BioAPIBspDevice"


/*ModuleId*/
#define BIOAPI_SCHEMA_DEVICE_MODULEID { \
	1, /* AttributeNameFormat */ \
	"ModuleId", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* BioAPI DeviceId */
#define BIOAPI_SCHEMA_DEVICE_DEVICEID { \
	2, \
	"DeviceID", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}

/* BioAPI Device supported formats*/
#define BIOAPI_SCHEMA_DEVICE_SUPPORTEDFORMATS { \
	3, \
	"SupportedFormats", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}

/* SupportedEvents */
#define BIOAPI_SCHEMA_DEVICE_SUPPORTEDEVENTS { \
	4, \
	"SupportedEvents", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}

/* DeviceVendor */
#define BIOAPI_SCHEMA_DEVICE_VENDOR { \
	5, \
	"DeviceVendor", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* DeviceDescription */
#define BIOAPI_SCHEMA_DEVICE_DESCRIPTION { \
	6, \
	"DeviceDescription", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* DeviceSerialNumber */
#define BIOAPI_SCHEMA_DEVICE_SERIALNUMBER { \
	7, \
	"DeviceSerialNumber", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* DeviceHardwareVersion */
#define BIOAPI_SCHEMA_DEVICE_HARDWAREVERSION { \
	8, \
	"DeviceHWVersion", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* DeviceFirmwareVersion */
#define BIOAPI_SCHEMA_DEVICE_FIRMWAREVERSION { \
	9, \
	"DeviceFirmwareVersion", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* AuthenticatedDevice */
#define BIOAPI_SCHEMA_DEVICE_AUTHENTICATEDDEVICE { \
	10, \
	"AuthenticatedDevice", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}


/************************************************/

/* BIOAPI_BIO_DEVICE_RECORDTYPE */
#define MDSU_BIOAPI_DEV_MODULEID			   ( 0x00000001 )
#define MDSU_BIOAPI_DEV_DEVICEID			   ( 0x00000002 )
#define MDSU_BIOAPI_DEV_SUPPORTEDFORMATS	   ( 0x00000008 )
#define MDSU_BIOAPI_DEV_SUPPORTEDEVENTS		   ( 0x00000010 )
#define MDSU_BIOAPI_DEV_DEVICEVENDOR		   ( 0x00000020 )
#define MDSU_BIOAPI_DEV_DEVICEDESCRIPTION	   ( 0x00000040 )
#define MDSU_BIOAPI_DEV_DEVICESERALNUMBER	   ( 0x00000080 )
#define MDSU_BIOAPI_DEV_DEVICEHARDWAREVERSION  ( 0x00000100 )
#define MDSU_BIOAPI_DEV_DEVICEFIRMWAREVERSION  ( 0x00000200 )
#define MDSU_BIOAPI_DEV_AUTHENTICATEDDEVICE	   ( 0x00000400 )


BioAPI_RETURN BioAPI bioapi_UpdateDeviceInfo( BioAPI_DEVICE_SCHEMA* BioAPIDevice,
										  BioAPI_BOOL bInstall,
										  const BioAPI_UUID *uuid);

#endif
