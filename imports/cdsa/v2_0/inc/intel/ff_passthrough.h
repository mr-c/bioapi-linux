/*-----------------------------------------------------------------------
 * File: FF_PASSTHROUGH.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#include "cssmerr.h"

#define FFDL_PASSTHROUGH_SET_WRITE_BACK_CACHE    1

/* Passthrough data types */
typedef enum ffdl_write_back_cache_status
{
	FFDL_ENABLE_CACHE = 0,
	FFDL_DISABLE_CACHE = 1
} FFDL_WRITE_BACK_CACHE_STATUS;


/* Passthrough error codes */
#define FFDL_CACHE_STATUS_INVALID        (CSSM_DL_PRIVATE_ERROR + 0)
#define LAST_FFDL_PASSTHROUGH_ERROR      (FFDL_CACHE_STATUS_INVALID)
