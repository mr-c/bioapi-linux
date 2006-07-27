/*-----------------------------------------------------------------------
 * File: MDS_UTIL.H
 *-----------------------------------------------------------------------
 */

#ifndef __MDS_UTIL_H__
#define __MDS_UTIL_H__

#ifndef _CSSMTYPE_H
#error cssmtype.h must be included before this file
#endif
#ifndef MDS_H
#error mds.h must be included before this file
#endif

#if defined(WIN32)
#define MDS_UTIL_MAX_PATH	_MAX_PATH + 1
#elif defined(UNIX)
#include <limits.h>
#define MDS_UTIL_MAX_PATH	(PATH_MAX + 1)
#endif
typedef uint32 MDSU_TEMPLATE_MASK;

#define MDSU_MAX_VALUE_COUNT		( 16 )

/* MDS_CDSADIR_CSSM_RECORDTYPE */
#define MDSU_CSSM_MODULE_ID			 ( 0x00000001 )
#define MDSU_CSSM_CDSA_VERSION		 ( 0x00000002 )
#define MDSU_CSSM_VENDOR			 ( 0x00000004 )
#define MDSU_CSSM_DESC				 ( 0x00000008 )
#define MDSU_CSSM_NATIVE_SERVICES	 ( 0x00000010 )

typedef struct _mdsu_cdsa_cssm_schema
{
	CSSM_GUID ModuleID;
	CSSM_VERSION CDSAVersion;
	CSSM_STRING CssmVendor;
	CSSM_STRING Desc;
	CSSM_SERVICE_MASK NativeServices;
}
MDSU_CDSA_CSSM_SCHEMA;

/* MDS_OBJECT_RECORDTYPE */
#define MDSU_OBJ_MODULE_ID			 (0x00000001)
#define MDSU_OBJ_MANIFEST			 (0x00000002)
#define MDSU_OBJ_MODULE_NAME		 (0x00000004)
#define MDSU_OBJ_PATH				 (0x00000008)
#define MDSU_OBJ_PRODUCT_VERSION	 (0x00000010)

typedef struct _mdsu_object_schema
{
	CSSM_GUID ModuleID;
	CSSM_DATA Manifest;
	CSSM_STRING ModuleName;
	char Path[MDS_UTIL_MAX_PATH];
	CSSM_VERSION ProductVersion;
}
MDSU_OBJECT_SCHEMA;

/* 19.5 MDS_CDSADIR_EMM_RECORDTYPE */
#define MDSU_EMM_MODULE_ID			( 0x00000001 )
#define MDSU_EMM_MANIFEST			( 0x00000002 )
#define MDSU_EMM_MODULE_NAME		( 0x00000004 )
#define MDSU_EMM_PATH				( 0x00000008 )
#define MDSU_EMM_CDSA_VERSION		( 0x00000010 )
#define MDSU_EMM_EMM_SPEC_VERSION	( 0x00000020 )
#define MDSU_EMM_DESC				( 0x00000040 )
#define MDSU_EMM_POLICY_STMT		( 0x00000080 )
#define MDSU_EMM_EMM_VERSION		( 0x00000100 )
#define MDSU_EMM_EMM_VENDOR			( 0x00000200 )
#define MDSU_EMM_EMM_TYPE			( 0x00000400 )

typedef struct _mdsu_cdsa_emm_schema
{
	CSSM_GUID ModuleID;
	CSSM_DATA Manifest;
	CSSM_STRING ModuleName;
	char Path[MDS_UTIL_MAX_PATH];
	CSSM_VERSION CDSAVersion;
	CSSM_VERSION EmmSpecVersion;
	CSSM_STRING Desc;
	CSSM_DATA PolicyStmt;
	CSSM_VERSION EmmVersion;
	CSSM_STRING EmmVendor;
	CSSM_SERVICE_MASK EmmType;
}
MDSU_CDSA_EMM_SCHEMA;


