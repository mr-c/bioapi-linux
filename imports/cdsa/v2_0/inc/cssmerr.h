/*-----------------------------------------------------------------------
 * File: CSSMERR.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#ifndef _CSSMERR_H
#define _CSSMERR_H

/*************************************************************************
   Error code section (Appendix A)
**************************************************************************/

/* Configurable CSSM Error Code Constants */

/* The implementation-defined CSSM error code base value */
#define CSSM_BASE_ERROR (CSSM_ERRORCODE_MODULE_EXTENT)


/*
    The implementation-defined number of error codes allocated for each module type. This
    number must be greater than CSSM_ERRORCODE_COMMON_EXTENT, and should allow at
    least half the space for spec-defined error codes.
*/
#define CSSM_ERRORCODE_MODULE_EXTENT (0x00001000)

/*
    The platform-specific offset at which custom error codes are allocated. Must be greater than
    CSSM_ERRCODE_COMMON_EXTENT and less than CSSM_ERRORCODE_MODULE_EXTENT.
*/
#define CSSM_ERRORCODE_CUSTOM_OFFSET (0x00000800)


/* CSSM Error Code Constants */
#define CSSM_ERRORCODE_COMMON_EXTENT (0x100) /*The number of error codes allocated to indicate "common" errors. */

#define CSSM_CSSM_BASE_ERROR	(CSSM_BASE_ERROR)
#define CSSM_CSSM_PRIVATE_ERROR	(CSSM_BASE_ERROR + CSSM_ERRORCODE_CUSTOM_OFFSET)
#define CSSM_CSP_BASE_ERROR		(CSSM_CSSM_BASE_ERROR + CSSM_ERRORCODE_MODULE_EXTENT)
#define CSSM_CSP_PRIVATE_ERROR	(CSSM_CSP_BASE_ERROR + CSSM_ERRORCODE_CUSTOM_OFFSET)
#define CSSM_DL_BASE_ERROR		(CSSM_CSP_BASE_ERROR + CSSM_ERRORCODE_MODULE_EXTENT)
#define CSSM_DL_PRIVATE_ERROR	(CSSM_DL_BASE_ERROR + CSSM_ERRORCODE_CUSTOM_OFFSET)
#define CSSM_CL_BASE_ERROR		(CSSM_DL_BASE_ERROR + CSSM_ERRORCODE_MODULE_EXTENT)
#define CSSM_CL_PRIVATE_ERROR	(CSSM_CL_BASE_ERROR + CSSM_ERRORCODE_CUSTOM_OFFSET)
#define CSSM_TP_BASE_ERROR		(CSSM_CL_BASE_ERROR + CSSM_ERRORCODE_MODULE_EXTENT)
#define CSSM_TP_PRIVATE_ERROR	(CSSM_TP_BASE_ERROR + CSSM_ERRORCODE_CUSTOM_OFFSET )
#define CSSM_KR_BASE_ERROR		(CSSM_TP_BASE_ERROR + CSSM_ERRORCODE_MODULE_EXTENT)
#define CSSM_KR_PRIVATE_ERROR	(CSSM_KR_BASE_ERROR + CSSM_ERRORCODE_CUSTOM_OFFSET)
#define CSSM_AC_BASE_ERROR		(CSSM_KR_BASE_ERROR + CSSM_ERRORCODE_MODULE_EXTENT)
#define CSSM_AC_PRIVATE_ERROR	(CSSM_AC_BASE_ERROR + CSSM_ERRORCODE_CUSTOM_OFFSET)

/* 
    General Error Values 
    The following error values can be returned from any CSSM API.
*/
#define CSSMERR_CSSM_INVALID_ADDIN_HANDLE \
(CSSM_CSSM_BASE_ERROR + CSSM_ERRORCODE_COMMON_EXTENT + 1)
/* The given service provider handle is not valid */

#define CSSMERR_CSSM_NOT_INITIALIZED \
(CSSM_CSSM_BASE_ERROR + CSSM_ERRORCODE_COMMON_EXTENT + 2)
/* A function is called without initializing CSSM */

#define CSSMERR_CSSM_INVALID_HANDLE_USAGE \
(CSSM_CSSM_BASE_ERROR + CSSM_ERRORCODE_COMMON_EXTENT + 3)
/* Handle does not match with service type */

#define CSSMERR_CSSM_PVC_REFERENT_NOT_FOUND \
(CSSM_CSSM_BASE_ERROR + CSSM_ERRORCODE_COMMON_EXTENT + 4)
/* A reference to the calling module was not found in the list of authorized callers. */

#define CSSMERR_CSSM_FUNCTION_INTEGRITY_FAIL \
(CSSM_CSSM_BASE_ERROR + CSSM_ERRORCODE_COMMON_EXTENT + 5)
/* Function address is not within the verified module */


/* 
    Common Error Codes For All Module Types
    The following codes can be returned by multiple module types.
*/
#define CSSM_ERRCODE_INTERNAL_ERROR (0x0001)
/* General system error; indicates that an operating system or internal state error has occurred and 
the system may not be in a known state */

#define CSSM_ERRCODE_MEMORY_ERROR ( 0x0002 )
/* A memory error occurred */

#define CSSM_ERRCODE_MDS_ERROR ( 0x0003 )
/* The MDS could not be accessed to complete the operation */

#define CSSM_ERRCODE_INVALID_POINTER (0x0004)
/* An input/output function parameter or input/output field inside of a data structure is an invalid pointer */

#define CSSM_ERRCODE_INVALID_INPUT_POINTER (0x0005)
/* An input function parameter or input field in a data structure is an invalid pointer */

#define CSSM_ERRCODE_INVALID_OUTPUT_POINTER (0x0006)
/* An output function parameter or output field in a data structure is an invalid pointer */

#define CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED (0x0007)
/* The function is not implemented by the service provider */

#define CSSM_ERRCODE_SELF_CHECK_FAILED (0x0008)
/* The module failed a self-check */

#define CSSM_ERRCODE_OS_ACCESS_DENIED (0x0009)
/* The operating system denied access to a required resource */

#define CSSM_ERRCODE_FUNCTION_FAILED (0x000A)
/* The function failed for an unknown reason. */

#define CSSM_ERRCODE_MODULE_MANIFEST_VERIFY_FAILED (0x000B)
/* Module’s manifest verification failed */

#define CSSM_ERRCODE_INVALID_GUID (0x000C)
/* Invalid GUID */


/*
    Common Error Codes for ACLs
    The following code enumeration values apply to any module that supports ACLs.
*/
#define CSSM_ERRCODE_OPERATION_AUTH_DENIED (0x0020)
/* The access credentials were insufficient to permit the requested action to complete */

#define CSSM_ERRCODE_OBJECT_USE_AUTH_DENIED (0x0021)
/* The access credentials were insufficient to permit the associated object to be used */

#define CSSM_ERRCODE_OBJECT_MANIP_AUTH_DENIED (0x0022)
/* The access credentials were insufficient to permit manipulation of the object that is the target of
   an operation (i.e. CSSM_WrapKey(),CSSM_UnwrapKey( )) */

#define CSSM_ERRCODE_OBJECT_ACL_NOT_SUPPORTED (0x0023)
/* An ACL is specified for a new object and the service provider does not support ACLs for objects of that type */

#define CSSM_ERRCODE_OBJECT_ACL_REQUIRED (0x0024)
/* An ACL is not specified for a new object and the service provider requires an ACLs for objects of that type */

#define CSSM_ERRCODE_INVALID_ACCESS_CREDENTIALS (0x0025)
/* The access credentials are invalid */

#define CSSM_ERRCODE_INVALID_ACL_BASE_CERTS (0x0026)
/* The base certificates were corrupt or not recognized as the indicated type */

#define CSSM_ERRCODE_ACL_BASE_CERTS_NOT_SUPPORTED (0x0027)
/* The type of at least one certificate in the base certificates is not supported */

#define CSSM_ERRCODE_INVALID_SAMPLE_VALUE (0x0028)
/* The sample value is corrupt or not recognized as the indicated type */

#define CSSM_ERRCODE_SAMPLE_VALUE_NOT_SUPPORTED (0x0029)
/* The type of at least one sample is not supported */

#define CSSM_ERRCODE_INVALID_ACL_SUBJECT_VALUE (0x002A)
/* The subject value is corrupt or not recognized as the indicated type */

#define CSSM_ERRCODE_ACL_SUBJECT_TYPE_NOT_SUPPORTED (0x002B)
/* The type of the subject value is not supported */

#define CSSM_ERRCODE_INVALID_ACL_CHALLENGE_CALLBACK (0x002C)
/* The challenge or subject callback function pointer is invalid */

#define CSSM_ERRCODE_ACL_CHALLENGE_CALLBACK_FAILED (0x002D)
/* The challenge or subject callback to the client failed */

#define CSSM_ERRCODE_INVALID_ACL_ENTRY_TAG (0x002E)
/* The entry tag value is not valid. */

#define CSSM_ERRCODE_ACL_ENTRY_TAG_NOT_FOUND (0x002F)
/* No ACL entry was found with the specified entry tag */

#define CSSM_ERRCODE_INVALID_ACL_EDIT_MODE (0x0030)
/* The edit mode specified when changing an ACL entry is not valid */

#define CSSM_ERRCODE_ACL_CHANGE_FAILED (0x0031)
/* The ACL update operation failed */

