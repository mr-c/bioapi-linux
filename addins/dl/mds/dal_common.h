/*-----------------------------------------------------------------------
 * File: DAL_COMMON.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file declares several data structures and macros that are used throughout
 * the DAL and the DL backend.
 */

#ifndef DAL_COMMON_H
#define DAL_COMMON_H

#include "port/bioapi_port.h"
#include "cssmerr.h"

#include "intel/mds_util_type.h"

typedef struct cssm_error {
	uint32 error;
	CSSM_GUID guid;
} CSSM_ERROR, *CSSM_ERROR_PTR;

#ifdef MODULE_DIRECTORY_SERVICES
	#include "../mds/defines.h" /* Flags defined by the backend to change the behavior of the DAL */
#else
	#include "defines.h" /* Flags defined by the backend to change the behavior of the DAL */
#endif

#ifdef __cplusplus
	#define DLINTERNAL extern "C"
	DLINTERNAL {
#else
	#define DLINTERNAL extern
#endif


/* Maximum number of fields (attributes/indexes) in a database */
#define MAX_NUM_FIELDS				128

#ifdef DAL_SOURCE_FILE /* begin section for DAL source only */

/*
 * Check the Validity of defines.h
 */

#if !defined(USE_AUTHENTICATION)
#error USE_AUTHENTICATION not defined in "defines.h"
#endif

#if !defined(USE_RECORD_SIGNING)
#error USE_RECORD_SIGNING not defined in "defines.h"
#endif

#if !defined(USE_DL_PASSTHROUGH)
#error USE_DL_PASSTHROUGH not defined in "defines.h"
#endif

#if !defined(ALLOW_REMOTE_LOCALITY)
#error ALLOW_MULTIPLE_QUERIES not defined in "defines.h"
#endif

#if !defined(ALLOW_LOCAL_LOCALITY)
#error ALLOW_MULTIPLE_QUERIES not defined in "defines.h"
#endif

#if !defined(MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS)
#error MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS not defined in "defines.h"
#endif

#if !defined(MODULE_PARAMETERS_USE_DB_INFO)
#error MODULE_PARAMETERS_USE_DB_INFO not defined in "defines.h"
#endif

#if !defined(MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION)
#error MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION not defined in "defines.h"
#endif

#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION && !defined(ADDITIONAL_INFORMATION_MAX_LENGTH)
#error ADDITIONAL_INFORMATION_MAX_LENGTH not defined in "defines.h"
#endif

#if !MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION && defined(ADDITIONAL_INFORMATION_MAX_LENGTH)
#error ADDITIONAL_INFORMATION_MAX_LENGTH defined in "defines.h", will be ignored
#endif

#if !defined(DAL_ENSURE_UNIQUENESS)
#error DAL_ENSURE_UNIQUENESS not defined in "defines.h"
#endif

#if !defined(MAX_DB_NAME_LENGTH)
#error MAX_DB_NAME_LENGTH not defined in "defines.h"
#endif

#if !defined(CASE_SENSITIVE_DB_NAMES)
#error MAX_DB_NAME_LENGTH not defined in "defines.h"
#endif

#if !defined(USE_DEFAULT_DB_NAME)
#error USE_DEFAULT_DB_NAME not defined in "defines.h"
#endif

#if !defined(LOCK_TIMEOUT_PERIOD)
#error LOCK_TIMEOUT_PERIOD not defined in "defines.h"
#endif

#if !defined(UNIQUENESS_CHECK_TIMEOUT_PERIOD)
#error UNIQUENESS_CHECK_TIMEOUT_PERIOD not defined in "defines.h"
#endif

#if USE_DEFAULT_DB_NAME
	#if !defined(DEFAULT_DB_NAME)
	#error DEFAULT_DB_NAME needs to be defined if USE_DEFAULT_DB_NAME is TRUE
	#endif
#else
	#if defined(DEFAULT_DB_NAME)
	#error DEFAULT_DB_NAME can not be defined if USE_DEFAULT_DB_NAME is FALSE
	#endif
#endif

#if !defined(IS_SUPPORTED_DB_OPERATOR)
#error MAX_DB_NAME_LENGTH not defined in "defines.h"
#endif

#if !defined(ATTRIBUTE_FORMAT_STRING_SUPPORTED)
#error ATTRIBUTE_FORMAT_STRING_SUPPORTED not defined in "defines.h"
#endif

#if !defined(ATTRIBUTE_FORMAT_SINT32_SUPPORTED)
#error ATTRIBUTE_FORMAT_SINT32_SUPPORTED not defined in "defines.h"
#endif

#if !defined(ATTRIBUTE_FORMAT_UINT32_SUPPORTED)
#error ATTRIBUTE_FORMAT_UINT32_SUPPORTED not defined in "defines.h"
#endif

#if !defined(ATTRIBUTE_FORMAT_BIG_NUM_SUPPORTED)
#error ATTRIBUTE_FORMAT_BIG_NUM_SUPPORTED not defined in "defines.h"
#endif

#if !defined(ATTRIBUTE_FORMAT_REAL_SUPPORTED)
#error ATTRIBUTE_FORMAT_REAL_SUPPORTED not defined in "defines.h"
#endif

#if !defined(ATTRIBUTE_FORMAT_TIME_DATE_SUPPORTED)
#error ATTRIBUTE_FORMAT_TIME_DATE_SUPPORTED not defined in "defines.h"
#endif

#if !defined(ATTRIBUTE_FORMAT_BLOB_SUPPORTED)
#error ATTRIBUTE_FORMAT_BLOB_SUPPORTED not defined in "defines.h"
#endif

#if !defined(ATTRIBUTE_FORMAT_MULTI_UINT32_SUPPORTED)
#error ATTRIBUTE_FORMAT_MULTI_UINT32_SUPPORTED not defined in "defines.h"
#endif

#if !defined(ATTRIBUTE_FORMAT_COMPLEX_SUPPORTED)
#error ATTRIBUTE_FORMAT_COMPLEX_SUPPORTED not defined in "defines.h"
#endif

#if !defined(MODULE_GUID)
#error MODULE_GUID not defined in "defines.h"
#endif

#if !defined(MODULE_GUID_STRING)
#error MODULE_GUID_STRING not defined in "defines.h"
#endif

#if USE_AUTHENTICATION || USE_RECORD_SIGNING || ALLOW_REMOTE_LOCALITY || !ALLOW_LOCAL_LOCALITY || MODULE_PARAMETERS_REQUIRED || ATTRIBUTE_FORMAT_COMPLEX_SUPPORTED || ATTRIBUTE_FORMAT_BIG_NUM_SUPPORTED || ATTRIBUTE_FORMAT_TIME_DATE_SUPPORTED
#error Feature not yet supported by the DAL
#endif

#endif /* end section for DAL source */

/*
 * Constants
 */

#define MAX_REG		 256
#define MAX_TABLE_NAME_LEN 129
#define CSSM_PREFERENCE_GLOBAL 0
#define DAL_OOB ((uint32)(-1))
#define DAL_RECORDTYPE_OOB ((CSSM_DB_RECORDTYPE)((uint32)CSSM_DB_RECORDTYPE_APP_DEFINED_START - 1))

/*
 * Macro Definitions
 */

#ifdef _DEBUG /* Debug Macros can be defined as nothing */

	#define VERIFY_PTR(p) ASSERT(p)
	#define ASSERT_VALID_HEAP_POINTER(p, size) IsValidHeapPointer((void *)p, size)
	#define ASSERT_DAL_VALID()

	DLINTERNAL void IsValidHeapPointer(void *, uint32 size);

	#ifdef _WIN32
		#define DEBUG_BREAK() { \
			if (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, NULL)) \
				_CrtDbgBreak(); \
			}

		#ifndef ASSERT
			#include <crtdbg.h>
			#define ASSERT(expr) { \
					if ((!(expr)) && (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, NULL))) \
						_CrtDbgBreak(); \
					}
		#endif
	#else /* not Win32 */
		#define DEBUG_BREAK() ASSERT(FALSE)
		#ifndef ASSERT
			#include <assert.h>
			/* Passing this boolean expression is nice when checking
			 * pointers on 64-bit architectures. It prevents lots of
			 * compiler warnings about "conversion from pointer to
			 * smaller integer"
			 */
			#define ASSERT(n) assert(0 != (n))
		#endif
	#endif /* end of not win32 */

