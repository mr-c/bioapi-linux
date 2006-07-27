/*-----------------------------------------------------------------------
 * File: MDS_UTIL_HELPER.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#ifndef _MDS_UTIL_HELPER_H
#define _MDS_UTIL_HELPER_H
#include "cssmtype.h"
#include "mds_util_type.h"

/* API Functions */

#ifdef __cplusplus
extern "C" {
#endif


BioAPI_RETURN BioAPI mdsutil_GetModulePath (CSSM_DATA ModuleName, 
										   CSSM_DATA ModuleSearchPath, 
										   CSSM_DATA_PTR ModulePath);

BioAPI_RETURN BioAPI mdsutil_GetModuleCredentialInfo (const BioAPI_UUID *guid,
													 CSSM_DATA_PTR Credential, 
													 CSSM_DATA_PTR ModuleName, 
													 CSSM_DATA_PTR ModulePath);

BioAPI_RETURN BioAPI mdsutil_RegGetEMMInfo (const CSSM_SERVICE_MASK EMMType,
										   BioAPI_UUID_PTR pManagerGUID,
										   CSSM_DATA_PTR EMMCredential,
										   CSSM_DATA_PTR EMMModuleName, 
										   CSSM_DATA_PTR EMMPath);

BioAPI_RETURN BioAPI mdsutil_RegReadUsage (const BioAPI_UUID *ModuleGUID,
										  CSSM_SERVICE_MASK *ServiceMask);

BioAPI_RETURN BioAPI mdsutil_RegGetThreadSafe (const BioAPI_UUID *ModuleGUID,
											  CSSM_BOOL *ThreadSafe);

BioAPI_RETURN BioAPI mdsutil_LoadFile (const char *FileName, 
									  CSSM_DATA_PTR FileData);

BioAPI_RETURN BioAPI mdsutil_GetPrimaryManifest( const BioAPI_UUID *ModuleGUID, 
							  CSSM_DATA *Credential,
							  CSSM_SERVICE_TYPE ServiceType);

/* CSSM memory functions required by the MDS itself. Created to
 * operate in an environment free from a CSSM.
 */

#ifndef CSSM_MEMTRACK_ON

void* BioAPI _MDSUTIL_malloc(uint32 mem_size,
                              void*  allocRef);

void* BioAPI _MDSUTIL_calloc(uint32 num_elem,
                              uint32 num_bytes,
                              void*  allocRef);

void* BioAPI _MDSUTIL_realloc(void*  old_ptr,
                               uint32 num_bytes,
                               void*  allocRef);

void BioAPI _MDSUTIL_free(void* mem_ptr,
                           void* allocRef);

#define MDSUTIL_malloc(s, t) _MDSUTIL_malloc((s), (t))
#define MDSUTIL_calloc(s, t, u) _MDSUTIL_calloc((s), (t) ,(u))
#define MDSUTIL_realloc(s, t, u) _MDSUTIL_realloc((s), (t), (u))
#define MDSUTIL_free(s, t) _MDSUTIL_free((s), (t))

#else   /* CSSM_MEMTRACK_ON */

void* BioAPI _MDSUTIL_malloc(uint32 mem_size,
                              void*  allocRef,
                              const  char* szFilename,
                              uint32 u32LineNumber);

void* BioAPI _MDSUTIL_calloc(uint32 num_elem,
                              uint32 num_bytes,
                              void*  allocRef,
                              const  char *szFilename,
                              uint32 u32LineNumber);

void* BioAPI _MDSUTIL_realloc(void*  old_ptr,
                               uint32 num_bytes,
                               void*  allocRef,
                               const  char *szFilename,
                               uint32 u32LineNumber);

void BioAPI _MDSUTIL_free(void* mem_ptr,
                           void* allocRef,
                           const char *File,
                           uint32 Line);

#define MDSUTIL_malloc(s, t) _MDSUTIL_malloc((s), (t), __FILE__, __LINE__)
#define MDSUTIL_calloc(s, t, u) _MDSUTIL_calloc((s), (t) ,(u), __FILE__, __LINE__)
#define MDSUTIL_realloc(s, t, u) _MDSUTIL_realloc((s), (t), (u), __FILE__, __LINE__)
#define MDSUTIL_free(s, t) _MDSUTIL_free((s), (t), __FILE__, __LINE__)

#endif  /* CSSM_MEMTRACK_ON */


#ifdef __cplusplus
}
#endif

#endif
