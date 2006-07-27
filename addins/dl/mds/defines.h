/*-----------------------------------------------------------------------
 * File: DEFINES.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file #defines for flags to adjust the functionality of the DAL (flat
 * file version)
 */

#ifndef DEFINES_H
#define DEFINES_H
#include "port/bioapi_port.h"

/* Flag to determine if the backend supports authentication (must be FALSE) */
#define USE_AUTHENTICATION				 CSSM_FALSE

/* Flag to determine if the backend supports record signing (must be FALSE) */
#define USE_RECORD_SIGNING				 CSSM_FALSE

/* Flag to determine if the backend supports DL_Passthrough (must be FALSE) */
#define USE_DL_PASSTHROUGH				 CSSM_FALSE

/* Flag to determine if the backend supports remote records (must be TRUE) */
#define ALLOW_REMOTE_LOCALITY			 CSSM_FALSE

/* Flag to determine if the backend supports local records (must be TRUE) */
#define ALLOW_LOCAL_LOCALITY			 CSSM_TRUE

/* Effects the contence of DAL_MODULE_PARAMETERS, see definition for more information */
#define MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS CSSM_TRUE

/* Effects the contence of DAL_MODULE_PARAMETERS, see definition for more information */
#define MODULE_PARAMETERS_USE_DB_INFO	 CSSM_FALSE

/* Effects the contence of DAL_MODULE_PARAMETERS, see definition for more information */
#define MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION CSSM_TRUE

/* The Maximum Length of the additional information, define ONLY if
   MODULE_PARAMETERS_USE_ADDITIONAL_INFORMATION is defined */
#define ADDITIONAL_INFORMATION_MAX_LENGTH 155

/*
 * Should the DAL ensure the Uniqueness of Relation's Primary Key? (Otherwise,
 * the backend must do it).
 */
#define DAL_ENSURE_UNIQUENESS            CSSM_TRUE


/*
 * The maximum supported length of a DbName. Please Keep in mind that arrays of
 * this size will be allocated
 */
#define MAX_DB_NAME_LENGTH				 (255 - ADDITIONAL_INFORMATION_MAX_LENGTH)

/* Are Db Names Case Sensitive? (FALSE == No, TRUE == yes) */
#define CASE_SENSITIVE_DB_NAMES          CSSM_TRUE

/* Is there a default DbName that will be used if DbName = NULL or ""*/
#define USE_DEFAULT_DB_NAME              CSSM_FALSE

/* Name of the Db used if DbName = NULL or "" (not defined if USE_DEFAULT_DB_NAME is false) */
/* #define DEFAULT_DB_NAME						"DefaultDbNameGoesHere" */

/* Timeout value used for obtaining a lock. (in milliseconds) */
#define LOCK_TIMEOUT_PERIOD				 1000

/* Timeout value used for checking the uniqueness of indexes on an insert (in seconds) */
#define UNIQUENESS_CHECK_TIMEOUT_PERIOD	 2

/* Macro to define supported Db Operators */
#define IS_SUPPORTED_DB_OPERATOR(op) \
	((op) == CSSM_DB_CONTAINS || (op) == CSSM_DB_EQUAL || (op) == CSSM_DB_NOT_EQUAL)

/* Flags to define which attribute formats are supported supported */
#define ATTRIBUTE_FORMAT_STRING_SUPPORTED             CSSM_TRUE 
#define ATTRIBUTE_FORMAT_SINT32_SUPPORTED             CSSM_TRUE 
#define ATTRIBUTE_FORMAT_UINT32_SUPPORTED             CSSM_TRUE 
#define ATTRIBUTE_FORMAT_BIG_NUM_SUPPORTED            CSSM_FALSE 
#define ATTRIBUTE_FORMAT_REAL_SUPPORTED               CSSM_TRUE 
#define ATTRIBUTE_FORMAT_TIME_DATE_SUPPORTED          CSSM_FALSE 
#define ATTRIBUTE_FORMAT_BLOB_SUPPORTED               CSSM_TRUE 
#define ATTRIBUTE_FORMAT_MULTI_UINT32_SUPPORTED       CSSM_TRUE 
#define ATTRIBUTE_FORMAT_COMPLEX_SUPPORTED            CSSM_FALSE 

/* Define the major and minor version, and GUID of the addin module */
#define MODULE_GUID_INIT					intel_mds_guid_init
#define MODULE_GUID							intel_mds_guid
#define MODULE_GUID_STRING					"{692bcef0-4540-11d3-a8f3-0090271d266f}"

#endif
