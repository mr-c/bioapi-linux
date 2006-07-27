/*-----------------------------------------------------------------------
 * File: MDS_UTIL_TYPE.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/* Portions:
 * (C) COPYRIGHT International Business Machines Corp. 1996, 1997
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _MDS_UTIL_TYPE_H
#define _MDS_UTIL_TYPE_H

#if defined(WIN32)
#define MDS_UTIL_MAX_PATH	_MAX_PATH + 1
#define MDS_UTIL_MAX_REG	256 + 1
#elif defined(UNIX)
#include <limits.h>
#define MDS_UTIL_MAX_PATH	(PATH_MAX + 1)
#define MDS_UTIL_MAX_REG	256
#endif  /* WIN32 */

/* 
    Module Flags 
*/

typedef uint32 CSSM_MODULE_FLAGS;

#define CSSM_MODULE_THREADSAFE				0x1	/* Module is threadsafe */
#define CSSM_MODULE_EXPORTABLE				0x2	/* Module can be exported */

#define MODULE_STATIC_TYPE	CSSM_FALSE
#define MODULE_DYNAMIC_TYPE	CSSM_TRUE

typedef uint32 CSSM_APP_SERVICE_FLAGS;

#define CSSM_APP_SERVICE_AUTHENTOAPP  0x1 /* CSSM authenticates the service module
										   * based on application-supplied points of
										   * trust 
										   */

typedef struct mdsutil_credential_info {
	CSSM_SAMPLE_TYPE SampleTypes;
	CSSM_ACL_SUBJECT_TYPE AclSubjectTypes;
	CSSM_ACL_AUTHORIZATION_TAG	AuthTags;
} MDSUTIL_CREDENTIAL_INFO, *MDSUTIL_CREDENTIAL_INFO_PTR;

/*
    CSSM_DL_WRAPPEDPRODUCTINFO
*/

typedef struct mdsutil_dl_wrappedproductinfo {
    CSSM_VERSION StandardVersion;     /* Version of standard this product conforms to */
    CSSM_STRING StandardDescription;  /* Description of standard this product conforms to */
    CSSM_VERSION ProductVersion;      /* Version of wrapped product/library */
    CSSM_STRING ProductDescription;   /* Description of wrapped product/library */
    CSSM_STRING ProductVendor;        /* Vendor of wrapped product/library */
    uint32 ProductFlags;              /* ProductFlags */
	CSSM_NET_PROTOCOL NetworkProtocol;/* The network protocol supported by a remote storage service */

} MDSUTIL_DL_WRAPPEDPRODUCTINFO, *MDSUTIL_DL_WRAPPEDPRODUCTINFO_PTR;

typedef struct mdsutil_dlsubservice {
    uint32 SubServiceId;
    CSSM_STRING Description;          /* Description of this sub service */
    CSSM_DLTYPE Type;
    union mdsutil_dlsubservice_attributes {
        CSSM_DL_CUSTOM_ATTRIBUTES CustomAttributes;
        CSSM_DL_LDAP_ATTRIBUTES LdapAttributes;
        CSSM_DL_ODBC_ATTRIBUTES OdbcAttributes;        
        CSSM_DL_FFS_ATTRIBUTES FfsAttributes;
    } Attributes;

    MDSUTIL_DL_WRAPPEDPRODUCTINFO WrappedProduct;
    MDSUTIL_CREDENTIAL_INFO CredentialInfo;

    /* meta information about the query support provided by the module */
    uint32 NumberOfRelOperatorTypes;
    CSSM_DB_OPERATOR_PTR RelOperatorTypes;
    uint32 NumberOfConjOperatorTypes;
    CSSM_DB_CONJUNCTIVE_PTR ConjOperatorTypes;
    CSSM_BOOL QueryLimitsSupported;

    /* meta information about the encapsulated data stores (if known) */
    uint32 NumberOfDataStores;
    CSSM_NAME_LIST_PTR DataStoreNames;
    CSSM_DBINFO_PTR DataStoreInfo;

    /* additional information */
    void *Reserved;
} MDSUTIL_DLSUBSERVICE, *MDSUTIL_DLSUBSERVICE_PTR;

/*
    Software Csp SubService Info
*/

