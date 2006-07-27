/*-----------------------------------------------------------------------
 * File: DLUTIL_REG.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file declares the Registry Routines used in dlutil and mds_install
 */

#ifndef _DLUTIL_REG_H
#define _DLUTIL_REG_H

#include "cssmerr.h"
#include "port/bioapi_port.h"

#define REGISTRY_PREFERENCE_GLOBAL 0

#ifdef __cplusplus
extern "C" {
#endif

typedef char **CSSM_REG_LIST, ***CSSM_REG_LIST_PTR;

/* Registry Routines */
#define DLUTIL_CONFIG_PERSONAL	1
#define DLUTIL_CONFIG_GLOBAL	0
#define DLUTIL_VALUE_BINARY		0
#define DLUTIL_VALUE_STRING		1

CSSM_RETURN dlutil_port_SetValue(const char* Section,
								 const char* Name,
								 const void* Value,
								 sint32 ValueLength,
								 sint32 ValueType,
								 sint32 PersonalPref);

CSSM_RETURN dlutil_port_GetValue(const char* Section,
								 const char* Name,
								 void* Value,
								 sint32* ValueLength,
								 sint32* ValueType,
								 sint32 PersonalPref);

CSSM_RETURN dlutil_port_DeleteSection(const char* Section,
									  sint32 PersonalPref);

CSSM_RETURN dlutil_port_GetValueCount(const char* section,
									  sint32* count,
									  sint32 PersonalPref);

CSSM_RETURN dlutil_port_GetSectionList(const char* section,
									   CSSM_REG_LIST_PTR list,
									   sint32 PersonalPref);
#ifdef __cplusplus
}
#endif

#endif /* _DLUTIL_REG_H */
