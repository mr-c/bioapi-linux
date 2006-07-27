/*-----------------------------------------------------------------------
 * File: MDS_SCHEMA.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */
 
/*
 * Contains constants and #defines for schema information for MDS
 */

#ifndef MDS_SCHEMA_H
#define MDS_SCHEMA_H

#include <cssmtype.h>

/** defines for object directory ***********************************************************************/
#define MDS_OBJECT_NUM_RELATIONS                              (1)

#define MDS_OBJECT_RECORDTYPE                                 (CSSM_DB_RELATIONID_MDS_START)
#define MDS_OBJECT_NUM_ATTRIBUTES                             (5)
#define MDS_OBJECT_NUM_INDEXES                                (MDS_OBJECT_NUM_ATTRIBUTES)

#define MDS_CDSA_SCHEMA_START							  	  (MDS_OBJECT_RECORDTYPE)

/** defines for cdsa directory *************************************************************************/
#define MDS_CDSADIR_NUM_RELATIONS                             (19)

#define MDS_CDSADIR_CSSM_RECORDTYPE                           (MDS_CDSA_SCHEMA_START + 1)
#define MDS_CDSADIR_CSSM_NUM_ATTRIBUTES                       (5)
#define MDS_CDSADIR_CSSM_NUM_INDEXES                          (MDS_CDSADIR_CSSM_NUM_ATTRIBUTES)

#define MDS_CDSADIR_EMM_RECORDTYPE                            (MDS_CDSA_SCHEMA_START + 2)
#define MDS_CDSADIR_EMM_NUM_ATTRIBUTES                        (11)
#define MDS_CDSADIR_EMM_NUM_INDEXES                           (MDS_CDSADIR_EMM_NUM_ATTRIBUTES)

#define MDS_CDSADIR_COMMON_RECORDTYPE                         (MDS_CDSA_SCHEMA_START + 3)
#define MDS_CDSADIR_COMMON_NUM_ATTRIBUTES                     (9)
#define MDS_CDSADIR_COMMON_NUM_INDEXES                        (MDS_CDSADIR_COMMON_NUM_ATTRIBUTES)

#define MDS_CDSADIR_CSP_PRIMARY_RECORDTYPE                    (MDS_CDSA_SCHEMA_START + 4)
#define MDS_CDSADIR_CSP_PRIMARY_NUM_ATTRIBUTES                (13)
#define MDS_CDSADIR_CSP_PRIMARY_NUM_INDEXES                   (MDS_CDSADIR_CSP_PRIMARY_NUM_ATTRIBUTES)

#define MDS_CDSADIR_CSP_CAPABILITY_RECORDTYPE                 (MDS_CDSA_SCHEMA_START + 5)
#define MDS_CDSADIR_CSP_CAPABILITY_NUM_ATTRIBUTES             (9)
#define MDS_CDSADIR_CSP_CAPABILITY_NUM_INDEXES                (MDS_CDSADIR_CSP_CAPABILITY_NUM_ATTRIBUTES)

#define MDS_CDSADIR_CSP_ENCAPSULATED_PRODUCT_RECORDTYPE       (MDS_CDSA_SCHEMA_START + 6)
#define MDS_CDSADIR_CSP_ENCAPSULATED_PRODUCT_NUM_ATTRIBUTES   (16)
#define MDS_CDSADIR_CSP_ENCAPSULATED_PRODUCT_NUM_INDEXES      (MDS_CDSADIR_CSP_ENCAPSULATED_PRODUCT_NUM_ATTRIBUTES)

#define MDS_CDSADIR_CSP_SC_INFO_RECORDTYPE		              (MDS_CDSA_SCHEMA_START + 7)
#define MDS_CDSADIR_CSP_SC_INFO_NUM_ATTRIBUTES				  (9)
#define MDS_CDSADIR_CSP_SC_INFO_NUM_INDEXES					  (MDS_CDSADIR_CSP_SC_INFO_NUM_ATTRIBUTES)

#define MDS_CDSADIR_DL_PRIMARY_RECORDTYPE                     (MDS_CDSA_SCHEMA_START + 8)
#define MDS_CDSADIR_DL_PRIMARY_NUM_ATTRIBUTES                 (13)
#define MDS_CDSADIR_DL_PRIMARY_NUM_INDEXES                    (MDS_CDSADIR_DL_PRIMARY_NUM_ATTRIBUTES)