typedef struct mdsutil_softwarecspsubserviceinfo {
    uint32 NumberOfCapabilities;                /* Number of capabilities in list */
    CSSM_CONTEXT_PTR CapabilityList;			  /* List of capabilitites */
    void* Reserved;                             /* Reserved field */
} MDSUTIL_SOFTWARE_CSPSUBSERVICE_INFO, *MDSUTIL_SOFTWARE_CSPSUBSERVICE_INFO_PTR;

typedef struct mdsutil_hardwarecspsubserviceinfo {
    uint32 NumberOfCapabilities;                       /* Number of capabilities in list */
    CSSM_CONTEXT_PTR CapabilityList;            /* List of capabilitites */
    void* Reserved;                                    /* Reserved field */

    /* Reader/Slot Info */
    CSSM_STRING ReaderDescription;
    CSSM_STRING ReaderVendor;
    CSSM_STRING ReaderSerialNumber;
    CSSM_VERSION ReaderHardwareVersion;
    CSSM_VERSION ReaderFirmwareVersion;
    uint32 ReaderFlags;                                /* See ReaderFlags #define's */
    uint32 ReaderCustomFlags;

    /* 
        Token Info, may not be available if reader supports removable device 
        AND device is not present.
    */
    CSSM_STRING TokenDescription;   
    CSSM_STRING TokenVendor;            
    CSSM_STRING TokenSerialNumber;    
    CSSM_VERSION TokenHardwareVersion;    
    CSSM_VERSION TokenFirmwareVersion;    
    
    uint32 TokenFlags;                                /* See TokenFlags #defines's */
    uint32 TokenCustomFlags;
    uint32 TokenMaxSessionCount;
    uint32 TokenOpenedSessionCount;
    uint32 TokenMaxRWSessionCount;
    uint32 TokenOpenedRWSessionCount;
    uint32 TokenTotalPublicMem;
    uint32 TokenFreePublicMem;
    uint32 TokenTotalPrivateMem;
    uint32 TokenFreePrivateMem;
    uint32 TokenMaxPinLen;
    uint32 TokenMinPinLen;
    char TokenUTCTime[16];

    /*
        User Info, may not be available if reader supports removable device 
        AND device is not present.
    */
    CSSM_STRING UserLabel;
    CSSM_DATA UserCACertificate;                    /* Certificate of CA */ 

} MDSUTIL_HARDWARE_CSPSUBSERVICE_INFO, *MDSUTIL_HARDWARE_CSPSUBSERVICE_INFO_PTR;

typedef MDSUTIL_HARDWARE_CSPSUBSERVICE_INFO
				MDSUTIL_HYBRID_CSPSUBSERVICE_INFO, * MDSUTIL_HYBRID_CSPSUBSERVICE_INFO_PTR;


/*
    MDSUTIL_CSP_WRAPPEDPRODUCTINFO
*/

/* (Wrapped)ProductFlags */
/* None curently defined */

typedef struct mdsutil_csp_wrappedproductinfo {
    CSSM_VERSION StandardVersion;                           /* Version of standard this product conforms to */
    CSSM_STRING StandardDescription;  /* Description of standard this product conforms to */
    CSSM_VERSION ProductVersion;                            /* Version of wrapped product/library */
    CSSM_STRING ProductDescription;   /* Description of wrapped product/library */
    CSSM_STRING ProductVendor;        /* Vendor of wrapped product/library */
    uint32 ProductFlags;              /* ProductFlags */
	uint32 ProductCustomFlags;
} MDSUTIL_CSP_WRAPPEDPRODUCTINFO, *MDSUTIL_CSP_WRAPPEDPRODUCTINFO_PTR;

/*
    MDSUTIL_CSPINFO
*/