#define CSSM_ERRCODE_INVALID_NEW_ACL_ENTRY (0x0032)
/* The ACL entry specified for an initial or modified value is invalid */

#define CSSM_ERRCODE_INVALID_NEW_ACL_OWNER (0x0033)
/* The ACL owner specified for a modified value is invalid */

#define CSSM_ERRCODE_ACL_DELETE_FAILED (0x0034)
/* ACL entry to be deleted was not found */

#define CSSM_ERRCODE_ACL_REPLACE_FAILED (0x0035)
/* ACL entry to be replaced was not found */

#define CSSM_ERRCODE_ACL_ADD_FAILED (0x0036)
/* Unable to add new ACL entry */



/*  Common Error Codes for Specific Data Types */


/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input a Cryptographic Context Handle.
*/
#define CSSM_ERRCODE_INVALID_CONTEXT_HANDLE (0x0040)
/* Invalid context handle */


/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input a version.
*/
#define CSSM_ERRCODE_INCOMPATIBLE_VERSION (0x0041)
/*  Version is not compatible with the current version */



/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input the associated input pointer types.
*/
#define CSSM_ERRCODE_INVALID_CERTGROUP_POINTER (0x0042)
/* Invalid pointer for certificate group */

#define CSSM_ERRCODE_INVALID_CERT_POINTER (0x0043)
/* Invalid pointer for certificate */

#define CSSM_ERRCODE_INVALID_CRL_POINTER (0x0044)
/* Invalid pointer for certificate revocation list */

#define CSSM_ERRCODE_INVALID_FIELD_POINTER (0x0045)
/* Invalid pointer for certificate or CRL fields (OID/value pairs) */



/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input a CSSM_DATA.
*/
#define CSSM_ERRCODE_INVALID_DATA (0x0046)
/* The data in an input parameter is invalid */



/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input an encoded unsigned CRL.
*/
#define CSSM_ERRCODE_CRL_ALREADY_SIGNED (0x0047)
/* Attempted to modify a CRL that has already been signed */



/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input a number of fields.
*/
#define CSSM_ERRCODE_INVALID_NUMBER_OF_FIELDS (0x0048)
/* Invalid argument for number of certificate fields */



/*
    Error values with the following code enumeration values may be returned from any function
    whose operation includes verification of a certificate or CRL.
*/
#define CSSM_ERRCODE_VERIFICATION_FAILURE (0x0049)
/* Certificate or CRL verification failed */



/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input a DB handle.
*/
#define CSSM_ERRCODE_INVALID_DB_HANDLE (0x004A)
/* Invalid database handle */



/*
    Error values with the following code enumeration values may be returned from any function
    that deals with privilege.
*/
#define CSSM_ERRCODE_PRIVILEGE_NOT_GRANTED (0x004B)
/* Requested privilege has not been granted to the module requesting the privilege */



/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input a CSSM_DL_DB_LIST.
*/
#define CSSM_ERRCODE_INVALID_DB_LIST (0x004C)
/* Invalid CSSM_DL_DB_LIST. */

#define CSSM_ERRCODE_INVALID_DB_LIST_POINTER (0x004D)
/* Invalid database list pointer */



/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input a certificate template, certificate or CRL.
*/
#define CSSM_ERRCODE_UNKNOWN_FORMAT (0x004E)
/* Unknown format or invalid structure for certificate template, certificate or CRL */

#define CSSM_ERRCODE_UNKNOWN_TAG (0x004F)
/* Unknown OID specified in certificate template, certificate or CRL field */



/*
    Error values with the following code enumeration values may be returned from any function
    that takes as input the associated handle.
*/
#define CSSM_ERRCODE_INVALID_CSP_HANDLE (0x0050)
/* Invalid CSP handle */

#define CSSM_ERRCODE_INVALID_DL_HANDLE (0x0051)
/* Invalid DL handle */

#define CSSM_ERRCODE_INVALID_CL_HANDLE (0x0052)
/* Invalid CL handle */

#define CSSM_ERRCODE_INVALID_TP_HANDLE (0x0053)
/* Invalid TP handle */

#define CSSM_ERRCODE_INVALID_KR_HANDLE (0x0054)
/* Invalid KR handle */

#define CSSM_ERRCODE_INVALID_AC_HANDLE (0x0055)
/* Invalid AC handle */



/* 
    Error values with the following code enumeration values may be returned from any function
    that takes as input a passthrough ID.
*/
#define CSSM_ERRCODE_INVALID_PASSTHROUGH_ID (0x0056)
/* Invalid passthrough ID */

#define CSSM_ERRCODE_INVALID_NETWORK_ADDR (0x0057)
/* The network address is invalid */

#define CSSM_ERRCODE_INVALID_CRYPTO_DATA (0x0058)
/* The CSSM_CRYPTO_DATA structure is invalid */

/*************************************************************************
    Core Services API Error Values 
**************************************************************************/

/* Error Values Derived from Common Error Codes */
#define CSSMERR_CSSM_INTERNAL_ERROR \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_INTERNAL_ERROR)
#define CSSMERR_CSSM_MEMORY_ERROR \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_MEMORY_ERROR)
#define CSSMERR_CSSM_MDS_ERROR \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_MDS_ERROR)
#define CSSMERR_CSSM_INVALID_POINTER \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_INVALID_POINTER)
#define CSSMERR_CSSM_INVALID_INPUT_POINTER \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_INVALID_INPUT_POINTER)
#define CSSMERR_CSSM_INVALID_OUTPUT_POINTER \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_INVALID_OUTPUT_POINTER)
#define CSSMERR_CSSM_FUNCTION_NOT_IMPLEMENTED \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED)
#define CSSMERR_CSSM_SELF_CHECK_FAILED \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_SELF_CHECK_FAILED)
#define CSSMERR_CSSM_OS_ACCESS_DENIED \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_OS_ACCESS_DENIED)
#define CSSMERR_CSSM_FUNCTION_FAILED \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_FUNCTION_FAILED)
#define CSSMERR_CSSM_MODULE_MANIFEST_VERIFY_FAILED \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_MODULE_MANIFEST_VERIFY_FAILED)
#define CSSMERR_CSSM_INVALID_GUID \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_INVALID_GUID)
#define CSSMERR_CSSM_INVALID_CONTEXT_HANDLE \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_INVALID_CONTEXT_HANDLE)
#define CSSMERR_CSSM_INCOMPATIBLE_VERSION \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_INCOMPATIBLE_VERSION)
#define CSSMERR_CSSM_PRIVILEGE_NOT_GRANTED \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRCODE_PRIVILEGE_NOT_GRANTED)

/* CSSM Module-Specific Error Values */

#define CSSM_CSSM_BASE_CSSM_ERROR \
(CSSM_CSSM_BASE_ERROR+CSSM_ERRORCODE_COMMON_EXTENT+0x10)
/* The first 16 CSSM Error Codes are reserved for general errors */

#define CSSMERR_CSSM_SCOPE_NOT_SUPPORTED (CSSM_CSSM_BASE_CSSM_ERROR+0x01)
/* Privilege scope requested is not supported in the platform */

#define CSSMERR_CSSM_PVC_ALREADY_CONFIGURED (CSSM_CSSM_BASE_CSSM_ERROR+0x02)
/* PvcPolicy is already configured in the first call to CSSM_Init */

#define CSSMERR_CSSM_INVALID_PVC (CSSM_CSSM_BASE_CSSM_ERROR+0x03)
/* PvcPolicy requested is invalid */

#define CSSMERR_CSSM_EMM_LOAD_FAILED (CSSM_CSSM_BASE_CSSM_ERROR+0x04)
/* EMM load failed */

#define CSSMERR_CSSM_EMM_UNLOAD_FAILED (CSSM_CSSM_BASE_CSSM_ERROR+0x05)
/* EMM unload failed */

#define CSSMERR_CSSM_ADDIN_LOAD_FAILED (CSSM_CSSM_BASE_CSSM_ERROR+0x06)
/* Addin Load function failed */

#define CSSMERR_CSSM_INVALID_KEY_HIERARCHY (CSSM_CSSM_BASE_CSSM_ERROR+0x07)
/* Invalid key hierarchy requested */

#define CSSMERR_CSSM_ADDIN_UNLOAD_FAILED (CSSM_CSSM_BASE_CSSM_ERROR+0x08)
/* Addin Unload function failed */

#define CSSMERR_CSSM_LIB_REF_NOT_FOUND (CSSM_CSSM_BASE_CSSM_ERROR+0x09)
/* A reference to the loaded library cannot be obtained */

#define CSSMERR_CSSM_INVALID_ADDIN_FUNCTION_TABLE \
(CSSM_CSSM_BASE_CSSM_ERROR+0x0a)
/* Addin function table registered with CSSM is invalid */

#define CSSMERR_CSSM_EMM_AUTHENTICATE_FAILED \
(CSSM_CSSM_BASE_CSSM_ERROR+0x0b)
/* ModuleManager authentication failed */

#define CSSMERR_CSSM_ADDIN_AUTHENTICATE_FAILED \
(CSSM_CSSM_BASE_CSSM_ERROR+0x0c)
/* Addin authenticate function failed */

#define CSSMERR_CSSM_INVALID_SERVICE_MASK \
(CSSM_CSSM_BASE_CSSM_ERROR+0x0d)
/* Invalid service mask */