#define MDS_CDSADIR_DL_ENCAPSULATED_PRODUCT_RECORDTYPE        (MDS_CDSA_SCHEMA_START + 9)
#define MDS_CDSADIR_DL_ENCAPSULATED_PRODUCT_NUM_ATTRIBUTES    (10)
#define MDS_CDSADIR_DL_ENCAPSULATED_PRODUCT_NUM_INDEXES       (MDS_CDSADIR_DL_ENCAPSULATED_PRODUCT_NUM_ATTRIBUTES)

#define MDS_CDSADIR_CL_PRIMARY_RECORDTYPE                     (MDS_CDSA_SCHEMA_START + 10)
#define MDS_CDSADIR_CL_PRIMARY_NUM_ATTRIBUTES                 (13)
#define MDS_CDSADIR_CL_PRIMARY_NUM_INDEXES                    (MDS_CDSADIR_CL_PRIMARY_NUM_ATTRIBUTES)

#define MDS_CDSADIR_CL_ENCAPSULATED_PRODUCT_RECORDTYPE        (MDS_CDSA_SCHEMA_START + 11)
#define MDS_CDSADIR_CL_ENCAPSULATED_PRODUCT_NUM_ATTRIBUTES    (8)
#define MDS_CDSADIR_CL_ENCAPSULATED_PRODUCT_NUM_INDEXES       (MDS_CDSADIR_CL_ENCAPSULATED_PRODUCT_NUM_ATTRIBUTES)

#define MDS_CDSADIR_TP_PRIMARY_RECORDTYPE                     (MDS_CDSA_SCHEMA_START + 12)
#define MDS_CDSADIR_TP_PRIMARY_NUM_ATTRIBUTES                 (10)
#define MDS_CDSADIR_TP_PRIMARY_NUM_INDEXES                    (MDS_CDSADIR_TP_PRIMARY_NUM_ATTRIBUTES)

#define MDS_CDSADIR_TP_OIDS_RECORDTYPE                        (MDS_CDSA_SCHEMA_START + 13)
#define MDS_CDSADIR_TP_OIDS_NUM_ATTRIBUTES                    (4)
#define MDS_CDSADIR_TP_OIDS_NUM_INDEXES                       (MDS_CDSADIR_TP_OIDS_NUM_ATTRIBUTES)

#define MDS_CDSADIR_TP_ENCAPSULATED_PRODUCT_RECORDTYPE        (MDS_CDSA_SCHEMA_START + 14)
#define MDS_CDSADIR_TP_ENCAPSULATED_PRODUCT_NUM_ATTRIBUTES    (14)
#define MDS_CDSADIR_TP_ENCAPSULATED_PRODUCT_NUM_INDEXES       (MDS_CDSADIR_TP_ENCAPSULATED_PRODUCT_NUM_ATTRIBUTES)

#define MDS_CDSADIR_EMM_PRIMARY_RECORDTYPE					  (MDS_CDSA_SCHEMA_START + 15)
#define MDS_CDSADIR_EMM_PRIMARY_NUM_ATTRIBUTES                (11)
#define MDS_CDSADIR_EMM_PRIMARY_NUM_INDEXES                   (MDS_CDSADIR_EMM_PRIMARY_NUM_ATTRIBUTES)

#define MDS_CDSADIR_AC_PRIMARY_RECORDTYPE					  (MDS_CDSA_SCHEMA_START + 16)
#define MDS_CDSADIR_AC_PRIMARY_NUM_ATTRIBUTES                 (6)
#define MDS_CDSADIR_AC_PRIMARY_NUM_INDEXES                    (MDS_CDSADIR_AC_PRIMARY_NUM_ATTRIBUTES)

#define MDS_CDSADIR_SCHEMA_RELATIONS_RECORDTYPE				  (MDS_CDSA_SCHEMA_START + 17)
#define MDS_CDSADIR_SCHEMA_RELATIONS_NUM_ATTRIBUTES           (2)
#define MDS_CDSADIR_SCHEMA_RELATIONS_NUM_INDEXES              (MDS_CDSADIR_SCHEMA_RELATIONS_NUM_ATTRIBUTES)

#define MDS_CDSADIR_SCHEMA_ATTRIBUTES_RECORDTYPE			  (MDS_CDSA_SCHEMA_START + 18)
#define MDS_CDSADIR_SCHEMA_ATTRIBUTES_NUM_ATTRIBUTES          (6)
#define MDS_CDSADIR_SCHEMA_ATTRIBUTES_NUM_INDEXES             (MDS_CDSADIR_SCHEMA_ATTRIBUTES_NUM_ATTRIBUTES)