typedef struct mdsutil_cspsubservice {
    uint32 SubServiceId;
    CSSM_USEE_TAG USEETag;
    CSSM_STRING Description;
    CSSM_CSP_FLAGS CspFlags;       /* General flags defined by CSSM for CSPs */   
    uint32 CspCustomFlags;         /* Flags defined by individual CSP */  
    uint32 AccessFlags;            /* Access Flags used by CSP */
    CSSM_CSPTYPE CspType;          /* CSP type number for dereferencing CspInfo*/
    union mdsutil_subservice_info {   /* info struct of type defined by CspType */
        MDSUTIL_SOFTWARE_CSPSUBSERVICE_INFO SoftwareCspSubService;
        MDSUTIL_HARDWARE_CSPSUBSERVICE_INFO HardwareCspSubService;
		MDSUTIL_HYBRID_CSPSUBSERVICE_INFO HybridCspSubService;
    } SubServiceInfo;
    MDSUTIL_CSP_WRAPPEDPRODUCTINFO WrappedProduct;    /* Pointer to wrapped product info */
} MDSUTIL_CSPSUBSERVICE, *MDSUTIL_CSPSUBSERVICE_PTR;
typedef struct mdsutil_tp_wrappedproductinfo {
    CSSM_VERSION StandardVersion;	  /* Version of standard this product conforms to */
    CSSM_STRING StandardDescription;  /* Description of standard this product conforms to */
	CSSM_VERSION ProductVersion;      /* Version of wrapped product or library */
	CSSM_STRING  ProductDescription;  /* Desc of wrapped product library */
    CSSM_STRING ProductVendor;        /* Vendor of wrapped product/library */
    uint32 ProductFlags;			  /* ProductFlags */
} MDSUTIL_TP_WRAPPEDPRODUCTINFO, *MDSUTIL_TP_WRAPPEDPRODUCTINFO_PTR;

typedef struct mdsutil_tpsubservice {
    uint32 SubServiceId;
    CSSM_STRING Description; /* Description of this sub service */
    CSSM_CERT_TYPE CertType;       /* Type of certificate accepted by the TP */
	CSSM_CERT_ENCODING CertEncoding; 	/* Encoding of cert accepted by TP */
	MDSUTIL_CREDENTIAL_INFO CredentialInfo;
    uint32 NumberOfPolicyIdentifiers;    
    CSSM_FIELD_PTR PolicyIdentifiers;
	CSSM_TP_SERVICES AdditionalServiceFlags;/* Mask of additional services a caller can request */
    MDSUTIL_TP_WRAPPEDPRODUCTINFO WrappedProduct;  /* Pointer to wrapped product info */
} MDSUTIL_TPSUBSERVICE, *MDSUTIL_TPSUBSERVICE_PTR;

typedef struct mdsutil_acsubservice {
    uint32 SubServiceId;
    CSSM_STRING Description; /* Description of this sub service */
} MDSUTIL_ACSUBSERVICE, *MDSUTIL_ACSUBSERVICE_PTR;


/*
 * Structure to describe the attributes of the CSSM infrastructure.
 */
typedef struct mdsutil_cssminfo {
    CSSM_VERSION CDSAVersion; 
    CSSM_STRING Description;	/* Description of CSSM */ 
    CSSM_VERSION ProductVersion;
    CSSM_STRING Vendor;		/* Vendor of CSSM */
    CSSM_BOOL ThreadSafe;
    char Location[MDS_UTIL_MAX_PATH];
    CSSM_GUID CssmGUID;
}MDSUTIL_CSSMINFO, *MDSUTIL_CSSMINFO_PTR;
/*
    MDSUTIL_CL_WRAPPEDPRODUCTINFO
*/

/* CL_CA_ProductInfo */
typedef struct mdsutil_cl_ca_cert_classinfo {
    CSSM_STRING CertClassName;        /* Name of a cert class issued by this CA */
    CSSM_DATA CACert;                 /* CA Certificate for this cert class */
} MDSUTIL_CL_CA_CERT_CLASSINFO, *MDSUTIL_CL_CA_CERT_CLASSINFO_PTR;

typedef struct mdsutil_cl_ca_productinfo {
    CSSM_VERSION StandardVersion;     /* Version of standard this product conforms to */
    CSSM_STRING StandardDescription;  /* Description of standard this product conforms to */
    CSSM_VERSION ProductVersion;      /* Version of wrapped product/library */
    CSSM_STRING ProductDescription;   /* Description of wrapped product/library */
    CSSM_STRING ProductVendor;        /* Vendor of wrapped product/library */
	CSSM_NET_PROTOCOL NetworkProtocol;/* The network protocol supported by the CA service */

    CSSM_CERT_TYPE CertType;		  /* Type of certs and CRLs supported by the CA */
    CSSM_CERT_ENCODING CertEncoding; /* Encoding of certs supported by CA */
    CSSM_CRL_TYPE CrlType; /* Type of CRLs supported by CA */
    CSSM_CRL_ENCODING CrlEncoding; /* Encoding of CRLs supported by CA */
    uint32 NumberOfCertClasses;             /* Number of different cert types or classes the CA can issue */
    MDSUTIL_CL_CA_CERT_CLASSINFO_PTR CertClasses;                                  
} MDSUTIL_CL_CA_PRODUCTINFO, *MDSUTIL_CL_CA_PRODUCTINFO_PTR;