#define CSSMERR_CSSM_MODULE_NOT_LOADED (CSSM_CSSM_BASE_CSSM_ERROR+0x0e)
/* Module was not loaded */

#define CSSMERR_CSSM_INVALID_SUBSERVICEID (CSSM_CSSM_BASE_CSSM_ERROR+0x0f)
/* Invalid subservice Id was requested */

#define CSSMERR_CSSM_BUFFER_TOO_SMALL (CSSM_CSSM_BASE_CSSM_ERROR+0x10)
/* Buffer size for the ModuleManagerGuids is less than the required size */

#define CSSMERR_CSSM_INVALID_ATTRIBUTE (CSSM_CSSM_BASE_CSSM_ERROR+0x11)
/* Invalid attribute in context */

#define CSSMERR_CSSM_ATTRIBUTE_NOT_IN_CONTEXT \
(CSSM_CSSM_BASE_CSSM_ERROR+0x12)
/* Requested attribute is not in the context */

#define CSSMERR_CSSM_MODULE_MANAGER_INITIALIZE_FAIL \
(CSSM_CSSM_BASE_CSSM_ERROR+0x13)
/* ModuleManger initialize failed */

#define CSSMERR_CSSM_MODULE_MANAGER_NOT_FOUND \
(CSSM_CSSM_BASE_CSSM_ERROR+0x14)
/* ModuleManger to be notified is not loaded */

#define CSSMERR_CSSM_EVENT_NOTIFICATION_CALLBACK_NOT_FOUND \
(CSSM_CSSM_BASE_CSSM_ERROR+0x15)
/* Event Notification callback not found */

/*************************************************************************
    Cryptographic Services API Error Values 
**************************************************************************/

/* Common Error Values for All Module Types */
#define CSSMERR_CSP_INTERNAL_ERROR \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INTERNAL_ERROR)
#define CSSMERR_CSP_MEMORY_ERROR \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_MEMORY_ERROR)
#define CSSMERR_CSP_MDS_ERROR \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_MDS_ERROR)
#define CSSMERR_CSP_INVALID_POINTER \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_POINTER)
#define CSSMERR_CSP_INVALID_INPUT_POINTER \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_INPUT_POINTER)
#define CSSMERR_CSP_INVALID_OUTPUT_POINTER \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_OUTPUT_POINTER)
#define CSSMERR_CSP_FUNCTION_NOT_IMPLEMENTED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED)
#define CSSMERR_CSP_SELF_CHECK_FAILED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_SELF_CHECK_FAILED)
#define CSSMERR_CSP_OS_ACCESS_DENIED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_OS_ACCESS_DENIED)
#define CSSMERR_CSP_FUNCTION_FAILED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_FUNCTION_FAILED)


/* Common ACL Error Values */
#define CSSMERR_CSP_OPERATION_AUTH_DENIED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_OPERATION_AUTH_DENIED)
#define CSSMERR_CSP_OBJECT_USE_AUTH_DENIED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_OBJECT_USE_AUTH_DENIED)
#define CSSMERR_CSP_OBJECT_MANIP_AUTH_DENIED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_OBJECT_MANIP_AUTH_DENIED)
#define CSSMERR_CSP_OBJECT_ACL_NOT_SUPPORTED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_OBJECT_ACL_NOT_SUPPORTED)
#define CSSMERR_CSP_OBJECT_ACL_REQUIRED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_OBJECT_ACL_REQUIRED)
#define CSSMERR_CSP_INVALID_ACCESS_CREDENTIALS \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_ACCESS_CREDENTIALS)
#define CSSMERR_CSP_INVALID_ACL_BASE_CERTS \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_BASE_CERTS)
#define CSSMERR_CSP_ACL_BASE_CERTS_NOT_SUPPORTED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_ACL_BASE_CERTS_NOT_SUPPORTED)
#define CSSMERR_CSP_INVALID_SAMPLE_VALUE \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_SAMPLE_VALUE)
#define CSSMERR_CSP_SAMPLE_VALUE_NOT_SUPPORTED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_SAMPLE_VALUE_NOT_SUPPORTED)
#define CSSMERR_CSP_INVALID_ACL_SUBJECT_VALUE \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_SUBJECT_VALUE)
#define CSSMERR_CSP_ACL_SUBJECT_TYPE_NOT_SUPPORTED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_ACL_SUBJECT_TYPE_NOT_SUPPORTED)
#define CSSMERR_CSP_INVALID_ACL_CHALLENGE_CALLBACK \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_CHALLENGE_CALLBACK)
#define CSSMERR_CSP_ACL_CHALLENGE_CALLBACK_FAILED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_ACL_CHALLENGE_CALLBACK_FAILED)
#define CSSMERR_CSP_INVALID_ACL_ENTRY_TAG \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_ENTRY_TAG)
#define CSSMERR_CSP_ACL_ENTRY_TAG_NOT_FOUND \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_ACL_ENTRY_TAG_NOT_FOUND)
#define CSSMERR_CSP_INVALID_ACL_EDIT_MODE \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_EDIT_MODE)
#define CSSMERR_CSP_ACL_CHANGE_FAILED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_ACL_CHANGE_FAILED)
#define CSSMERR_CSP_INVALID_NEW_ACL_ENTRY \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_NEW_ACL_ENTRY)
#define CSSMERR_CSP_INVALID_NEW_ACL_OWNER \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_NEW_ACL_OWNER)
#define CSSMERR_CSP_ACL_DELETE_FAILED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_ACL_DELETE_FAILED)
#define CSSMERR_CSP_ACL_REPLACE_FAILED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_ACL_REPLACE_FAILED)
#define CSSMERR_CSP_ACL_ADD_FAILED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_ACL_ADD_FAILED)

/* Common Error Values for Specific Data Types */
#define CSSMERR_CSP_INVALID_CONTEXT_HANDLE \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_CONTEXT_HANDLE)
#define CSSMERR_CSP_PRIVILEGE_NOT_GRANTED \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_PRIVILEGE_NOT_GRANTED)
#define CSSMERR_CSP_INVALID_DATA \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_DATA)
#define CSSMERR_CSP_INVALID_PASSTHROUGH_ID \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_PASSTHROUGH_ID)
#define CSSMERR_CSP_INVALID_CRYPTO_DATA \
(CSSM_CSP_BASE_ERROR+CSSM_ERRCODE_INVALID_CRYPTO_DATA)

/* 
    General CSP Error Values
    These error values can be returned from any CSP function.
*/
#define CSSM_CSP_BASE_CSP_ERROR \
(CSSM_CSP_BASE_ERROR+CSSM_ERRORCODE_COMMON_EXTENT)
#define CSSMERR_CSP_INPUT_LENGTH_ERROR (CSSM_CSP_BASE_CSP_ERROR+0x01)
/* An input buffer does not have the expected length */

#define CSSMERR_CSP_OUTPUT_LENGTH_ERROR (CSSM_CSP_BASE_CSP_ERROR+0x02)
/* An output buffer was supplied, but it was too small to hold the output data */

#define CSSMERR_CSP_PRIVILEGE_NOT_SUPPORTED (CSSM_CSP_BASE_CSP_ERROR+0x03)
/* The CSP does not support the requested privilege level */

#define CSSMERR_CSP_DEVICE_ERROR (CSSM_CSP_BASE_CSP_ERROR+0x04)
/* General device error; Indicates that a hardware subsystem has failed in some way */

#define CSSMERR_CSP_DEVICE_MEMORY_ERROR (CSSM_CSP_BASE_CSP_ERROR+0x05)
/* General device error; Indicates that a hardware subsystem has run out of memory */

#define CSSMERR_CSP_ATTACH_HANDLE_BUSY (CSSM_CSP_BASE_CSP_ERROR+0x06)
/* The attach handle used to attempt an operation currently has an operation in progress that will
   not allow other operations to begin until it completes */
#define CSSMERR_CSP_NOT_LOGGED_IN (CSSM_CSP_BASE_CSP_ERROR+0x07)
/* The operation can not be performed without authenticating using CSSM_CSP_Login */



/*
    CSP Key Error Values
    These error values can be returned from CSP functions that use a key. The key may be passed
    directly into the function or specified as an attribute through the cryptographic context.
*/
#define CSSMERR_CSP_INVALID_KEY (CSSM_CSP_BASE_CSP_ERROR+0x10)
/* The supplied key is invalid or incompatible with the operation */

#define CSSMERR_CSP_INVALID_KEY_REFERENCE (CSSM_CSP_BASE_CSP_ERROR+0x11)
/* The CSSM_KEY contains a reference that does not indicate a key in the CSP */

#define CSSMERR_CSP_INVALID_KEY_CLASS (CSSM_CSP_BASE_CSP_ERROR+0x12)
/* The supplied key is not the proper class (i.e. Public key is supplied for a private key operation) */

#define CSSMERR_CSP_ALGID_MISMATCH (CSSM_CSP_BASE_CSP_ERROR+0x13)
/* The algorithm ID in the key header does not match the algorithm to be performed */

#define CSSMERR_CSP_KEY_USAGE_INCORRECT (CSSM_CSP_BASE_CSP_ERROR+0x14)
/* The key does not have the proper usage flags to perform the operation */

#define CSSMERR_CSP_KEY_BLOB_TYPE_INCORRECT (CSSM_CSP_BASE_CSP_ERROR+0x15)
/*  The key data blob type is not the correct type (i.e. The key is wrapped when a raw key or
    reference is expected) */