/* 19.6 MDS_CDSADIR_EMM_PRIMARY_RECORDTYPE */
#define MDSU_EMMP_MODULE_ID			( 0x00000001 )
#define MDSU_EMMP_SSID				( 0x00000002 )
#define MDSU_EMMP_SERVICE_TYPE		( 0x00000004 )
#define MDSU_EMMP_MANIFEST			( 0x00000008 )
#define MDSU_EMMP_MODULE_NAME		( 0x00000010 )
#define MDSU_EMMP_VERSION			( 0x00000020 )
#define MDSU_EMMP_VENDOR			( 0x00000040 )
#define MDSU_EMMP_SAMPLE_TYPES		( 0x00000080 )
#define MDSU_EMMP_ACL_SUBJECT_TYPES ( 0x00000100 )
#define MDSU_EMMP_AUTH_TAGS			( 0x00000200 )
#define MDSU_EMMP_EMM_SPEC_VERSION	( 0x00000400 )

typedef struct _mdsu_cdsa_emm_primary
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_SERVICE_MASK ServiceType;
	CSSM_DATA Manifest;
	CSSM_STRING ModuleName;
	CSSM_VERSION Version;
	CSSM_STRING Vendor;
	uint32 NumSampleTypes;
	CSSM_SAMPLE_TYPE SampleTypes[MDSU_MAX_VALUE_COUNT];
	uint32 NumAclSubjectTypes;
	CSSM_ACL_SUBJECT_TYPE AclSubjectTypes[MDSU_MAX_VALUE_COUNT];
	uint32 NumAuthTags;
	CSSM_ACL_AUTHORIZATION_TAG AuthTags[MDSU_MAX_VALUE_COUNT];
	CSSM_VERSION EmmSpecVersion;
}
MDSU_CDSA_EMM_PRIMARY_SCHEMA;

/* MDS_CDSADIR_COMMON_RECORDTYPE */
#define MDSU_COM_MODULE_ID			( 0x00000001 )
#define MDSU_COM_MANIFEST			( 0x00000002 )
#define MDSU_COM_MODULE_NAME		( 0x00000004 )
#define MDSU_COM_PATH				( 0x00000008 )
#define MDSU_COM_CDSA_VERSION		( 0x00000010 )
#define MDSU_COM_DESC				( 0x00000020 )
#define MDSU_COM_DYNAMIC_FLAG		( 0x00000040 )
#define MDSU_COM_MULTI_THREAD_FLAG	( 0x00000080 )
#define MDSU_COM_SERVICE_MASK		( 0x00000100 )

typedef struct _mdsu_cdsa_common_schema
	{
	CSSM_GUID ModuleID;
	CSSM_DATA Manifest;
	CSSM_STRING ModuleName;
	char Path[MDS_UTIL_MAX_PATH];
	CSSM_VERSION CDSAVersion;
	CSSM_STRING Desc;
	CSSM_BOOL DynamicFlag;
	CSSM_BOOL MultiThreadFlag;
	CSSM_SERVICE_MASK ServiceMask;
}
MDSU_CDSA_COMMON_SCHEMA;

/* MDS_CDSADIR_CSP_PRIMARY_RECORDTYPE */
#define MDSU_CSPP_MODULE_ID			( 0x00000001 )
#define MDSU_CSPP_SSID				( 0x00000002 )
#define MDSU_CSPP_MANIFEST			( 0x00000004 )
#define MDSU_CSPP_MODULE_NAME		( 0x00000008 )
#define MDSU_CSPP_PRODUCT_VERSION	( 0x00000010 )
#define MDSU_CSPP_VENDOR			( 0x00000020 )
#define MDSU_CSPP_CSP_TYPE			( 0x00000040 )
#define MDSU_CSPP_CSP_FLAGS			( 0x00000080 )
#define MDSU_CSPP_CSP_CUSTOM_FLAGS	( 0x00000100 )
#define MDSU_CSPP_USEE_TAGS			( 0x00000200 )
#define MDSU_CSPP_SAMPLE_TYPES		( 0x00000400 )
#define MDSU_CSPP_ACL_SUBJ_TYPES	( 0x00000800 )
#define MDSU_CSPP_ACL_AUTH_TAGS		( 0x00001000 )

