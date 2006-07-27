/*-----------------------------------------------------------------------
 * File: FF_UTIL.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file defines flat file specific private functions
 */

#ifndef FF_UTIL_H
#define FF_UTIL_H

#ifdef DAL_SOURCE_FILE
#error This file should not be included by the DAL
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "ff_common.h"

uint32 ffutil_GetTime();

/* Functions to work with an array of CSSM_DATA's */
CSSM_RETURN ffutil_AllocateData(CSSM_DATA_PTR *prgData, uint32 NumDataStructs);

void ffutil_nrFreeData(CSSM_DATA_PTR rgData, uint32 NumDataStructs);

/* Functions to work with a CSSM_DATA structure */
CSSM_RETURN ffutil_CopyData(CSSM_DATA_PTR Destination, DAL_CONST_DATA_PTR Source);

CSSM_RETURN ffutil_CopyDataToApp(
	CSSM_DL_HANDLE DLHandle,
	CSSM_DATA_PTR Destination,
	DAL_CONST_DATA_PTR Source);

/* Misc functions */
uint32 ffutil_Hash(DAL_CONST_DATA_PTR pData);

CSSM_RETURN ffutil_GetUniqueRecordIdVal(CSSM_DB_UNIQUE_RECORD const * UniqueIdPtr,
										uint32 *pData);

CSSM_BOOL ffutil_fIsBadDbName(const char *DbName);

FF_BOOL ffutil_Contains(const CSSM_DATA * pData, const CSSM_DATA *pPredicate, uint32 WordSize);
FF_BOOL ffutil_MemCompare(const CSSM_DATA * pData, const CSSM_DATA *pPredicate, uint32 IsNotEqualReq);

#ifdef __cplusplus
}
#endif

#endif