#define CSSMERR_CSP_KEY_HEADER_INCONSISTENT (CSSM_CSP_BASE_CSP_ERROR+0x16)
/* The key header information is corrupt, or does not match the key data */

#define CSSMERR_CSP_UNSUPPORTED_KEY_FORMAT (CSSM_CSP_BASE_CSP_ERROR+0x17)
/* The key format is not supported by the CSP */

#define CSSMERR_CSP_UNSUPPORTED_KEY_SIZE (CSSM_CSP_BASE_CSP_ERROR+0x18)
/* The key size is not supported or not allowed by the current privilege or supported by the CSP */

#define CSSMERR_CSP_INVALID_KEY_POINTER (CSSM_CSP_BASE_CSP_ERROR+0x19)
/* The pointer to a CSSM_KEY structure is invalid */

#define CSSMERR_CSP_INVALID_KEYUSAGE_MASK (CSSM_CSP_BASE_CSP_ERROR+0x1a)
/* A requested key usage is not valid for the key type, or two of the uses are not compatible */

#define CSSMERR_CSP_UNSUPPORTED_KEYUSAGE_MASK (CSSM_CSP_BASE_CSP_ERROR+0x1b)
/* The key usage mask is valid, but not supported by the CSP */

#define CSSMERR_CSP_INVALID_KEYATTR_MASK (CSSM_CSP_BASE_CSP_ERROR+0x1c)
/* A requested key attribute is not valid for the key type, or two of the attributes are not compatible */

#define CSSMERR_CSP_UNSUPPORTED_KEYATTR_MASK (CSSM_CSP_BASE_CSP_ERROR+0x1d)
/* The key attribute mask is valid, but not supported by the CSP */

#define CSSMERR_CSP_INVALID_KEY_LABEL (CSSM_CSP_BASE_CSP_ERROR+0x1e)
/* The label specified for a key is invalid */

#define CSSMERR_CSP_UNSUPPORTED_KEY_LABEL (CSSM_CSP_BASE_CSP_ERROR+0x1f)
/* The CSP does not support the use of labels for the key. */

#define CSSMERR_CSP_INVALID_KEY_FORMAT (CSSM_CSP_BASE_CSP_ERROR+0x20)
/* Invalid key format */




/*
    CSP Vector of Buffers Error Values
    These error values can be returned by APIs that accept a vector of buffers as input or output.
*/
#define CSSMERR_CSP_INVALID_DATA_COUNT (CSSM_CSP_BASE_CSP_ERROR+0x28)
/* Input vector length is invalid; buffer count can not be zero. */

#define CSSMERR_CSP_VECTOR_OF_BUFS_UNSUPPORTED (CSSM_CSP_BASE_CSP_ERROR+0x29)
/* The CSP only supports input of a single buffer per API call */

#define CSSMERR_CSP_INVALID_INPUT_VECTOR (CSSM_CSP_BASE_CSP_ERROR+0x2a)
/* A vector of buffers for input does not contain valid information */

#define CSSMERR_CSP_INVALID_OUTPUT_VECTOR (CSSM_CSP_BASE_CSP_ERROR+0x2b)
/* A vector of buffers for output does not contain valid information */



/*
    CSP Cryptographic Context Error Values
    These error values can be returned by CSP APIs that take A cryptographic context handle as
    input.
*/
#define CSSMERR_CSP_INVALID_CONTEXT (CSSM_CSP_BASE_CSP_ERROR+0x30)
/* The cryptographic context and operation types are not compatible */

#define CSSMERR_CSP_INVALID_ALGORITHM (CSSM_CSP_BASE_CSP_ERROR+0x31)
/* Algorithm is not supported by the CSP */

#define CSSMERR_CSP_INVALID_ATTR_KEY					( CSSM_CSP_BASE_CSP_ERROR + 0x36 )
#define CSSMERR_CSP_MISSING_ATTR_KEY					( CSSM_CSP_BASE_CSP_ERROR + 0x37 )
/* The cryptographic key is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_INIT_VECTOR			( CSSM_CSP_BASE_CSP_ERROR + 0x38 )
#define CSSMERR_CSP_MISSING_ATTR_INIT_VECTOR			( CSSM_CSP_BASE_CSP_ERROR + 0x39 )
/* The algorithm mode required an initialization vector and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_SALT					( CSSM_CSP_BASE_CSP_ERROR + 0x3a )
#define CSSMERR_CSP_MISSING_ATTR_SALT					( CSSM_CSP_BASE_CSP_ERROR + 0x3b )
/* The operation requires salt and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_PADDING				( CSSM_CSP_BASE_CSP_ERROR + 0x3c )
#define CSSMERR_CSP_MISSING_ATTR_PADDING				( CSSM_CSP_BASE_CSP_ERROR + 0x3d )
/* An algorithm mode with padding is specified and the padding type is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_RANDOM					( CSSM_CSP_BASE_CSP_ERROR + 0x3e )
#define CSSMERR_CSP_MISSING_ATTR_RANDOM					( CSSM_CSP_BASE_CSP_ERROR + 0x3f )
/* The operation required random data and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_SEED					( CSSM_CSP_BASE_CSP_ERROR + 0x40 )
#define CSSMERR_CSP_MISSING_ATTR_SEED					( CSSM_CSP_BASE_CSP_ERROR + 0x41 )
/* The algorithm requires a seed value and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_PASSPHRASE				( CSSM_CSP_BASE_CSP_ERROR + 0x42 )
#define CSSMERR_CSP_MISSING_ATTR_PASSPHRASE				( CSSM_CSP_BASE_CSP_ERROR + 0x43 )
/* The operation requires a passphrase value and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_KEY_LENGTH				( CSSM_CSP_BASE_CSP_ERROR + 0x44 )
#define CSSMERR_CSP_MISSING_ATTR_KEY_LENGTH				( CSSM_CSP_BASE_CSP_ERROR + 0x45 )
/* The operation requires a key length and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_BLOCK_SIZE				( CSSM_CSP_BASE_CSP_ERROR + 0x46 )
#define CSSMERR_CSP_MISSING_ATTR_BLOCK_SIZE				( CSSM_CSP_BASE_CSP_ERROR + 0x47 )
/* The algorithm has a configurable block size and the value is missing or invalid. */

/*
    CSP Staged Cryptographic API Error Values
    These error values can be returned by staged cryptographic APIs. The names of the staged
    cryptographic APIs end in "Init", "Update", "Final" and "InitP".
*/
#define CSSMERR_CSP_STAGED_OPERATION_IN_PROGRESS \
(CSSM_CSP_BASE_CSP_ERROR+0x48)
/* The application has already started a staged operation using the specified CC */

#define CSSMERR_CSP_STAGED_OPERATION_NOT_STARTED \
(CSSM_CSP_BASE_CSP_ERROR+0x49)
/* An "Update" or "Final" API has been called without calling the corresponding "Init" */


/* Other CSP Error Values */
#define CSSMERR_CSP_VERIFY_FAILED (CSSM_CSP_BASE_CSP_ERROR+0x4a)
/* The signature is not valid */

#define CSSMERR_CSP_INVALID_SIGNATURE (CSSM_CSP_BASE_CSP_ERROR+0x4b)
/* The signature data is not in the proper format */

#define CSSMERR_CSP_QUERY_SIZE_UNKNOWN (CSSM_CSP_BASE_CSP_ERROR+0x4c)
/* The size of the output data can not be determined */

#define CSSMERR_CSP_BLOCK_SIZE_MISMATCH (CSSM_CSP_BASE_CSP_ERROR+0x4d)
/*  The size of the input is not equal to a multiple of the algorithm block size; valid for symmetric
    block ciphers in an unpadded mode */

#define CSSMERR_CSP_PRIVATE_KEY_NOT_FOUND (CSSM_CSP_BASE_CSP_ERROR+0x4e)
/* The private key matching the public key was not found */

#define CSSMERR_CSP_PUBLIC_KEY_INCONSISTENT (CSSM_CSP_BASE_CSP_ERROR+0x4f)
/* The public key specified does not match the private key being unwrapped */

#define CSSMERR_CSP_DEVICE_VERIFY_FAILED (CSSM_CSP_BASE_CSP_ERROR+0x50)
/* The logical device could not be verified by the service provider */

#define CSSMERR_CSP_INVALID_LOGIN_NAME (CSSM_CSP_BASE_CSP_ERROR+0x51)
/* The login name is not recognized by the CSP */

#define CSSMERR_CSP_ALREADY_LOGGED_IN (CSSM_CSP_BASE_CSP_ERROR+0x52)
/* The device is already logged in and can not be reauthenticated */

#define CSSMERR_CSP_PRIVATE_KEY_ALREADY_EXISTS \
(CSSM_CSP_BASE_CSP_ERROR+0x53)
/* The private key already exists in the CSP. */

#define CSSMERR_CSP_KEY_LABEL_ALREADY_EXISTS (CSSM_CSP_BASE_CSP_ERROR+0x54)
/* Key label already exists in the CSP. */

#define CSSMERR_CSP_INVALID_DIGEST_ALGORITHM (CSSM_CSP_BASE_CSP_ERROR+0x55)
/* The digest algorithm passed in to the Sign/Verify operation is invalid. */

/* Offest value jumps past the end of the existing CSP specific errors */

