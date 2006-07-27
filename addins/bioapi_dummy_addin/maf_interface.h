/*-----------------------------------------------------------------------
 * File: MAF_INTERFACE.H
 *-----------------------------------------------------------------------
 */


#ifndef MAF_INTERFACE_H
#define MAF_INTERFACE_H

#ifndef MAF_CONFIG_H
#error maf_config.h must be included before this file
#endif
#if ADDIN_NEED_ADDIN_ATTACH_STRUCT
# ifndef __COLLECTN_H__
# error maf_collectn.h must be included before this file
# endif
#endif
#ifndef _BIOSPITYPE_H
#error biospi_type.h must be included before this file
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * Name: MAF_MODULE_LOAD_TRACKER
 *
 * Description:
 * An instance of this structure is created for every framework that loads the
 * module. This structure holds state that is global to a framework, and tracks
 * the number of times a module has been loaded
 *---------------------------------------------------------------------------*/
typedef struct _maf_module_load_tracker
{
#ifdef _DEBUG
	/* Magic Number that is used to verify the integrity of the structure */
    unsigned long debug_MagicNumber;
#endif

	/* UUID of the framework to which this load belongs */
	BioAPI_UUID FrameworkUuid;

	/* Module event callback and context for this framework */
	BioSPI_ModuleEventHandler ModuleEventHandler;
	const void *ModuleEventHandlerCtx;

	/* Number of times that this framework has loaded this module */
	uint32 NumLoads;

	/* Verified handle of the framework */
	BioAPI_HANDLE hFramework;

#if ADDIN_NEED_ADDIN_LOAD_STRUCT
	/* Custom data for every addin module, customized through maf_config.h */
	ADDIN_LOAD_DATA AddinLoadData;
#endif
}
MAF_MODULE_LOAD_TRACKER, *MAF_MODULE_LOAD_TRACKER_PTR;

/*-----------------------------------------------------------------------------
 * Name: BioAPI_MODULE_ATTACH_TRACKER
 *
 * Description:
 * An instance of this structure is created for every attach to the addin. This
 * structure holds state that is global to an attach.
 *---------------------------------------------------------------------------*/
typedef struct _maf_module_attach_tracker
{
#ifdef _DEBUG
	/* Magic Number that is used to verify the integrity of the structure */
    unsigned long debug_MagicNumber;
#endif

	/* Parameter values passed by framework during attach */
	BioAPI_DEVICE_ID DeviceID;
	BioAPI_HANDLE ModuleHandle;

	/* Memory functs to use when returning memory to the application through
	 * protected SPIs.
	 */
	BioAPI_UPCALLS AppUpcalls;

	/* Reference to the framework that made this attach.
	 */
	BioAPI_HANDLE hFramework;

	/* Reference to a BioAPI_MODULE_LOAD_TRACKER node */
	MLC_LOCK_REF LoadLockRef;

#if ADDIN_NEED_ADDIN_ATTACH_STRUCT
	/* Custom data for every addin module, customized through maf_config.h */
	ADDIN_ATTACH_DATA AddinAttachData;
#endif
}
MAF_MODULE_ATTACH_TRACKER, *MAF_MODULE_ATTACH_TRACKER_PTR;

#if ADDIN_NEED_ADDIN_LOAD_STRUCT
/*-----------------------------------------------------------------------------
 * Name: Addin_GetLoadData
 *
 * Description:
 * Function that returns the ADDIN_LOAD_DATA structure inside the load
 * tracker for the current thread. Addin must call Addin_SPIBegin before calling
 * this function. This data should not be modified unless BSP_NEED_LOAD_WRITER_LOCK
 * is true.
 *
 * Parameters:
 * None.
 *
 * Return Code:
 * On success, a pointer to a ADDIN_LOAD_DATA, otherwise NULL
 *---------------------------------------------------------------------------*/
