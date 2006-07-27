/*-----------------------------------------------------------------------
 * File: INTERNAL.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */
 
/*-----------------------------------------------------------------------
 * This file contains all of the includes and several definitions needed
 * for the porting library.
 */


#ifndef _INTERNAL_H_
#define _INTERNAL_H_

/* CDSA header files */
#include "cssmport.h"
#include "cssmerr.h"
#include "cssmlock.h"

#if defined (UNIX)
#define DELIMITER           DIRECTORY_SEPARATOR_STRING
#define MAX_SECTION_LENGTH  256
#define MAX_CRITERIA_LENGTH 256
#endif

#if defined (WIN32)
#include <process.h>    /* _beginthread, _beginthreadex */
#include <direct.h>     /* getcwd */
#define MAP_SIZE 0x4000
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#endif  /* WIN32 */

#if defined(__digital__)
#include "compaq/dprintf.h"
#endif  /* __digital__ */
/* standard includes */
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* trace header files */
#include "trc_util.h"
#include "port_codefile.h"

/* private error codes */
#include "fwkpriverr.h"

#endif /* _INTERNAL_H_ */