#define CSSMERR_CSP_INVALID_ATTR_OUTPUT_SIZE			( CSSM_CSP_BASE_CSP_ERROR + 0x64 )
#define CSSMERR_CSP_MISSING_ATTR_OUTPUT_SIZE			( CSSM_CSP_BASE_CSP_ERROR + 0x65 )
/* The algorithm has a configurable output size and the value is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_ROUNDS					( CSSM_CSP_BASE_CSP_ERROR + 0x66 )
#define CSSMERR_CSP_MISSING_ATTR_ROUNDS					( CSSM_CSP_BASE_CSP_ERROR + 0x67 )
/* The algorithm has a configurable number of rounds and the value is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_ALG_PARAMS				( CSSM_CSP_BASE_CSP_ERROR + 0x68 )
#define CSSMERR_CSP_MISSING_ATTR_ALG_PARAMS				( CSSM_CSP_BASE_CSP_ERROR + 0x69 )
/* The algorithm required a set of parameters and they are missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_LABEL					( CSSM_CSP_BASE_CSP_ERROR + 0x6a )
#define CSSMERR_CSP_MISSING_ATTR_LABEL					( CSSM_CSP_BASE_CSP_ERROR + 0x6b )
/* The operation creates an object that requires a label and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_KEY_TYPE				( CSSM_CSP_BASE_CSP_ERROR + 0x6c )
#define CSSMERR_CSP_MISSING_ATTR_KEY_TYPE				( CSSM_CSP_BASE_CSP_ERROR + 0x6d )
/* The operation requires a key type and the value is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_MODE					( CSSM_CSP_BASE_CSP_ERROR + 0x6e )
#define CSSMERR_CSP_MISSING_ATTR_MODE					( CSSM_CSP_BASE_CSP_ERROR + 0x6f )
/* The algorithm requires a mode to be specified and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_EFFECTIVE_BITS			( CSSM_CSP_BASE_CSP_ERROR + 0x70 )
#define CSSMERR_CSP_MISSING_ATTR_EFFECTIVE_BITS			( CSSM_CSP_BASE_CSP_ERROR + 0x71 )
/* The algorithm has a configurable number of effective bits and the value it missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_START_DATE				( CSSM_CSP_BASE_CSP_ERROR + 0x72 )
#define CSSMERR_CSP_MISSING_ATTR_START_DATE				( CSSM_CSP_BASE_CSP_ERROR + 0x73 )
/* The operation creates an object with a validity date and the starting date is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_END_DATE				( CSSM_CSP_BASE_CSP_ERROR + 0x74 )
#define CSSMERR_CSP_MISSING_ATTR_END_DATE				( CSSM_CSP_BASE_CSP_ERROR + 0x75 )
/* The operation creates an object with a validity date and the ending date is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_VERSION				( CSSM_CSP_BASE_CSP_ERROR + 0x76 )
#define CSSMERR_CSP_MISSING_ATTR_VERSION				( CSSM_CSP_BASE_CSP_ERROR + 0x77 )
/* The operation requires a version number and the value is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_PRIME					( CSSM_CSP_BASE_CSP_ERROR + 0x78 )
#define CSSMERR_CSP_MISSING_ATTR_PRIME					( CSSM_CSP_BASE_CSP_ERROR + 0x79 )
/* The operation requires a prime value and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_BASE					( CSSM_CSP_BASE_CSP_ERROR + 0x7a )
#define CSSMERR_CSP_MISSING_ATTR_BASE					( CSSM_CSP_BASE_CSP_ERROR + 0x7b )
/* The operation requires a base value and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_SUBPRIME				( CSSM_CSP_BASE_CSP_ERROR + 0x7c )
#define CSSMERR_CSP_MISSING_ATTR_SUBPRIME				( CSSM_CSP_BASE_CSP_ERROR + 0x7d )
/* The operation requires a sub-prime value and it is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_ITERATION_COUNT		( CSSM_CSP_BASE_CSP_ERROR + 0x7e )
#define CSSMERR_CSP_MISSING_ATTR_ITERATION_COUNT		( CSSM_CSP_BASE_CSP_ERROR + 0x7f )
/* The operation has a configurable iteration count and the value is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_DL_DB_HANDLE			( CSSM_CSP_BASE_CSP_ERROR + 0x80 )
#define CSSMERR_CSP_MISSING_ATTR_DL_DB_HANDLE			( CSSM_CSP_BASE_CSP_ERROR + 0x81 )
/* The operation can store new objects in a specific location and the value is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_ACCESS_CREDENTIALS		( CSSM_CSP_BASE_CSP_ERROR + 0x82 )
#define CSSMERR_CSP_MISSING_ATTR_ACCESS_CREDENTIALS		( CSSM_CSP_BASE_CSP_ERROR + 0x83 )
/* The operation requires access credentials and they are missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_PUBLIC_KEY_FORMAT		( CSSM_CSP_BASE_CSP_ERROR + 0x84 )
#define CSSMERR_CSP_MISSING_ATTR_PUBLIC_KEY_FORMAT		( CSSM_CSP_BASE_CSP_ERROR + 0x85 )
/* The resulting public key format is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_PRIVATE_KEY_FORMAT		( CSSM_CSP_BASE_CSP_ERROR + 0x86 )
#define CSSMERR_CSP_MISSING_ATTR_PRIVATE_KEY_FORMAT		( CSSM_CSP_BASE_CSP_ERROR + 0x87 )
/* The resulting private key format is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_SYMMETRIC_KEY_FORMAT	( CSSM_CSP_BASE_CSP_ERROR + 0x88 )
#define CSSMERR_CSP_MISSING_ATTR_SYMMETRIC_KEY_FORMAT	( CSSM_CSP_BASE_CSP_ERROR + 0x89 )
/* The resulting symmetric key format is missing or invalid. */

#define CSSMERR_CSP_INVALID_ATTR_WRAPPED_KEY_FORMAT		( CSSM_CSP_BASE_CSP_ERROR + 0x8a )
#define CSSMERR_CSP_MISSING_ATTR_WRAPPED_KEY_FORMAT		( CSSM_CSP_BASE_CSP_ERROR + 0x8b )
/* The resulting wrapped key format is missing or invalid. */


/*************************************************************************
    Trust Policy Services API Error Values 
**************************************************************************/

/* TP Error Values Derived from Common Error Codes */

#define CSSMERR_TP_INTERNAL_ERROR \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INTERNAL_ERROR)
#define CSSMERR_TP_MEMORY_ERROR \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_MEMORY_ERROR)
#define CSSMERR_TP_MDS_ERROR \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_MDS_ERROR)
#define CSSMERR_TP_INVALID_POINTER \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_POINTER)
#define CSSMERR_TP_INVALID_INPUT_POINTER \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_INPUT_POINTER)
#define CSSMERR_TP_INVALID_OUTPUT_POINTER \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_OUTPUT_POINTER)
#define CSSMERR_TP_FUNCTION_NOT_IMPLEMENTED \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED)
#define CSSMERR_TP_SELF_CHECK_FAILED \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_SELF_CHECK_FAILED)
#define CSSMERR_TP_OS_ACCESS_DENIED \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_OS_ACCESS_DENIED)
#define CSSMERR_TP_FUNCTION_FAILED \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_FUNCTION_FAILED)
#define CSSMERR_TP_INVALID_CONTEXT_HANDLE \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_CONTEXT_HANDLE)
#define CSSMERR_TP_INVALID_DATA \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_DATA)
#define CSSMERR_TP_INVALID_DB_LIST \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_DB_LIST)
#define CSSMERR_TP_INVALID_CERTGROUP_POINTER \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_CERTGROUP_POINTER)
#define CSSMERR_TP_INVALID_CERT_POINTER \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_CERT_POINTER)
#define CSSMERR_TP_INVALID_CRL_POINTER \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_CRL_POINTER)
#define CSSMERR_TP_INVALID_FIELD_POINTER \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_FIELD_POINTER)
#define CSSMERR_TP_INVALID_DATA \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_DATA)
#define CSSMERR_TP_CRL_ALREADY_SIGNED \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_CRL_ALREADY_SIGNED)
#define CSSMERR_TP_INVALID_NUMBER_OF_FIELDS \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_NUMBER_OF_FIELDS)
#define CSSMERR_TP_VERIFICATION_FAILURE \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_VERIFICATION_FAILURE)
#define CSSMERR_TP_INVALID_DB_HANDLE \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_DB_HANDLE)
#define CSSMERR_TP_UNKNOWN_FORMAT \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_UNKNOWN_FORMAT)
#define CSSMERR_TP_UNKNOWN_TAG \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_UNKNOWN_TAG)
#define CSSMERR_TP_INVALID_PASSTHROUGH_ID \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_PASSTHROUGH_ID)
#define CSSMERR_TP_INVALID_CSP_HANDLE \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_CSP_HANDLE)
#define CSSMERR_TP_INVALID_DL_HANDLE \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_DL_HANDLE)
#define CSSMERR_TP_INVALID_CL_HANDLE \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_CL_HANDLE)
#define CSSMERR_TP_INVALID_DB_LIST_POINTER \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_DB_LIST_POINTER)
#define CSSMERR_TP_INVALID_NETWORK_ADDR \
(CSSM_TP_BASE_ERROR+CSSM_ERRCODE_INVALID_NETWORK_ADDR)

