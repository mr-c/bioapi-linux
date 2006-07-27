/*-----------------------------------------------------------------------
 * File: mod_manage.c
 *-----------------------------------------------------------------------
 */

/*
 * This file implements the module management functions for a BioAPI addin based
 * on MAF.
 */

#include "precomp.h"

/* Declare the custom attach functions for BioAPI service. */
BioAPI_RETURN BSP_ModuleAttach(BioAPI_MODULE_FUNCS_PTR *FuncTbl);


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
 * BioAPI_OK if successful, otherwise BioAPI_FAIL
 *---------------------------------------------------------------------------*/

BioAPI_RETURN
Addin_callout_Initialize(void)
{
	return BioAPI_OK;
}



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
 * BioAPI_OK if successful, otherwise BioAPI_FAIL
 *---------------------------------------------------------------------------*/

BioAPI_RETURN
Addin_callout_Terminate(void)
{
	return BioAPI_OK;
}



/*-------------------------------------------------------------------------
 * Name: Addin_callout_ModuleLoad
 *
 * Description:
 * Call used to indicate that the application has requested a module load
 *
 * Parameters:
 * pLoadTracker (input/output) : The load tracker node that was created.
 *	This node may be writen to.
 * BioAPINotifyCallback (input) : Callback function to indicate to the
 *	framework that a subservice is ready for attach (must be called before
 *	an attach can occur).
 * BioAPINotifyCallbackCtx (input) : Opaque data to pass to the callback
 *	function.
 *
 * Return Code:
 * BioAPI_OK if successful, otherwise BioAPI_FAIL
 *-------------------------------------------------------------------------
 */

BioAPI_RETURN
Addin_callout_ModuleLoad(MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker,
						 BioSPI_ModuleEventHandler BioAPINotifyCallback,
						 const void* BioAPINotifyCallbackCtx)
{
	BioAPI_RETURN ret = BioAPI_OK;

	ERR(ret = BioAPINotifyCallback(&BSP_UUID,
								 (void *)BioAPINotifyCallbackCtx,
								 BSP_DEVICE_ID,
								 0, /* reserved */
								 BioAPI_NOTIFY_INSERT));
	return ret;
}



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
 * BioAPINotifyCallback (input) : Identifies the corresponding ModuleLoad
 *	call BioAPINotifyCallbackCtx (input) : Identifies the corresponding
 *	ModuleLoad call
 *
 * Note:
 * There is no need to call the callback function on unload.
 *
 * Return Code:
 * BioAPI_OK if successful, otherwise BioAPI_FAIL
 *-------------------------------------------------------------------------
 */

BioAPI_RETURN Addin_callout_ModuleUnload(
						MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker,
						BioSPI_ModuleEventHandler BioAPINotifyCallback,
						const void* BioAPINotifyCallbackCtx )
{
	return BioAPI_OK;
}



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
 * to register with the framework
 *
 * Return Code:
 * BioAPI_OK if successful, otherwise BioAPI_FAIL
 *-------------------------------------------------------------------------
 */

BioAPI_RETURN Addin_callout_ModuleAttach(
						MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker,
						MAF_MODULE_ATTACH_TRACKER_PTR pAttachTracker,
						BioAPI_MODULE_FUNCS_PTR *FuncTbl )
{
	if (!pAttachTracker)
	{
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	return BSP_ModuleAttach(FuncTbl);
}
