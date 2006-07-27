/*-----------------------------------------------------------------------
 * File: MDS_INTERNAL.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */




#ifndef MDS_INTERNAL_H
#define MDS_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* functions in mds_emulate.cpp */
BioAPI_RETURN mds_SetFunctionPointers(
	const BioAPI_MEMORY_FUNCS * pMemFuncs,
	MDS_HANDLE *hMds);

CSSM_RETURN mds_Terminate(MDS_HANDLE Handle);

/* functions in mds_create.c */
const CSSM_DBINFO *mds_bioapi_schema_GetDBInfo();

#ifdef __cplusplus
}
#endif

#endif