/*
    Common TP Error Values
    These values can be returned by one or more TP APIs.
*/
#define CSSM_TP_BASE_TP_ERROR \
(CSSM_TP_BASE_ERROR+CSSM_ERRORCODE_COMMON_EXTENT)

#define CSSMERR_TP_INVALID_CALLERAUTH_CONTEXT_POINTER \
(CSSM_TP_BASE_TP_ERROR+0x01)
/* Invalid context pointer */

#define CSSMERR_TP_INVALID_IDENTIFIER_POINTER (CSSM_TP_BASE_TP_ERROR+0x02)
/* Invalid identifier pointer */

#define CSSMERR_TP_INVALID_KEYCACHE_HANDLE (CSSM_TP_BASE_TP_ERROR+0x03)
/* Invalid key cache handle */

#define CSSMERR_TP_INVALID_CERTGROUP (CSSM_TP_BASE_TP_ERROR+0x04)
/* Invalid structure or unknown format for certificate group or certificates in the group */

#define CSSMERR_TP_INVALID_CRLGROUP (CSSM_TP_BASE_TP_ERROR+0x05)
/* Invalid structure or unknown format for CRL group or CRLs in the group */

#define CSSMERR_TP_INVALID_CRLGROUP_POINTER (CSSM_TP_BASE_TP_ERROR+0x06)
/* Invalid CRL group pointer */

#define CSSMERR_TP_AUTHENTICATION_FAILED (CSSM_TP_BASE_TP_ERROR+0x07)
/* Invalid/unauthorized credentials */

#define CSSMERR_TP_CERTGROUP_INCOMPLETE (CSSM_TP_BASE_TP_ERROR+0x08)
/* Incomplete certificate group */

#define CSSMERR_TP_CERTIFICATE_CANT_OPERATE (CSSM_TP_BASE_TP_ERROR+0x09)
/* Cannot perform the requested operation (sign/verify/database-apply) with input signer or 
   revoker certificate */

#define CSSMERR_TP_CERT_EXPIRED (CSSM_TP_BASE_TP_ERROR+0x0a)
/* Certificate has expired */

#define CSSMERR_TP_CERT_NOT_VALID_YET (CSSM_TP_BASE_TP_ERROR+0x0b)
/* Certificate not valid until a future date */

#define CSSMERR_TP_CERT_REVOKED (CSSM_TP_BASE_TP_ERROR+0x0c)
/* Certificate has been revoked */

#define CSSMERR_TP_CERT_SUSPENDED (CSSM_TP_BASE_TP_ERROR+0x0d)
/* Certificate is currently suspended from use */

#define CSSMERR_TP_INSUFFICIENT_CREDENTIALS (CSSM_TP_BASE_TP_ERROR+0x0e)
/* Insufficient caller credentials for this operation */

#define CSSMERR_TP_INVALID_ACTION (CSSM_TP_BASE_TP_ERROR+0x0f)
/* Invalid action */

#define CSSMERR_TP_INVALID_ACTION_DATA (CSSM_TP_BASE_TP_ERROR+0x10)
/* Invalid data specified for this action */

#define CSSMERR_TP_INVALID_ANCHOR_CERT (CSSM_TP_BASE_TP_ERROR+0x12)
/* Invalid anchor certificate */

#define CSSMERR_TP_INVALID_AUTHORITY (CSSM_TP_BASE_TP_ERROR+0x13)
/* Invalid or unreachable authority */

#define CSSMERR_TP_VERIFY_ACTION_FAILED (CSSM_TP_BASE_TP_ERROR+0x14)
/* Unable to determine trust for action */

#define CSSMERR_TP_INVALID_CERTIFICATE (CSSM_TP_BASE_TP_ERROR+0x15)
/* Invalid certificate */

#define CSSMERR_TP_INVALID_CERT_AUTHORITY (CSSM_TP_BASE_TP_ERROR+0x16)
/* Certificate group is not signed by a recognized issuing authority */

#define CSSMERR_TP_INVALID_CRL_AUTHORITY (CSSM_TP_BASE_TP_ERROR+0x17)
/* Certificate Revocation List is from an unrecognized issuing authority. */

#define CSSMERR_TP_INVALID_CRL_ENCODING (CSSM_TP_BASE_TP_ERROR+0x18)
/* Invalid encoding for CRL */

#define CSSMERR_TP_INVALID_CRL_TYPE (CSSM_TP_BASE_TP_ERROR+0x19)
/* Invalid type for CRL */

#define CSSMERR_TP_INVALID_CRL (CSSM_TP_BASE_TP_ERROR+0x1a)
/* Invalid CRL */

#define CSSMERR_TP_INVALID_FORM_TYPE (CSSM_TP_BASE_TP_ERROR+0x1b)
/* Invalid argument for form type */

#define CSSMERR_TP_INVALID_ID (CSSM_TP_BASE_TP_ERROR+0x1c)
/* Invalid pass through ID */

#define CSSMERR_TP_INVALID_IDENTIFIER (CSSM_TP_BASE_TP_ERROR+0x1d)
/* Unknown reference identifier */

#define CSSMERR_TP_INVALID_INDEX (CSSM_TP_BASE_TP_ERROR+0x1e)
/* Certficate index is invalid */

#define CSSMERR_TP_INVALID_NAME (CSSM_TP_BASE_TP_ERROR+0x1f)
/* Certificate contains unrecognized names */

#define CSSMERR_TP_INVALID_POLICY_IDENTIFIERS (CSSM_TP_BASE_TP_ERROR+0x20)
/* Invalid policy identifier */

#define CSSMERR_TP_INVALID_TIMESTRING (CSSM_TP_BASE_TP_ERROR+0x21)
/* Invalid CSSM_TIMESTRING */

#define CSSMERR_TP_INVALID_REASON (CSSM_TP_BASE_TP_ERROR+0x22)
/* Invalid argument for reason */

#define CSSMERR_TP_INVALID_REQUEST_INPUTS (CSSM_TP_BASE_TP_ERROR+0x23)
/* Invalid request input parameters */

#define CSSMERR_TP_INVALID_RESPONSE_VECTOR (CSSM_TP_BASE_TP_ERROR+0x24)
/* Invalid vector of responses */

#define CSSMERR_TP_INVALID_SIGNATURE (CSSM_TP_BASE_TP_ERROR+0x25)
/* Certificate signature is invalid */

#define CSSMERR_TP_INVALID_STOP_ON_POLICY (CSSM_TP_BASE_TP_ERROR+0x26)
/* Invalid stop on policy */

#define CSSMERR_TP_INVALID_CALLBACK (CSSM_TP_BASE_TP_ERROR+0x27)
/* Invalid callback */

#define CSSMERR_TP_INVALID_TUPLE (CSSM_TP_BASE_TP_ERROR+0x28)
/* Invalid tuple */

#define CSSMERR_TP_NOT_SIGNER (CSSM_TP_BASE_TP_ERROR+0x29)
/* Signer certificate is not signer of subject */

#define CSSMERR_TP_NOT_TRUSTED (CSSM_TP_BASE_TP_ERROR+0x2a)
/* Signature can not be trusted */

#define CSSMERR_TP_NO_DEFAULT_AUTHORITY (CSSM_TP_BASE_TP_ERROR+0x2b)
/* Unspecified authority with no default authority */

#define CSSMERR_TP_REJECTED_FORM (CSSM_TP_BASE_TP_ERROR+0x2c)
/* Form was rejected */

#define CSSMERR_TP_REQUEST_LOST (CSSM_TP_BASE_TP_ERROR+0x2d)
/* Authority lost the request. Must resubmit */

#define CSSMERR_TP_REQUEST_REJECTED (CSSM_TP_BASE_TP_ERROR+0x2e)
/* Authority rejected the request with no specific results returned */

#define CSSMERR_TP_UNSUPPORTED_ADDR_TYPE (CSSM_TP_BASE_TP_ERROR+0x2f)
/* Unsupported type of network address */

#define CSSMERR_TP_UNSUPPORTED_SERVICE (CSSM_TP_BASE_TP_ERROR+0x30)
/* Unsupported TP service requested */

#define CSSMERR_TP_INVALID_TUPLEGROUP_POINTER (CSSM_TP_BASE_TP_ERROR+0x31)
/* Invalid tuple group pointer */

#define CSSMERR_TP_INVALID_TUPLEGROUP (CSSM_TP_BASE_TP_ERROR+0x32)
/* Invalid structure or unknown format for tuple group or tuples in the group */





/*************************************************************************
    Auth Compute Services API Error Values 
**************************************************************************/