typedef struct _mdsu_cdsa_csp_primary_schema
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_DATA Manifest;
	CSSM_STRING ModuleName;
	CSSM_VERSION ProductVersion;
	CSSM_STRING Vendor;
	CSSM_CSPTYPE CspType;
	uint32 CspFlags;
	uint32 CspCustomFlags;
	CSSM_USEE_TAG UseeTags[MDSU_MAX_VALUE_COUNT];
	uint32 NumUseeTags;
	CSSM_SAMPLE_TYPE SampleTypes[MDSU_MAX_VALUE_COUNT];
	uint32 NumSampleTypes;
	CSSM_ACL_SUBJECT_TYPE AclSubjectTypes[MDSU_MAX_VALUE_COUNT];
	uint32 NumAclSubjectTypes;
	CSSM_ACL_AUTHORIZATION_TAG AclAuthTags[MDSU_MAX_VALUE_COUNT];
	uint32 NumAclAuthTags;
}
MDSU_CDSA_CSP_PRIMARY_SCHEMA;

/* MDS_CDSADIR_CSP_CAPABILITY_RECORDTYPE */
#define MDSU_CSPCAP_MODULE_ID		( 0x00000001 )
#define MDSU_CSPCAP_SSID			( 0x00000002 )
#define MDSU_CSPCAP_USEE_TAG		( 0x00000004 )
#define MDSU_CSPCAP_CONTEXT_TYPE	( 0x00000008 )
#define MDSU_CSPCAP_ALG_TYPE		( 0x00000010 )
#define MDSU_CSPCAP_GROUP_ID		( 0x00000020 )
#define MDSU_CSPCAP_ATTRIBUTE_TYPE	( 0x00000040 )
#define MDSU_CSPCAP_ATTRIBUTE_VALUE ( 0x00000080 )
#define MDSU_CSPCAP_DESCRIPTION		( 0x00000100 )

typedef struct _mdsu_cdsa_csp_capability
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_USEE_TAG UseeTag;
	CSSM_CONTEXT_TYPE ContextType;
	CSSM_ALGORITHMS AlgType;
	uint32 GroupId;
	CSSM_ATTRIBUTE_TYPE AttributeType;
	uint32 AttributeValue[MDSU_MAX_VALUE_COUNT];
	uint32 NumAttributeValues;
	CSSM_STRING Description;
}
MDSU_CDSA_CSP_CAPABILITY_SCHEMA;

/* MDS_CDSADIR_CSP_ENCAPSULATED_PRODUCT_RECORDTYPE */
#define MDSU_CSPEP_MODULE_ID				( 0x00000001 )
#define MDSU_CSPEP_SSID						( 0x00000002 )
#define MDSU_CSPEP_PRODUCT_DESC				( 0x00000004 )
#define MDSU_CSPEP_PRODUCT_VENDOR			( 0x00000008 )
#define MDSU_CSPEP_PRODUCT_VERSION			( 0x00000010 )
#define MDSU_CSPEP_PRODUCT_FLAGS			( 0x00000020 )
#define MDSU_CSPEP_CUSTOM_FLAGS				( 0x00000040 )
#define MDSU_CSPEP_STANDARD_DESC			( 0x00000080 )
#define MDSU_CSPEP_STANDARD_VERSION			( 0x00000100 )
#define MDSU_CSPEP_READER_DESC				( 0x00000200 )
#define MDSU_CSPEP_READER_VENDOR			( 0x00000400 )
#define MDSU_CSPEP_READER_VERSION			( 0x00000800 )
#define MDSU_CSPEP_READER_FIRMWARE_VERSION	( 0x00001000 )
#define MDSU_CSPEP_READER_FLAGS				( 0x00002000 )
#define MDSU_CSPEP_READER_CUSTOM_FLAGS		( 0x00004000 )
#define MDSU_CSPEP_READER_SERIAL_NUMBER		( 0x00008000 )

