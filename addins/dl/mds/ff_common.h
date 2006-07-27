/*-----------------------------------------------------------------------
 * File: FF_COMMON.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the declaration of data types, constants and macros that
 * are used throughout the flatfile backend
 */

#ifndef FF_COMMON_H
#define FF_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dal_common.h"

typedef uint32 FF_ERROR;

/* this is NOT one of those silly CSSM_BOOLs, it is a standard BOOL, zero for false, non-zero for true,
   changing the #defines will just break stuff */
typedef uint32 FF_BOOL;
#define FF_FALSE (0) /* This can't change */
#define FF_TRUE ((FF_BOOL)(~0)) /* any non-zero value will do */

/* Flatfile Errors */
#define FF_FAIL ((uint32)CSSM_FAIL)
#define FF_OK ((uint32)CSSM_OK)

#define FF_FUNCTION_BEGIN(FunctionName)

/* For catching the error code returned by functions that return FF_ERROR */
#define CATCH_FUNCTION(func) { FF_ERROR Error; if ((Error = func) != FF_OK)
#define IGNORE_ERROR() }
#define THROW_ERROR() return Error; }
#define RETURN_ERROR() return Error; }
/*#define RETURN_ERROR() dal_SetError(Error); return CSSM_FAIL; }*/

/* For functions that can just return a failure if a function failes */
#define RETURN_FAILURE(func) \
	CATCH_FUNCTION(func) { \
	RETURN_ERROR() }

#define THROW_FAILURE(func) \
	CATCH_FUNCTION(func) { \
	THROW_ERROR() }

/* Macro that is used to throw a new error code */
#if defined (_DEBUG) && defined(_WIN32)
	FF_ERROR ffutil_debug_eOutputError(FF_ERROR Error, uint32 Line, const char * File);

	#define THROW_NEW_ERROR(Error) return ffutil_debug_eOutputError(Error, __LINE__, __FILE__)
#else
	#define THROW_NEW_ERROR(Error) return Error
#endif

/* Maximum length for a filename, directoy name w/ db name prefix, and a directory name and
   filename togethor */
#define MAX_FILE_NAME_LENGTH (sizeof(".freelist.ffdb") + 8 - 1)
#define MAX_PATH_START_LENGTH (ADDITIONAL_INFORMATION_MAX_LENGTH - 1)
#define MAX_PATH_NAME_LENGTH (MAX_FILE_NAME_LENGTH + MAX_PATH_START_LENGTH)

/* Maximum size in bytes for a null terminated string containing a filename, a directoy
   name w/ db name prefix, and a directory name and filename togethor */
#define MAX_FILE_NAME_SIZE (MAX_FILE_NAME_LENGTH + 1)
#define MAX_PATH_START_SIZE (MAX_PATH_START_LENGTH + 1)
#define MAX_PATH_NAME_SIZE	(MAX_PATH_NAME_LENGTH + 1)

typedef char PATH_NAME[MAX_PATH_NAME_SIZE];
typedef char PATH_START[MAX_PATH_START_SIZE];
typedef char FILE_NAME[MAX_FILE_NAME_SIZE];

#ifdef __cplusplus
}
#endif

#endif
