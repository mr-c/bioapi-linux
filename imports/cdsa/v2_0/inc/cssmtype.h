/*-----------------------------------------------------------------------
 * File: CSSMTYPE.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#ifndef _CSSMTYPE_H
#define _CSSMTYPE_H


/* 
    Operating System Dependent Primitive Declarations 
    Not in TOG standard: added from cssmtype.h
*/

#if defined (WIN32)

#define CSSMAPI __stdcall
#define CSSM_FUNC __cdecl

#elif defined (UNIX) 

#define CSSMAPI
#define CALLBACK
#define CSSM_FUNC

#endif

#if defined (WIN32)

/* Disable Warnings due to non-ANSI content of windows.h */
/* Warning 4100 "unreferenced formal parameter", not flagged in gcc */
#pragma warning (disable:4201 4514 4214 4115 4100)
#include <windows.h>
#pragma warning (default:4201 4214 4115)

/* Turn parameter list mismatch warnings into errors */
#pragma warning (error:4113 4020 4021)
#endif /* defined(WIN32) */

/* End of: Not in TOG */

/* Linux-Port: added __BASIC_TYPES__ branch, see bioapi_type.h */
#if !defined(__BASIC_TYPES__)
#define __BASIC_TYPES__
/* Basic Types */
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef short sint16;
typedef unsigned int uint32;
typedef int sint32;

#if defined(WIN32)
typedef unsigned __int64 uint64; /* MSVC++ 5/6 declaration */
#else
#ifdef __arch64__
typedef unsigned long uint64;
#else
typedef unsigned long long uint64; /* gcc 2.7.2 declaration */
#endif
#endif
#endif /* __BASIC_TYPES__ */

/*****************************************************************************
    Core Services API Datatypes
*****************************************************************************/

/* 
    CSSM_BOOL
    This data type is used to indicate a true or false condition
*/
typedef uint32 CSSM_BOOL;
#define CSSM_FALSE (0)
#define CSSM_TRUE (!CSSM_FALSE)

/*
    CSSM_RETURN
    This data type is returned by most CDSA functions. 
*/
typedef uint32 CSSM_RETURN;
#define CSSM_OK (0)

/*
    CSSM_STRING
    This is used by CSSM data structures to represent a character string
    inside of a fixed-length buffer. The character string is expected to
    be NULL-terminated.
*/
#define CSSM_MODULE_STRING_SIZE (64)
typedef char CSSM_STRING [CSSM_MODULE_STRING_SIZE + 4];

/*
    CSSM_DATA
    The CSSM_DATA structure is used to associate a length, in bytes, with
    an arbitrary block of contiguous memory.
*/
typedef struct cssm_data
{
    uint32 Length; /* in bytes */
    uint8 *Data;
} CSSM_DATA, *CSSM_DATA_PTR;

/*
    CSSM_GUID
    This structure designates a global unique identifier (GUID) that
    distinguishes one security service module from another.
*/
typedef struct cssm_guid
{
    uint32 Data1;
    uint16 Data2;
    uint16 Data3;
    uint8 Data4[8];
} CSSM_GUID, *CSSM_GUID_PTR;

/*
    CSSM_KEY_HIERARCHY
    The CSSM_KEY_HIERARCHY is a bitmask of values defining classes of key
    hierarchies from which CDSA components may be signed.
*/
typedef uint32 CSSM_BITMASK;
typedef CSSM_BITMASK CSSM_KEY_HIERARCHY;
#define CSSM_KEY_HIERARCHY_NONE (0)
#define CSSM_KEY_HIERARCHY_INTEG (1)
#define CSSM_KEY_HIERARCHY_EXPORT (2)

/*
    CSSM_PVC_MODE
    Pointer validation checking policy
*/
typedef CSSM_BITMASK CSSM_PVC_MODE;
#define CSSM_PVC_NONE (0)
#define CSSM_PVC_APP (1)
#define CSSM_PVC_SP (2)

/*
    CSSM_PRIVILEGE_SCOPE
    The privilege scope identifies whether the privilege specified using
    the CSSM_SetPrivilege( ) call applies to the entire process,
    CSSM_PRIVILEGE_SCOPE_PROCESS, or to the current thread,
    CSSM_PRIVILEGE_SCOPE_THREAD.
*/
typedef uint32 CSSM_PRIVILEGE_SCOPE;
#define CSSM_PRIVILEGE_SCOPE_NONE (0)
#define CSSM_PRIVILEGE_SCOPE_PROCESS (1)
#define CSSM_PRIVILEGE_SCOPE_THREAD (2)

/*
    CSSM_VERSION
    This structure is used to represent the version of CDSA components.
*/
typedef struct cssm_version 
{
    uint32 Major;
    uint32 Minor;
} CSSM_VERSION, *CSSM_VERSION_PTR;


/*
    CSSM_SERVICE_MASK
    This defines a bit mask of all the types of CSSM services a single
    module can implement.
*/
typedef uint32 CSSM_SERVICE_MASK;
#define CSSM_SERVICE_CSSM (0x1)
#define CSSM_SERVICE_CSP (0x2)
#define CSSM_SERVICE_DL (0x4)
#define CSSM_SERVICE_CL (0x8)
#define CSSM_SERVICE_TP (0x10)
#define CSSM_SERVICE_AC (0x20)
#define CSSM_SERVICE_KR (0x40)

/*
    CSSM_SERVICE_TYPE
    This data type is used to identify a single service from the
    CSSM_SERVICE_MASK options defined above.
*/
typedef CSSM_SERVICE_MASK CSSM_SERVICE_TYPE;


/*
    CSSM_SUBSERVICE_UID
    This structure uniquely identifies a set of behaviors within a
    subservice within a CSSM security service module.
*/
typedef struct cssm_subservice_uid 
{
    CSSM_GUID Guid;
    CSSM_VERSION Version;
    uint32 SubserviceId;
    CSSM_SERVICE_TYPE SubserviceType;
} CSSM_SUBSERVICE_UID, *CSSM_SUBSERVICE_UID_PTR;

/*
    CSSM_HANDLE
    A unique identifier for an object managed by CSSM or by an security
    service module.
*/
typedef uint32 CSSM_HANDLE, *CSSM_HANDLE_PTR;
#define CSSM_INVALID_HANDLE (0)

/*
    CSSM_LONG_HANDLE
    A unique, long identifier for an object managed by CSSM or by a
    service module.
*/
typedef uint64 CSSM_LONG_HANDLE, *CSSM_LONG_HANDLE_PTR;

/*
    CSSM_MODULE_HANDLE
    A unique identifier for an attached service provider module.
*/
typedef CSSM_HANDLE CSSM_MODULE_HANDLE, *CSSM_MODULE_HANDLE_PTR;

/*
    Handles for various SPs
*/
typedef CSSM_MODULE_HANDLE CSSM_AC_HANDLE;    /* Authorization Computation Handle */
typedef CSSM_MODULE_HANDLE CSSM_TP_HANDLE;    /* Trust Policy Handle */
typedef CSSM_MODULE_HANDLE CSSM_CL_HANDLE;    /* Certificate Library Handle */
typedef CSSM_MODULE_HANDLE CSSM_DL_HANDLE;    /* Database Library Handle */
typedef CSSM_MODULE_HANDLE CSSM_DB_HANDLE;    /* Database Handle */
typedef CSSM_MODULE_HANDLE CSSM_CSP_HANDLE;   /* Cryptographic SP Handle */
typedef CSSM_MODULE_HANDLE CSSM_KRSP_HANDLE;  /* Key Recovery SP Handle */
typedef CSSM_HANDLE CSSM_KRC_HANDLE;          /* Key Recovery Context Handle */


/*
    CSSM_DL_DB_HANDLE
    This data structure holds a pair of handles, one for a data storage
    library and another for a data store opened and being managed by the
    data storage library.
*/
typedef struct cssm_dl_db_handle 
{
    CSSM_DL_HANDLE DLHandle;
    CSSM_DB_HANDLE DBHandle;
} CSSM_DL_DB_HANDLE, *CSSM_DL_DB_HANDLE_PTR;

/*
    CSSM_MODULE_EVENT
    This enumeration defines the event types that can be raised by any
    service module.
*/
typedef enum cssm_module_event 
{
    CSSM_NOTIFY_INSERT = 1,
    CSSM_NOTIFY_REMOVE = 2,
    CSSM_NOTIFY_FAULT = 3
} CSSM_MODULE_EVENT, *CSSM_MODULE_EVENT_PTR;

/*
    CSSM_API_ModuleEventHandler
    This defines the event handler interface that an application must define
    and implement to receive asynchronous notification of events such as
    insertion or removal of a hardware service module, or a fault detected
    by the service module.
*/
typedef CSSM_RETURN (CSSMAPI *CSSM_API_ModuleEventHandler)
    (const CSSM_GUID *ModuleGuid,
    void* AppNotifyCallbackCtx,
    uint32 SubserviceId,
    CSSM_SERVICE_TYPE ServiceType,
    CSSM_MODULE_EVENT EventType);
/*
    CSSM_ATTACH_FLAGS
    This bitmask is used to specify the behaivor of the service provider
    being attached.
*/
typedef uint32 CSSM_ATTACH_FLAGS;
#define CSSM_ATTACH_READ_ONLY (0x00000001)

/*
    CSSM_PRIVILEGE
    The USE exemption tag lists the exemption categories for cryptographic
    services.
*/
typedef uint32 CSSM_USEE_TAG;
typedef uint64 CSSM_PRIVILEGE;
#define CSSM_USEE_LAST (0xFF)
#define CSSM_USEE_NONE (0)
#define CSSM_USEE_DOMESTIC (1)
#define CSSM_USEE_FINANCIAL (2)
#define CSSM_USEE_KRLE (3)
#define CSSM_USEE_KRENT (4)
#define CSSM_USEE_SSL (5)
#define CSSM_USEE_AUTHENTICATION (6)
#define CSSM_USEE_KEYEXCH (7)
#define CSSM_USEE_MEDICAL (8)
#define CSSM_USEE_INSURANCE (9)
#define CSSM_USEE_WEAK (10)

/*
    CSSM_NET_ADDRESS_TYPE
    This enumerated type defines representations for specifying the location
    of a service.
*/
typedef enum cssm_net_address_type 
{
    CSSM_ADDR_NONE = 0,
    CSSM_ADDR_CUSTOM = 1,
    CSSM_ADDR_URL = 2, /* char* */
    CSSM_ADDR_SOCKADDR = 3,
    CSSM_ADDR_NAME =4 /*char* - qualified by access method */
} CSSM_NET_ADDRESS_TYPE;

/*
    CSSM_NET_ADDRESS
    This structure holds the address of a service.
*/
typedef struct cssm_net_address 
{
    CSSM_NET_ADDRESS_TYPE AddressType;
    CSSM_DATA Address;
} CSSM_NET_ADDRESS, *CSSM_NET_ADDRESS_PTR;

/*
    CSSM_NET_PROTOCOL
    This data type defines the application-level protocols that could be
    supported by a Data Storage Library Module that communicates with
    service-based storage and directory services.
*/
typedef uint32 CSSM_NET_PROTOCOL;
#define CSSM_NET_PROTO_NONE (0)     /* local */
#define CSSM_NET_PROTO_CUSTOM (1)   /* proprietary implementation */
#define CSSM_NET_PROTO_UNSPECIFIED (2)  /* implementation default */
#define CSSM_NET_PROTO_LDAP (3)     /* light weight directory access protocol */
#define CSSM_NET_PROTO_LDAPS (4)    /* ldap/ssl where SSL initiates the connection */
#define CSSM_NET_PROTO_LDAPNS (5)   /* ldap where ldap negotiates an SSL session */
#define CSSM_NET_PROTO_X500DAP (6)  /* X.500 Directory access protocol */
#define CSSM_NET_PROTO_FTP (7)      /* ftp for cert/crl fetch */
#define CSSM_NET_PROTO_FTPS (8)     /* ftp/ssl/tls where SSL/TLS initiates connection */
#define CSSM_NET_PROTO_OCSP (9)     /* online certificate status protocol */
#define CSSM_NET_PROTO_CMP (10)     /* the cert request protocol in PKIX3 */
#define CSSM_NET_PROTO_CMPS (11)    /* The ssl/tls derivative of CMP */

/*
    CSSM_CALLBACK
    An application uses this data type to request that an security service
    module call back into the application for certain cryptographic information.
*/
typedef CSSM_RETURN (CSSMAPI *CSSM_CALLBACK) (CSSM_DATA_PTR OutData, void *CallerCtx);


/*
    CSSM_CRYPTO_DATA
    This data structure is used to encapsulate cryptographic information
    passed from the application to a service provider module via a callback
    function. Typical information passed by this callback mechanism includes
    a seed value for cryptographic operations.
*/
typedef struct cssm_crypto_data 
{
    CSSM_DATA Param;
    CSSM_CALLBACK Callback;
    void *CallerCtx;
} CSSM_CRYPTO_DATA, *CSSM_CRYPTO_DATA_PTR;

/*
    CSSM_WORDID_TYPE
    This data type defines common symbols for integer values used to represent
    sample types, ACL subject types, and authorization tag values.
*/
typedef sint32 CSSM_WORDID_TYPE;
#define CSSM_WORDID__UNK_ (-1) /* not in dictionary */
#define CSSM_WORDID__NLU_ (0)  /* not yet looked up */
#define CSSM_WORDID__STAR_ (1)
#define CSSM_WORDID_A (2)
#define CSSM_WORDID_ACL (3)
#define CSSM_WORDID_ALPHA (4)
#define CSSM_WORDID_B (5)
#define CSSM_WORDID_BER (6)
#define CSSM_WORDID_BINARY (7)
#define CSSM_WORDID_BIOMETRIC (8)
#define CSSM_WORDID_C (9)
#define CSSM_WORDID_CANCELED (10)
#define CSSM_WORDID_CERT (11)
#define CSSM_WORDID_COMMENT (12)
#define CSSM_WORDID_CRL (13)
#define CSSM_WORDID_CUSTOM (14)
#define CSSM_WORDID_D (15)
#define CSSM_WORDID_DATE (16)
#define CSSM_WORDID_DB_DELETE (17)
#define CSSM_WORDID_DB_EXEC_STORED_QUERY (18)
#define CSSM_WORDID_DB_INSERT (19)
#define CSSM_WORDID_DB_MODIFY (20)
#define CSSM_WORDID_DB_READ (21)
#define CSSM_WORDID_DBS_CREATE (22)
#define CSSM_WORDID_DBS_DELETE (23)
#define CSSM_WORDID_DECRYPT (24)
#define CSSM_WORDID_DELETE (25)
#define CSSM_WORDID_DELTA_CRL (26)
#define CSSM_WORDID_DER (27)
#define CSSM_WORDID_DERIVE (28)
#define CSSM_WORDID_DISPLAY (29)
#define CSSM_WORDID_DO (30)
#define CSSM_WORDID_DSA (31)
#define CSSM_WORDID_DSA_SHA1 (32)
#define CSSM_WORDID_E (33)
#define CSSM_WORDID_ELGAMAL (34)
#define CSSM_WORDID_ENCRYPT (35)
#define CSSM_WORDID_ENTRY (36)
#define CSSM_WORDID_EXPORT_CLEAR (37)
#define CSSM_WORDID_EXPORT_WRAPPED (38)
#define CSSM_WORDID_G (39)
#define CSSM_WORDID_GE (40)
#define CSSM_WORDID_GENKEY (41)
#define CSSM_WORDID_HASH (42)
#define CSSM_WORDID_HAVAL (43)
#define CSSM_WORDID_IBCHASH (44)
#define CSSM_WORDID_IMPORT_CLEAR (45)
#define CSSM_WORDID_IMPORT_WRAPPED (46)
#define CSSM_WORDID_INTEL (47)
#define CSSM_WORDID_ISSUER (48)
#define CSSM_WORDID_ISSUER_INFO (49)
#define CSSM_WORDID_K_OF_N (50)
#define CSSM_WORDID_KEA (51)
#define CSSM_WORDID_KEYHOLDER (52)
#define CSSM_WORDID_L (53)
#define CSSM_WORDID_LE (54)
#define CSSM_WORDID_LOGIN (55)
#define CSSM_WORDID_LOGIN_NAME (56)
#define CSSM_WORDID_MAC (57)
#define CSSM_WORDID_MD2 (58)
#define CSSM_WORDID_MD2WITHRSA (59)
#define CSSM_WORDID_MD4 (60)
#define CSSM_WORDID_MD5 (61)
#define CSSM_WORDID_MD5WITHRSA (62)
#define CSSM_WORDID_N (63)
#define CSSM_WORDID_NAME (64)
#define CSSM_WORDID_NDR (65)
#define CSSM_WORDID_NHASH (66)
#define CSSM_WORDID_NOT_AFTER (67)
#define CSSM_WORDID_NOT_BEFORE (68)
#define CSSM_WORDID_NULL (69)
#define CSSM_WORDID_NUMERIC (70)
#define CSSM_WORDID_OBJECT_HASH (71)
#define CSSM_WORDID_ONE_TIME (72)
#define CSSM_WORDID_ONLINE (73)
#define CSSM_WORDID_OWNER (74)
#define CSSM_WORDID_P (75)
#define CSSM_WORDID_PAM_NAME (76)
#define CSSM_WORDID_PASSWORD (77)
#define CSSM_WORDID_PGP (78)
#define CSSM_WORDID_PREFIX (79)
#define CSSM_WORDID_PRIVATE_KEY (80)
#define CSSM_WORDID_PROMPTED_PASSWORD (81)
#define CSSM_WORDID_PROPAGATE (82)
#define CSSM_WORDID_PROTECTED_BIOMETRIC (83)
#define CSSM_WORDID_PROTECTED_PASSWORD (84)
#define CSSM_WORDID_PROTECTED_PIN (85)
#define CSSM_WORDID_PUBLIC_KEY (86)
#define CSSM_WORDID_PUBLIC_KEY_FROM_CERT (87)
#define CSSM_WORDID_Q (88)
#define CSSM_WORDID_RANGE (89)
#define CSSM_WORDID_REVAL (90)
#define CSSM_WORDID_RIPEMAC (91)
#define CSSM_WORDID_RIPEMD (92)
#define CSSM_WORDID_RIPEMD160 (93)
#define CSSM_WORDID_RSA (94)
#define CSSM_WORDID_RSA_ISO9796 (95)
#define CSSM_WORDID_RSA_PKCS (96)
#define CSSM_WORDID_RSA_PKCS_MD5 (97)
#define CSSM_WORDID_RSA_PKCS_SHA1 (98)
#define CSSM_WORDID_RSA_PKCS1 (99)
#define CSSM_WORDID_RSA_PKCS1_MD5 (100)
#define CSSM_WORDID_RSA_PKCS1_SHA1 (101)
#define CSSM_WORDID_RSA_PKCS1_SIG (102)
#define CSSM_WORDID_RSA_RAW (103)
#define CSSM_WORDID_SDSIV1 (104)
#define CSSM_WORDID_SEQUENCE (105)
#define CSSM_WORDID_SET (106)
#define CSSM_WORDID_SEXPR (107)
#define CSSM_WORDID_SHA1 (108)
#define CSSM_WORDID_SHA1WITHDSA (109)
#define CSSM_WORDID_SHA1WITHECDSA (110)
#define CSSM_WORDID_SHA1WITHRSA (111)
#define CSSM_WORDID_SIGN (112)
#define CSSM_WORDID_SIGNATURE (113)
#define CSSM_WORDID_SIGNED_NONCE (114)
#define CSSM_WORDID_SIGNED_SECRET (115)
#define CSSM_WORDID_SPKI (116)
#define CSSM_WORDID_SUBJECT (117)
#define CSSM_WORDID_SUBJECT_INFO (118)
#define CSSM_WORDID_TAG (119)
#define CSSM_WORDID_THRESHOLD (120)
#define CSSM_WORDID_TIME (121)
#define CSSM_WORDID_URI (122)
#define CSSM_WORDID_VERSION (123)
#define CSSM_WORDID_X509_ATTRIBUTE (124)
#define CSSM_WORDID_X509V1 (125)
#define CSSM_WORDID_X509V2 (126)
#define CSSM_WORDID_X509V3 (127)
#define CSSM_WORDID_X9_ATTRIBUTE (128)
#define CSSM_WORDID_VENDOR_START (0x00010000)
#define CSSM_WORDID_VENDOR_END (0xFFFF0000)

