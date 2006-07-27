/*-----------------------------------------------------------------------
 * File: MDS_UTIL_API.H
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

#ifndef _MDS_UTIL_API_H
#define _MDS_UTIL_API_H    

/* API Functions */

#ifdef __cplusplus
extern "C" {
#endif

#include "mds_util_type.h"

/* Module Management functions */
CSSM_RETURN CSSMAPI MDSUTIL_ModuleInstall (const char *ModuleName,
										   const char *ModuleFileNames,
										   const char *ModulePathName,
										   const char *ModuleCredentialName,
										   const char *ModuleCredentialPath,
										   const CSSM_GUID *GUID,
										   const MDSUTIL_MODULE_INFO *ModuleDescription,
										   const void * Reserved1,
										   const CSSM_DATA *Reserved2);

CSSM_RETURN CSSMAPI MDSUTIL_ModuleUninstall (const CSSM_GUID *GUID);

CSSM_RETURN CSSMAPI MDSUTIL_ListModules (CSSM_SERVICE_MASK UsageMask,
                                         CSSM_BOOL MatchAll, 
										 MDSUTIL_LIST_PTR *pList);

CSSM_RETURN CSSMAPI MDSUTIL_GetModuleInfo (const CSSM_GUID *ModuleGUID,
										   CSSM_SERVICE_MASK UsageMask,
										   uint32 SubserviceID,										   
										   CSSM_USEE_TAG USEERequest,
										   MDSUTIL_MODULE_INFO_PTR *pModuleInfo);

CSSM_RETURN CSSMAPI MDSUTIL_GetCredLocationFromGUID (const CSSM_GUID *ModuleGUID, 
													 CSSM_DATA *pModulePath, /* full path to module*/
													 CSSM_DATA *pModuleCredentialPath,
													 CSSM_API_MEMORY_FUNCS_PTR MemoryFuncs);/* Full path to ModuleCredential*/ 

CSSM_RETURN CSSMAPI MDSUTIL_FreeModuleInfo (MDSUTIL_MODULE_INFO_PTR ModuleInfo);

/* Utility Functions */

CSSM_RETURN CSSMAPI MDSUTIL_FreeModuleList (MDSUTIL_LIST_PTR List);

CSSM_RETURN CSSMAPI MDSUTIL_ListModuleManagers (CSSM_GUID_PTR *ModuleManagerGuids,
												uint32 *NumberOfModuleManagers);

CSSM_RETURN CSSMAPI MDSUTIL_GetModuleManagerInfo (const CSSM_GUID *ModuleGUID,
												  CSSM_SERVICE_MASK ServiceType,
												  MDSUTIL_MODULE_MANAGER_INFO_PTR *moduleManagerInfo);

CSSM_RETURN CSSMAPI MDSUTIL_ModuleManagerInstall (const char *ModuleManagerName,
												  const char *ModuleManagerFileName,
												  const char *ModuleManagerPathName,
												  const char *ModuleManagerCredentialName,
												  const char *ModuleManagerCredentialPath,
												  const CSSM_GUID *ModuleManagerGuid,
												  const MDSUTIL_MODULE_MANAGER_INFO *ModuleManagerDescription,
												  const void *Reserved1,
												  const CSSM_DATA *Reserved2);

CSSM_RETURN CSSMAPI MDSUTIL_ModuleManagerUninstall (const CSSM_GUID *ModuleManagerGuid);

CSSM_RETURN CSSMAPI MDSUTIL_InitEx(CSSM_BOOL ReadWrite);

#define MDSUTIL_Init() MDSUTIL_InitEx(CSSM_FALSE)

void CSSMAPI MDSUTIL_Term(void);


#ifdef __cplusplus
}
#endif

#endif