typedef struct _mdsu_cdsa_csp_enc_product
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_STRING ProductDesc;
	CSSM_STRING ProductVendor;
	CSSM_VERSION ProductVersion;
	uint32 ProductFlags;
	uint32 CustomFlags;
	CSSM_STRING StandardDesc;
	CSSM_VERSION StandardVersion;
	CSSM_STRING ReaderDesc;
	CSSM_STRING ReaderVendor;
	CSSM_VERSION ReaderVersion;
	CSSM_VERSION ReaderFirmwareVersion;
	uint32 ReaderFlags;
	uint32 ReaderCustomFlags;
	CSSM_STRING ReaderSerialNumber;
}
MDSU_CDSA_CSP_ENC_PRODUCT_SCHEMA;

/* MDS_CDSADIR_CSP_SC_INFO_RECORDTYPE */
#define MDSU_CSPSC_MODULE_ID				( 0x00000001 )
#define MDSU_CSPSC_SSID						( 0x00000002 )
#define MDSU_CSPSC_DESC						( 0x00000004 )
#define MDSU_CSPSC_VENDOR					( 0x00000008 )
#define MDSU_CSPSC_VERSION					( 0x00000010 )
#define MDSU_CSPSC_FW_VERSION				( 0x00000020 )
#define MDSU_CSPSC_FLAGS					( 0x00000040 )
#define MDSU_CSPSC_CUSTOM_FLAGS				( 0x00000080 )
#define MDSU_CSPSC_SERIAL_NUMBER			( 0x00000100 )

typedef struct _mdsu_cdsa_csp_smartcard_info
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_STRING ScDesc;
	CSSM_STRING ScVendor;
	CSSM_VERSION ScVersion;
	CSSM_VERSION ScFirmwareVersion;
	CSSM_SC_FLAGS ScFlags;
	uint32 ScCustomFlags;
	CSSM_STRING ScSerialNumber;
}
MDSU_CDSA_CSP_SMARTCARD_INFO_SCHEMA;

/* MDS_CDSADIR_DL_PRIMARY_RECORDTYPE */
#define MDSU_DLP_MODULE_ID			( 0x00000001 )
#define MDSU_DLP_SSID				( 0x00000002 )
#define MDSU_DLP_MANIFEST			( 0x00000004 )
#define MDSU_DLP_MODULE_NAME		( 0x00000008 )
#define MDSU_DLP_PRODUCT_VERSION	( 0x00000010 )
#define MDSU_DLP_VENDOR				( 0x00000020 )
#define MDSU_DLP_DL_TYPE			( 0x00000040 )
#define MDSU_DLP_QUERY_LIMITS_FLAG	( 0x00000080 )
#define MDSU_DLP_SAMPLE_TYPES		( 0x00000100 )
#define MDSU_DLP_ACL_SUBJ_TYPES		( 0x00000200 )
#define MDSU_DLP_ACL_AUTH_TAGS		( 0x00000400 )
#define MDSU_DLP_CONJUNCTIVE_OPS	( 0x00000800 )
#define MDSU_DLP_RELATIONAL_OPS		( 0x00001000 )

typedef struct _mdsu_cdsa_dl_primary
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_DATA Manifest;
	CSSM_STRING ModuleName;
	CSSM_VERSION ProductVersion;
	CSSM_STRING Vendor;
	CSSM_DLTYPE DLType;
	CSSM_BOOL QueryLimitsFlag;
	CSSM_SAMPLE_TYPE SampleTypes[MDSU_MAX_VALUE_COUNT];
	uint32 NumSampleTypes;
	CSSM_ACL_SUBJECT_TYPE AclSubjectTypes[MDSU_MAX_VALUE_COUNT];
	uint32 NumAclSubjectTypes;
	CSSM_ACL_AUTHORIZATION_TAG AclAuthTags[MDSU_MAX_VALUE_COUNT];
	uint32 NumAclAuthTags;
	CSSM_DB_CONJUNCTIVE ConjunctiveOps[MDSU_MAX_VALUE_COUNT];
	uint32 NumConjunctiveOps;
	CSSM_DB_OPERATOR RelationalOps[MDSU_MAX_VALUE_COUNT];
	uint32 NumRelationalOps;
}
MDSU_CDSA_DL_PRIMARY_SCHEMA;