/* CL_Encoder_ProductInfo */
typedef struct mdsutil_cl_encoder_productinfo {
    CSSM_VERSION StandardVersion;       /* Version of standard this product conforms to */
    CSSM_STRING StandardDescription;  /* Description of standard this product conforms to */
    CSSM_VERSION ProductVersion;        /* Version of wrapped product/library */
    CSSM_STRING ProductDescription; /* Description of wrapped product/library */
    CSSM_STRING ProductVendor;    /* Vendor of wrapped product/library */       
    CSSM_CERT_TYPE CertType;            /* Type of certs and CRLs supported by the CA */
	CSSM_CRL_TYPE CrlType; /* Type of crls supported by encoder */
    uint32 ProductFlags;                /* Mask of selectable encoder features actually used by the CL */
} MDSUTIL_CL_ENCODER_PRODUCTINFO, *MDSUTIL_CL_ENCODER_PRODUCTINFO_PTR;

typedef struct mdsutil_cl_wrappedproductinfo {
    /* List of encode/decode/parse libraries embedded in the CL module */
    MDSUTIL_CL_ENCODER_PRODUCTINFO_PTR EmbeddedEncoderProducts;    /* library product description */
    uint32 NumberOfEncoderProducts;     /* number of encode/decode/parse libraries used in CL */

    /* List of CAs accessible to the CL module */
    MDSUTIL_CL_CA_PRODUCTINFO_PTR AccessibleCAProducts;            /* CA product description*/
    uint32 NumberOfCAProducts;    /* Number of accessible CAs */
} MDSUTIL_CL_WRAPPEDPRODUCTINFO, *MDSUTIL_CL_WRAPPEDPRODUCTINFO_PTR;


typedef struct mdsutil_clsubservice {
	uint32 SubServiceId;
	CSSM_STRING Description;  
	CSSM_CERT_TYPE CertType;
	CSSM_CERT_ENCODING CertEncoding;
	uint32 NumberOfBundleInfos; 
	CSSM_CERT_BUNDLE_HEADER_PTR BundleInfo;   /* first is default value */
	MDSUTIL_CREDENTIAL_INFO CredentialInfo;
	uint32 NumberOfTemplateFields;
	CSSM_OID_PTR CertTemplate;
	uint32 NumberOfTranslationTypes;
	CSSM_CERT_TYPE_PTR CertTranslationTypes;
	MDSUTIL_CL_WRAPPEDPRODUCTINFO WrappedProduct;
} MDSUTIL_CLSUBSERVICE, *MDSUTIL_CLSUBSERVICE_PTR;
#ifdef KEY_RECOVERY

typedef struct mdsutil_kr_wrappedproductinfo {
    CSSM_VERSION StandardVersion;      /* Version of standard this product conforms to */
    CSSM_STRING  StandardDescription;  /* Description of standard this product conforms to */
    CSSM_VERSION ProductVersion;       /* Version of wrapped product/library */
    CSSM_STRING ProductDescription;    /* Description of wrapped product/library */
    CSSM_STRING ProductVendor;         /* Vendor of wrapped product/library */
    uint32 ProductFlags;    
} MDSUTIL_KR_WRAPPEDPRODUCTINFO, *MDSUTIL_KR_WRAPPEDPRODUCTINFO_PTR;

typedef struct mdsutil_krspsubservice {
    uint32 SubServiceId;
    CSSM_STRING Description;      /* Description of this sub service */
    CSSM_KR_WRAPPEDPRODUCTINFO WrappedProduct;
} MDSUTIL_KRSPSUBSERVICE, *MDSUTIL_KRSPSUBSERVICE_PTR;
#endif /* KEY_RECOVERY */

typedef struct mdsutil_subservice_list {/* List of sub services */
	uint32  ModuleType;
	uint32 SubServiceId;
    void *SubServiceInfo;                        
} MDSUTIL_SUBSERVICE_LIST, *MDSUTIL_SUBSERVICE_LIST_PTR;