/*
    CSSM_LIST_ELEMENT_TYPE
    This data element defines the type of a list element. There are only
    two possibilities: a byte string or a sub-list.
*/
typedef uint32 CSSM_LIST_ELEMENT_TYPE, *CSSM_LIST_ELEMENT_TYPE_PTR;
#define CSSM_LIST_ELEMENT_DATUM (0x00)
#define CSSM_LIST_ELEMENT_SUBLIST (0x01)
#define CSSM_LIST_ELEMENT_WORDID (0x02)

/*
    CSSM_LIST_TYPE
    This extensible list defines the type of linked lists used to contain
    the parsed elements of a certificate.
*/
typedef uint32 CSSM_LIST_TYPE, * CSSM_LIST_TYPE_PTR;
#define CSSM_LIST_TYPE_UNKNOWN (0)
#define CSSM_LIST_TYPE_CUSTOM (1)
#define CSSM_LIST_TYPE_SEXPR (2)

/*
    CSSM_LIST
    This structure defines a linked list.
*/
typedef struct cssm_list_element *CSSM_LIST_ELEMENT_PTR;
typedef struct cssm_list 
{
    CSSM_LIST_TYPE ListType; /* type of this list */
    CSSM_LIST_ELEMENT_PTR Head; /* head of the list */
    CSSM_LIST_ELEMENT_PTR Tail; /* tail of the list */
} CSSM_LIST, *CSSM_LIST_PTR;

/*
    CSSM_LIST_ELEMENT
    This structure defines a single element in a linked list.
*/
typedef struct cssm_list_element 
{
    struct cssm_list_element *NextElement; /* next list element */
    CSSM_WORDID_TYPE WordID; /* integer identifier associated with a Word value */
    CSSM_LIST_ELEMENT_TYPE ElementType;
    union 
    {
        CSSM_LIST Sublist; /* sublist */
        CSSM_DATA Word; /* a byte-string */
    } Element;
} CSSM_LIST_ELEMENT;

/*
    CSSM_TUPLE
    This structure defines a full 5-tuple, representing one certificate (or
    a fragment of a more complex certificate).
*/
typedef struct 
{ /* 5-tuple definition */
    CSSM_LIST Issuer; /* issuer, or empty if ACL */
    CSSM_LIST Subject; /* subject */
    CSSM_BOOL Delegate; /* permission to delegate */
    CSSM_LIST AuthorizationTag; /* authorization field */
    CSSM_LIST ValidityPeriod; /* validity information (dates) */
} CSSM_TUPLE, *CSSM_TUPLE_PTR;

/*
    CSSM_TUPLEGROUP
    This data structure contains a set of CSSM_TUPLE structures. The tuples
    are grouped from the purpose of input-to or output-from a function or service.
*/
typedef struct cssm_tuplegroup 
{
    uint32 NumberOfTuples;
    CSSM_TUPLE_PTR Tuples;
} CSSM_TUPLEGROUP, *CSSM_TUPLEGROUP_PTR;

/*
    CSSM_SAMPLE_TYPE
    This data type defines integer values identifying the types of samples a caller
    can present to a service provider.
*/
typedef CSSM_WORDID_TYPE CSSM_SAMPLE_TYPE;
#define CSSM_SAMPLE_TYPE_PASSWORD CSSM_WORDID_PASSWORD
#define CSSM_SAMPLE_TYPE_PROTECTED_PASSWORD CSSM_WORDID_PROTECTED_PASSWORD
#define CSSM_SAMPLE_TYPE_PROMPTED_PASSWORD CSSM_WORDID_PROMPTED_PASSWORD
#define CSSM_SAMPLE_TYPE_SIGNED_NONCE CSSM_WORDID_SIGNED_NONCE
#define CSSM_SAMPLE_TYPE_SIGNED_SECRET CSSM_WORDID_SIGNED_SECRET
#define CSSM_SAMPLE_TYPE_BIOMETRIC CSSM_WORDID_BIOMETRIC
#define CSSM_SAMPLE_TYPE_PROTECTED_BIOMETRIC CSSM_WORDID_PROTECTED_BIOMETRIC
#define CSSM_SAMPLE_TYPE_THRESHOLD CSSM_WORDID_THRESHOLD

/*
    CSSM_SAMPLE
    This structure contains a typed sample and a Globally Unique ID identifying a
    service provider module that can verify, authenticate, and process the sample value.
*/
typedef struct cssm_sample 
{
    CSSM_LIST TypedSample;
    const CSSM_SUBSERVICE_UID *Verifier;
} CSSM_SAMPLE, *CSSM_SAMPLE_PTR;

/*
    CSSM_SAMPLEGROUP
    This structure contains a group of related samples.
*/
typedef struct cssm_samplegroup 
{
    uint32 NumberOfSamples;
    const CSSM_SAMPLE *Samples;
} CSSM_SAMPLEGROUP, *CSSM_SAMPLEGROUP_PTR;


/*
    CSSM_MEMORY_FUNCS and CSSM_API_MEMORY_FUNCS
    This structure is used by applications to supply memory functions for the
    CSSM and the security service modules.
*/

/* CSSM_MEMTRACK_ON defined if doing memtracking: i.e., pass __FILE__ and __LINE__ */
/* as the last two parameters into the memory functions */

#ifndef CSSM_MEMTRACK_ON
typedef void * (CSSMAPI *CSSM_MALLOC) (uint32 size, void * allocref);
typedef void (CSSMAPI *CSSM_FREE) (void * memblock, void * allocref);
typedef void * (CSSMAPI *CSSM_REALLOC)(void * memblock, uint32 size, void * allocref);
typedef void * (CSSMAPI *CSSM_CALLOC)(uint32 num, uint32 size, void * allocref);
#else
typedef void * (CSSMAPI *CSSM_MALLOC) ( uint32 size,
                                        void * allocref,
                                        const char * File,
                                        uint32 Line);

typedef void (CSSMAPI *CSSM_FREE) (     void * memblock,
                                        void * allocref,
                                        const char * File,
                                        uint32 Line);

typedef void * (CSSMAPI *CSSM_REALLOC)( void * memblock,
                                        uint32 size,
                                        void * allocref,
                                        const char * File,
                                        uint32 Line);

typedef void * (CSSMAPI *CSSM_CALLOC)(  uint32 num,
                                        uint32 size,
                                        void * allocref,
                                        const char * File,
                                        uint32 Line);
#endif

typedef struct cssm_memory_funcs 
{
    CSSM_MALLOC malloc_func;
    CSSM_FREE free_func;
    CSSM_REALLOC realloc_func;
    CSSM_CALLOC calloc_func;
    void *AllocRef;
} CSSM_MEMORY_FUNCS, *CSSM_MEMORY_FUNCS_PTR;

typedef CSSM_MEMORY_FUNCS CSSM_API_MEMORY_FUNCS;
typedef CSSM_API_MEMORY_FUNCS *CSSM_API_MEMORY_FUNCS_PTR;


/*
    CSSM_CHALLENGE_CALLBACK
    This type defines the form of the callback function a service provider
    module must use to challenge a requester to acquire samples during an
    authentication and authorization verification procedure.
*/
typedef CSSM_RETURN (CSSMAPI *CSSM_CHALLENGE_CALLBACK)
                    (const CSSM_LIST *Challenge,
                    CSSM_SAMPLEGROUP_PTR Response,
                    void *CallerCtx,
                    const CSSM_MEMORY_FUNCS *MemFuncs);


/*
    CSSM_CERT_TYPE
    This variable specifies the type of certificate format supported by a
    certificate library.
*/
typedef enum cssm_cert_type 
{
    CSSM_CERT_UNKNOWN = 0x00,
    CSSM_CERT_X_509v1 = 0x01,
    CSSM_CERT_X_509v2 = 0x02,
    CSSM_CERT_X_509v3 = 0x03,
    CSSM_CERT_PGP = 0x04,
    CSSM_CERT_SPKI = 0x05,
    CSSM_CERT_SDSIv1 = 0x06,
    CSSM_CERT_Intel = 0x08,
    CSSM_CERT_X_509_ATTRIBUTE = 0x09, /* X.509 attribute cert */
    CSSM_CERT_X9_ATTRIBUTE = 0x0A, /* X9 attribute cert */
    CSSM_CERT_TUPLE = 0x0B,
    CSSM_CERT_ACL_ENTRY = 0x0C,
    CSSM_CERT_MULTIPLE = 0x7FFE,
    CSSM_CERT_LAST = 0x7FFF
} CSSM_CERT_TYPE, *CSSM_CERT_TYPE_PTR;

/* 
    Applications wishing to define their own custom certificate
    type should define and publicly document a uint32 value greater
    than the CSSM_CL_CUSTOM_CERT_TYPE 
*/
#define CSSM_CL_CUSTOM_CERT_TYPE 0x08000

/*
    CSSM_CERT_ENCODING
    This variable specifies the certificate encoding format supported by
    a certificate library.
*/
typedef enum cssm_cert_encoding 
{
    CSSM_CERT_ENCODING_UNKNOWN = 0x00,
    CSSM_CERT_ENCODING_CUSTOM = 0x01,
    CSSM_CERT_ENCODING_BER = 0x02,
    CSSM_CERT_ENCODING_DER = 0x03,
    CSSM_CERT_ENCODING_NDR = 0x04,
    CSSM_CERT_ENCODING_SEXPR = 0x05,
    CSSM_CERT_ENCODING_PGP = 0x06,
    CSSM_CERT_ENCODING_MULTIPLE = 0x7FFE,
    CSSM_CERT_ENCODING_LAST = 0x7FFF
} CSSM_CERT_ENCODING, *CSSM_CERT_ENCODING_PTR;
/* 
    Applications wishing to define their own custom certificate
    encoding should create a uint32 value greater than the
    CSSM_CL_CUSTOM_CERT_ENCODING 
*/
#define CSSM_CL_CUSTOM_CERT_ENCODING 0x8000

/*
    CSSM_ENCODED_CERT
    This structure contains a pointer to a certificate in its encoded
    representation.
*/
typedef struct cssm_encoded_cert 
{
    CSSM_CERT_TYPE CertType ; /* type of certificate */
    CSSM_CERT_ENCODING CertEncoding ; /* encoding for this packed cert */
    CSSM_DATA CertBlob ; /* packed cert */
} CSSM_ENCODED_CERT, *CSSM_ENCODED_CERT_PTR ;

/*
    CSSM_CERT_PARSE_FORMAT
    This type defines an extensible list of formats for parsed certificates.
*/
typedef uint32 CSSM_CERT_PARSE_FORMAT, *CSSM_CERT_PARSE_FORMAT_PTR;
#define CSSM_CERT_PARSE_FORMAT_NONE (0x00)
#define CSSM_CERT_PARSE_FORMAT_CUSTOM (0x01)     /* void* */
#define CSSM_CERT_PARSE_FORMAT_SEXPR (0x02)      /* CSSM_LIST */
#define CSSM_CERT_PARSE_FORMAT_COMPLEX (0x03)    /* void* */
#define CSSM_CERT_PARSE_FORMAT_OID_NAMED (0x04)  /* CSSM_FIELDGROUP */
#define CSSM_CERT_PARSE_FORMAT_TUPLE (0x05)      /* CSSM_TUPLE */
#define CSSM_CERT_PARSE_FORMAT_MULTIPLE (0x7FFE) /* multiple forms, each cert carries */
                                                 /* a parse format indicator          */
#define CSSM_CERT_PARSE_FORMAT_LAST (0x7FFF)
/* 
    Applications wishing to define their own custom parse
    format should create a * uint32 value greater than the
    CSSM_CL_CUSTOM_CERT_PARSE_FORMAT 
*/
#define CSSM_CL_CUSTOM_CERT_PARSE_FORMAT (0x8000)

/*
    CSSM_PARSED_CERT
    This structure contains a parsed representation of a certificate.
*/
typedef struct cssm_parsed_cert 
{
    CSSM_CERT_TYPE CertType ; /* certificate type */
    CSSM_CERT_PARSE_FORMAT ParsedCertFormat ; /* struct of ParsedCert */
    void *ParsedCert ; /* parsed cert (to be typecast) */
} CSSM_PARSED_CERT, *CSSM_PARSED_CERT_PTR ;

/*
    CSSM_CERTPAIR
    This structure contains a certificate in two representations:
        A CSSM_ENCODED_CERT structure containing a single,
                            opaque blob of certificate data;
        A CSSM_PARSED_CERT structure containing a parsed,
                           structured set of certificate data components.
*/
typedef struct cssm_cert_pair 
{
    CSSM_ENCODED_CERT EncodedCert; /* an encoded certificate blob */
    CSSM_PARSED_CERT ParsedCert;   /* equivalent parsed certificate */
} CSSM_CERT_PAIR, *CSSM_CERT_PAIR_PTR;

/*
    CSSM_CERTGROUP_TYPE
    This extensible list defines the type of a certificate group.
*/
typedef uint32 CSSM_CERTGROUP_TYPE, *CSSM_CERTGROUP_TYPE_PTR;
#define CSSM_CERTGROUP_DATA (0x00)
#define CSSM_CERTGROUP_ENCODED_CERT (0x01)
#define CSSM_CERTGROUP_PARSED_CERT (0x02)
#define CSSM_CERTGROUP_CERT_PAIR (0x03)

/*
    CSSM_CERTGROUP
    This structure contains an arbitrary number of certificates.
*/
typedef struct cssm_certgroup 
{
    CSSM_CERT_TYPE CertType;
    CSSM_CERT_ENCODING CertEncoding;
    uint32 NumCerts ; /* # of certificates in this list */
    union 
    {
        CSSM_DATA_PTR CertList;        /* legacy list of single type certificate blobs */
        CSSM_ENCODED_CERT_PTR EncodedCertList; /* list of multi-type certificate blobs */
        CSSM_PARSED_CERT_PTR ParsedCertList;   /* list of multi-type parsed certs */
        CSSM_CERT_PAIR_PTR PairCertList;       /* list of single or multi-type certs with
                                                  two representations: blob and parsed */
    } GroupList;
    CSSM_CERTGROUP_TYPE CertGroupType;  /* type of structure in the GroupList */
    void *Reserved ; /* reserved for implementation dependent use */
} CSSM_CERTGROUP, *CSSM_CERTGROUP_PTR;

/*
    CSSM_BASE_CERTS
    This structure contains a group of zero or more certificates and optional
    handles identifying a Trust Policy Service Provider and a Certificate
    Library Service Provider that could be used to verify these certificates.
*/
typedef struct cssm_base_certs 
{
    CSSM_TP_HANDLE TPHandle;
    CSSM_CL_HANDLE CLHandle;
    CSSM_CERTGROUP Certs;
} CSSM_BASE_CERTS, *CSSM_BASE_CERTS_PTR;

/*
    CSSM_ACCESS_CREDENTIALS
    This data structure contains the set of credentials a caller must provide
    when initiating a request for authorized access to a resource managed by
    a service provider module.
*/
typedef struct cssm_access_credentials 
{
    CSSM_STRING EntryTag;
    CSSM_BASE_CERTS BaseCerts;
    CSSM_SAMPLEGROUP Samples;
    CSSM_CHALLENGE_CALLBACK Callback;
    void *CallerCtx;
} CSSM_ACCESS_CREDENTIALS, *CSSM_ACCESS_CREDENTIALS_PTR;

/*
    CSSM_ACL_SUBJECT_TYPE
    This type defines symbol names for the valid subject types contained
    in an ACL entry.
*/
typedef sint32 CSSM_ACL_SUBJECT_TYPE;
#define CSSM_ACL_SUBJECT_TYPE_ANY CSSM_WORDID__STAR_
#define CSSM_ACL_SUBJECT_TYPE_THRESHOLD CSSM_WORDID_THRESHOLD
#define CSSM_ACL_SUBJECT_TYPE_PASSWORD CSSM_WORDID_PASSWORD
#define CSSM_ACL_SUBJECT_TYPE_PROTECTED_PASSWORD CSSM_WORDID_PROTECTED_PASSWORD
#define CSSM_ACL_SUBJECT_TYPE_PROMPTED_PASSWORD CSSM_WORDID_PROMPTED_PASSWORD
#define CSSM_ACL_SUBJECT_TYPE_PUBLIC_KEY CSSM_WORDID_PUBLIC_KEY
#define CSSM_ACL_SUBJECT_TYPE_HASHED_SUBJECT CSSM_WORDID_HASHED_SUBJECT
#define CSSM_ACL_SUBJECT_TYPE_BIOMETRIC CSSM_WORDID_BIOMETRIC
#define CSSM_ACL_SUBJECT_TYPE_PROTECTED_BIOMETRIC CSSM_WORDID_PROTECTED_BIOMETRIC
#define CSSM_ACL_SUBJECT_TYPE_LOGIN_NAME CSSM_WORDID_LOGIN_NAME
#define CSSM_ACL_SUBJECT_TYPE_EXT_PAM_NAME CSSM_WORDID_PAM_NAME

/*
    CSSM_ACL_AUTHORIZATION_TAG
    This type defines a set of names for operations that can be performed on
    ACL entries and on the resource objects that are protected by ACL entries.
*/

/* Authorization tag type */
typedef sint32 CSSM_ACL_AUTHORIZATION_TAG;

/* 
    All vendor specific constants must be in the number range
    starting at CSSM_ACL_AUTHORIZATION_TAG_VENDOR_DEFINED_START
*/
#define CSSM_ACL_AUTHORIZATION_TAG_VENDOR_DEFINED_START (0x00010000)

/* 
    No restrictions. Permission to perform all operations on
    the resource or available to an ACL owner.
*/
#define CSSM_ACL_AUTHORIZATION_ANY CSSM_WORDID__STAR_

/* Defined authorization tag values for CSPs */
#define CSSM_ACL_AUTHORIZATION_LOGIN CSSM_WORDID_LOGIN
#define CSSM_ACL_AUTHORIZATION_GENKEY CSSM_WORDID_GENKEY
#define CSSM_ACL_AUTHORIZATION_DELETE CSSM_WORDID_DELETE
#define CSSM_ACL_AUTHORIZATION_EXPORT_WRAPPED CSSM_WORDID_EXPORT_WRAPPED
#define CSSM_ACL_AUTHORIZATION_EXPORT_CLEAR CSSM_WORDID_EXPORT_CLEAR
#define CSSM_ACL_AUTHORIZATION_IMPORT_WRAPPED CSSM_WORDID_IMPORT_WRAPPED
#define CSSM_ACL_AUTHORIZATION_IMPORT_CLEAR CSSM_WORDID_IMPORT_CLEAR
#define CSSM_ACL_AUTHORIZATION_SIGN CSSM_WORDID_SIGN
#define CSSM_ACL_AUTHORIZATION_ENCRYPT CSSM_WORDID_ENCRYPT
#define CSSM_ACL_AUTHORIZATION_DECRYPT CSSM_WORDID_DECRYPT
#define CSSM_ACL_AUTHORIZATION_MAC CSSM_WORDID_MAC
#define CSSM_ACL_AUTHORIZATION_DERIVE CSSM_WORDID_DERIVE
/* Defined authorization tag values for DLs */
#define CSSM_ACL_AUTHORIZATION_DBS_CREATE CSSM_WORDID_DBS_CREATE
#define CSSM_ACL_AUTHORIZATION_DBS_DELETE CSSM_WORDID_DBS_DELETE
#define CSSM_ACL_AUTHORIZATION_DB_READ CSSM_WORDID_DB_READ
#define CSSM_ACL_AUTHORIZATION_DB_INSERT CSSM_WORDID_DB_INSERT
#define CSSM_ACL_AUTHORIZATION_DB_MODIFY CSSM_WORDID_DB_MODIFY
#define CSSM_ACL_AUTHORIZATION_DB_DELETE CSSM_WORDID_DB_DELETE