/* MDS_CDSADIR_DL_ENCAPSULATED_PRODUCT_RECORDTYPE */
#define MDSU_DLEP_MODULE_ID			( 0x00000001 )
#define MDSU_DLEP_SSID				( 0x00000002 )
#define MDSU_DLEP_PRODUCT_DESC		( 0x00000004 )
#define MDSU_DLEP_PRODUCT_VENDOR	( 0x00000008 )
#define MDSU_DLEP_PRODUCT_VERSION	( 0x00000010 )
#define MDSU_DLEP_PRODUCT_FLAGS		( 0x00000020 )
#define MDSU_DLEP_STANDARD_DESC		( 0x00000040 )
#define MDSU_DLEP_STANDARD_VERSION	( 0x00000080 )
#define MDSU_DLEP_PROTOCOL			( 0x00000100 )
#define MDSU_DLEP_RETRIEVAL_MODE	( 0x00000200 )

typedef struct _mdsu_cdsa_dl_enc_product
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_STRING ProductDesc;
	CSSM_STRING ProductVendor;
	CSSM_VERSION ProductVersion;
	uint32 ProductFlags;
	//uint32 CustomFlags;
	CSSM_STRING StandardDesc;
	CSSM_VERSION StandardVersion;
	CSSM_NET_PROTOCOL Protocol;
	uint32 RetrievalMode;
}
MDSU_CDSA_DL_ENC_PRODUCT_SCHEMA;


/* MDS_CDSADIR_CL_PRIMARY_RECORDTYPE */
#define MDSU_CLP_MODULE_ID				( 0x00000001 )
#define MDSU_CLP_SSID					( 0x00000002 )
#define MDSU_CLP_MANIFEST				( 0x00000004 )
#define MDSU_CLP_MODULE_NAME			( 0x00000008 )
#define MDSU_CLP_PRODUCT_VERSION		( 0x00000010 )
#define MDSU_CLP_VENDOR					( 0x00000020 )
#define MDSU_CLP_CERT_TYPE_FORMAT		( 0x00000040 )
#define MDSU_CLP_CRL_TYPE_FORMAT		( 0x00000080 )
#define MDSU_CLP_CERT_FIELD_NAMES		( 0x00000100 )
#define MDSU_CLP_BUNDLE_TYPE_FORMAT		( 0x00000200 )
#define MDSU_CLP_XLATION_TYPE_FORMAT	( 0x00000400 )
#define MDSU_CLP_DEFAULT_TEMPLATE_TYPE	( 0x00000800 )
#define MDSU_CLP_TEMPLATE_FIELD_NAMES	( 0x00001000 )

typedef struct _mdsu_cdsa_cl_primary
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_DATA Manifest;
	CSSM_STRING ModuleName;
	CSSM_VERSION ProductVersion;
	CSSM_STRING Vendor;
	CSSM_CERT_TYPE CertTypeFormat;
	CSSM_CRL_TYPE CrlTypeFormat;
	CSSM_DATA CertFieldNames;
	CSSM_CERT_BUNDLE_TYPE BundleTypeFormat[MDSU_MAX_VALUE_COUNT];
	uint32 NumBundleTypeFormat;
	//XlationTypeFormat is a leftover, hence its type is missing from cssmtype.h
	//Since we know it's a multi-uint32, let's call it a uint32.
	uint32 XlationTypeFormat[MDSU_MAX_VALUE_COUNT];
	uint32 NumXlationTypeFormat;
	CSSM_CL_TEMPLATE_TYPE DefaultTemplateType;
	CSSM_DATA TemplateFieldNames;
}
MDSU_CDSA_CL_PRIMARY_SCHEMA;

