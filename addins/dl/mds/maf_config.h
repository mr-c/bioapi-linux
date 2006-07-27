/*-----------------------------------------------------------------------
 * File: MAF_CONFIG.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


#ifndef MAF_CONFIG_H
#define MAF_CONFIG_H

#ifdef MODULE_DIRECTORY_SERVICES /* Turn this feature off for MDS */
#include "../mds/defines.h"
#else
#include "dlutil_struct.h"
#include "defines.h"
#endif


/* GUID of the addin module */
#define ADDIN_GUID_INITIALIZER				MODULE_GUID_INIT


/* Name of the module. */
#define ADDIN_NAME							"Intel Flat File DL Module"

#define ADDIN_DL_SUBSERVICE_ID				0

/* Major version of the addin module */
#define ADDIN_VERSION_MAJOR					(CSSM_MAJOR)

/* Minor version of the addin module */
#define ADDIN_VERSION_MINOR					(CSSM_MINOR)

/* Vendor Name */
#define ADDIN_VENDOR_NAME					 "Intel Corporation"

/* Product Version */
#define ADDIN_PRODUCT_VERSION_MAJOR			 INTEL_FF_DSM_MAJOR_VER
#define ADDIN_PRODUCT_VERSION_MINOR			 INTEL_FF_DSM_MINOR_VER

/* Standard Description */
#define ADDIN_STANDARD_DESCRIPTION			 "Common Data Security Architecture"

/* Standars Version */
#define ADDIN_STANDARD_VERSION_MAJOR		2
#define ADDIN_STANDARD_VERSION_MINOR		1

/* Is the addin module thread safe? */
#define ADDIN_THREAD_SAFE					1

/* Does the addin module require the writer lock durring SPI functions on the load
tracker node (is this node being modified?) */
#define ADDIN_NEED_LOAD_WRITER_LOCK			0

/* Does the addin module require the writer lock durring SPI functions on the attach
tracker node (is this node being modified?) */
#define ADDIN_NEED_ATTACH_WRITER_LOCK		0

#ifndef MODULE_DIRECTORY_SERVICES /* This is Not MDS */

/* Does the addin module require the custom ADDIN_LOAD_DATA structure (see below) */
#define ADDIN_NEED_ADDIN_LOAD_STRUCT		1

/* Does the addin module require the custom ADDIN_ATTACH_DATA structure (see below) */
#define ADDIN_NEED_ADDIN_ATTACH_STRUCT		1

#ifdef WIN32
#define ADDIN_SELF_CHECK_SECTION		"inffdl300"
#else
#define ADDIN_SELF_CHECK_SECTION        "libinffdl" 
#endif

#else /* MDS Begin */

#define ADDIN_NEED_ADDIN_LOAD_STRUCT		0
#define ADDIN_NEED_ADDIN_ATTACH_STRUCT		0

#endif /* MDS End */

/* Macro used to define if a subservice pair (id, type) is valid for an addin */
#define ADDIN_IS_VALID_SUBSERVICE(SubServiceId, SubServiceType) \
	((SubServiceId) == 0 && (SubServiceType) == CSSM_SERVICE_DL)

#if (ADDIN_NEED_ADDIN_LOAD_STRUCT)
/*-----------------------------------------------------------------------------
 * Description:
 * Structure containing custom fields in the CSSM_MODULE_LOAD_TRACKER structure
 *---------------------------------------------------------------------------*/
typedef DLUTIL_CSSM_FUNCTIONS ADDIN_LOAD_DATA;

#endif /* if ADDIN_NEED_ADDIN_LOAD_STRUCT */


#if (ADDIN_NEED_ADDIN_ATTACH_STRUCT)

/*-----------------------------------------------------------------------------
 * Description:
 * Structure containing custom fields in the CSSM_MODULE_ATTACH_TRACKER structure
 *---------------------------------------------------------------------------*/
struct addin_attach_data
{
#ifdef DAL_SOURCE_FILE
#ifdef __cplusplus
	DAL_DATABASE_INFO_LIST * pDatabaseList;
#endif
#else
	void * pDatabaseList;

#endif
};
typedef struct addin_attach_data ADDIN_ATTACH_DATA;

#endif /* if ADDIN_NEED_ADDIN_ATTACH_STRUCT */

#endif
