/*-----------------------------------------------------------------------
 * File: manage_interface.c
 *
 *-----------------------------------------------------------------------
 */

#include "precomp.h"

/* Linux-Port: added the const keyword */
const BioAPI_UUID bioapi_UUID = ref_h_layer_uuid_init;

CSSM_LIB_HANDLE HLayer_Library = 0;

BioAPI_UPCALLS InternalUpcalls;

bioapi_INTERNAL_MODULE_LIST_PTR hModuleListHead = NULL;

bioapi_INTERNAL_MODULE_LIST_PTR hModuleListTail = NULL;

cssm_SWMRLock_PTR hModuleListSWMRLock = NULL; 
/* 
 * For syncronizing the allocation and deallocation of 
 * attached addinInfo structure. 
 */

static PORT_MUTEX_HANDLE hInitMutex = NULL;
#ifdef POSIX
static pthread_mutex_t initMutex = PORT_STATIC_MUTEX_INITIALIZER;
#endif
/* This mutex is used to serialize BioAPI_Init and BioAPI_Terminate.  */

uint32	HLayerInitCount = 0; /* This counter is used to track the number of BioAPI_Init's,
							* BioAPI_Uninitialize will decrement the count and will
							* tear the state down, when count reaches 0.
							*/


#if defined (LINUX) || defined (SOLARIS) || defined (UNIX)
static int _init() __attribute__ ((constructor));
static int _fini() __attribute__ ((destructor));
#endif

#if defined (UNIX)

int _init()
{
	/* TBD: add	 code to differentiate between thread and process */

#if defined (POSIX)
    hInitMutex = &initMutex;
#endif
/*	  RETURN_EXPRESSION(DllMain (NULL, DLL_PROCESS_ATTACH, NULL))*/
}

int _fini()
{
	/* TBD: add code to differentiate between thread and process */

/*	  RETURN_EXPRESSION(DllMain (NULL, DLL_PROCESS_DETACH, NULL))*/
}
#endif



/*----------------------------------------------------------
*Name: bioapi_CheckInit
*
*Description:
* Make sure that client has initialized the H-Layer. Used in all APIs.
*
*Parameters:
* None
*
*Returns:
*
*--------------------------------------------------------*/
BioAPI_RETURN
bioapi_CheckInit (void)
{
	BioAPI_RETURN err = BioAPI_OK;

	if((hInitMutex) &&
	   (port_LockMutex(hInitMutex,
					   CSSM_INFINITE_WAIT) == BioAPI_OK))
	{

		if(0 == HLayerInitCount)
		{
			ERR(err = BioAPIERR_H_FRAMEWORK_NOT_INITIALIZED);
		}

		port_UnlockMutex(hInitMutex);
	}
	else
	{
		ERR(err = BioAPIERR_H_FRAMEWORK_NOT_INITIALIZED);
	}

	return err;
}



#ifdef WIN32

/*---------------------------------------------------------------
*
*Name: DllMain
*
*Description:
* This is the main entry point of DLL required by Windows
*
*
*Parameters:
* hInstance(iuput) - instance handle of DLL.
* dwReason (input) - reason for windows calling this function.
* lpReserved (input) - data associated with this callback.
*
*Returns:
* TRUE - call is successful
* FALSE - abort loading of DLL
*
*-------------------------------------------------------------*/
BOOL WINAPI
DllMain(HANDLE hInstance,
		DWORD dwReason,
		LPVOID lpReserved)
{
	switch (dwReason)
	{

		case DLL_PROCESS_ATTACH:
		{

#ifdef _DEBUG
			BOOL Ret;

			/* Initialize a tracing functionality */
			Ret = port_InitMemoryTracking();
			if(Ret != BioAPI_OK )
			{
				return FALSE;
			}

			/* DLL is attaching to the address space of the current process */
			OutputDebugString("BioAPI H Layer: Process attaching\n");
#endif
            if (port_CreateMutex(NULL, &hInitMutex) != CSSM_OK)
            {
                return FALSE;
            }
		}
		break;


		case DLL_THREAD_ATTACH:
		{
#ifdef _DEBUG
			OutputDebugString("BioAPI H Layer: Thread attaching\n");
#endif
		}
		break;


		case DLL_THREAD_DETACH:
		{
#ifdef _DEBUG
			OutputDebugString("BioAPI H Layer: Thread detaching\n");
#endif
		}
		break;


		case DLL_PROCESS_DETACH:
		{
			/* The calling process is detaching the DLL from its address space */
            port_CloseMutex(hInitMutex);

#ifdef _DEBUG
			/* Dump heap leaks when we are unloaded */
			port_DumpMemoryLeaks( "BioAPI H Layer" );


			OutputDebugString("BioAPI H Layer: Process detaching\n");
#endif
		}
		break;

		default:
		break;
	}

	return TRUE;
}