/*
    Service Info
*/

typedef struct mdsutil_serviceinfo
{
    CSSM_STRING Description;	/* Service description */
    CSSM_SERVICE_TYPE Type;		/* Service type */    
    CSSM_ATTACH_FLAGS Flags;	/* Service flags */  
    uint32 NumberOfSubServices;	/* Number of sub services in SubServiceList */ 
    MDSUTIL_SUBSERVICE_LIST_PTR SubServiceList;
    void* Reserved;
} MDSUTIL_SERVICE_INFO, *MDSUTIL_SERVICE_INFO_PTR;

/*
 * Module Info 
 */

typedef struct mdsutil_moduleinfo {
    CSSM_GUID ModuleGuid;
    CSSM_VERSION Version;			/* Module version */
    CSSM_VERSION CompatibleCSSMVersion;	/* Module written for CSSM version */
    CSSM_STRING Description;		/* Module description */
    CSSM_STRING Vendor;				/* Vendor name, etc */
    CSSM_MODULE_FLAGS Flags;		/* Flags to describe and control module use */
	CSSM_DATA_PTR AppAuthenRootKeys; /* Module-specific keys to authenticate apps */
    uint32 NumberOfAppAuthenRootKeys;/* Number of module-specific root keys */
    CSSM_SERVICE_MASK ServiceMask;	/* Bit mask of supported services */
    uint32 NumberOfServices;		/* Num of services in ServiceList */
    MDSUTIL_SERVICE_INFO_PTR ServiceList;	/* Pointer to list of service infos */
    void* Reserved;
} MDSUTIL_MODULE_INFO, *MDSUTIL_MODULE_INFO_PTR;
/* 
 * This structure aggregates the roots of trust for authenticating a particular 
 * add-in service module during module attach.
 */
typedef struct mdsutil_app_keys {
    CSSM_DATA_PTR ModuleAuthenRootKeys;	/* Application-specified keys to authen
										 * service modules */
    uint32 NumberOfModuleAuthenRootKeys;/* Number of application-specified root keys
										 */
} MDSUTIL_APP_KEYS, *MDSUTIL_APP_KEYS_PTR;

/*
 * This structure aggregates all information required by CSSM to perform additional
 * authentication of add-in service modules on behalf of an application during 
 * module attach processing.
 */
typedef struct mdsutil_app_service_info {
    CSSM_SUBSERVICE_UID_PTR ModuleList;  /* List of module service ID structs */
    uint32 NumberOfModules;       		/* Number of modules to authenticate */
    CSSM_APP_SERVICE_FLAGS Flags;	/* Flags selecting CSSM or app-specified roots 
									of trust */
    MDSUTIL_APP_KEYS_PTR Keys;/* Application-specified keys to authenticate modules */
    void *Reserved;
} MDSUTIL_APP_SERVICE_INFO, *MDSUTIL_APP_SERVICE_INFO_PTR; 

typedef struct mdsutil_module_manager_info {
    CSSM_VERSION EMMSpecVersion;
    CSSM_VERSION EmmVersion; /* Module Manager version */
    CSSM_VERSION CompatibleCSSMVersion;/* CSSM version the manager works with */
    CSSM_STRING Description; /* Module Manager description */
    CSSM_STRING Vendor; /* Vendor name */
    CSSM_SERVICE_MASK ServiceType; /* Bit mask of supported services */
    CSSM_DATA_PTR AppAuthenRootKeys; /* Mgr-specific keys to authenticate apps */
    uint32 NumberOfAppAuthenRootKeys;/* Number of Mgr-specific root keys */
    void *Reserved;
} MDSUTIL_MODULE_MANAGER_INFO, *MDSUTIL_MODULE_MANAGER_INFO_PTR; 

typedef struct mdsutil_list_item {
    CSSM_SUBSERVICE_UID	SubserviceUid;
    char *Name;
} MDSUTIL_LIST_ITEM, *MDSUTIL_LIST_ITEM_PTR;

typedef struct mdsutil_list {
    uint32 NumberItems;
    MDSUTIL_LIST_ITEM_PTR Items;
} MDSUTIL_LIST, *MDSUTIL_LIST_PTR;

#endif /* _MDS_UTIL_TYPE_H */
