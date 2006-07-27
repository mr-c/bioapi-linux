/*
 * PRECOMP.H
 *
 * Internal header for including all the things required for compilation of
 * the framework.
 */

#ifndef _PRECOMP_H
#define _PRECOMP_H

/* Public BioAPI headers that we'll need */
#include "bioapi.h"
#include "biospi_type.h"
#include "bioapi_uuid.h"
#include "bioapi_util.h"

/* CDSA header files used by this project */
#include "cssmtype.h"
#include "port/bioapi_port.h"
#include "port/bioapi_lock.h"
#include "intel/mds_util_api.h"
#include "intel/mds_util_helper.h"

/* Private headers */
#include "internal.h"

/* System-standard headers */
#include <time.h>
#include <assert.h>

#endif