#endif /* WIN32 */



/*---------------------------------------------------------------
 *
 *Name: BioAPI_Init
 *
 *Description:
 *	 This function initializes the H Layer and verifies that the version of
 *	 the BioAPI expected by the application is compatible with the version of
 *	 H Layer on the system
 *
 *Parameters:
 *	 Version (input)	  - major and minor version number of the BioAPI
 *							release application is compatible with
 *	 Reserved1-4 (input)	 - A reserved input.
 *
 *Returns:
 *-------------------------------------------------------------*/
BioAPI_RETURN BioAPI
BioAPI_Init(const BioAPI_VERSION *Version,
			uint32 Reserved1,
			const void *Reserved2,
			uint32 Reserved3,
			const void *Reserved4)
{
	BioAPI_RETURN err = BioAPI_OK;
	CSSM_DATA tmpPath;
	CSSM_DATA ModulePath;
	CSSM_DATA tmpModuleName;

	if((port_IsBadReadPtr(Version, (uint32)sizeof (BioAPI_VERSION))))
	{
		ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_POINTER);
	}
	else
	{
		/* Check for version information */
		if((BioAPI_MAJOR != Version->Major) ||
		   (BioAPI_MINOR != Version->Minor))
		{
			ERR(err = BioAPIERR_H_FRAMEWORK_INCOMPATIBLE_VERSION);
		}
	}

	/*
	 * Create the mutex in locked state. Only
	 * One thread should be able to create this semaphore in mutex
	 * state.
	*/
	if(BioAPI_OK == err)
	{
		if (ERR(err = port_LockMutex(hInitMutex,
									 CSSM_INFINITE_WAIT)) != BioAPI_OK)
		{
			ERR(err = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR);
		}

		if(BioAPI_OK == err)
		{
			/* in this 'if' we know we have a mutex locked */

			/*
			 * If already initialized, simply return success.
			 */
			if (HLayerInitCount != 0)
			{
				HLayerInitCount++;
			}
			else
			{
				if(port_init() != BioAPI_OK)
				{
					ERR(err = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR);
				}
				else
				{
					/*
					 * Open the MDS Object and CDSA directory.
					 */
					if(ERR(err = MDSUTIL_Init ()) == BioAPI_OK)
					{
						/* Initialize memory structure to hand to add-ins */
						InternalUpcalls.Malloc_func = bioapi_SpiMalloc;
						InternalUpcalls.Free_func = bioapi_SpiFree;
						InternalUpcalls.Realloc_func = bioapi_SpiRealloc;
						InternalUpcalls.Calloc_func = bioapi_SpiCalloc;
					}
					else
					{
						ERR(err = BioAPIERR_H_FRAMEWORK_REGISTRY_ERROR);
					}

					if( err == BioAPI_OK)
					{
						ModulePath.Data = NULL;
						ModulePath.Length = 0;

						if(ERR(err = mdsutil_GetModuleCredentialInfo(&bioapi_UUID,
																	 NULL,
																	 &tmpModuleName,
																	 &tmpPath)) != BioAPI_OK)
						{
							ERR(err = BioAPIERR_H_FRAMEWORK_REGISTRY_ERROR);
						}
						else
						{
							CSSM_DATA tmp1;
							CSSM_DATA tmp2;
							tmp1.Length = tmpModuleName.Length;
							tmp2.Length = tmpPath.Length;
							tmp1.Data = tmpModuleName.Data;
							tmp2.Data = tmpPath.Data;

							ERR(err = mdsutil_GetModulePath(tmp1,
															tmp2,
															&ModulePath));

							local_free (tmpModuleName.Data, NULL);
							local_free (tmpPath.Data, NULL);

							if(err != BioAPI_OK)
							{
								ERR(err = BioAPIERR_H_FRAMEWORK_REGISTRY_ERROR);
							}
							else
							{
								/* Load the library - actually this library */
								ERR(err = port_LoadLibrary((const char *)ModulePath.Data,
														   &HLayer_Library));
								local_free(ModulePath.Data, NULL);
							}
						}
					}

					if(BioAPI_OK == err)
					{
						/*
						 * Create the global multiple-reader-single-write
						 * synchronization object for addin list.
						 */
						ERR(err = bioapi_SWMRInit(&hModuleListSWMRLock));

						/*
						 * Set the init count to 1.
						 */
						HLayerInitCount = 1;
					}

					if(BioAPI_OK != err)
					{
						MDSUTIL_Term ();
					}
				}
			}

			/* unlock the mutex */
			ERR(port_UnlockMutex(hInitMutex));
		}
	}

	return err;
}