/*
    CSSM_AUTHORIZATIONGROUP
    This structure contains a group of authorization tags.
*/
typedef struct cssm_authorizationgroup 
{
    uint32 NumberOfAuthTags;
    CSSM_ACL_AUTHORIZATION_TAG *AuthTags;
} CSSM_AUTHORIZATIONGROUP, *CSSM_AUTHORIZATIONGROUP_PTR;

/*
    CSSM_ACL_VALIDITY_PERIOD
    This data type defines a structure containing a start date and end date
    for the validity of an ACL entry. The date and time for an ACL entry, a
    CSSM_TUPLE or an AuthCompute call is an ASCII string of the form, for example,
    "1999-06-30_15:05:39". Dates are compared by normal string comparison.
*/
typedef struct cssm_acl_validity_period 
{
    CSSM_DATA StartDate;
    CSSM_DATA EndDate;
} CSSM_ACL_VALIDITY_PERIOD, *CSSM_ACL_VALIDITY_PERIOD_PTR;

/*
    CSSM_ACL_ENTRY_PROTOTYPE
    This data type defines the abstract structure of an Access Control List (ACL) entry.
*/
typedef struct cssm_acl_entry_prototype 
{
    CSSM_LIST TypedSubject;
    CSSM_BOOL Delegate;
    CSSM_AUTHORIZATIONGROUP Authorization;
    CSSM_ACL_VALIDITY_PERIOD TimeRange;
    CSSM_STRING EntryTag;
} CSSM_ACL_ENTRY_PROTOTYPE, *CSSM_ACL_ENTRY_PROTOTYPE_PTR;

/*
    CSSM_ACL_OWNER_PROTOTYPE
    This data type is the abstract definition of an Access Control List (ACL)
    Owner (that is, the owner of the resource protected by the ACL).
*/
typedef struct cssm_acl_owner_prototype 
{
    CSSM_LIST TypedSubject;
    CSSM_BOOL Delegate;
} CSSM_ACL_OWNER_PROTOTYPE, *CSSM_ACL_OWNER_PROTOTYPE_PTR;

/*
    CSSM_ACL_SUBJECT_CALLBACK
    This type defines the form of the callback function a service provider
    module can use to acquire a value for the subject of a prototype ACL entry.
    The service provider initializes the SubjectRequest and the Application
    initializes the SubjectResponse based on the type of the request.
*/
typedef CSSM_RETURN (CSSMAPI * CSSM_ACL_SUBJECT_CALLBACK)
                    (const CSSM_LIST *SubjectRequest,
                    CSSM_LIST_PTR SubjectResponse,
                    void *CallerContext,
                    const CSSM_MEMORY_FUNCS *MemFuncs);

/*
    CSSM_ACL_ENTRY_INPUT
    This data structure is used by a caller to provide an ACL entry prototype
    to a service provider. An ACL entry prototype must be provided as input
    to a service provider when a new controlled resource is being created and
    when a new ACL entry is being added to an existing access-controlled resource.
*/
typedef struct cssm_acl_entry_input 
{
    CSSM_ACL_ENTRY_PROTOTYPE Prototype;
    CSSM_ACL_SUBJECT_CALLBACK Callback;
    void *CallerContext;
} CSSM_ACL_ENTRY_INPUT, *CSSM_ACL_ENTRY_INPUT_PTR;

/*
    CSSM_RESOURCE_CONTROL_CONTEXT
    This data structure is used by a caller when creating a new resource. The
    caller can provide AccessCredentials as evidence that the caller is allowed
    to create a new resource. The caller can also provide an ACL entry prototype
    to be used by the service provider in constructing theinitial ACL entry
    controlling access to the new resource.
*/
typedef struct cssm_resource_control_context 
{
    CSSM_ACCESS_CREDENTIALS_PTR AccessCred;
    CSSM_ACL_ENTRY_INPUT InitialAclEntry;
} CSSM_RESOURCE_CONTROL_CONTEXT, *CSSM_RESOURCE_CONTROL_CONTEXT_PTR;

/*
    CSSM_ACL_HANDLE
    This data type defines an opaque handle that uniquely identifies a single
    ACL entry associated with a particular resource.
*/
typedef CSSM_HANDLE CSSM_ACL_HANDLE;

/*
    CSSM_ACL_ENTRY_INFO
    This data structure is used to return ACL entry information from a service
    provider module to a caller.
*/
typedef struct cssm_acl_entry_info 
{
    CSSM_ACL_ENTRY_PROTOTYPE EntryPublicInfo;
    CSSM_ACL_HANDLE EntryHandle;
} CSSM_ACL_ENTRY_INFO, *CSSM_ACL_ENTRY_INFO_PTR;

/*
    CSSM_ACL_EDIT_MODE
    This data type defines identifiers for operations that modify an existing
    access control list (ACL).
*/
typedef uint32 CSSM_ACL_EDIT_MODE;
#define CSSM_ACL_EDIT_MODE_ADD (1)
#define CSSM_ACL_EDIT_MODE_DELETE (2)
#define CSSM_ACL_EDIT_MODE_REPLACE (3)

/*
    CSSM_ACL_EDIT
    This data structure contains the description of an edit operation to be
    applied to an existing Access Control List (ACL).
*/
typedef struct cssm_acl_edit 
{
    CSSM_ACL_EDIT_MODE EditMode;
    CSSM_ACL_HANDLE OldEntryHandle;
    const CSSM_ACL_ENTRY_INPUT *NewEntry;
} CSSM_ACL_EDIT, *CSSM_ACL_EDIT_PTR;

/*
    CSSM_PROC_ADDR
    Generic pointer to a CSSM function.
*/
#if defined (WIN32)
    typedef FARPROC CSSM_PROC_ADDR;
#else
    typedef void (CSSMAPI *CSSM_PROC_ADDR) ();
#endif
typedef CSSM_PROC_ADDR *CSSM_PROC_ADDR_PTR;

/*
    CSSM_KR_POLICY_TYPE
    Support for the KRMM relation.
*/
typedef uint32 CSSM_KR_POLICY_TYPE;
#define CSSM_KR_INDIV_POLICY (0x00000001)
#define CSSM_KR_ENT_POLICY (0x00000002)
#define CSSM_KR_LE_MAN_POLICY (0x00000003)
#define CSSM_KR_LE_USE_POLICY (0x00000004)


/*
    CSSM_FUNC_NAME_ADDR
    This structure binds a function to the runtime address of the procedure
    that implements the named function. Function names are limited in length
    to the size of a CSSM_STRING.
*/
typedef struct cssm_func_name_addr 
{
    CSSM_STRING Name;
    CSSM_PROC_ADDR Address;
}CSSM_FUNC_NAME_ADDR, *CSSM_FUNC_NAME_ADDR_PTR;

/*****************************************************************************
    Cryto Services API Datatypes
*****************************************************************************/
/*
    CSSM_CC_HANDLE
*/
typedef CSSM_LONG_HANDLE CSSM_CC_HANDLE; /* Cryptographic Context Handle */

/* 
    CSSM_CSP_HANDLE defined above under Core Services API Datatypes
*/

/*
    CSSM_DATE
*/
typedef struct cssm_date 
{
    uint8 Year[4];
    uint8 Month[2];
    uint8 Day[2];
} CSSM_DATE, *CSSM_DATE_PTR;

/*
    CSSM_RANGE
*/
typedef struct cssm_range 
{
    uint32 Min; /* inclusive minimum value */
    uint32 Max; /* inclusive maximum value */
} CSSM_RANGE, *CSSM_RANGE_PTR;

/*
    CSSM_QUERY_SIZE_DATA
*/
typedef struct cssm_query_size_data 
{
    uint32 SizeInputBlock; /* size of input data block */
    uint32 SizeOutputBlock; /* size of resulting output data block */
} CSSM_QUERY_SIZE_DATA, *CSSM_QUERY_SIZE_DATA_PTR;

/*
    CSSM_HEADERVERSION
    Represents the version number of a key header structure.
*/
typedef uint32 CSSM_HEADERVERSION;
#define CSSM_KEYHEADER_VERSION (2)

/*
    CSSM_KEY_SIZE
    This structure holds the key size and the effective key size for a given key.
*/
typedef struct cssm_key_size 
{
    uint32 LogicalKeySizeInBits; /* Logical key size in bits */
    uint32 EffectiveKeySizeInBits; /* Effective key size in bits */
} CSSM_KEY_SIZE, *CSSM_KEY_SIZE_PTR;

/*
    CSSM_KEYBLOB_TYPE
*/
typedef uint32 CSSM_KEYBLOB_TYPE;
#define CSSM_KEYBLOB_RAW (0) /* The blob is a clear, raw key */
#define CSSM_KEYBLOB_REFERENCE (1) /* The blob is a reference to a key */
#define CSSM_KEYBLOB_WRAPPED (2) /* The blob is a wrapped RAW key */
#define CSSM_KEYBLOB_OTHER (0xFFFFFFFF)

/*
    CSSM_KEYBLOB_FORMAT
*/
typedef uint32 CSSM_KEYBLOB_FORMAT;
/* Raw Format */
#define CSSM_KEYBLOB_RAW_FORMAT_NONE (0) /* No further conversion need to be done */
#define CSSM_KEYBLOB_RAW_FORMAT_PKCS1 (1)   /* RSA PKCS1 V1.5 */
#define CSSM_KEYBLOB_RAW_FORMAT_PKCS3 (2)   /* RSA PKCS3 V1.5 */
#define CSSM_KEYBLOB_RAW_FORMAT_MSCAPI (3)  /* Microsoft CAPI V2.0 */
#define CSSM_KEYBLOB_RAW_FORMAT_PGP (4)     /* PGP V */
#define CSSM_KEYBLOB_RAW_FORMAT_FIPS186 (5) /* US Gov. FIPS 186 - DSS V */
#define CSSM_KEYBLOB_RAW_FORMAT_BSAFE (6)   /* RSA Bsafe V3.0 */
#define CSSM_KEYBLOB_RAW_FORMAT_CCA (9)     /* CCA clear public key blob */
#define CSSM_KEYBLOB_RAW_FORMAT_PKCS8 (10)  /* RSA PKCS8 V1.2 */
#define CSSM_KEYBLOB_RAW_FORMAT_SPKI (11)   /* SPKI Specification */
#define CSSM_KEYBLOB_RAW_FORMAT_OCTET_STRING (12)
#define CSSM_KEYBLOB_RAW_FORMAT_OTHER (0xFFFFFFFF) /* Other, CSP defined */
/* Wrapped Format */
#define CSSM_KEYBLOB_WRAPPED_FORMAT_NONE (0) /* No further conversion need to be done */
#define CSSM_KEYBLOB_WRAPPED_FORMAT_PKCS8 (1) /* RSA PKCS8 V1.2 */
#define CSSM_KEYBLOB_WRAPPED_FORMAT_PKCS7 (2)
#define CSSM_KEYBLOB_WRAPPED_FORMAT_MSCAPI (3)
#define CSSM_KEYBLOB_WRAPPED_FORMAT_OTHER (0xFFFFFFFF) /* Other, CSP defined */
/* Reference Format */
#define CSSM_KEYBLOB_REF_FORMAT_INTEGER (0) /*Reference is a number or handle*/
#define CSSM_KEYBLOB_REF_FORMAT_STRING (1)  /* Reference is a string or label */
#define CSSM_KEYBLOB_REF_FORMAT_SPKI (2)    /* Reference is an SPKI S-expression 
                                               to be evaluated to locate the key */
#define CSSM_KEYBLOB_REF_FORMAT_UNIQUE_ID (3) /* A unique ID for the key relative to 
                                            the data base in which it is created */
#define CSSM_KEYBLOB_REF_FORMAT_OTHER (0xFFFFFFFF) /* Other, CSP defined */

/*
    CSSM_KEYCLASS
*/
typedef uint32 CSSM_KEYCLASS;
#define CSSM_KEYCLASS_PUBLIC_KEY (0)     /* Key is public key */
#define CSSM_KEYCLASS_PRIVATE_KEY (1)    /* Key is private key */
#define CSSM_KEYCLASS_SESSION_KEY (2)    /* Key is session or symmetric key */
#define CSSM_KEYCLASS_SECRET_PART (3)    /* Key is part of secret key */
#define CSSM_KEYCLASS_OTHER (0xFFFFFFFF) /* Other */

/*
    CSSM_KEYATTR_FLAGS
*/
typedef uint32 CSSM_KEYATTR_FLAGS;

/* Valid only during call to an API. Will never be valid when set in a key header */
#define CSSM_KEYATTR_RETURN_DEFAULT (0x00000000)
#define CSSM_KEYATTR_RETURN_DATA (0x10000000)
#define CSSM_KEYATTR_RETURN_REF (0x20000000)
#define CSSM_KEYATTR_RETURN_NONE (0x40000000)

/* Valid during an API call and in a key header */
#define CSSM_KEYATTR_PERMANENT (0x00000001)
#define CSSM_KEYATTR_PRIVATE (0x00000002)
#define CSSM_KEYATTR_MODIFIABLE (0x00000004)
#define CSSM_KEYATTR_SENSITIVE (0x00000008)
#define CSSM_KEYATTR_EXTRACTABLE (0x00000020)

/* Valid only in a key header generated by a CSP, not valid during an API call */
#define CSSM_KEYATTR_ALWAYS_SENSITIVE (0x00000010)
#define CSSM_KEYATTR_NEVER_EXTRACTABLE (0x00000040)

/*
    CSSM_KEYUSE
*/
typedef uint32 CSSM_KEYUSE;
#define CSSM_KEYUSE_ANY (0x80000000)
#define CSSM_KEYUSE_ENCRYPT (0x00000001)
#define CSSM_KEYUSE_DECRYPT (0x00000002)
#define CSSM_KEYUSE_SIGN (0x00000004)
#define CSSM_KEYUSE_VERIFY (0x00000008)
#define CSSM_KEYUSE_SIGN_RECOVER (0x00000010)
#define CSSM_KEYUSE_VERIFY_RECOVER (0x00000020)
#define CSSM_KEYUSE_WRAP (0x00000040)
#define CSSM_KEYUSE_UNWRAP (0x00000080)
#define CSSM_KEYUSE_DERIVE (0x00000100)


/*
    CSSM_CSP_TYPE
*/
typedef enum cssm_csptype 
{
    CSSM_CSP_SOFTWARE = 1,
    CSSM_CSP_HARDWARE = CSSM_CSP_SOFTWARE+1,
    CSSM_CSP_HYBRID = CSSM_CSP_SOFTWARE+2
}CSSM_CSPTYPE;

/*
    CSSM_CONTEXT_TYPE
    This type defines a set of constants used different classes of
    cryptographic algorithms. Each algorithm class corresponds to a
    context type. See the definition of CSSM_CONTEXT for a description
    of each algorithm class value.
*/
typedef uint32 CSSM_CONTEXT_TYPE;
#define CSSM_ALGCLASS_NONE (0)
#define CSSM_ALGCLASS_CUSTOM (CSSM_ALGCLASS_NONE+1)
#define CSSM_ALGCLASS_SIGNATURE (CSSM_ALGCLASS_NONE+2)
#define CSSM_ALGCLASS_SYMMETRIC (CSSM_ALGCLASS_NONE+3)
#define CSSM_ALGCLASS_DIGEST (CSSM_ALGCLASS_NONE+4)
#define CSSM_ALGCLASS_RANDOMGEN (CSSM_ALGCLASS_NONE+5)
#define CSSM_ALGCLASS_UNIQUEGEN (CSSM_ALGCLASS_NONE+6)
#define CSSM_ALGCLASS_MAC (CSSM_ALGCLASS_NONE+7)
#define CSSM_ALGCLASS_ASYMMETRIC (CSSM_ALGCLASS_NONE+8)
#define CSSM_ALGCLASS_KEYGEN (CSSM_ALGCLASS_NONE+9)
#define CSSM_ALGCLASS_DERIVEKEY (CSSM_ALGCLASS_NONE+10)

