/*-----------------------------------------------------------------------
 * File: PORTINIT.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/* Portions:
 * (C) COPYRIGHT International Business Machines Corp. 1996, 1997
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include "internal.h"


/*-----------------------------------------------------------------------------
 * Variables keeping track of initialized state
 *---------------------------------------------------------------------------*/
static CSSM_BOOL port_initialized = CSSM_FALSE;

/*-----------------------------------------------------------------------------
 * Name: port_init
 *
 * Description: Prepares this library for use by an application. 
 * 
 * Parameters: None. 
 *
 * Returns:
 * CSSM_OK if the library was successfully initialized
 * CSSM_FAIL if an error occured
 *
 * Error Codes: None.
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI port_init()
{
    TRACE(port_trace_info, FNCODE_cssm_port_init);

    port_initialized = CSSM_TRUE;
    RETURN(CSSM_OK);
}


/*-----------------------------------------------------------------------------
 * Name: port_terminate
 *
 * Description: Performs the necessary cleanup. 
 * 
 * Parameters: None. 
 *
 * Returns:
 * CSSM_OK if successfully completed
 * CSSM_FAIL if an error occured
 *
 * Error Codes: None.
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI port_terminate()
{
    port_initialized = CSSM_FALSE;

    RETURN(CSSM_OK);
}