/*-----------------------------------------------------------------------
*
*NAME:	BioAPI_Terminate
*
*DESCRIPTION:
*	 This function terminates the caller's use of BioAPI. BioAPI can cleanup
*	 all internal state associated with the calling application. This function
*	 must be called once by each application
*PARAMETERS:
*	 NONE
*
*RETURNS:
*	 A BioAPI_RETURN value indicating success or specifying a particular
*	 error condition. The value BioAPI_OK indicates success. All other
*	 values represent an error condition
*
*-----------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
BioAPI_Terminate (void)
{
	BioAPI_RETURN err = BioAPI_OK;

	/*
	* If not initiallized, simply return ok.
	*/
	ERR(err = bioapi_CheckInit());

	if(BioAPI_OK == err)
	{
		/*
		 * Get the init mutex.
		 */
		 port_LockMutex(hInitMutex,
						CSSM_INFINITE_WAIT);

		/*
		* Decrement the init counter
		*/
		HLayerInitCount--;

		/*
		* If last uninit tear the h layer state down.
		*/
		if(0 == HLayerInitCount)
		{

			bioapi_CleanUp ();
			MDSUTIL_Term ();

			/* Mutexes are closed */
			if(0 != hModuleListSWMRLock)
			{
				bioapi_SWMRDelete (hModuleListSWMRLock);
				hModuleListSWMRLock = NULL;
			}

			if(0 != HLayer_Library)
			{
				port_FreeLibrary(HLayer_Library);
				HLayer_Library = 0;
			}

			port_UnlockMutex(hInitMutex);
		}
		else
		{
			/*
			* Release the init mutex.
			*/
			port_UnlockMutex(hInitMutex);
		}
	}

	return err;
}



/*---------------------------------------------------------------
*
*Name: BioAPI_ModuleAttach
*
*Description:
*	Attach to the addin module. The module load need to be called before calling this function
*	Only one device type can be attached at a time and only one keyhoierarchy can be used
*	per attach
*Parameters:
*	UUID - UUID to addin module
*	Version - version requested of addin module
*	MemoryFuncs - memory functions for addin module
*	DeviceID - ID of device requested
*	DeviceType - device type requested
*	AttachFlags - session flags
*	KeyHierarchy - keyhierarchy to be used
*	FunctionTable
*	NumFunctionTable
*	Reserved
*	NewModuleHandle - new attached handle
*
*Returns:
*	A BioAPI_RETURN value indicating success or specifying a
particular error condition. The value BioAPI_OK indicates
success. All other values represent an error condition
*
*-------------------------------------------------------------*/
BioAPI_RETURN BioAPI
BioAPI_ModuleAttach(const BioAPI_UUID *ModuleUuid,
					const BioAPI_VERSION *Version,
					const BioAPI_MEMORY_FUNCS *MemoryFuncs,
					uint32 DeviceID,
					uint32 Reserved1,
					uint32 Reserved2,
					uint32 Reserved3,
					BioAPI_FUNC_NAME_ADDR *FunctionTable,
					uint32 NumFunctionTable,
					const void *Reserved4,
					BioAPI_HANDLE_PTR NewModuleHandle)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_HANDLE tempNewModuleHandle = BioAPI_INVALID_HANDLE;
	void *Reserved5 = NULL;

	/* make sure that app as done a BioAPI_Init */
	if(ERR(err = bioapi_CheckInit ()) == BioAPI_OK)
	{
		/* Check for valid parameters */
		if((port_IsBadWritePtr(NewModuleHandle,
							   sizeof(BioAPI_HANDLE))))
		{
			ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_POINTER);
		}
		else
		{
			if((port_IsBadReadPtr(Version,
								  (uint32)sizeof(BioAPI_VERSION))) ||
			   (port_IsBadReadPtr(MemoryFuncs,
								  (uint32)sizeof(BioAPI_MEMORY_FUNCS))))
			{
				ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_POINTER);
			}
			else
			{

				if((BioAPI_MAJOR != Version->Major) ||
				   (BioAPI_MINOR != Version->Minor))
				{
					ERR(err = BioAPIERR_H_FRAMEWORK_INCOMPATIBLE_VERSION);
				}
			}

			if(BioAPI_OK == err)
			{
				/* Make sure that memory functions are filled */
				if (ERR( err = bioapi_CheckAPIMemFuncs (MemoryFuncs)) == BioAPI_OK)
				{
						ERR(err =  bioapi_ModuleAttach(ModuleUuid,
													   Version,
													   MemoryFuncs,
													   DeviceID,
													   Reserved1,
													   Reserved2,
													   Reserved3,
													   FunctionTable,
													   NumFunctionTable,
													   Reserved4,
													   &tempNewModuleHandle,
													   Reserved5));

				}
			}

			if(BioAPI_OK == err)
			{
				*NewModuleHandle = tempNewModuleHandle;
			}
			else
			{
				*NewModuleHandle = BioAPI_INVALID_HANDLE;
			}
		}
	}

	return err;
}



