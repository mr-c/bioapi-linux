/*-----------------------------------------------------------------------
 * File: BioAPIERR.H
 *-----------------------------------------------------------------------
 */

#ifndef _BIOAPIERR_H_
#define _BIOAPIERR_H_


#define BioAPI_BASE_ERROR (0x00000000)
/* The configurable BioAPI error code base value. */

#define BioAPI_ERRORCODE_COMPONENT_EXTENT (0x00001000)
/* The configurable number of error codes allocated for each component type.
 * This number must be greater than BioAPI_ERRORCODE_COMMON_EXTENT, and
 * should allow at least half the space for specification-defined error codes.
 */

#define BioAPI_ERRORCODE_COMMON_EXTENT (0x100)
/* The number of error codes allocated to indicate "common" errors. */

#define BioAPI_ERRORCODE_CUSTOM_OFFSET (0x00000800)
/* The configurable offset at which custom error codes are allocated. Must be
 * greater than BioAPI_ERRCODE_COMMON_EXTENT and less than
 * BioAPI_ERRORCODE_COMPONENT_EXTENT.  A BSP with "custom" error codes simply
 * starts assigning them from this offset (without regard to any other BSPs.)
 */


/* BioAPI Error Code Constants */
#define BioAPI_H_FRAMEWORK_BASE_ERROR		(BioAPI_BASE_ERROR)
#define BioAPI_H_FRAMEWORK_PRIVATE_ERROR	(BioAPI_H_FRAMEWORK_BASE_ERROR + BioAPI_ERRORCODE_CUSTOM_OFFSET)

/* #define BioAPI_H_FRAMEWORK_BASE_ERROR (BioAPI_BASE_ERROR) /
 #define BioAPI_H_FRAMEWORK_PRIVATE_ERROR	\
		(BioAPI_H_FRAMEWORK_BASE_ERROR + BioAPI_ERRORCODE_CUSTOM_OFFSET) */

#define BioAPI_BSP_BASE_ERROR				\
		(BioAPI_H_FRAMEWORK_BASE_ERROR + BioAPI_ERRORCODE_COMPONENT_EXTENT)
#define BioAPI_BSP_PRIVATE_ERROR			\
		(BioAPI_BSP_BASE_ERROR + BioAPI_ERRORCODE_CUSTOM_OFFSET)

#define BioAPI_D_FRAMEWORK_BASE_ERROR		\
		(BioAPI_BSP_BASE_ERROR + BioAPI_ERRORCODE_COMPONENT_EXTENT)
#define BioAPI_D_FRAMEWORK_PRIVATE_ERROR	\
		(BioAPI_D_FRAMEWORK_BASE_ERROR + BioAPI_ERRORCODE_CUSTOM_OFFSET)

#define BioAPI_DEVICE_BASE_ERROR			\
		(BioAPI_D_FRAMEWORK_BASE_ERROR + BioAPI_ERRORCODE_COMPONENT_EXTENT)
#define BioAPI_DEVICE_PRIVATE_ERROR			\
		(BioAPI_DEVICE_BASE_ERROR + BioAPI_ERRORCODE_CUSTOM_OFFSET)

/*
	General Error Values
	The following error values can be returned by the H-Framework for any BioAPI function.
*/
#define BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE \
(BioAPI_H_FRAMEWORK_BASE_ERROR + BioAPI_ERRORCODE_COMMON_EXTENT + 1)
/* The given service provider handle is not valid */

#define BioAPIERR_H_FRAMEWORK_NOT_INITIALIZED \
(BioAPI_H_FRAMEWORK_BASE_ERROR + BioAPI_ERRORCODE_COMMON_EXTENT + 2)
/* A function is called without initializing the BioAPI */


/*
	Common Error Codes For All Module Types
	The following codes can be returned by multiple components.
*/

#define BioAPI_ERRCODE_INTERNAL_ERROR (0x0001)
/* General system error; indicates that an operating system or internal state
 * error has occurred and the system may not be in a known state.
 */