#else /* Release Mode */
	#define VERIFY_PTR(p); if (!p) { return CSSMERR_DL_INTERNAL_ERROR; }
	#define AssertValid()
	#define AssertValidNotInitialized()
	#define ASSERT_VALID_HEAP_POINTER(p, size)
	#define ASSERT_DAL_VALID()
	#define DEBUG_BREAK()

	#ifndef ASSERT
		#define ASSERT(expr)
	#endif
#endif /* End Release Mode */

#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif

/* macro to cast from standard bool to CSSM_BOOL */
#define CBOOL(Bool) ((Bool) ? CSSM_TRUE : CSSM_FALSE)

#define TODO(n)

/*
 * Type Definitions
 */

typedef CSSM_DATA const * DAL_CONST_DATA_PTR;

#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS || MODULE_PARAMETERS_USE_DB_INFO || MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION

	typedef struct dal_module_parameters
	{
	#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS
		/* Paramaters to DL API function */
		CSSM_NET_ADDRESS const * DbLocation;
		CSSM_DB_ACCESS_TYPE AccessRequest;
		CSSM_ACCESS_CREDENTIALS const * AccessCred;
		char* AccessPath;
		const void * OptionalParameters; /* Optional parameters passed into DB Create or DB open */
	#endif

	#if MODULE_PARAMETERS_USE_DB_INFO
		/* DbInfo of the database */
		CSSM_DBINFO DbInfo;
	#endif

	#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
		/* Additional Information to store and retieve from the registry; this information
		   is set by the backend in DbCreate. The DAL will then supply this information
		   to the backend during DbDelete and DbOpen */
		CSSM_DATA AdditionalInformation;
	#endif

	} DAL_MODULE_PARAMETERS;

	typedef DAL_MODULE_PARAMETERS *DAL_MODULE_PARAMETERS_PTR;

	#define CONST_MODULE_PARAMETERS_DECL , const DAL_MODULE_PARAMETERS * pParameters
	#define MODULE_PARAMETERS_DECL , DAL_MODULE_PARAMETERS * pParameters
	#define MODULE_PARAMETERS , pParameters
	#define DAL_USE_MODULE_PARAMATERS 1