/*---------------------------------------------------------------
*
*Name: BioAPI_ModuleDetach
*
*Description:
*	This function detaches the application from the device provider module
*
*Parameters:
*	ModuleHandle - handle to addin
*
*Returns:
*	A BioAPI_RETURN value indicating success or specifying a particular
*	error condition. The value BioAPI_OK indicates success. All other
*	values represent an error condition
*
*-------------------------------------------------------------*/
BioAPI_RETURN BioAPI
BioAPI_ModuleDetach(BioAPI_HANDLE ModuleHandle)
{
	BioAPI_RETURN err = BioAPI_OK;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord;
	bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord;

	/* make sure that app as done a BioAPI_Init */
	ERR(err = bioapi_CheckInit());

	if(BioAPI_OK == err)
	{
		ERR(err = bioapi_FindAttachAndMultiLock(ModuleHandle,
												BioAPI_NO_LOCK,
												NULL,
												BioAPI_NO_LOCK,
												&DeviceRecord,
												BioAPI_WRITER_LOCK,
												&AttachRecord,
												BioAPI_WRITER_LOCK));
		if(BioAPI_OK == err)
		{
			bioapi_CleanInternalAttachRecord(DeviceRecord,
											 &AttachRecord);

			bioapi_ReleaseDeviceLock(DeviceRecord,
									  BioAPI_WRITER_LOCK);
		}
	}

	return err;
}



/*---------------------------------------------------------------
*
*Name: BioAPI_QueryDevice
*
*Description:
*
*	Get the unique identifier of the attached module device, as identified
*	by the input handle.If provided, the MemoryFuncs override the default
*	memory functions.
*
*Parameters:
*	ModuleHandle - handle of the addin
*	 MemoryFuncs - Memory functions to be used for allocation.
*
*Returns:
*	NULL - error in getting the info
*	not NULL - pointer to device_uid associated with addin
*
*-------------------------------------------------------------*/
BioAPI_RETURN BioAPI
BioAPI_QueryDevice(BioAPI_HANDLE ModuleHandle,
				   BioAPI_SERVICE_UID_PTR DeviceUID)
{
	BioAPI_RETURN err = BioAPI_OK;

	/* make sure that app as done a BioAPI_Init */
	ERR(err = bioapi_CheckInit ());

	if(BioAPI_OK == err)
	{
		if(port_IsBadWritePtr(DeviceUID,
							  sizeof(BioAPI_SERVICE_UID)))
		{
			ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_POINTER);
		}
		else
		{
			ERR(err = bioapi_QueryDevice(ModuleHandle,
										 DeviceUID));
		}
	}

	return err;
}



