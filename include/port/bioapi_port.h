/*-----------------------------------------------------------------------
 * File: BIOAPI_PORT.H
 *-----------------------------------------------------------------------
 */

#ifndef _BIOAPIPORT_H
#define _BIOAPIPORT_H

/* We may have to define the CSSM version of memory tracking */
#ifdef BioAPI_MEMTRACK_ON
#define CSSM_MEMTRACK_ON
#endif

/* Just include the CSSM port header */
#include "port/cssmport.h"

/* Linux-Port: Windows to Linux/Unix specific definitions */
#if defined (LINUX) || defined (UNIX) || defined (SOLARIS)
#define DWORD uint32
#endif

#endif