/* MDS_CDSADIR_CL_ENCAPSULATED_PRODUCT_RECORDTYPE */
#define MDSU_CLEP_MODULE_ID			( 0x00000001 )
#define MDSU_CLEP_SSID				( 0x00000002 )
#define MDSU_CLEP_PRODUCT_DESC		( 0x00000004 )
#define MDSU_CLEP_PRODUCT_VENDOR	( 0x00000008 )
#define MDSU_CLEP_PRODUCT_VERSION	( 0x00000010 )
#define MDSU_CLEP_PRODUCT_FLAGS		( 0x00000020 )
#define MDSU_CLEP_STANDARD_DESC		( 0x00000040 )
#define MDSU_CLEP_STANDARD_VERSION	( 0x00000080 )

typedef struct _mdsu_cdsa_cl_enc_product
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_STRING ProductDesc;
	CSSM_STRING ProductVendor;
	CSSM_VERSION ProductVersion;
	uint32 ProductFlags;
	CSSM_STRING StandardDesc;
	CSSM_VERSION StandardVersion;
}
MDSU_CDSA_CL_ENC_PRODUCT_SCHEMA;


/* MDS_CDSADIR_TP_PRIMARY_RECORDTYPE */
#define MDSU_TPP_MODULE_ID			( 0x00000001 )
#define MDSU_TPP_SSID				( 0x00000002 )
#define MDSU_TPP_MANIFEST			( 0x00000004 )
#define MDSU_TPP_MODULE_NAME		( 0x00000008 )
#define MDSU_TPP_PRODUCT_VERSION	( 0x00000010 )
#define MDSU_TPP_VENDOR				( 0x00000020 )
#define MDSU_TPP_CERT_TYPE_FORMAT	( 0x00000040 )
#define MDSU_TPP_SAMPLE_TYPES		( 0x00000080 )
#define MDSU_TPP_ACL_SUBJ_TYPES		( 0x00000100 )
#define MDSU_TPP_ACL_AUTH_TAGS		( 0x00000200 )

typedef struct _mdsu_cdsa_tp_primary
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_DATA Manifest;
	CSSM_STRING ModuleName;
	CSSM_VERSION ProductVersion;
	CSSM_STRING Vendor;
	CSSM_CERT_TYPE CertTypeFormat;
	CSSM_SAMPLE_TYPE SampleTypes[MDSU_MAX_VALUE_COUNT];
	uint32 NumSampleTypes;
	CSSM_ACL_SUBJECT_TYPE AclSubjectTypes[MDSU_MAX_VALUE_COUNT];
	uint32 NumAclSubjectTypes;
	CSSM_ACL_AUTHORIZATION_TAG AclAuthTags[MDSU_MAX_VALUE_COUNT];
	uint32 NumAclAuthTags;
}
MDSU_CDSA_TP_PRIMARY_SCHEMA;

/* MDS_CDSADIR_TP_ENCAPSULATED_PRODUCT_RECORDTYPE */
#define MDSU_TPEP_MODULE_ID			( 0x00000001 )
#define MDSU_TPEP_SSID				( 0x00000002 )
#define MDSU_TPEP_PRODUCT_DESC		( 0x00000004 )
#define MDSU_TPEP_PRODUCT_VENDOR	( 0x00000008 )
#define MDSU_TPEP_PRODUCT_VERSION	( 0x00000010 )
#define MDSU_TPEP_PRODUCT_FLAGS		( 0x00000020 )
#define MDSU_TPEP_AUTH_REQUEST_TYPE ( 0x00000040 )
#define MDSU_TPEP_STANDARD_DESC		( 0x00000080 )
#define MDSU_TPEP_STANDARD_VERSION	( 0x00000100 )
#define MDSU_TPEP_PROTOCOL_DESC		( 0x00000200 )
#define MDSU_TPEP_PROTOCOL_FLAGS	( 0x00000400 )
#define MDSU_TPEP_CERT_CLASS_NAME	( 0x00000800 )
#define MDSU_TPEP_ROOT_CERTIFICATE	( 0x00001000 )
#define MDSU_TPEP_ROOT_CERT_TYPE_FORMAT ( 0x00002000 )