/* AC Error Values Derived from Common Error Codes */
#define CSSMERR_AC_INTERNAL_ERROR \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INTERNAL_ERROR)
#define CSSMERR_AC_MEMORY_ERROR \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_MEMORY_ERROR)
#define CSSMERR_AC_MDS_ERROR \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_MDS_ERROR)
#define CSSMERR_AC_INVALID_POINTER \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_POINTER)
#define CSSMERR_AC_INVALID_INPUT_POINTER \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_INPUT_POINTER)
#define CSSMERR_AC_INVALID_OUTPUT_POINTER \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_OUTPUT_POINTER)
#define CSSMERR_AC_FUNCTION_NOT_IMPLEMENTED \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED)
#define CSSMERR_AC_SELF_CHECK_FAILED \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_SELF_CHECK_FAILED)
#define CSSMERR_AC_OS_ACCESS_DENIED \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_OS_ACCESS_DENIED)
#define CSSMERR_AC_FUNCTION_FAILED \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_FUNCTION_FAILED)
#define CSSMERR_AC_INVALID_CONTEXT_HANDLE \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_CONTEXT_HANDLE)
#define CSSMERR_AC_INVALID_DATA \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_DATA)
#define CSSMERR_AC_INVALID_DB_LIST \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_DB_LIST)
#define CSSMERR_AC_INVALID_PASSTHROUGH_ID \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_PASSTHROUGH_ID)
#define CSSMERR_AC_INVALID_DL_HANDLE \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_DL_HANDLE)
#define CSSMERR_AC_INVALID_CL_HANDLE \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_CL_HANDLE)
#define CSSMERR_AC_INVALID_TP_HANDLE \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_TP_HANDLE)
#define CSSMERR_AC_INVALID_DB_HANDLE \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_DB_HANDLE)
#define CSSMERR_AC_INVALID_DB_LIST_POINTER \
(CSSM_AC_BASE_ERROR+CSSM_ERRCODE_INVALID_DB_LIST_POINTER)


/*
    AC Error Values
    Values that can be returned by one or more AC APIs.
*/
#define CSSM_AC_BASE_AC_ERROR \
(CSSM_AC_BASE_ERROR+CSSM_ERRORCODE_COMMON_EXTENT)

#define CSSMERR_AC_INVALID_BASE_ACLS (CSSM_AC_BASE_AC_ERROR+0x01)
/* One (or more) of the base ACLs was not in proper ACL format */

#define CSSMERR_AC_INVALID_TUPLE_CREDENTIALS (CSSM_AC_BASE_AC_ERROR+0x02)
/* One (or more) of the tuple credentials was not in proper CSSM_TUPLE format */

#define CSSMERR_AC_INVALID_ENCODING (CSSM_AC_BASE_AC_ERROR+0x03)
/* One (or more) of the input parameters was not properly encoded */

#define CSSMERR_AC_INVALID_VALIDITY_PERIOD (CSSM_AC_BASE_AC_ERROR+0x04)
/* The date and time descriptions provided were not properly encoded or did not specify a valid interval */

#define CSSMERR_AC_INVALID_REQUESTOR (CSSM_AC_BASE_AC_ERROR+0x05)
/* One (or more) of the Requestors was not properly encoded */

#define CSSMERR_AC_INVALID_REQUEST_DESCRIPTOR (CSSM_AC_BASE_AC_ERROR+0x06)
/* One (or more) of the base ACLs was not in proper ACL format */




/*************************************************************************
    Certificate Library Services API Error Values 
**************************************************************************/

/* CL Error Values Derived from Common Error Codes */

#define CSSMERR_CL_INTERNAL_ERROR \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INTERNAL_ERROR)
#define CSSMERR_CL_MEMORY_ERROR \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_MEMORY_ERROR)
#define CSSMERR_CL_MDS_ERROR \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_MDS_ERROR)
#define CSSMERR_CL_INVALID_POINTER \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_POINTER)
#define CSSMERR_CL_INVALID_INPUT_POINTER \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_INPUT_POINTER)
#define CSSMERR_CL_INVALID_OUTPUT_POINTER \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_OUTPUT_POINTER)
#define CSSMERR_CL_FUNCTION_NOT_IMPLEMENTED \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED)
#define CSSMERR_CL_SELF_CHECK_FAILED \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_SELF_CHECK_FAILED)
#define CSSMERR_CL_OS_ACCESS_DENIED \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_OS_ACCESS_DENIED)
#define CSSMERR_CL_FUNCTION_FAILED \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_FUNCTION_FAILED)
#define CSSMERR_CL_INVALID_CONTEXT_HANDLE \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_CONTEXT_HANDLE)
#define CSSMERR_CL_INVALID_CERTGROUP_POINTER \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_CERTGROUP_POINTER)
#define CSSMERR_CL_INVALID_CERT_POINTER \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_CERT_POINTER)
#define CSSMERR_CL_INVALID_CRL_POINTER \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_CRL_POINTER)
#define CSSMERR_CL_INVALID_FIELD_POINTER \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_FIELD_POINTER)
#define CSSMERR_CL_INVALID_DATA \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_DATA)
#define CSSMERR_CL_CRL_ALREADY_SIGNED \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_CRL_ALREADY_SIGNED)
#define CSSMERR_CL_INVALID_NUMBER_OF_FIELDS \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_NUMBER_OF_FIELDS)
#define CSSMERR_CL_VERIFICATION_FAILURE \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_VERIFICATION_FAILURE)
#define CSSMERR_CL_UNKNOWN_FORMAT \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_UNKNOWN_FORMAT)
#define CSSMERR_CL_UNKNOWN_TAG \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_UNKNOWN_TAG)
#define CSSMERR_CL_INVALID_PASSTHROUGH_ID \
(CSSM_CL_BASE_ERROR+CSSM_ERRCODE_INVALID_PASSTHROUGH_ID)

/*
    CL Error Values
    These codes can be returned by one or more CL APIs.
*/
#define CSSM_CL_BASE_CL_ERROR \
(CSSM_CL_BASE_ERROR+CSSM_ERRORCODE_COMMON_EXTENT)

#define CSSMERR_CL_INVALID_BUNDLE_POINTER (CSSM_CL_BASE_CL_ERROR+0x01)
/* Invalid pointer for certificate bundle */

#define CSSMERR_CL_INVALID_CACHE_HANDLE (CSSM_CL_BASE_CL_ERROR+0x02)
/* Invalid certificate or CRL cache handle */

#define CSSMERR_CL_INVALID_RESULTS_HANDLE (CSSM_CL_BASE_CL_ERROR+0x03)
/* Invalid handle for results of a certificate or CRL query */

#define CSSMERR_CL_INVALID_BUNDLE_INFO (CSSM_CL_BASE_CL_ERROR+0x04)
/* Unknown type or encoding for certificate bundle */

#define CSSMERR_CL_INVALID_CRL_INDEX (CSSM_CL_BASE_CL_ERROR+0x05)
/* Invalid index for revocation record in CRL */

#define CSSMERR_CL_INVALID_SCOPE (CSSM_CL_BASE_CL_ERROR+0x06)
/* Invalid sign or verify scope (function dependent) */

#define CSSMERR_CL_NO_FIELD_VALUES (CSSM_CL_BASE_CL_ERROR+0x07)
/* No field matched the specified certificate or CRL field OID */

#define CSSMERR_CL_SCOPE_NOT_SUPPORTED (CSSM_CL_BASE_CL_ERROR+0x08)
/* Field signing or verifying is not supported by this module */


/*************************************************************************
    Data Storage Library Services API Error Values 
**************************************************************************/
/* DL Error Values Derived from Common Error Codes */

#define CSSMERR_DL_INTERNAL_ERROR \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INTERNAL_ERROR)
#define CSSMERR_DL_MEMORY_ERROR \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_MEMORY_ERROR)
#define CSSMERR_DL_MDS_ERROR \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_MDS_ERROR)
#define CSSMERR_DL_INVALID_POINTER \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_POINTER)
#define CSSMERR_DL_INVALID_INPUT_POINTER \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_INPUT_POINTER)
#define CSSMERR_DL_INVALID_OUTPUT_POINTER \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_OUTPUT_POINTER)
#define CSSMERR_DL_FUNCTION_NOT_IMPLEMENTED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED)
#define CSSMERR_DL_SELF_CHECK_FAILED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_SELF_CHECK_FAILED)
#define CSSMERR_DL_OS_ACCESS_DENIED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_OS_ACCESS_DENIED)
#define CSSMERR_DL_FUNCTION_FAILED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_FUNCTION_FAILED)

/*
    DL Error Values Derived from ACL-based Error Codes
    This section lists DL error values derived from convertible ACL-based error codes.
*/
#define CSSMERR_DL_OPERATION_AUTH_DENIED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_OPERATION_AUTH_DENIED)
#define CSSMERR_DL_OBJECT_USE_AUTH_DENIED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_OBJECT_USE_AUTH_DENIED)
#define CSSMERR_DL_OBJECT_MANIP_AUTH_DENIED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_OBJECT_MANIP_AUTH_DENIED)
#define CSSMERR_DL_OBJECT_ACL_NOT_SUPPORTED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_OBJECT_ACL_NOT_SUPPORTED)
#define CSSMERR_DL_OBJECT_ACL_REQUIRED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_OBJECT_ACL_REQUIRED)
#define CSSMERR_DL_INVALID_ACCESS_CREDENTIALS \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_ACCESS_CREDENTIALS)
#define CSSMERR_DL_INVALID_ACL_BASE_CERTS \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_BASE_CERTS)
#define CSSMERR_DL_ACL_BASE_CERTS_NOT_SUPPORTED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_ACL_BASE_CERTS_NOT_SUPPORTED)
#define CSSMERR_DL_INVALID_SAMPLE_VALUE \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_SAMPLE_VALUE)
#define CSSMERR_DL_SAMPLE_VALUE_NOT_SUPPORTED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_SAMPLE_VALUE_NOT_SUPPORTED)
#define CSSMERR_DL_INVALID_ACL_SUBJECT_VALUE \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_SUBJECT_VALUE)
#define CSSMERR_DL_ACL_SUBJECT_TYPE_NOT_SUPPORTED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_ACL_SUBJECT_TYPE_NOT_SUPPORTED)
#define CSSMERR_DL_INVALID_ACL_CHALLENGE_CALLBACK \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_CHALLENGE_CALLBACK)
#define CSSMERR_DL_ACL_CHALLENGE_CALLBACK_FAILED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_ACL_CHALLENGE_CALLBACK_FAILED)
#define CSSMERR_DL_INVALID_ACL_ENTRY_TAG \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_ENTRY_TAG)
#define CSSMERR_DL_ACL_ENTRY_TAG_NOT_FOUND \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_ACL_ENTRY_TAG_NOT_FOUND)
#define CSSMERR_DL_INVALID_ACL_EDIT_MODE \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_ACL_EDIT_MODE)
#define CSSMERR_DL_ACL_CHANGE_FAILED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_ACL_CHANGE_FAILED)
#define CSSMERR_DL_INVALID_NEW_ACL_ENTRY \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_NEW_ACL_ENTRY)
#define CSSMERR_DL_INVALID_NEW_ACL_OWNER \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_NEW_ACL_OWNER)
#define CSSMERR_DL_ACL_DELETE_FAILED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_ACL_DELETE_FAILED)
#define CSSMERR_DL_ACL_REPLACE_FAILED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_ACL_REPLACE_FAILED)
#define CSSMERR_DL_ACL_ADD_FAILED \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_ACL_ADD_FAILED)
#define CSSMERR_DL_INVALID_DL_HANDLE \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_DL_HANDLE)