#define BioAPI_ERRCODE_MEMORY_ERROR (0x0002 )
/* A memory error occurred. */

#define BioAPI_ERRCODE_REGISTRY_ERROR (0x0003)
/* The registry could not be accessed to complete the operation */

#define BioAPI_ERRCODE_INVALID_POINTER (0x0004)
/* An input/output function parameter or input/output field inside of a data
 * structure is an invalid pointer.
 */

#define BioAPI_ERRCODE_INVALID_INPUT_POINTER (0x0005)
/* An input function parameter or input field in a data structure is an
 * invalid pointer.
 */

#define BioAPI_ERRCODE_INVALID_OUTPUT_POINTER (0x0006)
/* An output function parameter or output field in a data structure is an
 * invalid pointer.
 */

#define BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED (0x0007)
/* The function is not implemented by the service provider. */

#define BioAPI_ERRCODE_OS_ACCESS_DENIED (0x0009)
/* The operating system denied access to a required resource. */

#define BioAPI_ERRCODE_FUNCTION_FAILED (0x000A)
/* The function failed for an unknown reason. */

#define BioAPI_ERRCODE_INVALID_UUID (0x000C)
/* Invalid UUID */

#define BioAPI_ERRCODE_INCOMPATIBLE_VERSION (0x0041)
/* Version is not compatible with the current version. */

/* Error values with the following code enumeration values may be returned
 * from any function that takes as input a BioAPI_DATA.
 */

#define BioAPI_ERRCODE_INVALID_DATA (0x0046)
/* The data in an input parameter is invalid. */

/* Error values with the following code enumeration values may be returned
 * from any function that takes as input a DB handle.
 */

#define BioAPI_ERRCODE_INVALID_DB_HANDLE (0x004A)
/* Invalid database handle */

/*
	H-Framework Error Values derived from the Common Error Codes
*/

#define BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR				\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_INTERNAL_ERROR)

#define BioAPIERR_H_FRAMEWORK_MEMORY_ERROR					\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_MEMORY_ERROR)

#define BioAPIERR_H_FRAMEWORK_REGISTRY_ERROR				\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_REGISTRY_ERROR)

#define BioAPIERR_H_FRAMEWORK_INVALID_POINTER				\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_INVALID_POINTER)

#define BioAPIERR_H_FRAMEWORK_INVALID_INPUT_POINTER			\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_INVALID_INPUT_POINTER)

#define BioAPIERR_H_FRAMEWORK_INVALID_OUTPUT_POINTER		\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_INVALID_OUTPUT_POINTER)

#define BioAPIERR_H_FRAMEWORK_FUNCTION_NOT_IMPLEMENTED		\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED)

#define BioAPIERR_H_FRAMEWORK_OS_ACCESS_DENIED				\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_OS_ACCESS_DENIED)

#define BioAPIERR_H_FRAMEWORK_FUNCTION_FAILED				\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_FUNCTION_FAILED)

#define BioAPIERR_H_FRAMEWORK_INVALID_UUID					\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_INVALID_UUID)

#define BioAPIERR_H_FRAMEWORK_INCOMPATIBLE_VERSION			\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRCODE_INCOMPATIBLE_VERSION)

/*
	H-Framework-specific Error Values
	Reserve first 16 H-FRAMEWORK Error Codes for general errors
*/

#define BioAPI_H_FRAMEWORK_BASE_H_FRAMEWORK_ERROR			\
		(BioAPI_H_FRAMEWORK_BASE_ERROR+BioAPI_ERRORCODE_COMMON_EXTENT+0x10)

#define BioAPIERR_H_FRAMEWORK_MODULE_LOAD_FAILED			\
		(BioAPI_H_FRAMEWORK_BASE_H_FRAMEWORK_ERROR+6)
/* BSP Module Load function failed */

#define BioAPIERR_H_FRAMEWORK_MODULE_UNLOAD_FAILED			\
		(BioAPI_H_FRAMEWORK_BASE_H_FRAMEWORK_ERROR+8)