typedef struct _mdsu_cdsa_tp_enc_product
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_STRING ProductDesc;
	CSSM_STRING ProductVendor;
	CSSM_VERSION ProductVersion;
	uint32 ProductFlags;
	CSSM_TP_AUTHORITY_REQUEST_TYPE AuthorityRequestType[MDSU_MAX_VALUE_COUNT];
	uint32 NumAuthorityRequestType;
	CSSM_STRING StandardDesc;
	CSSM_VERSION StandardVersion;
	CSSM_STRING ProtocolDesc;
	CSSM_NET_PROTOCOL ProtocolFlags;
	CSSM_STRING CertClassName;
	CSSM_DATA RootCertificate;
	CSSM_CERT_TYPE RootCertTypeFormat;
}
MDSU_CDSA_TP_ENC_PRODUCT_SCHEMA;


/* MDS_CDSADIR_AC_PRIMARY_RECORDTYPE */
#define MDSU_ACP_MODULE_ID				( 0x00000001 )
#define MDSU_ACP_SSID					( 0x00000002 )
#define MDSU_ACP_MANIFEST				( 0x00000004 )
#define MDSU_ACP_MODULE_NAME			( 0x00000008 )
#define MDSU_ACP_PRODUCT_VERSION		( 0x00000010 )
#define MDSU_ACP_VENDOR					( 0x00000020 )

typedef struct _mdsu_cdsa_ac_primary
{
	CSSM_GUID ModuleID;
	uint32 SSID;
	CSSM_DATA Manifest;
	CSSM_STRING ModuleName;
	CSSM_VERSION ProductVersion;
	CSSM_STRING Vendor;
}
MDSU_CDSA_AC_PRIMARY_SCHEMA;


/* Memory management macros */
#ifdef BioAPI_MEMTRACK_ON
#define MDSU_malloc( context, size ) \
	(context)->MemFuncs.Malloc_func( (size), \
									 (context)->MemFuncs.AllocRef, \
									 __FILE__, __LINE__ )
#define MDSU_free( context, block ) \
	(context)->MemFuncs.Free_func( (block), \
								   (context)->MemFuncs.AllocRef, \
								   __FILE__, __LINE__ )
#define MDSU_realloc( context, block, size ) \
	(context)->MemFuncs.Realloc_func( (block), (size), \
									  (context)->MemFuncs.AllocRef, \
									  __FILE__, __LINE__ )
#define MDSU_calloc( context, size, count ) \
	(context)->MemFuncs.Calloc_func( (size), (count), \
									 (context)->MemFuncs.AllocRef, \
									 __FILE__, __LINE__ )
#else
#define MDSU_malloc( context, size ) \
	(context)->MemFuncs.Malloc_func( (size), \
									 (context)->MemFuncs.AllocRef )
#define MDSU_free( context, block ) \
	(context)->MemFuncs.Free_func( (block), \
								   (context)->MemFuncs.AllocRef )
#define MDSU_realloc( context, block, size ) \
	(context)->MemFuncs.Realloc_func( (block), (size), \
									  (context)->MemFuncs.AllocRef )
#define MDSU_calloc( context, size, count ) \
	(context)->MemFuncs.Calloc_func( (size), (count), \
									 (context)->MemFuncs.AllocRef )
#endif /* BioAPI_MEMTRACK_ON */


/* Forward declaration for interface pointers */
typedef struct _mdsu_context MDSU_CONTEXT;

/* Type definitions for the interface functions used to manipulate an MDS
 * schema.
 */
typedef BioAPI_RETURN (BioAPI *_MDSU_PredicateConstructor)(
							MDSU_CONTEXT *pContext,
							const void *pSchemaTemplate,
							MDSU_TEMPLATE_MASK ValidTemplates,
							CSSM_QUERY *pQuery,
							void **pState );

typedef BioAPI_RETURN (BioAPI *_MDSU_PredicateDestructor)(
							MDSU_CONTEXT *pContext,
							CSSM_QUERY *pQuery,
							void *pState );

typedef BioAPI_RETURN (BioAPI *_MDSU_AttributeConstructor)(
							MDSU_CONTEXT *pContext,
							const void *pSchemaData,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							CSSM_BOOL bFillValues,
							void **pState );

