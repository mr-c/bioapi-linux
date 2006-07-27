/*-----------------------------------------------------------------------
 * File: DLUTIL_STRUCT.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file declares the global variables that are created by the dl utility
 * library.
 */

#ifndef _DLUTIL_STRUCT_H
#define _DLUTIL_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cssmtype.h"

/*---------------------------------------------------------------------------------------*
 * CSSM APIs
 *---------------------------------------------------------------------------------------*/

struct dlutil_cssm_functions
{
	CSSM_RETURN
	(CSSMAPI *ModuleAttach)( const CSSM_GUID *ModuleGuid,
								const CSSM_VERSION *Version,
								const CSSM_API_MEMORY_FUNCS *MemoryFuncs,
								uint32 SubserviceID,
								CSSM_SERVICE_TYPE SubServiceType,
								CSSM_ATTACH_FLAGS AttachFlags,
								CSSM_KEY_HIERARCHY KeyHierarchy,
								CSSM_FUNC_NAME_ADDR *FunctionTable,
								uint32 NumFunctionTable,
								const void * Reserved,
								CSSM_MODULE_HANDLE_PTR NewModuleHandle );

	CSSM_RETURN
	(CSSMAPI *ModuleLoad)(const CSSM_GUID *ModuleGuid,
							CSSM_KEY_HIERARCHY KeyHierarchy,
							CSSM_API_ModuleEventHandler AppNotifyCallback,
							const void* AppNotifyCallbackCtx );

	CSSM_RETURN
	(CSSMAPI *ModuleDetach)( CSSM_MODULE_HANDLE ModuleHandle );

	CSSM_RETURN
	(CSSMAPI *ModuleUnload)( const CSSM_GUID *ModuleGuid,
								CSSM_API_ModuleEventHandler AppNotifyCallback,
								const void* AppNotifyCallbackCtx );

	CSSM_RETURN
	(CSSMAPI *GetAPIMemoryFunctions)( CSSM_MODULE_HANDLE AddInHandle,
						CSSM_API_MEMORY_FUNCS_PTR AppMemoryFuncs );

	/* cert functions */
	CSSM_RETURN
	(CSSMAPI *cl_CertGetFirstFieldValue)( CSSM_CL_HANDLE CLHandle,
											const CSSM_DATA *Cert,
											const CSSM_OID *CertField,
											CSSM_HANDLE_PTR ResultsHandle,
											uint32 *NumberOfMatchedFields,
											CSSM_DATA_PTR *Value );
	CSSM_RETURN
	(CSSMAPI *cl_CertGetNextFieldValue)( CSSM_CL_HANDLE CLHandle,
											CSSM_HANDLE ResultsHandle,
											CSSM_DATA_PTR *Value );

	CSSM_RETURN
	(CSSMAPI *cl_CertAbortQuery)( CSSM_CL_HANDLE CLHandle,
									  CSSM_HANDLE ResultsHandle );
	CSSM_RETURN
	(CSSMAPI *cl_CertGetAllFields)( CSSM_CL_HANDLE CLHandle,
										const CSSM_DATA *Cert,
										uint32 *NumberOfFields,
										CSSM_FIELD_PTR *FieldList );
	/* crl functions */
	CSSM_RETURN
	(CSSMAPI *cl_CrlGetFirstFieldValue)( CSSM_CL_HANDLE CLHandle,
												const CSSM_DATA *Crl,
												const CSSM_OID *CrlField,
												CSSM_HANDLE_PTR ResultsHandle,
												uint32 *NumberOfMatchedFields,
												CSSM_DATA_PTR *Value );
	CSSM_RETURN
	(CSSMAPI *cl_CrlGetNextFieldValue)( CSSM_CL_HANDLE CLHandle,
											CSSM_HANDLE ResultsHandle,
											CSSM_DATA_PTR *Value );
	CSSM_RETURN
	(CSSMAPI *cl_CrlAbortQuery)( CSSM_CL_HANDLE CLHandle,
									 CSSM_HANDLE ResultsHandle);

	CSSM_RETURN
	(CSSMAPI *cl_FreeFieldValue)( CSSM_CL_HANDLE CLHandle,
									const CSSM_OID *CertOrCrlOid,
									CSSM_DATA_PTR Value );
};

typedef struct dlutil_cssm_functions DLUTIL_CSSM_FUNCTIONS;

#ifdef __cplusplus
}
#endif

#endif