/*
    DL Error Values for Specific Data Types
    This section lists DL error values derived from Common Error Codes for specific data types.
*/
#define CSSMERR_DL_INVALID_DB_HANDLE \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_DB_HANDLE)
#define CSSMERR_DL_INVALID_PASSTHROUGH_ID \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_PASSTHROUGH_ID)
#define CSSMERR_DL_INVALID_NETWORK_ADDR \
(CSSM_DL_BASE_ERROR+CSSM_ERRCODE_INVALID_NETWORK_ADDR)

/*
    General DL Error Values
    These values can be returned from any DL function.
*/
#define CSSM_DL_BASE_DL_ERROR \
(CSSM_DL_BASE_ERROR+CSSM_ERRORCODE_COMMON_EXTENT)

#define CSSMERR_DL_DATABASE_CORRUPT (CSSM_DL_BASE_DL_ERROR+0x01)
/* The database file or other data form is corrupt */


/* DL Specific Error Values */
#define CSSMERR_DL_INVALID_RECORD_INDEX (CSSM_DL_BASE_DL_ERROR+0x08)
/* A record index in the DbInfo structure is invalid */

#define CSSMERR_DL_INVALID_RECORDTYPE (CSSM_DL_BASE_DL_ERROR+0x09)
/* Record types from DbInfo’s arrays do not match */

#define CSSMERR_DL_INVALID_FIELD_NAME (CSSM_DL_BASE_DL_ERROR+0x0a)
/* Attribute or index name is an illegal name */

#define CSSMERR_DL_UNSUPPORTED_FIELD_FORMAT (CSSM_DL_BASE_DL_ERROR+0x0b)
/* A field’s format (data type) is not supported */

#define CSSMERR_DL_UNSUPPORTED_INDEX_INFO (CSSM_DL_BASE_DL_ERROR+0x0c)
/* Requested IndexInfo struct is not supported */

#define CSSMERR_DL_UNSUPPORTED_LOCALITY (CSSM_DL_BASE_DL_ERROR+0x0d)
/* The value of DbInfo->IsLocal is not supported */

#define CSSMERR_DL_UNSUPPORTED_NUM_ATTRIBUTES (CSSM_DL_BASE_DL_ERROR+0x0e)
/* Unsupported number of attributes specified */

#define CSSMERR_DL_UNSUPPORTED_NUM_INDEXES (CSSM_DL_BASE_DL_ERROR+0x0f)
/* Unsupported number of indexes */

#define CSSMERR_DL_UNSUPPORTED_NUM_RECORDTYPES (CSSM_DL_BASE_DL_ERROR+0x10)
/* Unsupported number of record types */

#define CSSMERR_DL_UNSUPPORTED_RECORDTYPE (CSSM_DL_BASE_DL_ERROR+0x11)
/* Requested record type is not supported */

#define CSSMERR_DL_FIELD_SPECIFIED_MULTIPLE (CSSM_DL_BASE_DL_ERROR+0x12)
/* A record attribute or index was specified multiple times with differing information */

#define CSSMERR_DL_INCOMPATIBLE_FIELD_FORMAT (CSSM_DL_BASE_DL_ERROR+0x13)
/* The field format for an index from an attribute is different from the field format of that attribute */

#define CSSMERR_DL_INVALID_PARSING_MODULE (CSSM_DL_BASE_DL_ERROR+0x14)
/* A parsing module in the DB Info is invalid */

#define CSSMERR_DL_INVALID_DB_NAME (CSSM_DL_BASE_DL_ERROR+0x16)
/* The Database name is invalid. */

#define CSSMERR_DL_DATASTORE_DOESNOT_EXIST (CSSM_DL_BASE_DL_ERROR+0x17)
/* The specified datastore does not exists */

#define CSSMERR_DL_DATASTORE_ALREADY_EXISTS (CSSM_DL_BASE_DL_ERROR+0x18)
/* The specified datastore already exists */

#define CSSMERR_DL_DB_LOCKED (CSSM_DL_BASE_DL_ERROR+0x19)
/* Database is currently locked for exclusive update */

#define CSSMERR_DL_DATASTORE_IS_OPEN (CSSM_DL_BASE_DL_ERROR+0x1a)
/* The database is currently open */

#define CSSMERR_DL_RECORD_NOT_FOUND (CSSM_DL_BASE_DL_ERROR+0x1b)
/* The record does not exist */

#define CSSMERR_DL_MISSING_VALUE (CSSM_DL_BASE_DL_ERROR+0x1c)
/* Missing needed attribute or data value */

#define CSSMERR_DL_UNSUPPORTED_QUERY (CSSM_DL_BASE_DL_ERROR+0x1d)
/* An unsupported query was specified */

#define CSSMERR_DL_UNSUPPORTED_QUERY_LIMITS (CSSM_DL_BASE_DL_ERROR+0x1e)
/* The requested query limits are not supported */

#define CSSMERR_DL_UNSUPPORTED_NUM_SELECTION_PREDS \
(CSSM_DL_BASE_DL_ERROR+0x1f)
/* The number of selection predicates is not supported */

#define CSSMERR_DL_UNSUPPORTED_OPERATOR (CSSM_DL_BASE_DL_ERROR+0x21)
/* An unsupported operator was requested */

#define CSSMERR_DL_INVALID_RESULTS_HANDLE (CSSM_DL_BASE_DL_ERROR+0x22)
/* Invalid results handle */

#define CSSMERR_DL_INVALID_DB_LOCATION (CSSM_DL_BASE_DL_ERROR+0x23)
/* The Database Location is not valid */

#define CSSMERR_DL_INVALID_ACCESS_REQUEST (CSSM_DL_BASE_DL_ERROR+0x24)
/* Unrecognized access type */

#define CSSMERR_DL_INVALID_INDEX_INFO (CSSM_DL_BASE_DL_ERROR+0x25)
/* Invalid index information passed */

#define CSSMERR_DL_INVALID_SELECTION_TAG (CSSM_DL_BASE_DL_ERROR+0x26)
/* Invalid selection tag */

#define CSSMERR_DL_INVALID_NEW_OWNER (CSSM_DL_BASE_DL_ERROR+0x27)
/* Owner definition is invalid */

#define CSSMERR_DL_INVALID_RECORD_UID (CSSM_DL_BASE_DL_ERROR+0x28)
/* The data inside the unique record identifier is not valid */

#define CSSMERR_DL_INVALID_UNIQUE_INDEX_DATA (CSSM_DL_BASE_DL_ERROR+0x29)
/* The modification would have caused the new primary key to have a value that is already in use */

#define CSSMERR_DL_INVALID_MODIFY_MODE (CSSM_DL_BASE_DL_ERROR+0x2a)
/* The specified modification mode is undefined or could not be applied to the record attributes
   identified for modification. */

#define CSSMERR_DL_INVALID_OPEN_PARAMETERS (CSSM_DL_BASE_DL_ERROR+0x2b)
/* The open parameters are not valid */

#define CSSMERR_DL_ENDOFDATA (CSSM_DL_BASE_DL_ERROR+0x2d)
/* There are no more records satisfying the query. */

#define CSSMERR_DL_INVALID_QUERY (CSSM_DL_BASE_DL_ERROR+0x31)
/* Invalid Query specified */

#define CSSMERR_DL_INVALID_VALUE (CSSM_DL_BASE_DL_ERROR+0x32)
/* Invalid Value passed */

#define CSSMERR_DL_MULTIPLE_VALUES_UNSUPPORTED (CSSM_DL_BASE_DL_ERROR+0x33)
/* Multiple Values are not supported in this implementation */

#define CSSMERR_DL_RECORD_MODIFIED (CSSM_DL_BASE_DL_ERROR+0x34)
/* DL Record has been modified */

#define CSSMERR_DL_STALE_UNIQUE_RECORD (CSSM_DL_BASE_DL_ERROR+0x35)
/* The record returned has been changed by someone and is stale */

#endif /* _CSSMERR_H */