#if (BSP_NEED_LOAD_WRITER_LOCK == 0)
const
#endif
ADDIN_LOAD_DATA * Addin_GetLoadData();
#endif

#if ADDIN_NEED_ADDIN_ATTACH_STRUCT
/*-----------------------------------------------------------------------------
 * Name: Addin_GetAttachData
 *
 * Description:
 * Function that returns the ADDIN_ATTACH_DATA structure inside the attach
 * tracker for the current thread. Addin must call Addin_SPIBegin before calling
 * this function. This data should not be modified unless BSP_NEED_ATTACH_WRITER_LOCK
 * is true.
 *
 * Parameters:
 * None.
 *
 * Return Code:
 * On success, a pointer to a ADDIN_ATTACH_DATA, otherwise NULL
 *---------------------------------------------------------------------------*/
#if (BSP_NEED_ATTACH_WRITER_LOCK == 0)
const
#endif
ADDIN_ATTACH_DATA * Addin_GetAttachData();

/*-----------------------------------------------------------------------------
 * Name: Addin_GetAttachTrackerFromLock
 *
 * Description:
 * Returns the BioAPI_MODULE_ATTACH_TRACKER structure from a lock reference.
 * Caller must insure that the lock reference is appropriately locked.
 *
 * Parameters:
 * LockRef (input) : Lock reference to query.
 * ppAttachTracker (ouput) : Pointer that will receive the address of the
 *							 attach tracker record.
 *
 * Return Code:
 * BoiAPI_OK : Attach tracker returned.
 * BoiAPI_INVALID_HANDLE : LockRef does not protect an attach tracker record.
 *---------------------------------------------------------------------------*/
BioAPI_RETURN Addin_GetAttachTrackerFromLock(
							MLC_LOCK_REF LockRef,
							const MAF_MODULE_ATTACH_TRACKER **ppAttachTracker );

/*-----------------------------------------------------------------------------
 * Name: Addin_GetAttachDataFromLock
 *
 * Description:
 * Returns the ADDIN_ATTACH_DATA structure from a lock reference. Caller must
 * insure that the lock reference is appropriately locked.
 *
 * Parameters:
 * LockRef (input) : Lock reference to query.
 * ppAttachData (ouput) : Pointer that will receive the address of the
 *						  attach data record.
 *
 * Return Code:
 * BoiAPI_OK : Attach tracker returned.
 * BoiAPI_INVALID_HANDLE : LockRef does not protect an attach tracker record.
 *---------------------------------------------------------------------------*/
BioAPI_RETURN Addin_GetAttachDataFromLock(
							MLC_LOCK_REF LockRef,
							ADDIN_ATTACH_DATA **ppAttachData );
#endif

/*-----------------------------------------------------------------------------
 * Name: Addin_GetLoadTracker
 *
 * Description:
 * Function that returns the BoiAPI_MODULE_LOAD_TRACKER structure for the current
 * thread. Addin must call Addin_SPIBegin before calling this function. The
 * structure returned should not be modified.
 *
 * Parameters:
 * None.
 *
 * Return Code:
 * On success, a pointer to a BoiAPI_MODULE_LOAD_TRACKER, otherwise NULL
 *---------------------------------------------------------------------------*/
const MAF_MODULE_LOAD_TRACKER * Addin_GetLoadTracker();

/*-----------------------------------------------------------------------------
 * Name: Addin_GetAttachTracker
 *
 * Description:
 * Function that returns the BoiAPI_MODULE_ATTACH_TRACKER structure for the current
 * thread. Addin must call Addin_SPIBegin before calling this function. The
 * structure returned should not be modified.
 *
 * Parameters:
 * None.
 *
 * Return Code:
 * On success, a pointer to a BoiAPI_MODULE_ATTACH_TRACKER, otherwise NULL
 *---------------------------------------------------------------------------*/
const MAF_MODULE_ATTACH_TRACKER * Addin_GetAttachTracker();

#ifdef __cplusplus
}
#endif

#endif
