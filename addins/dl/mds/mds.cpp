/*-----------------------------------------------------------------------
 * File: MDS.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains module management functions for the DLM
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

/* MDS includes */
#include "bioapi_type.h"
#include "cssmtype.h"

#include "mds.h"
#include "mds_internal.h"

/* DL include files */
#define DEFINE_FAILURE_ERROR_CODES
#define DAL_SOURCE_FILE
#include "dal_classes.h"
#include "dal_src.h" /* include to define DL interface functions */
#include "dal_internal.h"

static BioAPI_RETURN mds_CheckParamters(
	const BioAPI_MEMORY_FUNCS * pMemoryFunctions,
	MDS_FUNCS * pDlFunctions,
	MDS_HANDLE *hMds);

extern CSSM_BOOL g_initComplete;

/*---------------------------------------------------------------
 *
 * Name: MDS_Initialize
 *
 * Description:
 *	 This function is called by an application to initialize the
 *	 directory service
 *
 * Parameters:
 *	 cssmCredentialPath (input) - path to credentials for CSSM
 *	 cssmSection (input)		- registry location of CSSM info
 *
 * Returns:
 *	 BioAPI_FAIL				- authentication failed
 *	 BioAPI_OK					- authentication succeeded,
 *								  mutual identity checks
 *								  completed successfully
 *-------------------------------------------------------------*/
BioAPI_RETURN BioAPI MDS_Initialize(
	const BioAPI_UUID *pCallerUuid,
	const BioAPI_DATA *pCallerManifest,
	const BioAPI_MEMORY_FUNCS *pMemoryFunctions,
	MDS_FUNCS_PTR pDlFunctions,
	MDS_HANDLE *hMds )
{
	BioAPI_RETURN retValue = CSSM_OK;

	pCallerUuid;
	pCallerManifest;

#ifdef _DEBUG
	if ( port_InitMemoryTracking() != BioAPI_OK )
	{
		return CSSMERR_DL_FUNCTION_FAILED;
	}
#endif

	/*
	 * Ensure that we are between _init and _fini
	 */
	if (g_initComplete == BioAPI_FALSE)
		return CSSMERR_DL_FUNCTION_FAILED;

	/*
	 * check the paramaters to the function
	 */
	if ((retValue = mds_CheckParamters(pMemoryFunctions, pDlFunctions, hMds)) != BioAPI_OK)
		return retValue;

	/*
	 * Set meory function pointers.
	 */
	if ((retValue = mds_SetFunctionPointers(pMemoryFunctions, hMds)) != BioAPI_OK)
		return retValue;

	/*
	 * Set the function pointers
	 */
	pDlFunctions->DbOpen = DL_DbOpen;
	pDlFunctions->DbClose = DL_DbClose;
	pDlFunctions->GetDbNames = DL_GetDbNames;
	pDlFunctions->GetDbNameFromHandle = DL_GetDbNameFromHandle;
	pDlFunctions->FreeNameList = DL_FreeNameList;
	pDlFunctions->DataInsert = DL_DataInsert;
	pDlFunctions->DataDelete = DL_DataDelete;
	pDlFunctions->DataModify = DL_DataModify;
	pDlFunctions->DataGetFirst = DL_DataGetFirst;
	pDlFunctions->DataGetNext = DL_DataGetNext;
	pDlFunctions->DataAbortQuery = DL_DataAbortQuery;
	pDlFunctions->DataGetFromUniqueRecordId = DL_DataGetFromUniqueRecordId;
	pDlFunctions->FreeUniqueRecord = DL_FreeUniqueRecord;
	pDlFunctions->CreateRelation = DL_CreateRelation;
	pDlFunctions->DestroyRelation = DL_DestroyRelation;

	return BioAPI_OK;
}

CSSM_RETURN CSSMAPI MDS_Terminate(MDS_HANDLE Handle)
{
	return mds_Terminate (Handle);
}

static CSSM_RETURN mds_CheckParamters(
	const BioAPI_MEMORY_FUNCS * pMemoryFunctions,
	MDS_FUNCS * pDlFunctions,
	MDS_HANDLE *hMds)
{
	if (IsBadReadPtr(pMemoryFunctions, sizeof(BioAPI_MEMORY_FUNCS)) ||
		IsBadCodePtr((CSSM_PROC_ADDR)pMemoryFunctions->Malloc_func) ||
		IsBadCodePtr((CSSM_PROC_ADDR)pMemoryFunctions->Free_func) ||
		IsBadCodePtr((CSSM_PROC_ADDR)pMemoryFunctions->Calloc_func) ||
		IsBadCodePtr((CSSM_PROC_ADDR)pMemoryFunctions->Realloc_func))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	if (IsBadWritePtr(pDlFunctions, sizeof(MDS_FUNCS_PTR)))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	if (IsBadWritePtr(hMds, sizeof(MDS_HANDLE)))
	{
		return CSSMERR_DL_INVALID_POINTER;
	}

	return CSSM_OK;
}