/*
    CSSM Algorithms
    This type defines a set of constants used to identify cryptographic
    algorithms. See the definition of CSSM_CONTEXT for a description of
    each algorithm value.
*/
typedef uint32 CSSM_ALGORITHMS;
#define CSSM_ALGID_NONE (0)
#define CSSM_ALGID_CUSTOM (CSSM_ALGID_NONE+1)
#define CSSM_ALGID_DH (CSSM_ALGID_NONE+2)
#define CSSM_ALGID_PH (CSSM_ALGID_NONE+3)
#define CSSM_ALGID_KEA (CSSM_ALGID_NONE+4)
#define CSSM_ALGID_MD2 (CSSM_ALGID_NONE+5)
#define CSSM_ALGID_MD4 (CSSM_ALGID_NONE+6)
#define CSSM_ALGID_MD5 (CSSM_ALGID_NONE+7)
#define CSSM_ALGID_SHA1 (CSSM_ALGID_NONE+8)
#define CSSM_ALGID_NHASH (CSSM_ALGID_NONE+9)
#define CSSM_ALGID_HAVAL (CSSM_ALGID_NONE+10)
#define CSSM_ALGID_RIPEMD (CSSM_ALGID_NONE+11)
#define CSSM_ALGID_IBCHASH (CSSM_ALGID_NONE+12)
#define CSSM_ALGID_RIPEMAC (CSSM_ALGID_NONE+13)
#define CSSM_ALGID_DES (CSSM_ALGID_NONE+14)
#define CSSM_ALGID_DESX (CSSM_ALGID_NONE+15)
#define CSSM_ALGID_RDES (CSSM_ALGID_NONE+16)
#define CSSM_ALGID_3DES_3KEY_EDE (CSSM_ALGID_NONE+17)
#define CSSM_ALGID_3DES_2KEY_EDE (CSSM_ALGID_NONE+18)
#define CSSM_ALGID_3DES_1KEY_EEE (CSSM_ALGID_NONE+19)
#define CSSM_ALGID_3DES_3KEY CSSM_ALGID_3DES_3KEY_EDE
#define CSSM_ALGID_3DES_3KEY_EEE (CSSM_ALGID_NONE+20)
#define CSSM_ALGID_3DES_2KEY CSSM_ALGID_3DES_2KEY_EDE
#define CSSM_ALGID_3DES_2KEY_EEE (CSSM_ALGID_NONE+21)
#define CSSM_ALGID_IDEA (CSSM_ALGID_NONE+22)
#define CSSM_ALGID_RC2 (CSSM_ALGID_NONE+23)
#define CSSM_ALGID_RC5 (CSSM_ALGID_NONE+24)
#define CSSM_ALGID_RC4 (CSSM_ALGID_NONE+25)
#define CSSM_ALGID_SEAL (CSSM_ALGID_NONE+26)
#define CSSM_ALGID_CAST (CSSM_ALGID_NONE+27)
#define CSSM_ALGID_BLOWFISH (CSSM_ALGID_NONE+28)
#define CSSM_ALGID_SKIPJACK (CSSM_ALGID_NONE+29)
#define CSSM_ALGID_LUCIFER (CSSM_ALGID_NONE+30)
#define CSSM_ALGID_MADRYGA (CSSM_ALGID_NONE+31)
#define CSSM_ALGID_FEAL (CSSM_ALGID_NONE+32)
#define CSSM_ALGID_REDOC (CSSM_ALGID_NONE+33)
#define CSSM_ALGID_REDOC3 (CSSM_ALGID_NONE+34)
#define CSSM_ALGID_LOKI (CSSM_ALGID_NONE+35)
#define CSSM_ALGID_KHUFU (CSSM_ALGID_NONE+36)
#define CSSM_ALGID_KHAFRE (CSSM_ALGID_NONE+37)
#define CSSM_ALGID_MMB (CSSM_ALGID_NONE+38)
#define CSSM_ALGID_GOST (CSSM_ALGID_NONE+39)
#define CSSM_ALGID_SAFER (CSSM_ALGID_NONE+40)
#define CSSM_ALGID_CRAB (CSSM_ALGID_NONE+41)
#define CSSM_ALGID_RSA (CSSM_ALGID_NONE+42)
#define CSSM_ALGID_DSA (CSSM_ALGID_NONE+43)
#define CSSM_ALGID_MD5WithRSA (CSSM_ALGID_NONE+44)
#define CSSM_ALGID_MD2WithRSA (CSSM_ALGID_NONE+45)
#define CSSM_ALGID_ElGamal (CSSM_ALGID_NONE+46)
#define CSSM_ALGID_MD2Random (CSSM_ALGID_NONE+47)
#define CSSM_ALGID_MD5Random (CSSM_ALGID_NONE+48)
#define CSSM_ALGID_SHARandom (CSSM_ALGID_NONE+49)
#define CSSM_ALGID_DESRandom (CSSM_ALGID_NONE+50)
#define CSSM_ALGID_SHA1WithRSA (CSSM_ALGID_NONE+51)
#define CSSM_ALGID_CDMF (CSSM_ALGID_NONE+52)
#define CSSM_ALGID_CAST3 (CSSM_ALGID_NONE+53)
#define CSSM_ALGID_CAST5 (CSSM_ALGID_NONE+54)
#define CSSM_ALGID_GenericSecret (CSSM_ALGID_NONE+55)
#define CSSM_ALGID_ConcatBaseAndKey (CSSM_ALGID_NONE+56)
#define CSSM_ALGID_ConcatKeyAndBase (CSSM_ALGID_NONE+57)
#define CSSM_ALGID_ConcatBaseAndData (CSSM_ALGID_NONE+58)
#define CSSM_ALGID_ConcatDataAndBase (CSSM_ALGID_NONE+59)
#define CSSM_ALGID_XORBaseAndData (CSSM_ALGID_NONE+60)
#define CSSM_ALGID_ExtractFromKey (CSSM_ALGID_NONE+61)
#define CSSM_ALGID_SSL3PreMasterGen (CSSM_ALGID_NONE+62)
#define CSSM_ALGID_SSL3MasterDerive (CSSM_ALGID_NONE+63)
#define CSSM_ALGID_SSL3KeyAndMacDerive (CSSM_ALGID_NONE+64)
#define CSSM_ALGID_SSL3MD5_MAC (CSSM_ALGID_NONE+65)
#define CSSM_ALGID_SSL3SHA1_MAC (CSSM_ALGID_NONE+66)
#define CSSM_ALGID_PKCS5_PBKDF1_MD5 (CSSM_ALGID_NONE+67)
#define CSSM_ALGID_PKCS5_PBKDF1_MD2 (CSSM_ALGID_NONE+68)
#define CSSM_ALGID_PKCS5_PBKDF1_SHA1 (CSSM_ALGID_NONE+69)
#define CSSM_ALGID_WrapLynks (CSSM_ALGID_NONE+70)
#define CSSM_ALGID_WrapSET_OAEP (CSSM_ALGID_NONE+71)
#define CSSM_ALGID_BATON (CSSM_ALGID_NONE+72)
#define CSSM_ALGID_ECDSA (CSSM_ALGID_NONE+73)
#define CSSM_ALGID_MAYFLY (CSSM_ALGID_NONE+74)
#define CSSM_ALGID_JUNIPER (CSSM_ALGID_NONE+75)
#define CSSM_ALGID_FASTHASH (CSSM_ALGID_NONE+76)
#define CSSM_ALGID_3DES (CSSM_ALGID_NONE+77)
#define CSSM_ALGID_SSL3MD5 (CSSM_ALGID_NONE+78)
#define CSSM_ALGID_SSL3SHA1 (CSSM_ALGID_NONE+79)
#define CSSM_ALGID_FortezzaTimestamp (CSSM_ALGID_NONE+80)
#define CSSM_ALGID_SHA1WithDSA (CSSM_ALGID_NONE+81)
#define CSSM_ALGID_SHA1WithECDSA (CSSM_ALGID_NONE+82)
#define CSSM_ALGID_DSA_BSAFE (CSSM_ALGID_NONE+83)
#define CSSM_ALGID_ECDH (CSSM_ALGID_NONE+84)
#define CSSM_ALGID_ECMQV (CSSM_ALGID_NONE+85)
#define CSSM_ALGID_PKCS12_SHA1_PBE (CSSM_ALGID_NONE+86)
#define CSSM_ALGID_ECNRA (CSSM_ALGID_NONE+87)
#define CSSM_ALGID_SHA1WithECNRA (CSSM_ALGID_NONE+88)
#define CSSM_ALGID_ECES (CSSM_ALGID_NONE+89)
#define CSSM_ALGID_ECAES (CSSM_ALGID_NONE+90)
#define CSSM_ALGID_SHA1HMAC (CSSM_ALGID_NONE+91)
#define CSSM_ALGID_FIPS186Random (CSSM_ALGID_NONE+92)
#define CSSM_ALGID_ECC (CSSM_ALGID_NONE+93)
#define CSSM_ALGID_MQV (CSSM_ALGID_NONE+94)
#define CSSM_ALGID_NRA (CSSM_ALGID_NONE+95)
#define CSSM_ALGID_IntelPlatformRandom (CSSM_ALGID_NONE+96)
#define CSSM_ALGID_UTC (CSSM_ALGID_NONE+97)
#define CSSM_ALGID_HAVAL3 (CSSM_ALGID_NONE+98)
#define CSSM_ALGID_HAVAL4 (CSSM_ALGID_NONE+99)
#define CSSM_ALGID_HAVAL5 (CSSM_ALGID_NONE+100)
#define CSSM_ALGID_TIGER (CSSM_ALGID_NONE+101)
#define CSSM_ALGID_MD5HMAC (CSSM_ALGID_NONE+102)
#define CSSM_ALGID_PKCS5_PBKDF2 (CSSM_ALGID_NONE+103)
#define CSSM_ALGID_RUNNING_COUNTER (CSSM_ALGID_NONE+104)
#define CSSM_ALGID_LAST (0x7FFFFFFF)

/*
    All algorithms IDs that are vendor specific, and not
    part of the CSSM specification should be defined relative
    to CSSM_ALGID_VENDOR_DEFINED.
*/
#define CSSM_ALGID_VENDOR_DEFINED (CSSM_ALGID_NONE+0x80000000)

/*
    CSSM_ATTRIBUTE_TYPE
    This type defines a set of constants used to identify the types of
    attributes store in a cryptographic context.
*/
/* Attribute data type tags */
#define CSSM_ATTRIBUTE_DATA_NONE (0x00000000)
#define CSSM_ATTRIBUTE_DATA_UINT32 (0x10000000)
#define CSSM_ATTRIBUTE_DATA_CSSM_DATA (0x20000000)
#define CSSM_ATTRIBUTE_DATA_CRYPTO_DATA (0x30000000)
#define CSSM_ATTRIBUTE_DATA_KEY (0x40000000)
#define CSSM_ATTRIBUTE_DATA_STRING (0x50000000)
#define CSSM_ATTRIBUTE_DATA_DATE (0x60000000)
#define CSSM_ATTRIBUTE_DATA_RANGE (0x70000000)
/*
 * The CSSM specification defines the value for 
 * CSSM_ATTRIBUTE_DATA_ACCESS_CREDENTIALS as 0x80000000. It is 
 * represented slightly differently to avoid warning about 
 * truncation on 64-bit architectures.
 *
 * This is a sneaky way of getting an int with a value of 0x8000000.
 * 0x80000000 by itself is actually a long on 64-bit architectures and
 * assigning it to an enumerator (which is of type int) generates a
 * warning about truncation.
 *
 */
#define CSSM_ATTRIBUTE_DATA_ACCESS_CREDENTIALS (-0x7fffffff-1)
#define CSSM_ATTRIBUTE_DATA_VERSION (0x01000000)
#define CSSM_ATTRIBUTE_DATA_DL_DB_HANDLE (0x02000000)
#define CSSM_ATTRIBUTE_TYPE_MASK (0xFF000000)


/*
    CSSM_ATTRIBUTE_TYPE Additions
    Several new attribute types were defined to support the key recovery
    context attributes. The following definitions are added to the enumerated
    type CSSM_ATTRIBUTE_TYPE:
*/
/* Attribute data type tags */
#define CSSM_ATTRIBUTE_DATA_KR_PROFILE 0x03000000



typedef enum cssm_attribute_type 
{
    CSSM_ATTRIBUTE_NONE = 0,
    CSSM_ATTRIBUTE_CUSTOM = CSSM_ATTRIBUTE_DATA_CSSM_DATA | 1,
    CSSM_ATTRIBUTE_DESCRIPTION = CSSM_ATTRIBUTE_DATA_STRING | 2,
    CSSM_ATTRIBUTE_KEY = CSSM_ATTRIBUTE_DATA_KEY | 3,
    CSSM_ATTRIBUTE_INIT_VECTOR = CSSM_ATTRIBUTE_DATA_CSSM_DATA | 4,
    CSSM_ATTRIBUTE_SALT = CSSM_ATTRIBUTE_DATA_CSSM_DATA | 5,
    CSSM_ATTRIBUTE_PADDING = CSSM_ATTRIBUTE_DATA_UINT32 | 6,
    CSSM_ATTRIBUTE_RANDOM = CSSM_ATTRIBUTE_DATA_CSSM_DATA | 7,
    CSSM_ATTRIBUTE_SEED = CSSM_ATTRIBUTE_DATA_CRYPTO_DATA | 8,
    CSSM_ATTRIBUTE_PASSPHRASE = CSSM_ATTRIBUTE_DATA_CRYPTO_DATA | 9,
    CSSM_ATTRIBUTE_KEY_LENGTH = CSSM_ATTRIBUTE_DATA_UINT32 | 10,
    CSSM_ATTRIBUTE_KEY_LENGTH_RANGE = CSSM_ATTRIBUTE_DATA_RANGE | 11,
    CSSM_ATTRIBUTE_BLOCK_SIZE = CSSM_ATTRIBUTE_DATA_UINT32 | 12,
    CSSM_ATTRIBUTE_OUTPUT_SIZE = CSSM_ATTRIBUTE_DATA_UINT32 | 13,
    CSSM_ATTRIBUTE_ROUNDS = CSSM_ATTRIBUTE_DATA_UINT32 | 14,
    CSSM_ATTRIBUTE_IV_SIZE = CSSM_ATTRIBUTE_DATA_UINT32 | 15,
    CSSM_ATTRIBUTE_ALG_PARAMS = CSSM_ATTRIBUTE_DATA_CSSM_DATA | 16,
    CSSM_ATTRIBUTE_LABEL = CSSM_ATTRIBUTE_DATA_CSSM_DATA | 17,
    CSSM_ATTRIBUTE_KEY_TYPE = CSSM_ATTRIBUTE_DATA_UINT32 | 18,
    CSSM_ATTRIBUTE_MODE = CSSM_ATTRIBUTE_DATA_UINT32 | 19,
    CSSM_ATTRIBUTE_EFFECTIVE_BITS = CSSM_ATTRIBUTE_DATA_UINT32 | 20,
    CSSM_ATTRIBUTE_START_DATE = CSSM_ATTRIBUTE_DATA_DATE | 21,
    CSSM_ATTRIBUTE_END_DATE = CSSM_ATTRIBUTE_DATA_DATE | 22,
    CSSM_ATTRIBUTE_KEYUSAGE = CSSM_ATTRIBUTE_DATA_UINT32 | 23,
    CSSM_ATTRIBUTE_KEYATTR = CSSM_ATTRIBUTE_DATA_UINT32 | 24,
    CSSM_ATTRIBUTE_VERSION = CSSM_ATTRIBUTE_DATA_VERSION | 25,
    CSSM_ATTRIBUTE_PRIME = CSSM_ATTRIBUTE_DATA_CSSM_DATA | 26,
    CSSM_ATTRIBUTE_BASE = CSSM_ATTRIBUTE_DATA_CSSM_DATA | 27,
    CSSM_ATTRIBUTE_SUBPRIME = CSSM_ATTRIBUTE_DATA_CSSM_DATA | 28,
    CSSM_ATTRIBUTE_ALG_ID = CSSM_ATTRIBUTE_DATA_UINT32 | 29,
    CSSM_ATTRIBUTE_ITERATION_COUNT = CSSM_ATTRIBUTE_DATA_UINT32 | 30,
    CSSM_ATTRIBUTE_ROUNDS_RANGE = CSSM_ATTRIBUTE_DATA_RANGE | 31,
    CSSM_ATTRIBUTE_CSP_HANDLE = CSSM_ATTRIBUTE_DATA_UINT32 | 34,
    CSSM_ATTRIBUTE_DL_DB_HANDLE = CSSM_ATTRIBUTE_DATA_DL_DB_HANDLE | 35,
    CSSM_ATTRIBUTE_ACCESS_CREDENTIALS = CSSM_ATTRIBUTE_DATA_ACCESS_CREDENTIALS | 36,
    CSSM_ATTRIBUTE_PUBLIC_KEY_FORMAT = CSSM_ATTRIBUTE_DATA_UINT32 | 37,
    CSSM_ATTRIBUTE_PRIVATE_KEY_FORMAT = CSSM_ATTRIBUTE_DATA_UINT32 | 38,
    CSSM_ATTRIBUTE_SYMMETRIC_KEY_FORMAT = CSSM_ATTRIBUTE_DATA_UINT32 | 39,
    CSSM_ATTRIBUTE_WRAPPED_KEY_FORMAT = CSSM_ATTRIBUTE_DATA_UINT32 | 40,
    /* local entity profile */
    CSSM_ATTRIBUTE_KRPROFILE_LOCAL = (CSSM_ATTRIBUTE_DATA_KR_PROFILE | 32),
    /* remote entity profile */
    CSSM_ATTRIBUTE_KRPROFILE_REMOTE = (CSSM_ATTRIBUTE_DATA_KR_PROFILE | 33)
} CSSM_ATTRIBUTE_TYPE;


/*
    CSSM_ENCRYPT_MODE
    This type defines a set of constants used to identify encryption modes
    used by different cryptographic algorithms. See the definition of
    CSSM_CONTEXT for a description of each encryption mode.
*/
typedef uint32 CSSM_ENCRYPT_MODE;
#define CSSM_ALGMODE_NONE (0)
#define CSSM_ALGMODE_CUSTOM (CSSM_ALGMODE_NONE+1)
#define CSSM_ALGMODE_ECB (CSSM_ALGMODE_NONE+2)
#define CSSM_ALGMODE_ECBPad (CSSM_ALGMODE_NONE+3)
#define CSSM_ALGMODE_CBC (CSSM_ALGMODE_NONE+4)
#define CSSM_ALGMODE_CBC_IV8 (CSSM_ALGMODE_NONE+5)
#define CSSM_ALGMODE_CBCPadIV8 (CSSM_ALGMODE_NONE+6)
#define CSSM_ALGMODE_CFB (CSSM_ALGMODE_NONE+7)
#define CSSM_ALGMODE_CFB_IV8 (CSSM_ALGMODE_NONE+8)
#define CSSM_ALGMODE_CFBPadIV8 (CSSM_ALGMODE_NONE+9)
#define CSSM_ALGMODE_OFB (CSSM_ALGMODE_NONE+10)
#define CSSM_ALGMODE_OFB_IV8 (CSSM_ALGMODE_NONE+11)
#define CSSM_ALGMODE_OFBPadIV8 (CSSM_ALGMODE_NONE+12)
#define CSSM_ALGMODE_COUNTER (CSSM_ALGMODE_NONE+13)
#define CSSM_ALGMODE_BC (CSSM_ALGMODE_NONE+14)
#define CSSM_ALGMODE_PCBC (CSSM_ALGMODE_NONE+15)
#define CSSM_ALGMODE_CBCC (CSSM_ALGMODE_NONE+16)
#define CSSM_ALGMODE_OFBNLF (CSSM_ALGMODE_NONE+17)
#define CSSM_ALGMODE_PBC (CSSM_ALGMODE_NONE+18)
#define CSSM_ALGMODE_PFB (CSSM_ALGMODE_NONE+19)
#define CSSM_ALGMODE_CBCPD (CSSM_ALGMODE_NONE+20)
#define CSSM_ALGMODE_PUBLIC_KEY (CSSM_ALGMODE_NONE+21)
#define CSSM_ALGMODE_PRIVATE_KEY (CSSM_ALGMODE_NONE+22)
#define CSSM_ALGMODE_SHUFFLE (CSSM_ALGMODE_NONE+23)
#define CSSM_ALGMODE_ECB64 (CSSM_ALGMODE_NONE+24)
#define CSSM_ALGMODE_CBC64 (CSSM_ALGMODE_NONE+25)
#define CSSM_ALGMODE_OFB64 (CSSM_ALGMODE_NONE+26)
#define CSSM_ALGMODE_CFB32 (CSSM_ALGMODE_NONE+28)
#define CSSM_ALGMODE_CFB16 (CSSM_ALGMODE_NONE+29)
#define CSSM_ALGMODE_CFB8 (CSSM_ALGMODE_NONE+30)
#define CSSM_ALGMODE_WRAP (CSSM_ALGMODE_NONE+31)
#define CSSM_ALGMODE_PRIVATE_WRAP (CSSM_ALGMODE_NONE+32)
#define CSSM_ALGMODE_RELAYX (CSSM_ALGMODE_NONE+33)
#define CSSM_ALGMODE_ECB128 (CSSM_ALGMODE_NONE+34)
#define CSSM_ALGMODE_ECB96 (CSSM_ALGMODE_NONE+35)
#define CSSM_ALGMODE_CBC128 (CSSM_ALGMODE_NONE+36)
#define CSSM_ALGMODE_OAEP_HASH (CSSM_ALGMODE_NONE+37)
#define CSSM_ALGMODE_PKCS1_EME_V15 (CSSM_ALGMODE_NONE+38)
#define CSSM_ALGMODE_PKCS1_EME_OAEP (CSSM_ALGMODE_NONE+39)
#define CSSM_ALGMODE_PKCS1_EMSA_V15 (CSSM_ALGMODE_NONE+40)
#define CSSM_ALGMODE_ISO_9796 (CSSM_ALGMODE_NONE+41)
#define CSSM_ALGMODE_X9_31 (CSSM_ALGMODE_NONE+42)
#define CSSM_ALGMODE_LAST (0x7FFFFFFF)

