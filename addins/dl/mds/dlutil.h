/*-----------------------------------------------------------------------
 * File: DLUTIL.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file declares the function prototypes for all interfaces included in
 * the DL utility library (dlutil.lib)
 */

#ifndef _DLUTIL_H
#define _DLUTIL_H

#include "bioapi.h"
#include "port/bioapi_port.h"
#include "port/bioapi_lock.h"
#include "biospi_type.h"

#include "cssmerr.h"
#include "intel/mds_util_type.h"
#include "dlutil_struct.h"
#include "maf_config.h"
#include "maf_collectn.h"
#include "maf_interface.h"
#include "maf_util.h"
#include "maf_callouts.h"

#ifdef __cplusplus
extern "C" {
#endif


CSSM_RETURN
dl_GetCSSMAPIFuncPtrs( ADDIN_MODULE_HANDLE CSSMModulePtr,
					   DLUTIL_CSSM_FUNCTIONS * pDLUtilFuncs );

CSSM_RETURN
dl_CopyDbInfo(CSSM_DBINFO_PTR pDBInfoToRtn,
			  CSSM_DBINFO const * pDBInfoToCopy );

void dlnr_FreeDbInfo(CSSM_DBINFO *pDbInfo);
void dlnr_static_dlregGenerateKeyName(char* szBuffer,
									  const char* szModuleGuid,
									  const char* DbName);

/*
 * DL Schema file functions - interacting with the schema.ffdb file
 */
CSSM_RETURN dl_schemaFileAddDb(const char* szModuleGuid,
							   const char* DbName,
							   const CSSM_DBINFO* pDbInfo,
							   const CSSM_DATA* pAdditionalInformation);

CSSM_RETURN dl_schemaFileReadDb(const char* szModuleGuid,
								const char* DbName,
								CSSM_DBINFO* pDbInfo,
								uint32* pAdditionalInformationLength,
								uint8* pAdditionalInformationData);

CSSM_RETURN dl_schemaFileDeleteDb(const char* szModuleGuid,
								  const char* DbName);

CSSM_RETURN dl_ReadDLDataStoreInfo(char* schemaPath,
								   CSSM_DBINFO_PTR DSInfo);

CSSM_RETURN dl_WriteDLDataStoreInfo(char* schemaPath,
									CSSM_DBINFO_PTR DSInfo);

/*
 * DL Registry functions - interact with the DL registry (not the CSSM registry)
 * These functions should be used for application created databases
 */
CSSM_RETURN dl_dlregAddDb(const char* szModuleGuid,
						  const char* DbName,
						  const CSSM_DBINFO* pDbInfo,
						  const CSSM_DATA* pAdditionalInformation);

CSSM_BOOL dl_dlregDoesDbExist(const char* szModuleGuid,
							  const char* DbName);

CSSM_RETURN dl_dlregReadDb(const char* szModuleGuid,
						   const char* DbName,
						   CSSM_DBINFO* pDbInfo,
						   uint32* pAdditionalInformationLength,
						   uint8* pAdditionalInformationData);

CSSM_RETURN dl_dlregDeleteDb(const char* szModuleGuid,
							 const char* DbName);

CSSM_RETURN dl_dlregReadDbNames(const char* szModuleGuid,
								CSSM_NAME_LIST_PTR pNameList);

/* type-checking (IsBad) functions */
CSSM_BOOL dl_IsBadStrPtr(const char * StringIn, CSSM_BOOL *fInvalidChar);
CSSM_BOOL dl_IsBadCssmDataPtr( CSSM_DATA const * Data );
CSSM_RETURN dl_IsDbNameOk( const char *DbName );
CSSM_RETURN dl_IsAttributeInfoOk( CSSM_DB_ATTRIBUTE_INFO const * AttributeInfo );
CSSM_RETURN dlutil_IsIndexInfoOk(CSSM_DB_INDEX_INFO_PTR IndexInfo);
CSSM_RETURN dl_IsInputRecordAttributeDataOk( CSSM_DB_RECORD_ATTRIBUTE_DATA const * Attributes );
CSSM_RETURN dl_IsOutputRecordAttributeDataOk( CSSM_DB_RECORD_ATTRIBUTE_DATA const * Attributes );
CSSM_RETURN dl_IsDbInfoOk( CSSM_DBINFO const * DbInfoIn );
CSSM_RETURN dl_IsQueryOk( CSSM_QUERY const * Query );
CSSM_RETURN dl_IsUniqueRecordIdOk(CSSM_DB_UNIQUE_RECORD const * UniqueRecordIdentifier );
void cssm_FreeDbAttributeInfo(CSSM_DB_ATTRIBUTE_INFO_PTR Info);
CSSM_RETURN cssm_FreeDbInfo (CSSM_DBINFO_PTR Info);
CSSM_RETURN freeDbInfoRecord (CSSM_DBINFO_PTR Info, uint32 index);
unsigned dlutil_IsUnsupportedRecordType(CSSM_DB_RECORDTYPE rt);

#ifdef __cplusplus
}
#endif

#endif	/* _DLUTIL_H */