#define MDS_CDSADIR_SCHEMA_INDEXES_RECORDTYPE				  (MDS_CDSA_SCHEMA_START + 19)
#define MDS_CDSADIR_SCHEMA_INDEXES_NUM_ATTRIBUTES			  (5)	
#define MDS_CDSADIR_SCHEMA_INDEXES_NUM_INDEXES				  (MDS_CDSADIR_SCHEMA_INDEXES_NUM_ATTRIBUTES)


/** Declaration of common fields ***********************************************************************/

/* Module's Id (GUID) */
#define MDS_ATTRINFO_MODULEID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ModuleID" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ModuleId = MDS_ATTRINFO_MODULEID;
#define MDS_ATTRINFO_GUID MDS_ATTRINFO_MODULEID

/* Module's manifest */
#define MDS_ATTRINFO_MANIFEST { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Manifest" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_Manifest = MDS_ATTRINFO_MANIFEST;

/* Module's name (file name in Win32) */
#define MDS_ATTRINFO_MODULENAME { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ModuleName" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ModuleName = MDS_ATTRINFO_MODULENAME;

/* Module's search path (does not include file name) */
#define MDS_ATTRINFO_PATH { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Path" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_Path = MDS_ATTRINFO_PATH;

/* ProductVersion */
#define MDS_ATTRINFO_PRODUCTVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ProductVersion" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ProductVersion = MDS_ATTRINFO_PRODUCTVERSION;


/* CDSAVersion */
#define MDS_ATTRINFO_CDSAVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CDSAVersion" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_CDSAVersion = MDS_ATTRINFO_CDSAVERSION;

/* Vendor */
#define MDS_ATTRINFO_VENDOR { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Vendor" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_Vendor = MDS_ATTRINFO_VENDOR;

/* Desc */
#define MDS_ATTRINFO_DESC { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Desc" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_Desc = MDS_ATTRINFO_DESC;

/* NativeServices */
#define MDS_ATTRINFO_NATIVESERVICES { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "NativeServices" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_NativeServices = MDS_ATTRINFO_NATIVESERVICES;

/* EMMSpecVersion */
#define MDS_ATTRINFO_EMMSPECVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "EMMSpecVersion" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_EMMSpecVersion = MDS_ATTRINFO_EMMSPECVERSION;

/* PolicyStmt */
#define MDS_ATTRINFO_POLICYSTMT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "PolicyStmt" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_PolicyStmt = MDS_ATTRINFO_POLICYSTMT;

/* EmmVersion */
#define MDS_ATTRINFO_EMMVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "EmmVersion" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_EmmVersion = MDS_ATTRINFO_EMMVERSION;

/* EmmVendor */
#define MDS_ATTRINFO_EMMVENDOR { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "EmmVendor" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_EmmVendor = MDS_ATTRINFO_EMMVENDOR;

/* EmmType */
#define MDS_ATTRINFO_EMMTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "EmmType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_EmmType = MDS_ATTRINFO_EMMTYPE;

/* SSID */
#define MDS_ATTRINFO_SSID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SSID" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_SSID = MDS_ATTRINFO_SSID;

/* ServiceType */
#define MDS_ATTRINFO_SERVICETYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ServiceType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ServiceType = MDS_ATTRINFO_SERVICETYPE;

/* Version */
#define MDS_ATTRINFO_VERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Version" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_Version = MDS_ATTRINFO_VERSION;

/* SampleTypes */
#define MDS_ATTRINFO_SAMPLETYPES { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SampleTypes" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_SampleTypes = MDS_ATTRINFO_SAMPLETYPES;

/* AclSubjectTypes */
#define MDS_ATTRINFO_ACLSUBJECTTYPES { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AclSubjectTypes" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AclSubjectTypes = MDS_ATTRINFO_ACLSUBJECTTYPES;

/* AuthTags */
#define MDS_ATTRINFO_AUTHTAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AuthTags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AuthTags = MDS_ATTRINFO_AUTHTAGS;


/* DynamicFlag */
#define MDS_ATTRINFO_DYNAMICFLAG { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "DynamicFlag" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_DynamicFlag = MDS_ATTRINFO_DYNAMICFLAG;

/* MultiThreadFlag */
#define MDS_ATTRINFO_MULTITHREADFLAG { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "MultiThreadFlag" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_MultiThreadFlag = MDS_ATTRINFO_MULTITHREADFLAG;