/*
    All algorithms modes that are vendor specific, and
    not part of the CSSM specification should be defined
    relative to CSSM_ALGMODE_VENDOR_DEFINED.
*/
#define CSSM_ALGMODE_VENDOR_DEFINED (CSSM_ALGMODE_NONE+0x80000000)
/*
    CSSM_PADDING
    This type defines a set of constants used to identify padding methods
    used by different encryption algorithms.
*/
typedef uint32 CSSM_PADDING;
#define CSSM_PADDING_NONE (0)
#define CSSM_PADDING_CUSTOM (CSSM_PADDING_NONE+1)
#define CSSM_PADDING_ZERO (CSSM_PADDING_NONE+2)
#define CSSM_PADDING_ONE (CSSM_PADDING_NONE+3)
#define CSSM_PADDING_ALTERNATE (CSSM_PADDING_NONE+4)
#define CSSM_PADDING_FF (CSSM_PADDING_NONE+5)
#define CSSM_PADDING_PKCS5 (CSSM_PADDING_NONE+6)
#define CSSM_PADDING_PKCS7 (CSSM_PADDING_NONE+7)
#define CSSM_PADDING_CIPHERSTEALING (CSSM_PADDING_NONE+8)
#define CSSM_PADDING_RANDOM (CSSM_PADDING_NONE+9)
#define CSSM_PADDING_PKCS1 (CSSM_PADDING_NONE+10)

/*
    All padding types that are vendor specific, and not
    part of the CSSM specification should be defined
    relative to CSSM_PADDING_VENDOR_DEFINED.
*/
#define CSSM_PADDING_VENDOR_DEFINED (CSSM_PADDING_NONE+0x80000000)

/*
    CSSM_KEYHEADER
    The key header contains meta-data about a key.
*/
typedef struct cssm_keyheader 
{
    CSSM_HEADERVERSION HeaderVersion; /* Key header version */
    CSSM_GUID CspId; /* GUID of CSP generating the key */
    CSSM_KEYBLOB_TYPE BlobType; /* See BlobType #define’s */
    CSSM_KEYBLOB_FORMAT Format; /* Raw or Reference format */
    CSSM_ALGORITHMS AlgorithmId; /* Algorithm ID of key */
    CSSM_KEYCLASS KeyClass; /* Public/Private/Secret, etc. */
    uint32 LogicalKeySizeInBits; /* Logical key size in bits */
    CSSM_KEYATTR_FLAGS KeyAttr; /* Attribute flags */
    CSSM_KEYUSE KeyUsage; /* Key use flags */
    CSSM_DATE StartDate; /* Effective date of key */
    CSSM_DATE EndDate; /* Expiration date of key */
    CSSM_ALGORITHMS WrapAlgorithmId; /* == CSSM_ALGID_NONE if clear key */
    CSSM_ENCRYPT_MODE WrapMode; /* if alg supports multiple wrapping modes */
    uint32 Reserved;
} CSSM_KEYHEADER, *CSSM_KEYHEADER_PTR;

/*
    CSSM_KEY
    This structure is used to represent keys in CSSM.
*/
typedef struct cssm_key 
{
    CSSM_KEYHEADER KeyHeader; /* Fixed length key header */
    CSSM_DATA KeyData; /* Variable length key data */
} CSSM_KEY, *CSSM_KEY_PTR;

/*
    CSSM_WRAP_KEY
    This type is used to reference keys that are known to be in wrapped form.
*/
typedef CSSM_KEY CSSM_WRAP_KEY, *CSSM_WRAP_KEY_PTR;


/*
    CSSM_KEY_TYPE
*/
typedef CSSM_ALGORITHMS CSSM_KEY_TYPE;

/*
    CSSM_CONTEXT_ATTRIBUTE
*/
/* structure defined below in KR section */
typedef struct cssm_kr_profile *CSSM_KR_PROFILE_PTR;


typedef struct cssm_context_attribute
{
    CSSM_ATTRIBUTE_TYPE AttributeType;
    uint32 AttributeLength;
    union cssm_context_attribute_value
    {
        char *String;
        uint32 Uint32;
        CSSM_ACCESS_CREDENTIALS_PTR AccessCredentials;
        CSSM_KEY_PTR Key;
        CSSM_DATA_PTR Data;
        CSSM_PADDING Padding;
        CSSM_DATE_PTR Date;
        CSSM_RANGE_PTR Range;
        CSSM_CRYPTO_DATA_PTR CryptoData;
        CSSM_VERSION_PTR Version;
        CSSM_DL_DB_HANDLE_PTR DLDBHandle;
        CSSM_KR_PROFILE_PTR KRProfile; /* the profile of the local or remote client */
    } Attribute;
} CSSM_CONTEXT_ATTRIBUTE, *CSSM_CONTEXT_ATTRIBUTE_PTR;

/*
    CSSM_CONTEXT
*/
typedef struct cssm_context 
{
    CSSM_CONTEXT_TYPE ContextType;
    CSSM_ALGORITHMS AlgorithmType;
    uint32 NumberOfAttributes;
    CSSM_CONTEXT_ATTRIBUTE_PTR ContextAttributes;
    CSSM_CSP_HANDLE CSPHandle;
    CSSM_BOOL Privileged;        /* when this flag is CSSM_TRUE, the context can
                                    perform crypto operations without being forced
                                    to follow the key recovery policy */
    uint32 EncryptionProhibited; /* if 0, then encryption is allowed.  Otherwise,
                                    the flags indicate which policy disallowed
                                    encryption */
    uint32 WorkFactor;           /* WorkFactor is the maximum number of bits which
                                    can be left out of key recovery fields when
                                    they are generated. The recovered of the key
                                    must then search this number of bits to recover
                                    the key.  */
    uint32 Reserved;
} CSSM_CONTEXT, *CSSM_CONTEXT_PTR;

/*
    CSSM_SC_FLAGS
    A bit mask containing information about a subservice capabilities.
*/
typedef uint32 CSSM_SC_FLAGS;
#define CSSM_CSP_TOK_RNG (0x00000001)
#define CSSM_CSP_TOK_CLOCK_EXISTS (0x00000040)

/*
    CSSM_CSP_READER_FLAGS
    A bit mask containing information about the state of a reader.
*/
typedef uint32 CSSM_CSP_READER_FLAGS;
#define CSSM_CSP_RDR_TOKENPRESENT (0x00000001) /* Token is present in reader/slot */
#define CSSM_CSP_RDR_EXISTS (0x00000002) /* Device is a reader with a removable token */
#define CSSM_CSP_RDR_HW (0x00000004) /* Slot is a hardware slot */

/*
    CSSM_CSP_FLAGS
    A bit mask containing information about the state of a service provider.
*/
typedef uint32 CSSM_CSP_FLAGS;
#define CSSM_CSP_TOK_WRITE_PROTECTED (0x00000002)
#define CSSM_CSP_TOK_LOGIN_REQUIRED (0x00000004)
#define CSSM_CSP_TOK_USER_PIN_INITIALIZED (0x00000008)
#define CSSM_CSP_TOK_PROT_AUTHENTICATION (0x00000100)
#define CSSM_CSP_TOK_USER_PIN_EXPIRED (0x00100000)
#define CSSM_CSP_TOK_SESSION_KEY_PASSWORD (0x00200000)
#define CSSM_CSP_TOK_PRIVATE_KEY_PASSWORD (0x00400000)
#define CSSM_CSP_STORES_PRIVATE_KEYS (0x01000000)
#define CSSM_CSP_STORES_PUBLIC_KEYS (0x02000000)
#define CSSM_CSP_STORES_SESSION_KEYS (0x04000000)
#define CSSM_CSP_STORES_CERTIFICATES (0x08000000)
#define CSSM_CSP_STORES_GENERIC (0x10000000)

/*
    CSSM_PKCS_OAEP
*/
typedef uint32 CSSM_PKCS_OAEP_MGF;
typedef uint32 CSSM_PKCS_OAEP_PSOURCE;
#define CSSM_PKCS_OAEP_MGF_NONE (0)
#define CSSM_PKCS_OAEP_MGF1_SHA1 (CSSM_PKCS_OAEP_MGF_NONE+1)
#define CSSM_PKCS_OAEP_MGF1_MD5 (CSSM_PKCS_OAEP_MGF_NONE+2)
#define CSSM_PKCS_OAEP_PSOURCE_NONE (0)
#define CSSM_PKCS_OAEP_PSOURCE_Pspecified (CSSM_PKCS_OAEP_PSOURCE_NONE+1)


/*
    CSSM_PKCS_OAEP_PARAMS
    When using PKCS #1 RSA with Optimal Asymmetric Encryption Padding (OAEP)
    encoding, this structure must be added to the asymmetric cryptography
    context for that operation.
*/
typedef struct cssm_pkcs1_oaep_params 
{
    uint32 HashAlgorithm;
    CSSM_DATA HashParams;
    CSSM_PKCS_OAEP_MGF MGF;
    CSSM_DATA MGFParams;
    CSSM_PKCS_OAEP_PSOURCE PSource;
    CSSM_DATA PSourceParams;
} CSSM_PKCS1_OAEP_PARAMS, *CSSM_PKCS1_OAEP_PARAMS_PTR;

/*
    CSSM_CSP_OPERATIONAL_STATISTICS
*/
typedef struct cssm_csp_operational_statistics
{
    CSSM_BOOL UserAuthenticated;  /* CSSM_TRUE if the user is logged in to
                                     the token, CSSM_FALSE otherwise. */
    CSSM_CSP_FLAGS DeviceFlags;
    uint32 TokenMaxSessionCount;  /* Exported by Cryptoki modules. */
    uint32 TokenOpenedSessionCount;
    uint32 TokenMaxRWSessionCount;
    uint32 TokenOpenedRWSessionCount;
    uint32 TokenTotalPublicMem;   /* Storage space statistics. */
    uint32 TokenFreePublicMem;
    uint32 TokenTotalPrivateMem;
    uint32 TokenFreePrivateMem;
} CSSM_CSP_OPERATIONAL_STATISTICS, *CSSM_CSP_OPERATIONAL_STATISTICS_PTR;

#define CSSM_VALUE_NOT_AVAILABLE ((uint32)(~0))

typedef struct cssm_pkcs5_pbkdf1_params 
{
    CSSM_DATA Passphrase;
    CSSM_DATA InitVector;
} CSSM_PKCS5_PBKDF1_PARAMS, *CSSM_PKCS5_PBKDF1_PARAMS_PTR;

/*
    CSSM_PKCS5_PBKDF2_PARAMS
    This structure is used to provide input parameters to key derivation
    algorithms based on PCKS #5 PBKDF2 password based key derivation.
*/
typedef uint32 CSSM_PKCS5_PBKDF2_PRF;
#define CSSM_PKCS5_PBKDF2_PRF_HMAC_SHA1		( 0 )

typedef struct cssm_pkcs5_pbkdf2_params
{
	CSSM_DATA Passphrase;
	CSSM_PKCS5_PBKDF2_PRF PseudoRandomFunction;
}
CSSM_PKCS5_PBKDF2_PARAMS;

/*
    CSSM_KEA_DERIVE_PARAMS
    This structure is used during phase 2 of the Key Exchange Algorithm (KEA).
*/
typedef struct cssm_kea_derive_params 
{
    CSSM_DATA Rb;
    CSSM_DATA Yb;
} CSSM_KEA_DERIVE_PARAMS, *CSSM_KEA_DERIVE_PARAMS_PTR;

/*****************************************************************************
    Certificate Library Services API Datatypes
*****************************************************************************/

/*
    CSSM_CL_HANDLE defined under Core Services API Datatypes
*/

/*
    CSSM_CL_TEMPLATE_TYPE
    This type defines an initial set of certificate template types.
*/
typedef uint32 CSSM_CL_TEMPLATE_TYPE;
#define CSSM_CL_TEMPLATE_INTERMEDIATE_CERT 1 /* for X509 certificates, a fully-
                                                formed encoded certificate with
                                                empty signature field */
#define CSSM_CL_TEMPLATE_PKIX_CERTTEMPLATE 2 /* as defined in RFC2511, section 5
                                                CertTemplate */

/*
    CSSM_CERT_BUNDLE_TYPE
    This enumerated type lists the industry-defined formats for aggregating
    certificates and possibly CRLs and other security objects.
*/
typedef enum cssm_cert_bundle_type 
{
    CSSM_CERT_BUNDLE_UNKNOWN = 0x00,
    CSSM_CERT_BUNDLE_CUSTOM = 0x01,
    CSSM_CERT_BUNDLE_PKCS7_SIGNED_DATA = 0x02,
    CSSM_CERT_BUNDLE_PKCS7_SIGNED_ENVELOPED_DATA = 0x03,
    CSSM_CERT_BUNDLE_PKCS12 = 0x04,
    CSSM_CERT_BUNDLE_PFX = 0x05,
    CSSM_CERT_BUNDLE_SPKI_SEQUENCE = 0x06,
    CSSM_CERT_BUNDLE_PGP_KEYRING = 0x07,
    CSSM_CERT_BUNDLE_LAST = 0x7FFF
} CSSM_CERT_BUNDLE_TYPE;

/* 
    Applications wishing to define their own custom certificate
    bundle type should define and publicly document a uint32
    value greater than CSSM_CL_CUSTOM_CERT_BUNDLE_TYPE 
*/
#define CSSM_CL_CUSTOM_CERT_BUNDLE_TYPE 0x8000

/*
    CSSM_CERT_BUNDLE_ENCODING
    This enumerated type lists the encoding methods applied to the signed
    certificate aggregates that are considered to be certificate bundles.
*/
typedef enum cssm_cert_bundle_encoding 
{
    CSSM_CERT_BUNDLE_ENCODING_UNKNOWN = 0x00,
    CSSM_CERT_BUNDLE_ENCODING_CUSTOM = 0x01,
    CSSM_CERT_BUNDLE_ENCODING_BER = 0x02,
    CSSM_CERT_BUNDLE_ENCODING_DER = 0x03,
    CSSM_CERT_BUNDLE_ENCODING_SEXPR = 0x04,
    CSSM_CERT_BUNDLE_ENCODING_PGP = 0x05
} CSSM_CERT_BUNDLE_ENCODING;


/*
    CSSM_CERT_BUNDLE_HEADER
    This structure defines a bundle header, which describes the type and
    encoding of a certificate bundle.
*/
typedef struct cssm_cert_bundle_header 
{
    CSSM_CERT_BUNDLE_TYPE BundleType;
    CSSM_CERT_BUNDLE_ENCODING BundleEncoding;
} CSSM_CERT_BUNDLE_HEADER, *CSSM_CERT_BUNDLE_HEADER_PTR;

/*
    CSSM_CERT_BUNDLE
    This structure defines a certificate bundle, which consists of a descriptive
    header and a pointer to the opaque bundle. The bundle is an opaque aggregation
    of certificates and possibly other security-related objects.
*/
typedef struct cssm_cert_bundle 
{
    CSSM_CERT_BUNDLE_HEADER BundleHeader;
    CSSM_DATA Bundle;
} CSSM_CERT_BUNDLE, *CSSM_CERT_BUNDLE_PTR;


/*
    CSSM_OID
    The object identifier (OID) structure is used to hold a unique identifier for
    the atomic data fields and the compound substructure that comprise the fields
    of a certificate or CRL.
*/
typedef CSSM_DATA CSSM_OID, *CSSM_OID_PTR;

/*
    CSSM_CRL_TYPE
    This structure represents the type of format used for revocation lists.
*/
typedef enum cssm_crl_type 
{
    CSSM_CRL_TYPE_UNKNOWN,
    CSSM_CRL_TYPE_X_509v1,
    CSSM_CRL_TYPE_X_509v2,
    CSSM_CRL_TYPE_SPKI = 0x03,
    CSSM_CRL_TYPE_MULTIPLE = 0x7FFE
} CSSM_CRL_TYPE, *CSSM_CRL_TYPE_PTR;

/*
    CSSM_CRL_ENCODING
    This structure represents the encoding format used for revocation lists.
*/
typedef enum cssm_crl_encoding 
{
    CSSM_CRL_ENCODING_UNKNOWN,
    CSSM_CRL_ENCODING_CUSTOM,
    CSSM_CRL_ENCODING_BER,
    CSSM_CRL_ENCODING_DER,
    CSSM_CRL_ENCODING_BLOOM = 0x04,
    CSSM_CRL_ENCODING_SEXPR = 0x05,
    CSSM_CRL_ENCODING_MULTIPLE = 0x7FFE
} CSSM_CRL_ENCODING, *CSSM_CRL_ENCODING_PTR;

/*
    CSSM_ENCODED_CRL
    This structure contains a pointer to a certificate revocation list (CRL) in
    its encoded representation.
*/
typedef struct cssm_encoded_crl 
{
    CSSM_CRL_TYPE CrlType; /* type of CRL */
    CSSM_CRL_ENCODING CrlEncoding; /* encoding for this packed CRL */
    CSSM_DATA CrlBlob ; /* packed CRL */
} CSSM_ENCODED_CRL, *CSSM_ENCODED_CRL_PTR ;

/*
    CSSM_FIELD
    This structure contains the OID/value pair for any item that can be
    identified by an OID.
*/
typedef struct cssm_field 
{
    CSSM_OID FieldOid;
    CSSM_DATA FieldValue;
} CSSM_FIELD, *CSSM_FIELD_PTR;

/*
    CSSM_FIELDVALUE_COMPLEX_DATA_TYPE
    The value to which the Length component of a CSSM_FIELD, FieldValue is set
    to indicate that the FieldValue Data pointer points to a complex data type.
*/
#define CSSM_FIELDVALUE_COMPLEX_DATA_TYPE (0xFFFFFFFF)

/*****************************************************************************
    Data Storage Library Services API Datatypes
*****************************************************************************/

/*
    CSSM_DL_HANDLE defined under Core Services API Datatypes
*/

/*
    CSSM_DB_HANDLE defined under Core Services API Datatypes
*/

/*
    CSSM_DL_DB_HANDLE defined under Core Services API Datatypes
*/

/*
    CSSM_DL_DB_LIST
    This data structure defines a list of handle pairs of (data storage
    library handle, data store handle).
*/
typedef struct cssm_dl_db_list 
{
    uint32 NumHandles;
    CSSM_DL_DB_HANDLE_PTR DLDBHandle;
} CSSM_DL_DB_LIST, *CSSM_DL_DB_LIST_PTR;

/*
    CSSM_DB_ATTRIBUTE_NAME_FORMAT
    This enumerated list defines the two formats used to represent an
    attribute name.
*/
typedef enum cssm_db_attribute_name_format 
{
    CSSM_DB_ATTRIBUTE_NAME_AS_STRING = 0,
    CSSM_DB_ATTRIBUTE_NAME_AS_OID = 1,
    CSSM_DB_ATTRIBUTE_NAME_AS_INTEGER = 2
} CSSM_DB_ATTRIBUTE_NAME_FORMAT, *CSSM_DB_ATTRIBUTE_NAME_FORMAT_PTR;

