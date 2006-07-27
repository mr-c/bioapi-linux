/*-----------------------------------------------------------------------
 * File: MAF_INCLUDE.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/*
 * Main include file for MAF. All files should include this file first
 * before any custom files. This will insure proper inclusion of all MAF
 * files. The file "maf_setup.h" should be created by the SP developer to
 * limit the included files to those used by a specific SP type.
 */

#ifndef __MAF_INCLUDE_H__
#define __MAF_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Standard headers */
#include <assert.h>

/* BioAPI headers */
#include "bioapi_type.h"
#include "biospi_type.h"
#include "bioapi_err.h"
#include "bioapi_uuid.h"

/* Porting library headers */
#include "port/bioapi_port.h"
#include "port/bioapi_lock.h"

/* MAF headers required by all SPs */
#include "maf_collectn.h"
#include "maf_util.h"

/* Project specific configuration */
#ifndef MAF_NO_CONFIG_HEADER
# include "maf_config.h"
#else
  /* Make the other headers think maf_config.h was included */
# define MAF_CONFIG_H
#endif

/* MAF headers required by all SPs; cont. */
#include "maf_interface.h"
#include "maf_callouts.h"

/* MAF Utility headers usable by all SPs */

#ifdef __cplusplus
}
#endif

#endif /* ifndef __MAF_INCLUDE_H__ */