/* ServiceMaskFlag */
#define MDS_ATTRINFO_SERVICEMASK { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ServiceMaskFlag" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ServiceMaskFlag = MDS_ATTRINFO_SERVICEMASK;

/* CspType */
#define MDS_ATTRINFO_CSPTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CspType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_CspType = MDS_ATTRINFO_CSPTYPE;

/* CspFlags */
#define MDS_ATTRINFO_CSPFLAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CspFlags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_CspFlags = MDS_ATTRINFO_CSPFLAGS;

/* CspCustomFlags */
#define MDS_ATTRINFO_CSPCUSTOMFLAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CspCustomFlags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_CspCustomFlags = MDS_ATTRINFO_CSPCUSTOMFLAGS;

/* UseeTags */
#define MDS_ATTRINFO_USEETAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "UseeTags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_UseeTags = MDS_ATTRINFO_USEETAGS;

/* UseeTag */
#define MDS_ATTRINFO_USEETAG { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "UseeTag" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_UseeTag = MDS_ATTRINFO_USEETAG;

/* ContextType */
#define MDS_ATTRINFO_CONTEXTTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ContextType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ContextType = MDS_ATTRINFO_CONTEXTTYPE;

/* AlgType */
#define MDS_ATTRINFO_ALGTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AlgType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AlgType = MDS_ATTRINFO_ALGTYPE;

/* GroupId */
#define MDS_ATTRINFO_GROUPID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "GroupId" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_GroupId = MDS_ATTRINFO_GROUPID;

/* AttributeType */
#define MDS_ATTRINFO_ATTRIBUTETYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AttributeType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AttributeType = MDS_ATTRINFO_ATTRIBUTETYPE;

/* AttributeValue */
#define MDS_ATTRINFO_ATTRIBUTEVALUE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AttributeValue" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AttributeValue = MDS_ATTRINFO_ATTRIBUTEVALUE;

/* Description */
#define MDS_ATTRINFO_DESCRIPTION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Description" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_Description = MDS_ATTRINFO_DESCRIPTION;


/* ProductVendor */
#define MDS_ATTRINFO_PRODUCTVENDOR { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ProductVendor" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ProductVendor = MDS_ATTRINFO_PRODUCTVENDOR;

/* ProductDesc */
#define MDS_ATTRINFO_PRODUCTDESC { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ProductDesc" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ProductDesc = MDS_ATTRINFO_PRODUCTDESC;

/* ProductFlags */
#define MDS_ATTRINFO_PRODUCTFLAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ProductFlags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ProductFlags = MDS_ATTRINFO_PRODUCTFLAGS;

/* CustomFlags */
#define MDS_ATTRINFO_CUSTOMFLAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CustomFlags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_CustomFlags = MDS_ATTRINFO_CUSTOMFLAGS;

/* StandardDesc */
#define MDS_ATTRINFO_STANDARDDESC { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "StandardDesc" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_StandardDesc = MDS_ATTRINFO_STANDARDDESC;

/* StandardVersion */
#define MDS_ATTRINFO_STANDARDVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "StandardVersion" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_StandardVersion = MDS_ATTRINFO_STANDARDVERSION;

/* ReaderDesc */
#define MDS_ATTRINFO_READERDESC { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ReaderDesc" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ReaderDesc = MDS_ATTRINFO_READERDESC;

/* ReaderVendor */
#define MDS_ATTRINFO_READERVENDOR { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ReaderVendor" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ReaderVendor = MDS_ATTRINFO_READERVENDOR;

/* ReaderVersion */
#define MDS_ATTRINFO_READERVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ReaderVersion" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ReaderVersion = MDS_ATTRINFO_READERVERSION;

/* ReaderFirmwareVersion */
#define MDS_ATTRINFO_READERFIRMWAREVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ReaderFirmwareVersion" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ReaderFirmwareVersion = MDS_ATTRINFO_READERFIRMWAREVERSION;

/* ReaderFlags */
#define MDS_ATTRINFO_READERFLAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ReaderFlags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ReaderFlags = MDS_ATTRINFO_READERFLAGS;

/* ReaderCustomFlags */
#define MDS_ATTRINFO_READERCUSTOMFLAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ReaderCustomFlags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ReaderCustomFlags = MDS_ATTRINFO_READERCUSTOMFLAGS;