/*-------------------------------------------------------------
*
*NAME:	BioAPI_ModuleLoad
*
*
*DESCRIPTION:
*  This function initializes the device module. Initialization
*  includes registering the application's module-event handler and
*  enabling events with the device module
*
*PARAMETERS:
*  ModuleUuid (input) - The UUID of the module selected for loading.
*
*  AppNotifyCallback (input/optional) - The event notification function
*										the single callback for all event
*										notifications from the loaded
*										(and later attached) device module.
*
*	AppNotifyCallbackCtx (input/optional) - When the selected device module
*											raises an event, this context is
*											passed as an input to the event
*											handler specified by
*											AppNotifyCallback
*
*
*RETURNS:
*	 A BioAPI_RETURN value indicating success or specifying a particular
*	 error condition. The value BioAPI_OK indicates success. All other
*	 values represent an error condition
*
*-------------------------------------------------------------*/
BioAPI_RETURN BioAPI
BioAPI_ModuleLoad(const BioAPI_UUID *ModuleUuid,
				  uint32 Reserved,
				  BioAPI_ModuleEventHandler AppNotifyCallback,
				  void *AppNotifyCallbackCtx)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_UUID zeroUUID;

	/* make sure that app as done a BioAPI_Init */
	if(ERR(err = bioapi_CheckInit()) == BioAPI_OK)
	{
		/* make sure we have a valid UUID */
		if(port_IsBadReadPtr(ModuleUuid,
							 (uint32)sizeof(BioAPI_UUID)) == BioAPI_FALSE)
		{
			BioAPI_ClearUuid(&zeroUUID);
			if(BioAPI_TRUE == BioAPI_CompareUuids(&zeroUUID, ModuleUuid))
			{
				ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_UUID);
			}
			else
			{
				/* make sure that the callback function, if given is valid */
				if(NULL != AppNotifyCallback)
				{
					if(port_IsBadCodePtr((BioAPI_PROC_ADDR)AppNotifyCallback) == BioAPI_TRUE)
					{

						ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_POINTER);
					}
				}
			}

			if(BioAPI_OK == err)
			{
				/* call the internal function which will load the module */
				ERR(err = bioapi_ModuleLoad(ModuleUuid,
											Reserved,
											AppNotifyCallback,
											AppNotifyCallbackCtx));
			}
		}
		else
		{
			/* UUID was bad */
			ERR(err =  BioAPIERR_H_FRAMEWORK_INVALID_POINTER);
		}
	}

	return err;
}



/*------------------------------------------------------------
*
*NAME:	BioAPI_ModuleUnload
*
*DESCRIPTION:
*	The function disables event notification for the caller by
*	deregistering the callback function registered by a call to
*	BioAPI_ModuleLoad. If all registered callbacks are removed,
*	then BioAPI unloads the device module.
*
*PARAMETERS:
*	ModuleUuid (input) - The UUID of the module selected for unloading.
*
*	AppNotifyCallback (input/optional) - The event notification function
*										 to be deregistered. The function
*										 must have been provided by the
*										 caller in BioAPI_ModuleLoad.
*
*	AppNotifyCallbackCtx (input/optional) - The event notification context
*											that was provided in the corresponding
*											call to BioAPI_ModuleLoad
*
*
*RETURNS:
*
* A BioAPI_RETURN value indicating success or specifying a particular
* error condition. The value BioAPI_OK indicates success.
* All other values represent an error condition.
*------------------------------------------------------------*/

BioAPI_RETURN BioAPI
BioAPI_ModuleUnload(const BioAPI_UUID *ModuleUuid,
				  BioAPI_ModuleEventHandler AppNotifyCallback,
				  void* AppNotifyCallbackCtx)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_UUID zeroUUID;

	/* make sure that app as done a BioAPI_Init */
	if(ERR(err = bioapi_CheckInit()) == BioAPI_OK)
	{
		/* make sure we have a valid UUID */
		if(port_IsBadReadPtr(ModuleUuid,
							 (uint32)sizeof(BioAPI_UUID)) == BioAPI_FALSE)
		{
			BioAPI_ClearUuid (&zeroUUID);
			if(BioAPI_TRUE == BioAPI_CompareUuids(&zeroUUID, ModuleUuid))
			{
				ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_UUID);
			}
			else
			{
				/* make sure that the callback function, if given is valid */
				if(NULL != AppNotifyCallback)
				{
					if(port_IsBadCodePtr((BioAPI_PROC_ADDR)AppNotifyCallback) == BioAPI_TRUE)
					{

						ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_POINTER);
					}
				}
			}

			if(err == BioAPI_OK)
			{
				/* call the internal function which will unload the module */
				ERR(err = bioapi_ModuleUnload(ModuleUuid,
											  AppNotifyCallback,
											  AppNotifyCallbackCtx));
			}
		}
		else
		{
			/* UUID was bad */
			ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_POINTER);
		}
	}

	return err;
}
