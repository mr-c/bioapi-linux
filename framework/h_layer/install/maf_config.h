/*-----------------------------------------------------------------------
 * File: MAF_CONFIG.H
 *
 * Copyright (c) 1999-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#ifndef MAF_CONFIG_H
#define MAF_CONFIG_H

/* Major version of the BioAPI interface */
#define BIOAPI_SPEC_VERSION_MAJOR				  BioAPI_MAJOR

/* Minor version of the BioAPI interface */
#define BIOAPI_SPEC_VERSION_MINOR				  BioAPI_MINOR

/* Major and minor versions of the product */
#define BIOAPI_HLAYER_VERSION_MAJOR			BioAPI_MAJOR
#define BIOAPI_HLAYER_VERSION_MINOR			BioAPI_MINOR

/* Is the addin module thread safe? */
#define ADDIN_THREAD_SAFE					1

/* Macro used to define if a subservice pair (id, type) is valid for an addin */
#define ADDIN_HRS_SUBSERVICE_ID				0

/* Does the addin module require the writer lock durring SPI functions on the load
tracker node (is this node being modified?) */
#define ADDIN_NEED_LOAD_WRITER_LOCK			0

/* Does the addin module require the writer lock durring SPI functions on the attach
tracker node (is this node being modified?) */
#define ADDIN_NEED_ATTACH_WRITER_LOCK		0

/* Does the addin module require the custom ADDIN_LOAD_DATA structure (see below) */
#define ADDIN_NEED_ADDIN_LOAD_STRUCT		0

/* Does the addin module require the custom ADDIN_ATTACH_DATA structure (see below) */
#define ADDIN_NEED_ADDIN_ATTACH_STRUCT		0

/* Macro used to define if a subservice pair (id, type) is valid for an addin */
#define ADDIN_IS_VALID_SUBSERVICE(SubServiceId, SubServiceType) \
	(((SubServiceId) == 0) && ( ((SubServiceType) == CSSM_SERVICE_HRS) \
							  ) \
	)

#if (ADDIN_NEED_ADDIN_LOAD_STRUCT)
/*-----------------------------------------------------------------------------
 * Description:
 * Structure containing custom fields in the CSSM_MODULE_LOAD_TRACKER structure
 *---------------------------------------------------------------------------*/
struct addin_load_data
{
	/* put custom fields here */
};
typedef struct addin_load_data ADDIN_LOAD_DATA;

#endif /* if ADDIN_NEED_ADDIN_LOAD_STRUCT */


#if (ADDIN_NEED_ADDIN_ATTACH_STRUCT)

/*-----------------------------------------------------------------------------
 * Description:
 * Structure containing custom fields in the CSSM_MODULE_ATTACH_TRACKER structure
 *---------------------------------------------------------------------------*/
struct addin_attach_data
{
	/* put custom fields here */
};
typedef struct addin_attach_data ADDIN_ATTACH_DATA;

#endif /* if ADDIN_NEED_ADDIN_ATTACH_STRUCT */

#endif