/* ReaderSerialNumber */
#define MDS_ATTRINFO_READERSERIALNUMBER { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ReaderSerialNumber" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ReaderSerialNumber = MDS_ATTRINFO_READERSERIALNUMBER;

/* SCDesc */
#define MDS_ATTRINFO_SCDESC { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SCDesc" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_SCDesc = MDS_ATTRINFO_SCDESC;

/* SCVendor */
#define MDS_ATTRINFO_SCVENDOR { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SCVendor" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_SCVendor = MDS_ATTRINFO_SCVENDOR;

/* SCVersion */
#define MDS_ATTRINFO_SCVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SCVersion" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_SCVersion = MDS_ATTRINFO_SCVERSION;

/* SCFirmwareVersion */
#define MDS_ATTRINFO_SCFIRMWAREVERSION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SCFirmwareVersion" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_SCFirmwareVersion = MDS_ATTRINFO_SCFIRMWAREVERSION;

/* SCFlags */
#define MDS_ATTRINFO_SCFLAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SCFlags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_SCFlags = MDS_ATTRINFO_SCFLAGS;

/* SCCustomFlags */
#define MDS_ATTRINFO_SCCUSTOMFLAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SCCustomFlags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_SCCustomFlags = MDS_ATTRINFO_SCCUSTOMFLAGS;

/* SCSerialNumber */
#define MDS_ATTRINFO_SCSERIALNUMBER { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SCSerialNumber" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_SCSerialNumber = MDS_ATTRINFO_SCSERIALNUMBER;

/* DlType */
#define MDS_ATTRINFO_DLTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "DlType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_DlType = MDS_ATTRINFO_DLTYPE;

 /* QueryLimitsFlag */
#define MDS_ATTRINFO_QUERYLIMITSFLAG { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "QueryLimitsFlag" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_QueryLimitsFlag = MDS_ATTRINFO_QUERYLIMITSFLAG;

/* ConjunctiveOps */
#define MDS_ATTRINFO_CONJUNCTIVEOPS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ConjunctiveOps" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ConjunctiveOps = MDS_ATTRINFO_CONJUNCTIVEOPS;

/* RelationalOps */
#define MDS_ATTRINFO_RELATIONALOPS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "RelationalOps" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_RelationalOps = MDS_ATTRINFO_RELATIONALOPS;

/* Protocol */
#define MDS_ATTRINFO_PROTOCOL { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Protocol" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_Protocol = MDS_ATTRINFO_PROTOCOL;

/* RetrievalMode */
#define MDS_ATTRINFO_RETRIEVALMODE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "RetrievalMode" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_RetrievalMode = MDS_ATTRINFO_RETRIEVALMODE;

/* CertTypeFormat */
#define MDS_ATTRINFO_CERTTYPEFORMAT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CertTypeFormat" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_CertTypeFormat = MDS_ATTRINFO_CERTTYPEFORMAT;

/* CrlTypeFormat */
#define MDS_ATTRINFO_CRLTYPEFORMAT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CrlTypeFormat" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_CrlTypeFormat = MDS_ATTRINFO_CRLTYPEFORMAT;

/* CertFieldNames */
#define MDS_ATTRINFO_CERTFIELDNAMES { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CertFieldNames" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_CertFieldNames = MDS_ATTRINFO_CERTFIELDNAMES;

/* BundleTypeFormat */
#define MDS_ATTRINFO_BUNDLETYPEFORMAT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "BundleTypeFormat" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_BundleTypeFormat = MDS_ATTRINFO_BUNDLETYPEFORMAT;

/* XlationTypeFormat */
#define MDS_ATTRINFO_XLATIONTYPEFORMAT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "XlationTypeFormat" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_XlationTypeFormat = MDS_ATTRINFO_XLATIONTYPEFORMAT;

/* DefaultTemplateType */
#define MDS_ATTRINFO_DEFAULTTEMPLATETYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "DefaultTemplateType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_DefaultTemplateType = MDS_ATTRINFO_DEFAULTTEMPLATETYPE;

/* TemplateFieldNames */
#define MDS_ATTRINFO_TEMPLATEFIELDNAMES { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "TemplateFieldNames" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_TemplateFieldNames = MDS_ATTRINFO_TEMPLATEFIELDNAMES;

/* OID */
#define MDS_ATTRINFO_OID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "OID" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_OID = MDS_ATTRINFO_OID;

/* Value */
#define MDS_ATTRINFO_VALUE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Value" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_Value = MDS_ATTRINFO_VALUE;

