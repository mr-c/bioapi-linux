/*-----------------------------------------------------------------------
 * File: BIOSPI_TYPE.H
 *
 *-----------------------------------------------------------------------
 */

#ifndef _BIOSPITYPE_H
#define _BIOSPITYPE_H

/*
	BioSPI_ModuleEventHandler

	This defines the event handler interface that the BioAPI H-level framework
	defines and implements to receive asynchronous notification of events of
	type BioAPI_MODULE_EVENT from a service provider module. Example events
	include insertion or removal of a hardware service module, or fault detection.
*/
typedef BioAPI_RETURN (BioAPI *BioSPI_ModuleEventHandler)
			(const BioAPI_UUID *BSPUuid,
			void* BioAPINotifyCallbackCtx,
			BioAPI_DEVICE_ID DeviceID,
			uint32	Reserved,
			BioAPI_MODULE_EVENT EventType);

/*
	BioAPI_MODULE_FUNCS

	This structure is used by BSPs to return function pointers for all service
	provider interfaces that can be invoked by the BioAPI framework.
*/
typedef struct bioapi_module_funcs
{
	uint32 Reserved;
	uint32 NumberOfServiceFuncs;
	const BioAPI_PROC_ADDR *ServiceFuncs;
} BioAPI_MODULE_FUNCS, *BioAPI_MODULE_FUNCS_PTR;


/*
	BioAPI_UPCALLS
	This structure is used by BioAPI to provide memory management functions
	to biometric service modules. A BSP can invoke these functions at anytime
	during the life cycle of an attach-session.
*/
#ifndef BioAPI_MEMTRACK_ON

typedef void * (BioAPI *BioAPI_UPCALLS_MALLOC)
			(BioAPI_HANDLE ModuleHandle,
			uint32 Size);

typedef void (BioAPI *BioAPI_UPCALLS_FREE)
			(BioAPI_HANDLE ModuleHandle,
			void *Memblock);

typedef void * (BioAPI *BioAPI_UPCALLS_REALLOC)
			(BioAPI_HANDLE ModuleHandle,
			void *Memblock,
			uint32 Size);

typedef void * (BioAPI *BioAPI_UPCALLS_CALLOC)
			(BioAPI_HANDLE ModuleHandle,
			uint32 Num,
			uint32 Size);
#else

typedef void * (BioAPI *BioAPI_UPCALLS_MALLOC)
			(BioAPI_HANDLE ModuleHandle,
			 uint32 Size,
			 const char * File,
			 uint32 Line);

typedef void (BioAPI *BioAPI_UPCALLS_FREE)
			(BioAPI_HANDLE ModuleHandle,
			 void *Memblock,
			 const char * File,
			 uint32 Line);

typedef void * (BioAPI *BioAPI_UPCALLS_REALLOC)
			(BioAPI_HANDLE ModuleHandle,
			 void *Memblock,
			 uint32 Size,
			 const char * File,
			 uint32 Line);

typedef void * (BioAPI *BioAPI_UPCALLS_CALLOC)
			(BioAPI_HANDLE ModuleHandle,
			 uint32 Num,
			 uint32 Size,
			 const char * File,
			 uint32 Line);
#endif


typedef struct bioapi_upcalls
{
	BioAPI_UPCALLS_MALLOC Malloc_func;
	BioAPI_UPCALLS_FREE Free_func;
	BioAPI_UPCALLS_REALLOC Realloc_func;
	BioAPI_UPCALLS_CALLOC Calloc_func;
} BioAPI_UPCALLS, *BioAPI_UPCALLS_PTR;


#endif // _BIOSPITYPE_H