/*
    CSSM_DB_ATTRIBUTE_FORMAT
    This enumerated list defines the formats for attribute values.
*/
typedef enum cssm_db_attribute_format 
{
    CSSM_DB_ATTRIBUTE_FORMAT_STRING = 0,
    CSSM_DB_ATTRIBUTE_FORMAT_SINT32 = 1,
    CSSM_DB_ATTRIBUTE_FORMAT_UINT32 = 2,
    CSSM_DB_ATTRIBUTE_FORMAT_BIG_NUM = 3,
    CSSM_DB_ATTRIBUTE_FORMAT_REAL = 4,
    CSSM_DB_ATTRIBUTE_FORMAT_TIME_DATE = 5,
    CSSM_DB_ATTRIBUTE_FORMAT_BLOB = 6,
    CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 = 7,
    CSSM_DB_ATTRIBUTE_FORMAT_COMPLEX = 8
} CSSM_DB_ATTRIBUTE_FORMAT, *CSSM_DB_ATTRIBUTE_FORMAT_PTR;

/*
    CSSM_DB_ATTRIBUTE_INFO
    This data structure describes an attribute of a persistent record.
*/
typedef struct cssm_db_attribute_info 
{
    CSSM_DB_ATTRIBUTE_NAME_FORMAT AttributeNameFormat;
    union cssm_db_attribute_label 
    {
        char * AttributeName; /* e.g., "record label" */
        CSSM_OID AttributeOID; /* e.g., CSSMOID_RECORDLABEL */
        uint32 AttributeID;
    } Label;
    CSSM_DB_ATTRIBUTE_FORMAT AttributeFormat;
} CSSM_DB_ATTRIBUTE_INFO, *CSSM_DB_ATTRIBUTE_INFO_PTR;


/*
    CSSM_DB_ATTRIBUTE_DATA
    This data structure holds an attribute value that can be stored in an
    attribute field of a persistent record. The structure contains a value
    for the data item and a reference to the meta information (typing
    information and schema information) associated with the attribute.
*/
typedef struct cssm_db_attribute_data 
{
    CSSM_DB_ATTRIBUTE_INFO Info;
    uint32 NumberOfValues;
    CSSM_DATA_PTR Value;
} CSSM_DB_ATTRIBUTE_DATA, *CSSM_DB_ATTRIBUTE_DATA_PTR;


/*
    CSSM_DB_RECORDTYPE
    These constants partition the space of record type names into three
    primary groups:
        Record types for schema management;
        Record types recognized and documented in this specification for
            application use;
        Record types defined independently by the industry at large for
            application use;
    All record types defined in this specification are defined in the
    Schema Management name space and the Open Group name space.
*/
typedef uint32 CSSM_DB_RECORDTYPE;
/* Schema Management Name Space Range Definition*/
#define CSSM_DB_RECORDTYPE_SCHEMA_START (0x00000000)
#define CSSM_DB_RECORDTYPE_SCHEMA_END (CSSM_DB_RECORDTYPE_SCHEMA_START + 94)

/* Open Group Application Name Space Range Definition*/
#define CSSM_DB_RECORDTYPE_OPEN_GROUP_START (0x0000000A)
#define CSSM_DB_RECORDTYPE_OPEN_GROUP_END (CSSM_DB_RECORDTYPE_OPEN_GROUP_START + 8)

/* Industry At Large Application Name Space Range Definition */ 
#define CSSM_DB_RECORDTYPE_APP_DEFINED_START (0x80000000)
#define CSSM_DB_RECORDTYPE_APP_DEFINED_END (0xffffffff)
/* Record Types defined in the Schema Management Name Space */
#define CSSM_DL_DB_SCHEMA_INFO (CSSM_DB_RECORDTYPE_SCHEMA_START + 0)
#define CSSM_DL_DB_SCHEMA_INDEXES (CSSM_DB_RECORDTYPE_SCHEMA_START + 1)
#define CSSM_DL_DB_SCHEMA_ATTRIBUTES (CSSM_DB_RECORDTYPE_SCHEMA_START + 2)
#define CSSM_DL_DB_SCHEMA_PARSING_MODULE (CSSM_DB_RECORDTYPE_SCHEMA_START + 3)
/* Record Types defined in the Open Group Application Name Space */
#define CSSM_DL_DB_RECORD_ANY (CSSM_DB_RECORDTYPE_OPEN_GROUP_START + 0)
#define CSSM_DL_DB_RECORD_CERT (CSSM_DB_RECORDTYPE_OPEN_GROUP_START + 1)
#define CSSM_DL_DB_RECORD_CRL (CSSM_DB_RECORDTYPE_OPEN_GROUP_START + 2)
#define CSSM_DL_DB_RECORD_POLICY (CSSM_DB_RECORDTYPE_OPEN_GROUP_START + 3)
#define CSSM_DL_DB_RECORD_GENERIC (CSSM_DB_RECORDTYPE_OPEN_GROUP_START +4)
#define CSSM_DL_DB_RECORD_PUBLIC_KEY (CSSM_DB_RECORDTYPE_OPEN_GROUP_START + 5)
#define CSSM_DL_DB_RECORD_PRIVATE_KEY (CSSM_DB_RECORDTYPE_OPEN_GROUP_START + 6)
#define CSSM_DL_DB_RECORD_SYMMETRIC_KEY (CSSM_DB_RECORDTYPE_OPEN_GROUP_START + 7)
#define CSSM_DL_DB_RECORD_ALL_KEYS (CSSM_DB_RECORDTYPE_OPEN_GROUP_START + 8)


/*
    CSSM_DB_CERTRECORD_SEMANTICS
    These bit masks define a list of usage semantics for how certificates may
    be used.
*/
#define CSSM_DB_CERT_USE_TRUSTED 0x00000001 /* application-defined as trusted */
#define CSSM_DB_CERT_USE_SYSTEM 0x00000002  /* the CSSM system cert */
#define CSSM_DB_CERT_USE_OWNER 0x00000004   /* private key owned by system user*/
#define CSSM_DB_CERT_USE_REVOKED 0x00000008 /* revoked cert - used w CRL APIs */
#define CSSM_DB_CERT_USE_SIGNING 0x00000010 /* use cert for signing only */
#define CSSM_DB_CERT_USE_PRIVACY 0x00000020 /* use cert for confidentiality only */

/*
    CSSM_DB_RECORD_ATTRIBUTE_INFO
    This structure contains the meta information or schema information about
    all of the attributes in a particular record type.
*/
typedef struct cssm_db_record_attribute_info 
{
    CSSM_DB_RECORDTYPE DataRecordType;
    uint32 NumberOfAttributes;
    CSSM_DB_ATTRIBUTE_INFO_PTR AttributeInfo;
} CSSM_DB_RECORD_ATTRIBUTE_INFO, *CSSM_DB_RECORD_ATTRIBUTE_INFO_PTR;

/*
    CSSM_DB_RECORD_ATTRIBUTE_DATA
    This structure aggregates the actual data values for all of the attributes
    in a single record.
*/
typedef struct cssm_db_record_attribute_data 
{
    CSSM_DB_RECORDTYPE DataRecordType;
    uint32 SemanticInformation;
    uint32 NumberOfAttributes;
    CSSM_DB_ATTRIBUTE_DATA_PTR AttributeData;
} CSSM_DB_RECORD_ATTRIBUTE_DATA, *CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR;

/*
    CSSM_DB_PARSING_MODULE_INFO
    This structure aggregates the persistent subservice ID of a default parsing
    module with the record type that it parses.
*/
typedef struct cssm_db_parsing_module_info 
{
    CSSM_DB_RECORDTYPE RecordType;
    CSSM_SUBSERVICE_UID ModuleSubserviceUid;
} CSSM_DB_PARSING_MODULE_INFO, *CSSM_DB_PARSING_MODULE_INFO_PTR;

/*
    CSSM_DB_INDEX_TYPE
    This enumerated list defines two types of indexes: indexes with unique
    values (such as, primary database keys) and indexes with non-unique
    values. These values are used when creating a new data store and defining
    the schema for that data store.
*/
typedef enum cssm_db_index_type 
{
    CSSM_DB_INDEX_UNIQUE = 0,
    CSSM_DB_INDEX_NONUNIQUE = 1
} CSSM_DB_INDEX_TYPE;


/*
    CSSM_DB_INDEXED_DATA_LOCATION
    This enumerated list defines where within a CSSM record the indexed
    data values reside.
*/
typedef enum cssm_db_indexed_data_location 
{
    CSSM_DB_INDEX_ON_UNKNOWN = 0,
    CSSM_DB_INDEX_ON_ATTRIBUTE = 1,
    CSSM_DB_INDEX_ON_RECORD = 2
} CSSM_DB_INDEXED_DATA_LOCATION;


/*
    CSSM_DB_INDEX_INFO
    This structure contains the meta information or schema description of
    an index defined on an attribute.
*/
typedef struct cssm_db_index_info 
{
    CSSM_DB_INDEX_TYPE IndexType;
    CSSM_DB_INDEXED_DATA_LOCATION IndexedDataLocation;
    CSSM_DB_ATTRIBUTE_INFO Info;
} CSSM_DB_INDEX_INFO, *CSSM_DB_INDEX_INFO_PTR;

/*
    CSSM_DB_UNIQUE_RECORD
    This structure contains an index descriptor and a module-defined value.
*/
typedef struct cssm_db_unique_record 
{
    CSSM_DB_INDEX_INFO RecordLocator;
    CSSM_DATA RecordIdentifier;
} CSSM_DB_UNIQUE_RECORD, *CSSM_DB_UNIQUE_RECORD_PTR;

/*
    CSSM_DB_RECORD_INDEX_INFO
    This structure contains the meta information or schema description of
    the set of indexes defined on a single record type.
*/
typedef struct cssm_db_record_index_info 
{
    CSSM_DB_RECORDTYPE DataRecordType;
    uint32 NumberOfIndexes;
    CSSM_DB_INDEX_INFO_PTR IndexInfo;
} CSSM_DB_RECORD_INDEX_INFO, *CSSM_DB_RECORD_INDEX_INFO_PTR;

/*
    CSSM_DB_ACCESS_TYPE
    This bitmask describes a user’s desired level of access to a data store.
*/
typedef uint32 CSSM_DB_ACCESS_TYPE, *CSSM_DB_ACCESS_TYPE_PTR;
#define CSSM_DB_ACCESS_READ (0x00001)
#define CSSM_DB_ACCESS_WRITE (0x00002)
#define CSSM_DB_ACCESS_PRIVILEGED (0x00004) /* versus user mode */


/*
    CSSM_DB_MODIFY_MODE
    Constants of this type define the types of modifications that can be
    performed on record attributes using the function CSSM_DL_DataModify().
*/
typedef uint32 CSSM_DB_MODIFY_MODE;
#define CSSM_DB_MODIFY_ATTRIBUTE_NONE (0)
#define CSSM_DB_MODIFY_ATTRIBUTE_ADD (CSSM_DB_MODIFY_ATTRIBUTE_NONE + 1)
#define CSSM_DB_MODIFY_ATTRIBUTE_DELETE (CSSM_DB_MODIFY_ATTRIBUTE_NONE + 2)
#define CSSM_DB_MODIFY_ATTRIBUTE_REPLACE (CSSM_DB_MODIFY_ATTRIBUTE_NONE + 3)

/*
    CSSM_DBINFO
    This structure contains the meta-information about an entire data store.
*/
typedef struct cssm_dbinfo 
{
    /* 
        meta information about each record type stored in this
        data store including meta information about record
        attributes and indexes 
    */
    uint32 NumberOfRecordTypes;
    CSSM_DB_PARSING_MODULE_INFO_PTR DefaultParsingModules;
    CSSM_DB_RECORD_ATTRIBUTE_INFO_PTR RecordAttributeNames;
    CSSM_DB_RECORD_INDEX_INFO_PTR RecordIndexes;
    /* access restrictions for opening this data store */
    CSSM_BOOL IsLocal;
    char *AccessPath; /* URL, dir path, etc. */
    void *Reserved;
} CSSM_DBINFO, *CSSM_DBINFO_PTR;

/*
    CSSM_DB_OPERATOR
    These are the logical operators which can be used when specifying a
    selection predicate.
*/
typedef enum cssm_db_operator 
{
    CSSM_DB_EQUAL = 0,
    CSSM_DB_NOT_EQUAL = 1,
    CSSM_DB_LESS_THAN = 2,
    CSSM_DB_GREATER_THAN = 3,
    CSSM_DB_CONTAINS = 4,
    CSSM_DB_CONTAINS_INITIAL_SUBSTRING = 5,
    CSSM_DB_CONTAINS_FINAL_SUBSTRING = 6
} CSSM_DB_OPERATOR, *CSSM_DB_OPERATOR_PTR;

/*
    CSSM_DB_CONJUNCTIVE
    These are the conjunctive operations which can be used when specifying a
    selection criterion.
*/
typedef enum cssm_db_conjunctive
{
    CSSM_DB_NONE = 0,
    CSSM_DB_AND = 1,
    CSSM_DB_OR = 2
} CSSM_DB_CONJUNCTIVE, *CSSM_DB_CONJUNCTIVE_PTR;

/*
    CSSM_SELECTION_PREDICATE
    This structure defines the selection predicate to be used for data store
    queries.
*/
typedef struct cssm_selection_predicate 
{
    CSSM_DB_OPERATOR DbOperator;
    CSSM_DB_ATTRIBUTE_DATA Attribute;
} CSSM_SELECTION_PREDICATE, *CSSM_SELECTION_PREDICATE_PTR;

/*
    CSSM_QUERY_LIMITS
    This structure defines the time and space limits a caller can set to
    control early termination of the execution of a data store query.
*/
#define CSSM_QUERY_TIMELIMIT_NONE 0
#define CSSM_QUERY_SIZELIMIT_NONE 0
typedef struct cssm_query_limits 
{
    uint32 TimeLimit; /* in seconds */
    uint32 SizeLimit; /* max. number of records to return */
} CSSM_QUERY_LIMITS, *CSSM_QUERY_LIMITS_PTR;

/*
    CSSM_QUERY_FLAGS
    These flags may be used by the application to request query-related
    operation, such as the format of the returned data.
*/
typedef uint32 CSSM_QUERY_FLAGS;
#define CSSM_QUERY_RETURN_DATA (0x01)

/*
    CSSM_QUERY
    This structure holds a complete specification of a query to select
    records from a data store.
*/
typedef struct cssm_query 
{
    CSSM_DB_RECORDTYPE RecordType;
    CSSM_DB_CONJUNCTIVE Conjunctive;
    uint32 NumSelectionPredicates;
    CSSM_SELECTION_PREDICATE_PTR SelectionPredicate;
    CSSM_QUERY_LIMITS QueryLimits;
    CSSM_QUERY_FLAGS QueryFlags;
} CSSM_QUERY, *CSSM_QUERY_PTR;

/*
    CSSM_DLTYPE
    This enumerated list defines the types of underlying data management
    systems that can be used by the DL module to provide services.
*/
typedef enum cssm_dltype 
{
    CSSM_DL_UNKNOWN = 0,
    CSSM_DL_CUSTOM = 1,
    CSSM_DL_LDAP = 2,
    CSSM_DL_ODBC = 3,
    CSSM_DL_PKCS11 = 4,
    CSSM_DL_FFS = 5, /* flat file system */
    CSSM_DL_MEMORY = 6,
    CSSM_DL_REMOTEDIR = 7
} CSSM_DLTYPE, *CSSM_DLTYPE_PTR;


/*
    CSSM_DL_PKCS11_ATTRIBUTES
    Each type of DL module can define it own set of type specific attributes.
    This structure contains the attributes that are specific to a PKCS#11-
    compliant data storage device.
*/
typedef void *CSSM_DL_CUSTOM_ATTRIBUTES;
typedef void *CSSM_DL_LDAP_ATTRIBUTES;
typedef void *CSSM_DL_ODBC_ATTRIBUTES;
typedef void *CSSM_DL_FFS_ATTRIBUTES;
typedef struct cssm_dl_pkcs11_attributes 
{
    uint32 DeviceAccessFlags;
} *CSSM_DL_PKCS11_ATTRIBUTE, *CSSM_DL_PKCS11_ATTRIBUTE_PTR;


/*
    CSSM_DB_DATASTORES_UNKNOWN
    Not all DL modules can maintain a summary of managed data stores. In
    this case, the DL module reports its number of data stores as
    CSSM_DB_DATASTORES_UNKNOWN. Data stores can (and probably do) exist,
    but the DL module cannot provide a list of them.
*/
#define CSSM_DB_DATASTORES_UNKNOWN (0xFFFFFFFF)


/*
    CSSM_NAME_LIST
    The CSSM_NAME_LIST structure is used to return the logical names of the
    data stores that a DL module can access.
*/
typedef struct cssm_name_list 
{
    uint32 NumStrings;
    char** String;
} CSSM_NAME_LIST, *CSSM_NAME_LIST_PTR;

/*
    CSSM_DB_RETRIEVAL_MODES
    This defines the retrieval modes for CSSM_DL_DataGetFirst( ) operations.
*/
typedef uint32 CSSM_DB_RETRIEVAL_MODES;

#define CSSM_DB_TRANSACTIONAL_MODE (0)
#define CSSM_DB_FILESYSTEMSCAN_MODE (1)

/*
    CSSM_DB_SCHEMA_ATTRIBUTE_INFO
*/
typedef struct cssm_db_schema_attribute_info 
{
    uint32 AttributeId;
    char *AttributeName;
    CSSM_OID AttributeNameID;
    CSSM_DB_ATTRIBUTE_FORMAT DataType;
} CSSM_DB_SCHEMA_ATTRIBUTE_INFO, *CSSM_DB_SCHEMA_ATTRIBUTE_INFO_PTR;



/*
    CSSM_DB_SCHEMA_INDEX_INFO
*/
typedef struct cssm_db_schema_index_info 
{
    uint32 AttributeId;
    uint32 IndexId;
    CSSM_DB_INDEX_TYPE IndexType;
    CSSM_DB_INDEXED_DATA_LOCATION IndexedDataLocation;
} CSSM_DB_SCHEMA_INDEX_INFO, *CSSM_DB_SCHEMA_INDEX_INFO_PTR;



/*****************************************************************************
    Trust Policy Services API Datatypes
*****************************************************************************/
/*
    CSSM_TP_HANDLE defined above under Core Services API Datatypes
*/

/*
    CSSM_TP_AUTHORITY_ID
    This data structure identifies an Authority who provides security-related
    services. It is used as input to functions requesting authority services.
*/
typedef struct cssm_tp_authority_id 
{
    CSSM_DATA *AuthorityCert;
    CSSM_NET_ADDRESS_PTR AuthorityLocation;
} CSSM_TP_AUTHORITY_ID, *CSSM_TP_AUTHORITY_ID_PTR;


/*
    CSSM_TP_AUTHORITY_REQUEST_TYPE
    This extensible list defines the type of a request to an Authority providing
    certificate-related services.
*/
typedef uint32 CSSM_TP_AUTHORITY_REQUEST_TYPE, *CSSM_TP_AUTHORITY_REQUEST_TYPE_PTR;
#define CSSM_TP_AUTHORITY_REQUEST_CERTISSUE (0x01)
#define CSSM_TP_AUTHORITY_REQUEST_CERTREVOKE (0x02)
#define CSSM_TP_AUTHORITY_REQUEST_CERTSUSPEND (0x03)
#define CSSM_TP_AUTHORITY_REQUEST_CERTRESUME (0x04)
#define CSSM_TP_AUTHORITY_REQUEST_CERTVERIFY (0x05)
#define CSSM_TP_AUTHORITY_REQUEST_CERTNOTARIZE (0x06)
#define CSSM_TP_AUTHORITY_REQUEST_CERTUSERECOVER (0x07)
#define CSSM_TP_AUTHORITY_REQUEST_CRLISSUE (0x100)