/* AuthorityRequestType */
#define MDS_ATTRINFO_AUTHORITYREQUESTTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AuthorityRequestType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AuthorityRequestType = MDS_ATTRINFO_AUTHORITYREQUESTTYPE;

/* ProtocolDesc */
#define MDS_ATTRINFO_PROTOCOLDESC { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ProtocolDesc" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ProtocolDesc = MDS_ATTRINFO_PROTOCOLDESC;

/* ProtocolFlags */
#define MDS_ATTRINFO_PROTOCOLFLAGS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ProtocolFlags" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_ProtocolFlags = MDS_ATTRINFO_PROTOCOLFLAGS;

/* CertClassName */
#define MDS_ATTRINFO_CERTCLASSNAME { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CertClassName" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_CertClassName = MDS_ATTRINFO_CERTCLASSNAME;

/* RootCertificate */
#define MDS_ATTRINFO_ROOTCERTIFICATE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "RootCertificate" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_RootCertificate = MDS_ATTRINFO_ROOTCERTIFICATE;

/* RootCertTypeFormat */
#define MDS_ATTRINFO_ROOTCERTTYPEFORMAT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "RootCertTypeFormat" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_RootCertTypeFormat = MDS_ATTRINFO_ROOTCERTTYPEFORMAT;


/* RelationID */
#define MDS_ATTRINFO_RELATIONID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "RelationID" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_RelationID = MDS_ATTRINFO_RELATIONID;

/* RelationName */
#define MDS_ATTRINFO_RELATIONNAME { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "RelationName" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_RelationName = MDS_ATTRINFO_RELATIONNAME;

/* AttributeID */
#define MDS_ATTRINFO_ATTRIBUTEID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AttributeID" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AttributeID = MDS_ATTRINFO_ATTRIBUTEID;

/* AttributeID */
#define MDS_ATTRINFO_ATTRIBUTENAMEFORMAT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AttributeNameFormat" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AttributeNameFormat = MDS_ATTRINFO_ATTRIBUTENAMEFORMAT;

/* AttributeName */
#define MDS_ATTRINFO_ATTRIBUTENAME { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AttributeName" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AttributeName = MDS_ATTRINFO_ATTRIBUTENAME;

/* AttributeNameID */
#define MDS_ATTRINFO_ATTRIBUTENAMEID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AttributeNameID" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AttributeNameID = MDS_ATTRINFO_ATTRIBUTENAMEID;

/* AttributeDataType */
#define MDS_ATTRINFO_ATTRIBUTEDATATYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AttributeDataType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_AttributeDataType = MDS_ATTRINFO_ATTRIBUTEDATATYPE;

/* IndexID */
#define MDS_ATTRINFO_INDEXID { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "IndexID" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_IndexID = MDS_ATTRINFO_INDEXID;

/* IndexType */
#define MDS_ATTRINFO_INDEXTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "IndexType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_IndexType = MDS_ATTRINFO_INDEXTYPE;

/* IndexedDataLocation */
#define MDS_ATTRINFO_INDEXEDDATALOCATION { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "IndexedDataLocation" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_MdsAttrInfo_IndexedDataLocation = MDS_ATTRINFO_INDEXEDDATALOCATION;



/** CDSA Relation Attributes ***********************************************************************/

/*  MDS predefined values for a 16K name space */
#define CSSM_DB_ATTRIBUTE_MDS_START (0x40000000)
#define CSSM_DB_ATTRIBUTE_MDS_END   (0x40004000) 