/* BSP Module Unload function failed */

#define BioAPIERR_H_FRAMEWORK_LIB_REF_NOT_FOUND				\
		(BioAPI_H_FRAMEWORK_BASE_H_FRAMEWORK_ERROR+9)
/* A reference to the loaded library cannot be obtained */

#define BioAPIERR_H_FRAMEWORK_INVALID_MODULE_FUNCTION_TABLE \
		(BioAPI_H_FRAMEWORK_BASE_H_FRAMEWORK_ERROR+10)
/* BSP Module function table registered with CSSM is invalid */

#define BioAPIERR_H_FRAMEWORK_MODULE_NOT_LOADED				\
		(BioAPI_H_FRAMEWORK_BASE_H_FRAMEWORK_ERROR+14)
/* Module was not loaded */

#define BioAPIERR_H_FRAMEWORK_INVALID_DEVICE_ID				\
		(BioAPI_H_FRAMEWORK_BASE_H_FRAMEWORK_ERROR+15)
/* Invalid DeviceId was requested */

/*
	BSP Error Values derived from the Common Error Codes
*/

#define BioAPIERR_BSP_INTERNAL_ERROR						\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_INTERNAL_ERROR)

#define BioAPIERR_BSP_MEMORY_ERROR							\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_MEMORY_ERROR)

#define BioAPIERR_BSP_REGISTRY_ERROR				\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_REGISTRY_ERROR)

#define BioAPIERR_BSP_INVALID_POINTER						\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_INVALID_POINTER)

#define BioAPIERR_BSP_INVALID_INPUT_POINTER					\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_INVALID_INPUT_POINTER)

#define BioAPIERR_BSP_INVALID_OUTPUT_POINTER				\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_INVALID_OUTPUT_POINTER)

#define BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED				\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED)

#define BioAPIERR_BSP_OS_ACCESS_DENIED						\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_OS_ACCESS_DENIED)

#define BioAPIERR_BSP_FUNCTION_FAILED						\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_FUNCTION_FAILED)

#define BioAPIERR_BSP_INVALID_DATA							\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_INVALID_DATA)

#define BioAPIERR_BSP_INVALID_DB_HANDLE						\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRCODE_INVALID_DB_HANDLE)

/*
	BSP-specific Error Values
*/

#define BioAPI_BSP_BASE_BSP_ERROR							\
		(BioAPI_BSP_BASE_ERROR+BioAPI_ERRORCODE_COMMON_EXTENT)

#define BioAPIERR_BSP_UNABLE_TO_CAPTURE						\
		(BioAPI_BSP_BASE_BSP_ERROR+1)
/* BSP is unable to capture raw samples from the device */

#define BioAPIERR_BSP_TOO_MANY_HANDLES						\
		(BioAPI_BSP_BASE_BSP_ERROR+2)
/* The BSP has no more space to allocate BIR handles */

#define BioAPIERR_BSP_TIMEOUT_EXPIRED						\
		(BioAPI_BSP_BASE_BSP_ERROR+3)
/* The Function has been terminated because the timeout value has expired */

#define BioAPIERR_BSP_INVALID_BIR							\
		(BioAPI_BSP_BASE_BSP_ERROR+4)
/* The input BIR is invalid for the purpose required */

#define BioAPIERR_BSP_BIR_SIGNATURE_FAILURE					\
		(BioAPI_BSP_BASE_BSP_ERROR+5)
/* The BSP could not validate the signature on the BIR */

#define BioAPIERR_BSP_UNABLE_TO_WRAP_PAYLOAD				\
		(BioAPI_BSP_BASE_BSP_ERROR+6)
/* The BSP is unable to include the payload in the new BIR */

#define BioAPIERR_BSP_NO_INPUT_BIRS							\
		(BioAPI_BSP_BASE_BSP_ERROR+8)
/* The identify population is NULL */