#else
	#define CONST_MODULE_PARAMETERS_DECL
	#define MODULE_PARAMETERS_DECL
	#define MODULE_PARAMETERS
	#define DAL_USE_MODULE_PARAMATERS 0
#endif

#if MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION
	#define ADDITIONAL_INFORMATION &Parameters.AdditionalInformation.Length, Parameters.AdditionalInformation.Data
#else
	#define ADDITIONAL_INFORMATION NULL, NULL
#endif

typedef struct open_param {
	DAL_MODULE_PARAMETERS *dbParameters;
	char dbName[MAX_DB_NAME_LENGTH+1];
} DAL_DB_OPEN_PARAM, *DAL_DB_OPEN_PARAM_PTR;


/*
 * Function declarations
 */


#ifndef MODULE_DIRECTORY_SERVICES /* Turn this feature off for MDS */

CSSM_MODULE_HANDLE dal_CSSM_ModuleLoadAndAttach (
	const CSSM_SUBSERVICE_UID * pSubserviceUid,
	const CSSM_API_MEMORY_FUNCS * MemoryFuncs);

CSSM_RETURN dal_CSSM_ModuleDetachAndUnload (
	CSSM_MODULE_HANDLE ModuleHandle,
	const CSSM_GUID * pGuid);

CSSM_RETURN dal_CSSM_GetAPIMemoryFunctions (CSSM_HANDLE AddInHandle, CSSM_API_MEMORY_FUNCS_PTR MemFuncs);

#endif /* End of functions removed for MDS */

#ifdef __cplusplus
}
#endif

#endif