/*
    CSSM_TP_VERIFICATION_RESULTS_CALLBACK
    This type defines the form of the callback function a service provider
    module must use to incrementally return verified certificates to a caller
    whose credentials are being verified.
*/
typedef CSSM_RETURN (CSSMAPI * CSSM_TP_VERIFICATION_RESULTS_CALLBACK)
                    (CSSM_MODULE_HANDLE ModuleHandle,
                    void *CallerCtx,
                    CSSM_DATA_PTR VerifiedCert);
/*
    CSSM_TP_POLICYINFO
    This data structure contains a set of one or more policy identifiers and
    application-domain-specific information used to control evaluation of the
    named policies.
*/
typedef struct cssm_tp_policyinfo 
{
    uint32 NumberOfPolicyIds;
    CSSM_FIELD_PTR PolicyIds;
    void *PolicyControl;
} CSSM_TP_POLICYINFO, *CSSM_TP_POLICYINFO_PTR;

/*
    CSSM_TP_SERVICES
    This bit mask defines the additional back-office services that a
    Certification Authority (CA) can offer.
*/
typedef uint32 CSSM_TP_SERVICES;
/* bit masks for additional Authority services available through TP */
#define CSSM_TP_KEY_ARCHIVE (0x0001) /* archive cert & keys */
#define CSSM_TP_CERT_PUBLISH (0x0002) /* register cert in directory */
#define CSSM_TP_CERT_NOTIFY_RENEW (0x0004) /* notify at renewal time */
#define CSSM_TP_CERT_DIR_UPDATE (0x0008) /* update cert registry entry */
#define CSSM_TP_CRL_DISTRIBUTE (0x0010) /* push CRL to everyone */

/*
    CSSM_TP_ACTION
    This data structure represents a descriptive value defined by the trust
    policy module.
*/
typedef uint32 CSSM_TP_ACTION;
#define CSSM_TP_ACTION_DEFAULT (0)

/*
    CSSM_TP_STOP_ON
    This enumerated list defines the conditions controlling termination
    of the verification process by the trust policy module when a set of
    policies/conditions must be tested.
*/
typedef enum cssm_tp_stop_on 
{
    CSSM_TP_STOP_ON_POLICY = 0, /* use the pre-defined stopping criteria */
    CSSM_TP_STOP_ON_NONE = 1, /* evaluate all condition whether TRUE or FALSE */
    CSSM_TP_STOP_ON_FIRST_PASS = 2, /* stop evaluation at first TRUE */
    CSSM_TP_STOP_ON_FIRST_FAIL = 3 /* stop evaluation at first FALSE */
} CSSM_TP_STOP_ON;

/*
    CSSM_TIMESTRING
    Values must be expressed in Greenwich Mean Time (Zulu) and must include
    seconds, even when the value of seconds is "00". Values must not include
    fractional seconds. This string contains a date and time in the format
    "YYYYMMDDhhmmss", defined by:
        YYYY - four characters representing the year
        MM- two characters representing the month within a year
        DD - two characters representing the day within a month
        hh - two characters representing the hours within a day ["00" through "23"]
        mm - two characters representing the minutes within a hour ["00" through "59"]
        ss - two characters representing the seconds within a minute ["00" through "59"]
*/
typedef char *CSSM_TIMESTRING;

/*
    CSSM_TP_CALLERAUTH_CONTEXT
    This data structure contains the basic parameters required to authenticate
    the caller who is requesting a certificate-related service from a certificate
    authority.
*/
typedef struct cssm_tp_callerauth_context 
{
    CSSM_TP_POLICYINFO Policy;
    CSSM_TIMESTRING VerifyTime;
    CSSM_TP_STOP_ON VerificationAbortOn;
    CSSM_TP_VERIFICATION_RESULTS_CALLBACK CallbackWithVerifiedCert;
    uint32 NumberOfAnchorCerts;
    CSSM_DATA_PTR AnchorCerts;
    CSSM_DL_DB_LIST_PTR DBList;
    CSSM_ACCESS_CREDENTIALS_PTR CallerCredentials;
} CSSM_TP_CALLERAUTH_CONTEXT, *CSSM_TP_CALLERAUTH_CONTEXT_PTR;


/*
    CSSM_CRL_PARSE_FORMAT
    This extensible list defines the parse formats for a CRL.
*/
typedef uint32 CSSM_CRL_PARSE_FORMAT, * CSSM_CRL_PARSE_FORMAT_PTR;
#define CSSM_CRL_PARSE_FORMAT_NONE (0x00)
#define CSSM_CRL_PARSE_FORMAT_CUSTOM (0x01)
#define CSSM_CRL_PARSE_FORMAT_SEXPR (0x02)
#define CSSM_CRL_PARSE_FORMAT_COMPLEX (0x03)
#define CSSM_CRL_PARSE_FORMAT_OID_NAMED (0x04)
#define CSSM_CRL_PARSE_FORMAT_TUPLE (0x05)
#define CSSM_CRL_PARSE_FORMAT_MULTIPLE (0x7FFE)
#define CSSM_CRL_PARSE_FORMAT_LAST (0x7FFF)
/* 
    Applications wishing to define their own custom parse
    format should create a uint32 value greater than the
    CSSM_CL_CUSTOM_CRL_PARSE_FORMAT 
*/
#define CSSM_CL_CUSTOM_CRL_PARSE_FORMAT (0x8000)

/*
    CSSM_PARSED_CRL
    This structure holds a parsed representation of a CRL. The CRL type and
    representation format are included in the structure.
*/
typedef struct cssm_parsed_crl 
{
    CSSM_CRL_TYPE CrlType; /* CRL type */
    CSSM_CRL_PARSE_FORMAT ParsedCrlFormat; /* struct of ParsedCrl */
    void *ParsedCrl; /* parsed CRL (to be typecast) */
} CSSM_PARSED_CRL, *CSSM_PARSED_CRL_PTR ;

/*
    CSSM_CRL_PAIR
    This structure holds a parsed representation and an encoded representation
    of a CRL. The two elements should be different representations of a single CRL.
*/
typedef struct cssm_crl_pair 
{
    CSSM_ENCODED_CRL EncodedCrl; /* an encoded CRL blob */
    CSSM_PARSED_CRL ParsedCrl; /* equivalent parsed CRL */
} CSSM_CRL_PAIR, *CSSM_CRL_PAIR_PTR;

/*
    CSSM_CRLGROUP_TYPE
    This extensible list defines the type of a CRL group.
*/
typedef uint32 CSSM_CRLGROUP_TYPE, * CSSM_CRLGROUP_TYPE_PTR;
#define CSSM_CRLGROUP_DATA (0x00)
#define CSSM_CRLGROUP_ENCODED_CRL (0x01)
#define CSSM_CRLGROUP_PARSED_CRL (0x02)
#define CSSM_CRLGROUP_CRL_PAIR (0x03)

/*
    CSSM_CRLGROUP
    This data structure aggregates a group of one or more memory-resident CRLs.
    The CRLs can be of one type or of mixed types and encodings.
*/
typedef struct cssm_crlgroup 
{
    CSSM_CRL_TYPE CrlType;
    CSSM_CRL_ENCODING CrlEncoding;
    uint32 NumberOfCrls;
    union 
    {
        CSSM_DATA_PTR CrlList ; /* CRL blob */
        CSSM_ENCODED_CRL_PTR EncodedCrlList ; /* CRL blob w/ separate type */
        CSSM_PARSED_CRL_PTR ParsedCrlList; /* bushy, parsed CRL */
        CSSM_CRL_PAIR_PTR PairCrlList;
    } GroupList;
    CSSM_CRLGROUP_TYPE CrlGroupType;
} CSSM_CRLGROUP, *CSSM_CRLGROUP_PTR;

/*
    CSSM_FIELDGROUP
    This data structure groups a set of OID/value pairs into a single structure
    with a count of the number of fields in the array.
*/
typedef struct cssm_fieldgroup 
{
    int NumberOfFields ; /* number of fields in the array */
    CSSM_FIELD_PTR Fields ; /* array of fields */
} CSSM_FIELDGROUP, *CSSM_FIELDGROUP_PTR ;


/*
    CSSM_EVIDENCE_FORM
    This type defines constants corresponding to known representations of
    verification evidence returned by a verification function or service.
    The constant values are used as type indicators in the structure
    CSSM_EVIDENCE.
*/
typedef uint32 CSSM_EVIDENCE_FORM;
#define CSSM_EVIDENCE_FORM_UNSPECIFIC 0x0
#define CSSM_EVIDENCE_FORM_CERT 0x1
#define CSSM_EVIDENCE_FORM_CRL 0x2
#define CSSM_EVIDENCE_FORM_CERT_ID 0x3
#define CSSM_EVIDENCE_FORM_CRL_ID 0x4
#define CSSM_EVIDENCE_FORM_VERIFIER_TIME 0x5
#define CSSM_EVIDENCE_FORM_CRL_THISTIME 0x6
#define CSSM_EVIDENCE_FORM_CRL_NEXTTIME 0x7
#define CSSM_EVIDENCE_FORM_POLICYINFO 0x8
#define CSSM_EVIDENCE_FORM_TUPLEGROUP 0x9

/*
    CSSM_EVIDENCE
    This structure contains certificates, CRLs and other information used as
    audit trail evidence.
*/
typedef struct cssm_evidence 
{
    CSSM_EVIDENCE_FORM EvidenceForm;
    void *Evidence; /* Evidence content */
} CSSM_EVIDENCE, *CSSM_EVIDENCE_PTR;

/*
    CSSM_TP_VERIFY_CONTEXT
    This data structure contains all of the credentials and information
    required as input to a verification service. The verification service
    verifies a set of credentials for fitness to perform an application-
    defined action on a specified data object.
*/
typedef struct cssm_tp_verify_context 
{
    CSSM_TP_ACTION Action;
    CSSM_DATA ActionData;
    CSSM_CRLGROUP Crls;
    CSSM_TP_CALLERAUTH_CONTEXT_PTR Cred;
} CSSM_TP_VERIFY_CONTEXT, *CSSM_TP_VERIFY_CONTEXT_PTR;

/*
    CSSM_TP_VERIFY_CONTEXT_RESULT
    This data structure contains the results of an evaluation by a local
    trust policy. The trust policy evaluation process can return to the
    requester the following:
        Description of the how the trust policy was applied during the
            evaluation process;
        A set of evidence compiled during the evaluation process.
*/
typedef struct cssm_tp_verify_context_result 
{
    uint32 NumberOfEvidences;
    CSSM_EVIDENCE_PTR Evidence;
} CSSM_TP_VERIFY_CONTEXT_RESULT, *CSSM_TP_VERIFY_CONTEXT_RESULT_PTR;


/*
    CSSM_TP_REQUEST_SET
    This data structure specifies the input data required when requesting one
    or more requests from a Certification Authority. The initialized structure
    is input to CSSM_TP_SubmitCredRequest().
*/
typedef struct cssm_tp_request_set 
{
    uint32 NumberOfRequests;
    void *Requests;
} CSSM_TP_REQUEST_SET, *CSSM_TP_REQUEST_SET_PTR;

/*
    CSSM_TP_RESULT_SET
    This data structure specifies the final output data returned by a call to
    CSSM_TP_RetrieveCredAuthResult ( ). The structure contains an ordered array.
    Each array entry corresponds to one request.
*/
typedef struct cssm_tp_result_set 
{
    uint32 NumberOfResults;
    void *Results;
} CSSM_TP_RESULT_SET, *CSSM_TP_RESULT_SET_PTR;

/*
    CSSM_TP_CONFIRM_STATUS
    This type defines constants used to indicate acceptance or rejection of the
    results of a service authority.
*/
typedef uint32 CSSM_TP_CONFIRM_STATUS, *CSSM_TP_CONFIRM_STATUS_PTR;
#define CSSM_TP_CONFIRM_STATUS_UNKNOWN 0x0 /* indeterminate */
#define CSSM_TP_CONFIRM_ACCEPT 0x1         /* accept results of executing a
                                              submit-retrieve function pair */
#define CSSM_TP_CONFIRM_REJECT 0x2         /* reject results of executing a
                                              submit-retrieve function pair */

/*
    CSSM_TP_CONFIRM_RESPONSE
    This data structure specifies the input response vector required when
    confirming the results returned by a Certification Authority through the
    CSSM_TP_RetrieveCredResult( ) function. The response vector is ordered
    corresponding to the set of returned results, one response per result.
*/
typedef struct cssm_tp_confirm_response 
{
    uint32 NumberOfResponses;
    CSSM_TP_CONFIRM_STATUS_PTR Responses;
} CSSM_TP_CONFIRM_RESPONSE, *CSSM_TP_CONFIRM_RESPONSE_PTR;


/*
    CSSM_ESTIMATED_TIME_UNKNOWN
    The value used by an authority or process to indicate that an estimated
    completion time can not be determined.
*/
#define CSSM_ESTIMATED_TIME_UNKNOWN -1

/*
    CSSM_ELAPSED_TIME_UNKNOWN
    A constant value indicating that the service provider module does not know
    the time elapsed since a specific prior event.
*/
#define CSSM_ELAPSED_TIME_UNKNOWN (-1)

/*
    CSSM_ELAPSED_TIME_COMPLETE
    A constant value indicating that request has completed and the elapsed time
    since a specific prior event is no longer of significance.
*/
#define CSSM_ELAPSED_TIME_COMPLETE (-2)

/*
    CSSM_TP_CERTISSUE_INPUT
    This data structure aggregates the input information for a request to issue
    a single new certificate.
*/
typedef struct cssm_tp_certissue_input 
{
    CSSM_SUBSERVICE_UID CSPSubserviceUid;
    CSSM_CL_HANDLE CLHandle;
    uint32 NumberOfTemplateFields;
    CSSM_FIELD_PTR SubjectCertFields;
    CSSM_TP_SERVICES MoreServiceRequests;
    uint32 NumberOfServiceControls;
    CSSM_FIELD_PTR ServiceControls;
    CSSM_ACCESS_CREDENTIALS_PTR UserCredentials;
} CSSM_TP_CERTISSUE_INPUT, *CSSM_TP_CERTISSUE_INPUT_PTR;


/*
    CSSM_TP_CERTISSUE_STATUS
    This set of defined constants indicates the status of a service request to
    issue a certificate.
*/
typedef uint32 CSSM_TP_CERTISSUE_STATUS;
#define CSSM_TP_CERTISSUE_STATUS_UNKNOWN 0x0    /* indeterminate */
#define CSSM_TP_CERTISSUE_OK 0x1                /* cert issued as requested */
#define CSSM_TP_CERTISSUE_OKWITHCERTMODS 0x2    /* cert issued but cert contents
                                                   were updated by the issuing authority */
#define CSSM_TP_CERTISSUE_OKWITHSERVICEMODS 0x3 /* cert issued but some requested
                                                   backend services were not performed
                                                   by the issuing authority */
#define CSSM_TP_CERTISSUE_REJECTED 0x4          /* cert was not issued due to some error
                                                   condition */
#define CSSM_TP_CERTISSUE_NOT_AUTHORIZED 0x5    /* cert was not issued, the request was
                                                   not authorized */
#define CSSM_TP_CERTISSUE_WILL_BE_REVOKED 0x6   /* cert was issued, but TP has initiated
                                                   a revocation of the certificate */


/*
    CSSM_TP_CERTISSUE_OUTPUT
    This data structure aggregates the output information generated in
    response to a single request to issue a new certificate. Each result
    includes a certificate group composed of one or more certificates.
    The first certificate in the group is the newly issued certificate.
*/
typedef struct cssm_tp_certissue_output 
{
    CSSM_TP_CERTISSUE_STATUS IssueStatus;
    CSSM_CERTGROUP_PTR CertGroup;
    CSSM_TP_SERVICES PerformedServiceRequests;
} CSSM_TP_CERTISSUE_OUTPUT, *CSSM_TP_CERTISSUE_OUTPUT_PTR;

/*
    CSSM_TP_CERTCHANGE_ACTION
    This type defines the constants identifying the certificates state changes
    that are posted to a certificate revocation list for distribution to all
    concerned systems.
*/
typedef uint32 CSSM_TP_CERTCHANGE_ACTION;
#define CSSM_TP_CERTCHANGE_NONE (0x0) /* no change */

#define CSSM_TP_CERTCHANGE_REVOKE (0x1) /* Revoke the certificate */
/* This action type indicates a request to revoke a single
certificate. Notice of the revocation operation remains
in affect until the certificate itself expires. Revocation
should be used to permanently remove a certificate from use. */

#define CSSM_TP_CERTCHANGE_HOLD (0x2) /* Hold/suspend the certificate */
/* This action type indicates a request to suspend a
single certificate. A suspension operation implies
that the requester intends, at some time in the future,
to request that the certificate be released from hold,
making it available for use again. Placing a hold on
a certificate does not obligate the requester to
request a release. In practice, a certificate may
remain on hold until the certificate itself expires.
Revocation should be used to permanently remove a
certificate from use. */

#define CSSM_TP_CERTCHANGE_RELEASE (0x3) /* Release the held certificate */
/* This action type indicates a request to release a
single certificate currently on hold. A release
operation makes a certificate available for use again.
Revocation should be used to permanently remove a
certificate from use. */

/*
    CSSM_TP_CERTCHANGE_REASON
    This type defines the constants reasons for a change in certificate state.
*/
typedef uint32 CSSM_TP_CERTCHANGE_REASON;
#define CSSM_TP_CERTCHANGE_REASON_UNKNOWN (0x0)        /* unspecified */
#define CSSM_TP_CERTCHANGE_REASON_KEYCOMPROMISE (0x1)  /* Subject key believed to
                                                          be compromised */
#define CSSM_TP_CERTCHANGE_REASON_CACOMPROMISE (0x2)   /* CA’s key believed to be
                                                          compromised */
#define CSSM_TP_CERTCHANGE_REASON_CEASEOPERATION (0x3) /* certificate holder ceases
                                                          operation under the jurisdiction
                                                          of this certificate */
#define CSSM_TP_CERTCHANGE_REASON_AFFILIATIONCHANGE (0x4) /* certificate holder has moved
                                                             from this jurisdiction */
#define CSSM_TP_CERTCHANGE_REASON_SUPERCEDED (0x5)    /* certificate holder as issued
                                                         a new, superceding certificate */
#define CSSM_TP_CERTCHANGE_REASON_SUSPECTEDCOMPROMISE (0x6) /* certificate could be
                                                               compromised */
#define CSSM_TP_CERTCHANGE_REASON_HOLDRELEASE (0x7)   /* certificate holder resumes
                                                         operation under the jurisdiction
                                                         of this certificate */


/*
    CSSM_TP_CERTCHANGE_INPUT
    This data structure aggregates the input information for a request to change
    the state of a single certificate.
*/
typedef struct cssm_tp_certchange_input
{
    CSSM_TP_CERTCHANGE_ACTION Action;
    CSSM_TP_CERTCHANGE_REASON Reason;
    CSSM_CL_HANDLE CLHandle;
    CSSM_DATA_PTR Cert;
    CSSM_FIELD_PTR ChangeInfo;
    CSSM_TIMESTRING StartTime;
    CSSM_ACCESS_CREDENTIALS_PTR CallerCredentials;
} CSSM_TP_CERTCHANGE_INPUT, *CSSM_TP_CERTCHANGE_INPUT_PTR;

/*
    CSSM_TP_CERTCHANGE_STATUS
    These constants define the change values that can be returned by a certificate
    authority that provides state change services for certificates.
*/
typedef uint32 CSSM_TP_CERTCHANGE_STATUS;
#define CSSM_TP_CERTCHANGE_STATUS_UNKNOWN (0x0) /* indeterminate */
#define CSSM_TP_CERTCHANGE_OK (0x1)             /* cert state was successfully
                                                   changed beginning at the
                                                   specified time */