#define BioAPIERR_BSP_UNSUPPORTED_FORMAT					\
		(BioAPI_BSP_BASE_BSP_ERROR+9)
/* The BSP does not support the data form for the Import function */

#define BioAPIERR_BSP_UNABLE_TO_IMPORT						\
		(BioAPI_BSP_BASE_BSP_ERROR+10)
/* The BSP was unable to construct a BIR from the input data */

#define BioAPIERR_BSP_FUNCTION_NOT_SUPPORTED				\
		(BioAPI_BSP_BASE_BSP_ERROR+12)
/* The BSP does not support this operation. */

#define BioAPIERR_BSP_INCONSISTENT_PURPOSE					\
		(BioAPI_BSP_BASE_BSP_ERROR+13)
/* The purpose recorded in the BIR, and the requested purpose are inconsistent
 * with the function being performed.
 */

#define BioAPIERR_BSP_BIR_NOT_FULLY_PROCESSED				\
		(BioAPI_BSP_BASE_BSP_ERROR+14)
/* The function requires a fully-processed BIR. */

#define BioAPIERR_BSP_PURPOSE_NOT_SUPPORTED					\
		(BioAPI_BSP_BASE_BSP_ERROR+15)
/* The BSP does not support the requested purpose. */

#define BioAPIERR_BSP_INVALID_BIR_HANDLE					\
		(BioAPI_BSP_BASE_BSP_ERROR+16)
/* No BIR exists with the requested handle */

#define BioAPIERR_BSP_UNABLE_TO_OPEN_DATABASE				\
		(BioAPI_BSP_BASE_BSP_ERROR+256)
/* BSP is unable to open specified database */

#define BioAPIERR_BSP_DATABASE_IS_LOCKED					\
		(BioAPI_BSP_BASE_BSP_ERROR+257)
/* Database cannot be opened for the access requested because it is locked */

#define BioAPIERR_BSP_DATABASE_DOES_NOT_EXIST				\
		(BioAPI_BSP_BASE_BSP_ERROR+258)
/* The specified database name does not exist */

#define BioAPIERR_BSP_DATABASE_ALREADY_EXISTS				\
		(BioAPI_BSP_BASE_BSP_ERROR+259)
/* Create failed because the database already exists */

#define BioAPIERR_BSP_INVALID_DATABASE_NAME					\
		(BioAPI_BSP_BASE_BSP_ERROR+260)
/* Invalid database name */

#define BioAPIERR_BSP_RECORD_NOT_FOUND						\
		(BioAPI_BSP_BASE_BSP_ERROR+261)
/* No record exists with the requested key */

#define BioAPIERR_BSP_CURSOR_IS_INVALID						\
		(BioAPI_BSP_BASE_BSP_ERROR+262)
/* The specified cursor is invalid */

#define BioAPIERR_BSP_DATABASE_IS_OPEN						\
		(BioAPI_BSP_BASE_BSP_ERROR+263)
/* Database is already open */

#define BioAPIERR_BSP_INVALID_ACCESS_REQUEST				\
		(BioAPI_BSP_BASE_BSP_ERROR+264)
/* Unrecognized access type */

#define BioAPIERR_BSP_END_OF_DATABASE						\
		(BioAPI_BSP_BASE_BSP_ERROR+265)
/* End of database has been reached. */

#define BioAPIERR_BSP_UNABLE_TO_CREATE_DATABASE				\
		(BioAPI_BSP_BASE_BSP_ERROR+266)
/* BSP cannot create the database. */

#define BioAPIERR_BSP_UNABLE_TO_CLOSE_DATABASE				\
		(BioAPI_BSP_BASE_BSP_ERROR+267)
/* BSP cannot close the database. */

#define BioAPIERR_BSP_UNABLE_TO_DELETE_DATABASE				\
		(BioAPI_BSP_BASE_BSP_ERROR+268)
/* BSP cannot delete the database. */


#endif /* _BIOAPIERR_H_ */
