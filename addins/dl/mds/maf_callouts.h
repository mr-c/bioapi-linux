/*-----------------------------------------------------------------------
 * File: MAF_CALLOUTS.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


#ifndef MAF_CALLOUTS_H
#define MAF_CALLOUTS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAF_CONFIG_H
#error maf_config.h must be included before this file
#endif
#ifndef _CSSMTYPE_H
#error $/inc/cssmtype.h must be included before this file
#endif

// Required type defined in integrity header of CDSA.
typedef const void* ADDIN_MODULE_HANDLE;

/*-----------------------------------------------------------------------------
 * Name: Addin_callout_Initialize
 *
 * Description:
 * Preforms initialization that takes places when the module is loaded into
 * the process space
 *
 * Parameters:
 * None
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
BioAPI_RETURN Addin_callout_Initialize(void);

/*-----------------------------------------------------------------------------
 * Name: Addin_callout_Terminate
 *
 * Description:
 * Terminates state before the module is unloaded from the process space
 *
 * Parameters:
 * None.
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
BioAPI_RETURN Addin_callout_Terminate(void);


#if ADDIN_NEED_ADDIN_LOAD_STRUCT
/*-------------------------------------------------------------------------
 * Name: Addin_callout_LoadDataConstruct
 *
 * Description:
 * Constructs a ADDIN_LOAD_DATA structure
 *
 * Parameters:
 * pAddinLoadData (output) : variable to initialize
 *
 * Note:
 * If this function fails, Addin_callout_LoadDataDestroy will still be
 * called.
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *-------------------------------------------------------------------------
 */
BioAPI_RETURN Addin_callout_LoadDataConstruct(
						ADDIN_MODULE_HANDLE hCssm,
						ADDIN_LOAD_DATA * pAddinLoadData );

/*-------------------------------------------------------------------------
 * Name: Addin_callout_LoadDataDestroy
 *
 * Description:
 * Constructs a ADDIN_LOAD_DATA structure
 *
 * Parameters:
 * pAddinLoadData (output) : ADDIN_LOAD_DATA structure to destroy
 *
 * Return Code:
 * none
 *-------------------------------------------------------------------------
 */
void Addin_callout_LoadDataDestroy(
						ADDIN_LOAD_DATA * pAddinLoadData );

#endif /* endif if ADDIN_NEED_ADDIN_LOAD_STRUCT */

#if ADDIN_NEED_ADDIN_ATTACH_STRUCT
/*-------------------------------------------------------------------------
 * Name: Addin_callout_AttachDataConstruct
 *
 * Description:
 * Constructs a ADDIN_ATTACH_DATA structure
 *
 * Parameters:
 * pAddinAttachData (output) : variable to initialize
 *
 * Note:
 * If this function fails, Addin_callout_AttachDataDestroy will still be
 * called
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *-------------------------------------------------------------------------
 */
BioAPI_RETURN Addin_callout_AttachDataConstruct(
						ADDIN_ATTACH_DATA * pAddinAttachData );

/*-------------------------------------------------------------------------
 * Name: Addin_callout_AttachDataDestroy
 *
 * Description:
 * Constructs a ADDIN_ATTACH_DATA structure
 *
 * Parameters:
 * pAddinAttachData (output) : ADDIN_ATTACH_DATA structure to destroy
 *
 * Return Code:
 * none
 *-------------------------------------------------------------------------
 */
void Addin_callout_AttachDataDestroy(
						ADDIN_ATTACH_DATA * pAddinAttachData );

#endif /* endif if ADDIN_NEED_ADDIN_ATTACH_STRUCT */

/*-------------------------------------------------------------------------
 * Name: Addin_callout_ModuleAttach
 *
 * Description:
 * Functions is called to indicate that a module attach has been reqested.
 *
 * Parameters:
 * pLoadTracker (input/output) : An initialized load tracker, the
 *	ADDIN_LOAD_DATA structure can be modified
 * pAttachTracker (input/output) : An construcuted attach tracker, the
 *	ADDIN_ATTACH_DATA structure can be modified
 * FuncTbl (outpt) : Buffer to hold the SPI functions that the addin wants
 * to register with CSSM
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *-------------------------------------------------------------------------
 */
BioAPI_RETURN Addin_callout_ModuleAttach(
						MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker,
						MAF_MODULE_ATTACH_TRACKER_PTR pAttachTracker,
						BioAPI_MODULE_FUNCS_PTR *FuncTbl );

/*-------------------------------------------------------------------------
 * Name: Addin_callout_ModuleLoad
 *
 * Description:
 * Call used to indicate that the application has requested a module load
 *
 * Parameters:
 * pLoadTracker (input/output) : The load tracker node that was created.
 *	This node may be writen to.
 * CssmNotifyCallback (input) : Callback function to indicate to CSSM that
 *	a subservice is ready for attach (must be called before an attach can
 *	occurr).
 * CssmNotifyCallbackCtx (input) : Opaque data to pass to the callback
 *	function.
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *-------------------------------------------------------------------------
 */
BioAPI_RETURN Addin_callout_ModuleLoad(
						MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker,
						BioSPI_ModuleEventHandler BioAPINotifyCallback,
						const void* BioAPINotifyCallbackCtx );

/*-------------------------------------------------------------------------
 * Name: Addin_callout_ModuleUnload
 *
 * Description:
 * Function is called as part of the unload process to give indication to
 * the addin.
 *
 * Parameters:
 * pLoadTracker (input/output) : The load tracker node that was created.
 *	This node may be written to.
 * CssmNotifyCallback (input) : Identifies the corresponding ModuleLoad
 *	call CssmNotifyCallbackCtx (input) : Identifies the corresponding
 *	ModuleLoad call
 *
 * Note:
 * There is no need to call the callback function on unload.
 *
 * Return Code:
 * CSSM_OK if successful, otherwise CSSM_FAIL
 *-------------------------------------------------------------------------
 */
BioAPI_RETURN Addin_callout_ModuleUnload(
						MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker,
						BioSPI_ModuleEventHandler BioAPINotifyCallback,
						const void* BioAPINotifyCallbackCtx );


#ifdef __cplusplus
}
#endif

#endif