#define CSSM_TP_CERTCHANGE_OKWITHNEWTIME (0x2)  /* cert state was successfully
                                                   changed, at a modified
                                                   effective time */
#define CSSM_TP_CERTCHANGE_WRONGCA (0x3)        /* cert state was not changed,
                                                   the selected CA is not authorized
                                                   to change the cert state */
#define CSSM_TP_CERTCHANGE_REJECTED (0x4)       /* cert state was not changed
                                                   due to some error condition */
#define CSSM_TP_CERTCHANGE_NOT_AUTHORIZED (0x5) /* cert state was not changed,
                                                   the requester is not authorized
                                                   to change the cert state */

/*
    CSSM_TP_CERTCHANGE_OUTPUT
    This type defines the output results returned by a certificate authority
    in response to an action request to revoke, hold, or release a hold on a
    certificate. An array of output values is aggregated in a
    CSSM_TP_RESULT_SET structure and is an output parameter of the function
    CSSM_TP_RetrieveCredResult().
*/
typedef struct cssm_tp_certchange_output 
{
    CSSM_TP_CERTCHANGE_STATUS ActionStatus;
    CSSM_FIELD RevokeInfo;
} CSSM_TP_CERTCHANGE_OUTPUT, *CSSM_TP_CERTCHANGE_OUTPUT_PTR;

/*
    CSSM_TP_CERTVERIFY_INPUT
    This data structure aggregates the input information for a request to
    verify a single target certificate.
*/
typedef struct cssm_tp_certverify_input
{
    CSSM_CL_HANDLE CLHandle;
    CSSM_DATA_PTR Cert;
    CSSM_TP_VERIFY_CONTEXT_PTR VerifyContext;
} CSSM_TP_CERTVERIFY_INPUT, *CSSM_TP_CERTVERIFY_INPUT_PTR;

/*
    CSSM_TP_CERTVERIFY_STATUS
    These constants define the status values that can be returned by a
    certificate authority that provides a credential verification service.
*/
typedef uint32 CSSM_TP_CERTVERIFY_STATUS; 
#define CSSM_TP_CERTVERIFY_UNKNOWN (0x0)
#define CSSM_TP_CERTVERIFY_VALID (0x1)
#define CSSM_TP_CERTVERIFY_INVALID (0x2)
#define CSSM_TP_CERTVERIFY_REVOKED (0x3)
#define CSSM_TP_CERTVERIFY_SUSPENDED (0x4)
#define CSSM_TP_CERTVERIFY_EXPIRED (0x5)
#define CSSM_TP_CERTVERIFY_NOT_VALID_YET (0x6)
#define CSSM_TP_CERTVERIFY_INVALID_AUTHORITY (0x7)
#define CSSM_TP_CERTVERIFY_INVALID_SIGNATURE (0x8)
#define CSSM_TP_CERTVERIFY_INVALID_CERT_VALUE (0x9)
#define CSSM_TP_CERTVERIFY_INVALID_CERTGROUP (0xA)
#define CSSM_TP_CERTVERIFY_INVALID_POLICY (0xB)
#define CSSM_TP_CERTVERIFY_INVALID_POLICY_IDS (0xC)
#define CSSM_TP_CERTVERIFY_INVALID_BASIC_CONSTRAINTS (0xD)
#define CSSM_TP_CERTVERIFY_INVALID_CRL_DIST_PT (0xE)
#define CSSM_TP_CERTVERIFY_INVALID_NAME_TREE (0xF)
#define CSSM_TP_CERTVERIFY_UNKNOWN_CRITICAL_EXT (0x10)

/*
    CSSM_TP_CERTVERIFY_OUTPUT
    This data structure aggregates the verification status of a single
    target certificate.
*/
typedef struct cssm_tp_certverify_output 
{
    CSSM_TP_CERTVERIFY_STATUS VerifyStatus;
    uint32 NumberOfEvidence;
    CSSM_EVIDENCE_PTR Evidence;
} CSSM_TP_CERTVERIFY_OUTPUT, *CSSM_TP_CERTVERIFY_OUTPUT_PTR;

/*
    CSSM_TP_CERTNOTARIZE_INPUT
    This data structure aggregates the input information for a request to
    notarize a certificate and optionally add more field values to the
    certificate prior to over-signing by a CA process.
*/
typedef struct cssm_tp_certnotarize_input 
{
    CSSM_CL_HANDLE CLHandle;
    uint32 NumberOfFields;
    CSSM_FIELD_PTR MoreFields;
    CSSM_FIELD_PTR SignScope;
    uint32 ScopeSize;
    CSSM_TP_SERVICES MoreServiceRequests;
    uint32 NumberOfServiceControls;
    CSSM_FIELD_PTR ServiceControls;
    CSSM_ACCESS_CREDENTIALS_PTR UserCredentials;
} CSSM_TP_CERTNOTARIZE_INPUT, *CSSM_TP_CERTNOTARIZE_INPUT_PTR;

/*
    CSSM_TP_CERTNOTARIZE_STATUS
    This set of defined constants indicates the status of a service request
    to notarize an existing certificate by over-signing the certificate and
    one or more of its current signatures.
*/
typedef uint32 CSSM_TP_CERTNOTARIZE_STATUS;
#define CSSM_TP_CERTNOTARIZE_STATUS_UNKNOWN (0x0)    /* indeterminate */
#define CSSM_TP_CERTNOTARIZE_OK (0x1)                /* cert fields were added
                                                        and the result was notarized
                                                        as requested */
#define CSSM_TP_CERTNOTARIZE_OKWITHOUTFIELDS (0x2)   /* non-conflicting cert fields
                                                        were added, conflicting cert
                                                        fields were ignored, and the
                                                        result was notarized as
                                                        requested */
#define CSSM_TP_CERTNOTARIZE_OKWITHSERVICEMODS (0x3) /* cert fields were added and
                                                        the result was notarized
                                                        as requested, but some
                                                        requested backend services
                                                        were not performed by the
                                                        notary */
#define CSSM_TP_CERTNOTARIZE_REJECTED (0x4)          /* cert was not notarized due
                                                        to some error condition */
#define CSSM_TP_CERTNOTARIZE_NOT_AUTHORIZED (0x5)    /* cert was not notarized, the
                                                        request was not authorized */

/*
    CSSM_TP_CERTNOTARIZE_OUTPUT
    This data structure aggregates the output information generated in response
    to a single request to notarize an existing certificate. Each result
    includes a certificate group composed of one or more certificates. The
    first certificate in the group is the notarized certificate.
*/
typedef struct cssm_tp_certnotarize_output 
{
    CSSM_TP_CERTNOTARIZE_STATUS NotarizeStatus;
    CSSM_CERTGROUP_PTR NotarizedCertGroup;
    CSSM_TP_SERVICES PerformedServiceRequests;
} CSSM_TP_CERTNOTARIZE_OUTPUT, *CSSM_TP_CERTNOTARIZE_OUTPUT_PTR;


/*
    CSSM_TP_CERTRECLAIM_INPUT
    This data structure aggregates the input information for a request to
    reclaim an existing certificate and recover its associated private key.
*/
typedef struct cssm_tp_certreclaim_input 
{
    CSSM_CL_HANDLE CLHandle;
    uint32 NumberOfSelectionFields;
    CSSM_FIELD_PTR SelectionFields;
    CSSM_ACCESS_CREDENTIALS_PTR UserCredentials;
} CSSM_TP_CERTRECLAIM_INPUT, *CSSM_TP_CERTRECLAIM_INPUT_PTR;

/*
    CSSM_TP_CERTRECLAIM_STATUS
    This set of defined constants indicates the status of a service request to
    reclaim a certificate.
*/
typedef uint32 CSSM_TP_CERTRECLAIM_STATUS;
#define CSSM_TP_CERTRECLAIM_STATUS_UNKNOWN (0x0) /* indeterminate */
#define CSSM_TP_CERTRECLAIM_OK (0x1)             /* a set of one or more certificates
                                                    were returned by the CA for local
                                                    recovery of the associated private
                                                    key */
#define CSSM_TP_CERTRECLAIM_NOMATCH (0x2)        /* no certificates owned by the
                                                    requester were found matching
                                                    the specified selection fields */
#define CSSM_TP_CERTRECLAIM_REJECTED (0x3)       /* certificate reclamation failed
                                                    due to some error condition */
#define CSSM_TP_CERTRECLAIM_NOT_AUTHORIZED (0x4) /* certificate reclamation was not
                                                    performed, the request
                                                    was not authorized */

/*
    CSSM_TP_CERTRECLAIM_OUTPUT
    This data structure aggregates the output information generated in response
    to a single request to reclaim a set of certificates and recovery the use
    of each associated private key. The returned certificates must be scanned
    and reclaimed individually using the functions CSSM_TP_CertReclaimKey( )
    and CSSM_TP_CertReclaimAbort( ) to recover the private key to the local system.
*/
typedef struct cssm_tp_certreclaim_output 
{
    CSSM_TP_CERTRECLAIM_STATUS ReclaimStatus;
    CSSM_CERTGROUP_PTR ReclaimedCertGroup;
    CSSM_LONG_HANDLE KeyCacheHandle;
} CSSM_TP_CERTRECLAIM_OUTPUT, *CSSM_TP_CERTRECLAIM_OUTPUT_PTR;

/*
    CSSM_TP_CRLISSUE_INPUT
    This data structure aggregates the input information for a request to issue
    the most current certificate revocation list (CRL).
*/
typedef struct cssm_tp_crlissue_input 
{
    CSSM_CL_HANDLE CLHandle;
    uint32 CrlIdentifier;
    CSSM_TIMESTRING CrlThisTime;
    CSSM_FIELD_PTR PolicyIdentifier;
    CSSM_ACCESS_CREDENTIALS_PTR CallerCredentials;
} CSSM_TP_CRLISSUE_INPUT, *CSSM_TP_CRLISSUE_INPUT_PTR;

/*
    CSSM_TP_CRLISSUE_STATUS
    This set of defined constants indicates the status of a service request
    to issue the most recent certificate revocation list.
*/
typedef uint32 CSSM_TP_CRLISSUE_STATUS;
#define CSSM_TP_CRLISSUE_STATUS_UNKNOWN (0x0)     /* indeterminate */
#define CSSM_TP_CRLISSUE_OK (0x1)                 /* a copy of the most current
                                                     CRL was issued as requested
                                                     and the time for issuing the
                                                     next CRL is also returned */
#define CSSM_TP_CRLISSUE_NOT_CURRENT (0x2)        /* either no CRL has been issued
                                                     since the CRL identified in
                                                     the request, or it is not
                                                     time to issue an updated CRL.
                                                     no CRL has been returned, but
                                                     the time for issuing the next
                                                     CRL is included in the results */
#define CSSM_TP_CRLISSUE_INVALID_DOMAIN (0x3)     /* CRL domain was not recognized
                                                     or was outside the CA jurisdiction,
                                                     no CRL or time for the next CRL
                                                     has been returned */
#define CSSM_TP_CRLISSUE_UNKNOWN_IDENTIFIER (0x4) /* unrecognized CRL identifier,
                                                     no CRL or time for the next
                                                     CRL has been returned */
#define CSSM_TP_CRLISSUE_REJECTED (0x5)           /* CRL was not issued due to some
                                                     error condition, no CRL or time
                                                     for the next CRL has been returned */
#define CSSM_TP_CRLISSUE_NOT_AUTHORIZED (0x6)     /* CRL was not issued, the request
                                                     was not authorized, no CRL or time
                                                     for the next CRL has been returned */

/*
    CSSM_TP_CRLISSUE_OUTPUT
    This data structure aggregates the output information generated in
    response to a single request to issue a copy of the most recent
    certificate revocation list (CRL).
*/
typedef struct cssm_tp_crlissue_output 
{
    CSSM_TP_CRLISSUE_STATUS IssueStatus;
    CSSM_ENCODED_CRL_PTR Crl;
    CSSM_TIMESTRING CrlNextTime;
} CSSM_TP_CRLISSUE_OUTPUT, *CSSM_TP_CRLISSUE_OUTPUT_PTR;


/*
    CSSM_TP_FORM_TYPE
*/
typedef uint32 CSSM_TP_FORM_TYPE;
#define CSSM_TP_FORM_TYPE_GENERIC (0x0)
#define CSSM_TP_FORM_TYPE_REGISTRATION (0x1)

/*****************************************************************************
    Auth Compute Services API Datatypes
*****************************************************************************/

/*
    CSSM_AC_HANDLE defined under Core Services API Datatypes
*/




/*****************************************************************************
    Application Memory Functions defined under Core Services API Datatypes
*****************************************************************************/

/*****************************************************************************
    Key Recovery API Datatypes
*****************************************************************************/
/*
    CSSM_KRSP_HANDLE defined under Core Services API Datatypes
*/

/*
    CSSM_KR_NAME
    This data structure contains a typed name. The namespace type specifies
    what kind of name is contained in the third parameter.
*/
typedef struct cssm_kr_name 
{
    uint8 Type; /* namespace type */
    uint8 Length; /* name string length */
    char *Name; /* name string */
} CSSM_KR_NAME;


/*
    CSSM_KR_PROFILE
    This data structure encapsulates the key recovery profile for a given user
    and a given key recovery mechanism.
*/
typedef struct cssm_kr_profile 
{
    CSSM_KR_NAME UserName;              /* name of the user */
    CSSM_CERTGROUP_PTR UserCertificate; /* public key certificate of the user */
    CSSM_CERTGROUP_PTR KRSCertChain;    /* cert chain for the KRSP coordinator */
    uint8 LE_KRANum;                    /* number of KRA cert chains in the following list */
    CSSM_CERTGROUP_PTR LE_KRACertChainList; /* list of Law enforcement KRA certificate chains*/
    uint8 ENT_KRANum;                   /* number of KRA cert chains in the following list */
    CSSM_CERTGROUP_PTR ENT_KRACertChainList; /* list of Enterprise KRA certificate chains*/
    uint8 INDIV_KRANum;                 /* number of KRA cert chains in the following list */
    CSSM_CERTGROUP_PTR INDIV_KRACertChainList; /* list of Individual KRA certificate chains*/
    CSSM_DATA_PTR INDIV_AuthenticationInfo; /* authentication information for individual key recovery */
    uint32 KRSPFlags;                   /* flag values to be interpreted by KRSP */
    CSSM_DATA_PTR KRSPExtensions;       /* reserved for extensions specific to KRSPs */
} CSSM_KR_PROFILE; 


/*
    CSSM_KR_POLICY_FLAGS
*/
typedef uint32 CSSM_KR_POLICY_FLAGS;
#define CSSM_KR_INDIV (0x00000001)
#define CSSM_KR_ENT (0x00000002)
#define CSSM_KR_LE_MAN (0x00000004)
#define CSSM_KR_LE_USE (0x00000008)
#define CSSM_KR_LE (CSSM_KR_LE_MAN|CSSM_KR_LE_USE)
#define CSSM_KR_OPTIMIZE (0x00000010)
#define CSSM_KR_DROP_WORKFACTOR (0x00000020)


/*
    CSSM_KR_POLICY_LIST_ITEM
*/
typedef struct cssm_kr_policy_list_item 
{
    struct kr_policy_list_item *next;
    CSSM_ALGORITHMS AlgorithmId;
    CSSM_ENCRYPT_MODE Mode;
    uint32 MaxKeyLength;
    uint32 MaxRounds;
    uint8 WorkFactor;
    CSSM_KR_POLICY_FLAGS PolicyFlags;
    CSSM_CONTEXT_TYPE AlgClass;
} CSSM_KR_POLICY_LIST_ITEM, *CSSM_KR_POLICY_LIST_ITEM_PTR;


/*
    CSSM_KR_POLICY_INFO
*/
typedef struct cssm_kr_policy_info 
{
    CSSM_BOOL krbNotAllowed;
    uint32 numberOfEntries;
    CSSM_KR_POLICY_LIST_ITEM *policyEntry;
} CSSM_KR_POLICY_INFO, *CSSM_POLICY_KR_INFO_PTR;

/*****************************************************************************
    Add-In Module Administration Datatypes
*****************************************************************************/

/*
    CSSM_CONTEXT_EVENT_TYPE
    This list defines event in the lifecycle of cryptographic contexts.
*/
typedef enum cssm_context_event
{
    CSSM_CONTEXT_EVENT_CREATE = 1,
    CSSM_CONTEXT_EVENT_DELETE = 2,
    CSSM_CONTEXT_EVENT_UPDATE = 3
} CSSM_CONTEXT_EVENT;


/*
    CSSM_UPCALLS
    This structure is used by CSSM to provide service functions to add-in
    service modules and elective module managers.
*/

#ifndef CSSM_MEMTRACK_ON

typedef void * (CSSMAPI *CSSM_UPCALLS_MALLOC)
                (CSSM_HANDLE AddInHandle,
                 uint32 size);
typedef void (CSSMAPI *CSSM_UPCALLS_FREE)
                (CSSM_HANDLE AddInHandle,
                 void *memblock);
typedef void * (CSSMAPI *CSSM_UPCALLS_REALLOC)
                (CSSM_HANDLE AddInHandle,
                 void *memblock,
                 uint32 size);
typedef void * (CSSMAPI *CSSM_UPCALLS_CALLOC)
                (CSSM_HANDLE AddInHandle,
                 uint32 num,
                 uint32 size);

#else   /* CSSM_MEMTRACK_ON */

typedef void * (CSSMAPI *CSSM_UPCALLS_MALLOC)
                (CSSM_HANDLE AddInHandle,
                 uint32 size,
                 const char *File,
                 uint32 Line);
typedef void (CSSMAPI *CSSM_UPCALLS_FREE)
                (CSSM_HANDLE AddInHandle,
                 void *memblock,
                 const char *File,
                 uint32 Line);
typedef void * (CSSMAPI *CSSM_UPCALLS_REALLOC)
                (CSSM_HANDLE AddInHandle,
                 void *memblock,
                 uint32 size,
                 const char *File,
                 uint32 Line);
typedef void * (CSSMAPI *CSSM_UPCALLS_CALLOC)
                (CSSM_HANDLE AddInHandle,
                 uint32 num,
                 uint32 size,
                 const char *File,
                 uint32 Line);

#endif /* CSSM_MEMTRACK_ON */

typedef struct cssm_upcalls 
{
    CSSM_UPCALLS_MALLOC malloc_func;
    CSSM_UPCALLS_FREE free_func;
    CSSM_UPCALLS_REALLOC realloc_func;
    CSSM_UPCALLS_CALLOC calloc_func;
    CSSM_RETURN (CSSMAPI *CcToHandle_func)
                (CSSM_CC_HANDLE Cc,
                CSSM_MODULE_HANDLE_PTR ModuleHandle);
    CSSM_RETURN (CSSMAPI *GetModuleInfo_func)
                (CSSM_MODULE_HANDLE Module,
                CSSM_GUID_PTR Guid,
                CSSM_VERSION_PTR Version,
                uint32 *SubServiceId,
                CSSM_SERVICE_TYPE *SubServiceType,
                CSSM_ATTACH_FLAGS *AttachFlags,
                CSSM_KEY_HIERARCHY *KeyHierarchy,
                CSSM_API_MEMORY_FUNCS_PTR AttachedMemFuncs,
                CSSM_FUNC_NAME_ADDR_PTR FunctionTable,
                uint32 *NumFunctionTable);
} CSSM_UPCALLS, *CSSM_UPCALLS_PTR;


#endif /* _CSSMTYPE_H */
