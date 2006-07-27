/*-----------------------------------------------------------------------
 * File: BSP_SCHEMA.H
 *-----------------------------------------------------------------------
 */

#ifndef _BSP_SCHEMA_H_
#define _BSP_SCHEMA_H_

#include "mds.h"
#include "bioapi_schema.h"

#define BIOAPI_BSP_CAPABILITIES "BioAPIBspCapabilities"


/*ModuleId*/
#define BIOAPI_SCHEMA_BSP_MODULEID { \
	1, /* AttributeNameFormat */ \
	"ModuleId", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* BioAPI DeviceId */
#define BIOAPI_SCHEMA_BSP_DEVICEID { \
	2, \
	"DeviceID", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}


/* BioAPIBsp name */
#define BIOAPI_SCHEMA_BSP_BSPNAME { \
	3, \
	"BspName", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* BioAPI Bsp spec version */
#define BIOAPI_SCHEMA_BSP_SPECVERSION { \
	4, /* AttributeNameFormat */ \
	"SpecVersion", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* BioAPI Product version */
#define BIOAPI_SCHEMA_BSP_PRODUCTVERSION { \
	5, \
	"ProdVersion", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}


/* BioAPI Bsp vendor */
#define BIOAPI_SCHEMA_BSP_VENDOR { \
	6, \
	"Vendor", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* BioAPI Bsp supported formats*/
#define BIOAPI_SCHEMA_BSP_SUPPORTEDFORMATS { \
	7, \
	"SupportedFormats", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}

/* BioAPI Auth Factors Mask */
#define BIOAPI_SCHEMA_BSP_FACTORSMASK { \
	8, \
	"AuthFactors", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}

/* Operations */
#define BIOAPI_SCHEMA_BSP_OPERATIONS { \
	9, \
	"Operations", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}

/* Options */
#define BIOAPI_SCHEMA_BSP_OPTIONS { \
	10, \
	"Options", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}

/* PayloadPolicy */
#define BIOAPI_SCHEMA_BSP_PAYLOADPOLICY { \
	11, \
	"PayloadPolicy", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}

/* MaxPayloadSize */
#define BIOAPI_SCHEMA_BSP_MAXPAYLOADSIZE { \
	12, \
	"MaxPayloadSize", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}

/* DefaultVerifyTimeout */
#define BIOAPI_SCHEMA_BSP_DEFAULTVERIFYTIMEOUT { \
	13, \
	"DefaultVerifyTimeout", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}

/* DefaultIdentifyTimeout */
#define BIOAPI_SCHEMA_BSP_DEFAULTIDENTIFYTIMEOUT { \
	14, \
	"DefaultIdentifyTimeout", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}

/* DefaultCaptureTimeout */
#define BIOAPI_SCHEMA_BSP_DEFAULTCAPTURETIMEOUT { \
	15, \
	"DefaultCaptureTimeout", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}

/* DefaultEnrollTimeout */
#define BIOAPI_SCHEMA_BSP_DEFAULTENROLLTIMEOUT { \
	16, \
	"DefaultEnrollTimeout", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}

/* MaxBspDbSize */
#define BIOAPI_SCHEMA_BSP_MAXBSPDBSIZE { \
	17, \
	"MaxBspDbSize", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}

/* MaxIdentify */
#define BIOAPI_SCHEMA_BSP_MAXIDENTIFY { \
	18, \
	"MaxIdentify", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_SINT32 \
}

/* Description */
#define BIOAPI_SCHEMA_BSP_DESCRIPTION { \
	19, \
	"Description", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}

/* Path */
#define BIOAPI_SCHEMA_BSP_PATH { \
	20, \
	"Path", /* AttributeName */ \
	{0,NULL}, \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}


/************************************************/

/* BIOAPI_BIO_CAPABILITY_RECORDTYPE */
#define MDSU_BIOAPI_BSP_CAP_MODULEID				( 0x00000001 )
#define MDSU_BIOAPI_BSP_CAP_DEVICEID				( 0x00000002 )
#define MDSU_BIOAPI_BSP_CAP_BSPNAME					( 0x00000004 )
#define MDSU_BIOAPI_BSP_CAP_SPECVERSION				( 0x00000008 )
#define MDSU_BIOAPI_BSP_CAP_PRODUCTVERSION			( 0x00000010 )
#define MDSU_BIOAPI_BSP_CAP_VENDOR					( 0x00000020 )
#define MDSU_BIOAPI_BSP_CAP_SUPPORTEDFORMATS		( 0x00000040 )
#define MDSU_BIOAPI_BSP_CAP_FACTORSMASK				( 0x00000080 )
#define MDSU_BIOAPI_BSP_CAP_OPERATIONS				( 0x00000100 )
#define MDSU_BIOAPI_BSP_CAP_OPTIONS					( 0x00000200 )
#define MDSU_BIOAPI_BSP_CAP_PAYLOADPOLICY			( 0x00000400 )
#define MDSU_BIOAPI_BSP_CAP_MAXPAYLOADSIZE			( 0x00000800 )
#define MDSU_BIOAPI_BSP_CAP_DEFAULTVERIFYTIMEOUT	( 0x00001000 )
#define MDSU_BIOAPI_BSP_CAP_DEFAULTIDENTIFYTIMEOUT	( 0x00002000 )
#define MDSU_BIOAPI_BSP_CAP_DEFAULTCAPTURETIMEOUT	( 0x00004000 )
#define MDSU_BIOAPI_BSP_CAP_DEFAULTENROLLTIMEOUT	( 0x00008000 )
#define MDSU_BIOAPI_BSP_CAP_MAXBSPDBSIZE			( 0x00010000 )
#define MDSU_BIOAPI_BSP_CAP_MAXIDENTIFY				( 0x00020000 )
#define MDSU_BIOAPI_BSP_CAP_DESCRIPTION				( 0x00040000 )
#define MDSU_BIOAPI_BSP_CAP_PATH					( 0x00080000 )


#endif