#define MDS_CDSAATTR_MODULE_ID			(CSSM_DB_ATTRIBUTE_MDS_START + 1)
#define MDS_CDSAATTR_MANIFEST			(CSSM_DB_ATTRIBUTE_MDS_START + 2)
#define MDS_CDSAATTR_MODULE_NAME		(CSSM_DB_ATTRIBUTE_MDS_START + 3)
#define MDS_CDSAATTR_PATH				(CSSM_DB_ATTRIBUTE_MDS_START + 4)
#define MDS_CDSAATTR_VERSION			(CSSM_DB_ATTRIBUTE_MDS_START + 5)
#define MDS_CDSAATTR_VENDOR				(CSSM_DB_ATTRIBUTE_MDS_START + 6)
#define MDS_CDSAATTR_DESC				(CSSM_DB_ATTRIBUTE_MDS_START + 7)
#define MDS_CDSAATTR_INTERFACE_GUID		(CSSM_DB_ATTRIBUTE_MDS_START + 8)
#define MDS_CDSAATTR_POLICY_STMT		(CSSM_DB_ATTRIBUTE_MDS_START + 9)
#define MDS_CDSAATTR_EMM_VERSION		(CSSM_DB_ATTRIBUTE_MDS_START + 10)
#define MDS_CDSAATTR_EMM_VENDOR			(CSSM_DB_ATTRIBUTE_MDS_START + 11)
#define MDS_CDSAATTR_EMM_TYPE			(CSSM_DB_ATTRIBUTE_MDS_START + 12)
#define MDS_CDSAATTR_SSID				(CSSM_DB_ATTRIBUTE_MDS_START + 13)
#define MDS_CDSAATTR_SERVICE_TYPE		(CSSM_DB_ATTRIBUTE_MDS_START + 14)
#define MDS_CDSAATTR_NATIVE_SERVICES	(CSSM_DB_ATTRIBUTE_MDS_START + 15)
#define MDS_CDSAATTR_DYNAMIC_FLAG		(CSSM_DB_ATTRIBUTE_MDS_START + 16)
#define MDS_CDSAATTR_MULTITHREAD_FLAG	(CSSM_DB_ATTRIBUTE_MDS_START + 17)
#define MDS_CDSAATTR_SERVICE_MASK		(CSSM_DB_ATTRIBUTE_MDS_START + 18)
#define MDS_CDSAATTR_CSP_TYPE			(CSSM_DB_ATTRIBUTE_MDS_START + 19)
#define MDS_CDSAATTR_CSP_FLAGS			(CSSM_DB_ATTRIBUTE_MDS_START + 20)
#define MDS_CDSAATTR_CSP_CUSTOMFLAGS	(CSSM_DB_ATTRIBUTE_MDS_START + 21)
#define MDS_CDSAATTR_USEE_TAGS			(CSSM_DB_ATTRIBUTE_MDS_START + 22)
#define MDS_CDSAATTR_CONTEXT_TYPE		(CSSM_DB_ATTRIBUTE_MDS_START + 23)
#define MDS_CDSAATTR_ALG_TYPE			(CSSM_DB_ATTRIBUTE_MDS_START + 24)
#define MDS_CDSAATTR_GROUP_ID			(CSSM_DB_ATTRIBUTE_MDS_START + 25)
#define MDS_CDSAATTR_ATTRIBUTE_TYPE		(CSSM_DB_ATTRIBUTE_MDS_START + 26)
#define MDS_CDSAATTR_ATTRIBUTE_VALUE	(CSSM_DB_ATTRIBUTE_MDS_START + 27)
#define MDS_CDSAATTR_PRODUCT_DESC		(CSSM_DB_ATTRIBUTE_MDS_START + 28)
#define MDS_CDSAATTR_PRODUCT_VENDOR		(CSSM_DB_ATTRIBUTE_MDS_START + 29)
#define MDS_CDSAATTR_PRODUCT_VERSION	(CSSM_DB_ATTRIBUTE_MDS_START + 30)
#define MDS_CDSAATTR_PRODUCT_FLAGS		(CSSM_DB_ATTRIBUTE_MDS_START + 31)
#define MDS_CDSAATTR_PRODUCT_CUSTOMFLAGS	(CSSM_DB_ATTRIBUTE_MDS_START + 32)
#define MDS_CDSAATTR_STANDARD_DESC		(CSSM_DB_ATTRIBUTE_MDS_START + 33)
#define MDS_CDSAATTR_STANDARD_VERSION	(CSSM_DB_ATTRIBUTE_MDS_START + 34)
#define MDS_CDSAATTR_READER_DESC		(CSSM_DB_ATTRIBUTE_MDS_START + 35)
#define MDS_CDSAATTR_READER_VENDOR		(CSSM_DB_ATTRIBUTE_MDS_START + 36)
#define MDS_CDSAATTR_READER_VERSION		(CSSM_DB_ATTRIBUTE_MDS_START + 37)
#define MDS_CDSAATTR_READER_FWVERSION	(CSSM_DB_ATTRIBUTE_MDS_START + 38)
#define MDS_CDSAATTR_READER_FLAGS		(CSSM_DB_ATTRIBUTE_MDS_START + 39)
#define MDS_CDSAATTR_READER_CUSTOMFLAGS	(CSSM_DB_ATTRIBUTE_MDS_START + 40)
#define MDS_CDSAATTR_READER_SERIALNUMBER	(CSSM_DB_ATTRIBUTE_MDS_START + 41)
#define MDS_CDSAATTR_SC_DESC			(CSSM_DB_ATTRIBUTE_MDS_START + 42)
#define MDS_CDSAATTR_SC_VENDOR			(CSSM_DB_ATTRIBUTE_MDS_START + 43)
#define MDS_CDSAATTR_SC_VERSION			(CSSM_DB_ATTRIBUTE_MDS_START + 44)
#define MDS_CDSAATTR_SC_FWVERSION  		(CSSM_DB_ATTRIBUTE_MDS_START + 45)
#define MDS_CDSAATTR_SC_FLAGS			(CSSM_DB_ATTRIBUTE_MDS_START + 46)
#define MDS_CDSAATTR_SC_CUSTOMFLAGS		(CSSM_DB_ATTRIBUTE_MDS_START + 47)
#define MDS_CDSAATTR_SC_SERIALNUMBER	(CSSM_DB_ATTRIBUTE_MDS_START + 48)
#define MDS_CDSAATTR_DL_TYPE			(CSSM_DB_ATTRIBUTE_MDS_START + 49)
#define MDS_CDSAATTR_QUERY_LIMITS		(CSSM_DB_ATTRIBUTE_MDS_START + 50)
#define MDS_CDSAATTR_CONJUNCTIVE_OPS	(CSSM_DB_ATTRIBUTE_MDS_START + 51)
#define MDS_CDSAATTR_RELATIONAL_OPS		(CSSM_DB_ATTRIBUTE_MDS_START + 52)
#define MDS_CDSAATTR_PROTOCOL			(CSSM_DB_ATTRIBUTE_MDS_START + 53)
#define MDS_CDSAATTR_CERT_TYPEFORMAT	(CSSM_DB_ATTRIBUTE_MDS_START + 54)
#define MDS_CDSAATTR_CRL_TYPEFORMAT		(CSSM_DB_ATTRIBUTE_MDS_START + 55)
#define MDS_CDSAATTR_CERT_FIELDNAMES	(CSSM_DB_ATTRIBUTE_MDS_START + 56)
#define MDS_CDSAATTR_BUNDLE_TYPEFORMAT  (CSSM_DB_ATTRIBUTE_MDS_START + 57)
#define MDS_CDSAATTR_CERT_CLASSNAME		(CSSM_DB_ATTRIBUTE_MDS_START + 58)
#define MDS_CDSAATTR_ROOTCERT			(CSSM_DB_ATTRIBUTE_MDS_START + 59)
#define MDS_CDSAATTR_ROOTCERT_TYPEFORMAT	(CSSM_DB_ATTRIBUTE_MDS_START + 60)
#define MDS_CDSAATTR_VALUE				(CSSM_DB_ATTRIBUTE_MDS_START + 61)
#define MDS_CDSAATTR_REQCREDENTIALS		(CSSM_DB_ATTRIBUTE_MDS_START + 62)
#define MDS_CDSAATTR_SAMPLETYPES		(CSSM_DB_ATTRIBUTE_MDS_START + 63)
#define MDS_CDSAATTR_ACLSUBJECTTYPES	(CSSM_DB_ATTRIBUTE_MDS_START + 64)
#define MDS_CDSAATTR_AUTHTAGS			(CSSM_DB_ATTRIBUTE_MDS_START + 65)
#define MDS_CDSAATTR_USEETAG			(CSSM_DB_ATTRIBUTE_MDS_START + 66)
#define MDS_CDSAATTR_RETRIEVALMODE		(CSSM_DB_ATTRIBUTE_MDS_START + 67)
#define MDS_CDSAATTR_OID				(CSSM_DB_ATTRIBUTE_MDS_START + 68)
#define MDS_CDSAATTR_XLATIONTYPEFORMAT  (CSSM_DB_ATTRIBUTE_MDS_START + 69)
#define MDS_CDSAATTR_DEFAULT_TEMPLATE_TYPE  (CSSM_DB_ATTRIBUTE_MDS_START + 70)
#define MDS_CDSAATTR_TEMPLATE_FIELD_NAMES (CSSM_DB_ATTRIBUTE_MDS_START + 71)
#define MDS_CDSAATTR_AUTHORITY_REQUEST_TYPE (CSSM_DB_ATTRIBUTE_MDS_START + 72)


#endif