typedef BioAPI_RETURN (BioAPI *_MDSU_AttributeDestructor)(
							MDSU_CONTEXT *pContext,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							CSSM_BOOL bFreeAll,
							void *pState );

typedef BioAPI_RETURN (BioAPI *_MDSU_AttributeConverter)(
							MDSU_CONTEXT *pContext,
							CSSM_DB_RECORD_ATTRIBUTE_DATA *DbRecord,
							void *pSchemaData );

/* Interface structure for schema manipulators.
 */
typedef struct __if_mdsu_schema_manipulator
{
	_MDSU_AttributeConstructor AttributeConstructor;
	_MDSU_AttributeDestructor AttributeDestructor;
	_MDSU_PredicateConstructor PredicateConstructor;
	_MDSU_PredicateDestructor PredicateDestructor;
	_MDSU_AttributeConverter AttributeConverter;
	CSSM_DB_RECORDTYPE SchemaType;
}
IfMdsuSchemaManipulator;



/* Schema manipulator interfaces. */

//extern const IfMdsuSchemaManipulator IfiMdsuBioAPICapabilitySchema;
//extern const IfMdsuSchemaManipulator IfiMdsuBioAPIBspCapabilitySchema;
//extern const IfMdsuSchemaManipulator IfiMdsuBioAPIBspDeviceSchema;


/* MDS Manipulation Context */
struct _mdsu_context
{
	/* Session handles */
	MDS_HANDLE hMds;
	MDS_DB_HANDLE hDb;

	/* Guid of the module performing the operations */
	BioAPI_UUID ModuleUuid;

	/* Used to track the commit cache */
	CSSM_DB_UNIQUE_RECORD_PTR *pCommitCache;
	uint32 CacheCount;
	uint32 CommitSize;

	/* MDS query control */
	CSSM_HANDLE hResults;
	const IfMdsuSchemaManipulator *pManipulator;

	/* Memory functions used by the MDS operations */
	BioAPI_MEMORY_FUNCS MemFuncs;
};

/* Main interface functions */
BioAPI_RETURN BioAPI MDSU_Init(
						MDSU_CONTEXT *pContext,
						const BioAPI_MEMORY_FUNCS *pMemFuncs,
						const BioAPI_UUID *pUuid,
						const char *szDirectoryName,
						CSSM_DB_ACCESS_TYPE AccessType,
						uint32 CommitSize );

BioAPI_RETURN BioAPI MDSU_Term(
						MDSU_CONTEXT *pContext );

BioAPI_RETURN BioAPI MDSU_Revert(
						MDSU_CONTEXT *pContext );

BioAPI_RETURN BioAPI MDSU_Commit(
						MDSU_CONTEXT *pContext );

BioAPI_RETURN BioAPI MDSU_UpdateSchema(
						MDSU_CONTEXT *pContext,
						const IfMdsuSchemaManipulator *Manipulator,
						void *pSchemaData,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueRecord );

BioAPI_RETURN BioAPI MDSU_FreeUniqueRecord(
						MDSU_CONTEXT *pContext,
						CSSM_DB_UNIQUE_RECORD_PTR pUID );

BioAPI_RETURN BioAPI MDSU_FindFirst(
						MDSU_CONTEXT *pContext,
						const IfMdsuSchemaManipulator *Manipulator,
						void *pSchemaTemplate,
						MDSU_TEMPLATE_MASK ValidityMask,
						void *pSchemaData,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueRecord );

BioAPI_RETURN BioAPI MDSU_FindNext(
						MDSU_CONTEXT *pContext,
						void *pSchemaData,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueRecord );

BioAPI_RETURN BioAPI MDSU_FindAbort(
						MDSU_CONTEXT *pContext );

BioAPI_RETURN BioAPI MDSU_DeleteRecord(
						MDSU_CONTEXT *pContext,
						CSSM_DB_UNIQUE_RECORD *pUID );

#endif /* ifndef __MDS_UTIL_H__ */
