/*-----------------------------------------------------------------------
 * File: INTERNAL.H
 *-----------------------------------------------------------------------
 */

#ifndef INTERNAL_H
#define INTERNAL_H

#include "bioapi_type.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct bioapi_thread_data
{
	BioAPI_UUID BSPUuid;
	void* BioAPINotifyCallbackCtx;
	uint32 DeviceID;
	BioAPI_MODULE_EVENT EventType;
} bioapi_THREAD_DATA, *bioapi_THREAD_DATA_PTR;



typedef BioAPI_RETURN  (BioAPI *SPI_ATTACH_FUNC_PTR) (
							const BioAPI_UUID *BSPUuid,
							const BioAPI_VERSION *Version,
							BioAPI_DEVICE_ID DeviceID,
							uint32 Reserved1,
							uint32 Reserved2,
							BioAPI_HANDLE ModuleHandle,
							uint32 Reserved3,
							const void *Reserved4,
							const void *Reserved5,
							const void *Reserved6,
							const BioAPI_UPCALLS_PTR Upcalls,
							BioAPI_MODULE_FUNCS_PTR *FuncTbl);
typedef BioAPI_RETURN (BioAPI *SPI_DETACH_FUNC_PTR)(
							BioAPI_HANDLE Handle);

typedef BioAPI_RETURN (BioAPI * LOAD_MODULE_FUNC_PTR)
							   (const void *Reserved,
								const BioAPI_UUID *BSPUuid,
								BioSPI_ModuleEventHandler BioAPINotifyCallback,
								const void* BioAPINotifyCallbackCtx);
typedef BioAPI_RETURN (BioAPI * UNLOAD_MODULE_FUNC_PTR)
							   (const void *Reserved,
								const BioAPI_UUID *BSPUuid,
								BioSPI_ModuleEventHandler BioAPINotifyCallback,
								const void* BioAPINotifyCallbackCtx);



#include "bioapi_spi.h" /* Moved after the above typedef's because
					  * of possible circular inclusion
					  */

/* Linux-Port: added const */
extern const BioAPI_UUID bioapi_UUID;

extern CSSM_LIB_HANDLE HLayer_Library;

extern BioAPI_FREE app_free;
extern BioAPI_MALLOC app_malloc;
extern BioAPI_REALLOC app_realloc;
extern BioAPI_CALLOC app_calloc;
extern void *memRef;


/* internal locks available */
typedef uint32 BioAPI_LOCK_TYPE, *BioAPI_LOCK_TYPE_PTR;
#define BioAPI_NO_LOCK	   0
#define BioAPI_READER_LOCK 1
#define BioAPI_WRITER_LOCK 2


typedef struct bioapi_app_notify_callback_list
{
	uint32 RefCount;
	BioAPI_ModuleEventHandler AppCallbackFunc;
	void *AppCallbackCtx;
	struct bioapi_app_notify_callback_list *Next;
} bioapi_APP_NOTIFY_CALLBACK_LIST, *bioapi_APP_NOTIFY_CALLBACK_LIST_PTR;


typedef struct bioapi_internal_attach_list
{
	cssm_SWMRLock_PTR hAttachSWMRLock;

	BioAPI_HANDLE AttachHandle;

	BioAPI_BOOL AttachComplete;

    BioAPI_VERSION Version;
	BioAPI_MEMORY_FUNCS_PTR MemoryFunctions;
	BioAPI_MODULE_FUNCS_PTR AddInFunctions;	  /* the Add-in functions */

	struct bioapi_internal_attach_list *Next;
} bioapi_INTERNAL_ATTACH_LIST, *bioapi_INTERNAL_ATTACH_LIST_PTR;



typedef struct bioapi_internal_device_list
{
	cssm_SWMRLock_PTR hDeviceSWMRLock;

	uint32 DeviceHandleIndex;
	uint32 DeviceID;
	CSSM_LIB_HANDLE ModuleLibHandle;

	bioapi_INTERNAL_ATTACH_LIST_PTR AttachList;

	struct bioapi_internal_device_list *Next;
} bioapi_INTERNAL_DEVICE_LIST, *bioapi_INTERNAL_DEVICE_LIST_PTR;



typedef struct bioapi_internal_module_list
{
	uint32 ModuleHandleIndex;	/* top 9 bits used */

	BioAPI_UUID UUID;			  /* UUID of device provider or
								   docking module as appropriate
								*/

	cssm_SWMRLock_PTR hModuleSWMRLock;	/*
										 * Single writer multiple readers lock
										 * for synchronization between various
										 * load/attach instances of a module.
										 */

	CSSM_LIB_HANDLE LibHandle;	   /* Handle to actual library */

	bioapi_INTERNAL_DEVICE_LIST_PTR DeviceList;

	bioapi_APP_NOTIFY_CALLBACK_LIST_PTR EventNotificationCallbackList;

	struct bioapi_internal_module_list* Next;	  /* Next Plug-in in list */
} bioapi_INTERNAL_MODULE_LIST, *bioapi_INTERNAL_MODULE_LIST_PTR;



extern BioAPI_UPCALLS InternalUpcalls;


extern cssm_SWMRLock_PTR hModuleListSWMRLock; /* for syncronising the
												 allocation and deallocation of
												 attached addinInfo structure
											  */

/* Linked list tracking pointers */
extern bioapi_INTERNAL_MODULE_LIST_PTR hModuleListHead;
extern bioapi_INTERNAL_MODULE_LIST_PTR hModuleListTail;


/* Module information tree search functions. */
BioAPI_RETURN
bioapi_GetModuleListLock(BioAPI_LOCK_TYPE ModuleListLock);

BioAPI_RETURN
bioapi_FindModuleAndMultiLock(BioAPI_HANDLE hModule,
							BioAPI_LOCK_TYPE ModuleListLock,
							bioapi_INTERNAL_MODULE_LIST_PTR *ppModule,
							BioAPI_LOCK_TYPE ModuleEntryLock);

BioAPI_RETURN
bioapi_FindModuleByUUIDAndMultiLock(const BioAPI_UUID *UuidPtr,
								  BioAPI_LOCK_TYPE ModuleListLock,
								  bioapi_INTERNAL_MODULE_LIST_PTR *ppModule,
								  BioAPI_LOCK_TYPE ModuleEntryLock);

BioAPI_RETURN
bioapi_FindDeviceAndMultiLock(BioAPI_HANDLE hDevice,
							 BioAPI_LOCK_TYPE ModuleListLock,
							 bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
							 BioAPI_LOCK_TYPE ModuleEntryLock,
							 bioapi_INTERNAL_DEVICE_LIST_PTR *ppDevice,
							 BioAPI_LOCK_TYPE DeviceEntryLock);

BioAPI_RETURN
bioapi_FindDeviceByUUIDAndDeviceAndMultiLock(const BioAPI_UUID *pUUID,
										  uint32 DeviceID,
										  BioAPI_LOCK_TYPE ModuleListLock,
										  bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
										  BioAPI_LOCK_TYPE ModuleEntryLock,
										  bioapi_INTERNAL_DEVICE_LIST_PTR *ppDevice,
										  BioAPI_LOCK_TYPE DeviceEntryLock);

BioAPI_RETURN
bioapi_FindAttachAndMultiLock(BioAPI_HANDLE hAttach,
							BioAPI_LOCK_TYPE ModuleListLock,
							bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
							BioAPI_LOCK_TYPE ModuleEntryLock,
							bioapi_INTERNAL_DEVICE_LIST_PTR *DeviceRecord,
							BioAPI_LOCK_TYPE DeviceEntryLock,
							bioapi_INTERNAL_ATTACH_LIST_PTR *ppAttach,
							BioAPI_LOCK_TYPE AttachEntryLock);

void bioapi_ReleaseAttachLock(bioapi_INTERNAL_ATTACH_LIST_PTR pAttach,
							BioAPI_LOCK_TYPE ltLockType);
void bioapi_ReleaseDeviceLock(bioapi_INTERNAL_DEVICE_LIST_PTR pDevice,
							 BioAPI_LOCK_TYPE ltLockType);
void bioapi_ReleaseModuleLock(bioapi_INTERNAL_MODULE_LIST_PTR pModule,
							BioAPI_LOCK_TYPE ltLockType);
void bioapi_ReleaseModuleListLock(BioAPI_LOCK_TYPE ltLockType);


/* 9 most significant bits */
#define NUM_MODULE_INDEX_BITS	( 9 )
#define bioapi_ExtractModuleIndex(handle)  ( handle & 0xFF800000 )

/* Next 6 most significant bits */
#define NUM_DEVICE_INDEX_BITS  ( 6 )
#define bioapi_ExtractDeviceIndex(handle) ( handle & 0xFFFE0000 )

/* 17 least significant bits */
#define NUM_ATTACH_INDEX_BITS	( 17 )
#define bioapi_ExtractAttachIndex(handle)  ( handle & 0xFFFFFFFF )


/* getting the high and low parts of a 64-bit integer */
#define BioAPI_UINT64_HIGH(v) ( (uint32)( v >> 32 ) )
#define BioAPI_UINT64_LOW(v)  ( (uint32)v )
#define BioAPI_UINT64_SETHIGH(v,w) { v = ( ( v & 0xFFFFFFFF ) | ( (uint64)w << 32 ) ); }


BioAPI_RETURN BioAPI
bioapi_GetUpcallsTable(BioAPI_HANDLE hAddIn,
					 BioAPI_UPCALLS_PTR UpcallTable);

BioAPI_RETURN BioAPI
bioapi_GetAppMemoryFunctions(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord, BioAPI_MEMORY_FUNCS_PTR *MemFuncs);

BioAPI_RETURN BioAPI
bioapi_GetAddInCallBack(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord,
						void** CallBack);



/* setup/cleanup for standard API functions which are serviced by device
   providers
*/

BioAPI_RETURN
bioapi_SetupStandardSPICall(BioAPI_HANDLE ModuleHandle,
							void** CallBack,
							bioapi_INTERNAL_ATTACH_LIST_PTR *AttachRecord);

BioAPI_RETURN
bioapi_CleanupStandardSPICall(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord);



/* load/attach related internal functions */
BioAPI_RETURN
bioapi_ModuleLoad(const BioAPI_UUID *BSPUuid,
				  uint32 Reserved,
				  BioAPI_ModuleEventHandler AppNotifyCallback,
				  const void* AppNotifyCallbackCtx);

BioAPI_RETURN
bioapi_ModuleUnload(const BioAPI_UUID *BSPUuid,
					BioAPI_ModuleEventHandler AppNotifyCallback,
					const void* AppNotifyCallbackCtx);

BioAPI_RETURN
bioapi_ModuleAttach(const BioAPI_UUID *BSPUuid,
					const BioAPI_VERSION *Version,
					const BioAPI_MEMORY_FUNCS *MemoryFuncs,
					BioAPI_DEVICE_ID DeviceID,
					uint32 Reserved1,
					uint32 Reserved2,
					uint32 Reserved3,
					BioAPI_FUNC_NAME_ADDR_PTR FunctionTable,
					uint32 NumFunctionTable,
					const void *Reserved4,
					BioAPI_HANDLE_PTR NewModuleHandle,
					/* Linux-Port: replaced void * with BioAPI_UUID */
					const BioAPI_UUID *Reserved5);
					//const void *Reserved5);

BioAPI_RETURN
bioapi_ModuleDetach(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord,
					bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord);



BioAPI_RETURN
bioapi_QueryDevice(BioAPI_HANDLE ModuleHandle,
				   BioAPI_SERVICE_UID_PTR ServiceUID);

#ifndef BioAPI_MEMTRACK_ON

void * BioAPI
bioapi_SpiMalloc(BioAPI_HANDLE ModuleHandle,
				 uint32 Size);

void BioAPI
bioapi_SpiFree(BioAPI_HANDLE ModuleHandle,
			   void *Memblock);

void * BioAPI
bioapi_SpiRealloc(BioAPI_HANDLE ModuleHandle,
				  void *Memblock,
				  uint32 Size);

void * BioAPI
bioapi_SpiCalloc(BioAPI_HANDLE ModuleHandle,
				 uint32 Num,
				 uint32 Size);

#else

void * BioAPI
bioapi_SpiMalloc(BioAPI_HANDLE ModuleHandle,
				 uint32 Size,
				 const char* File,
				 uint32 Line);

void BioAPI
bioapi_SpiFree(BioAPI_HANDLE ModuleHandle,
			   void *Memblock,
				 const char* File,
				 uint32 Line);

void * BioAPI
bioapi_SpiRealloc(BioAPI_HANDLE ModuleHandle,
				  void *Memblock,
				  uint32 Size,
				 const char* File,
				 uint32 Line);

void * BioAPI
bioapi_SpiCalloc(BioAPI_HANDLE ModuleHandle,
				 uint32 Num,
				 uint32 Size,
				 const char* File,
				 uint32 Line);

#endif

BioAPI_RETURN
bioapi_CheckInit (void);



/* locking functions */
BioAPI_RETURN BioAPI bioapi_ReaderLock(cssm_SWMRLock_PTR SWMRLock);
BioAPI_RETURN BioAPI bioapi_ReaderUnlock (cssm_SWMRLock_PTR SWMRLock);
BioAPI_RETURN BioAPI bioapi_WriterLock (cssm_SWMRLock_PTR SWMRLock);
BioAPI_RETURN BioAPI bioapi_WriterUnlock (cssm_SWMRLock_PTR SWMRLock);
BioAPI_RETURN BioAPI bioapi_WriterToReader (cssm_SWMRLock_PTR SWMRLock);
BioAPI_RETURN BioAPI bioapi_SWMRInit (cssm_SWMRLock_PTR *SWMRLock);
BioAPI_RETURN BioAPI bioapi_SWMRDelete (cssm_SWMRLock_PTR SWMRLock);

BioAPI_RETURN BioAPI
bioapi_LockMultiTaskMutex(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord);
BioAPI_RETURN BioAPI
bioapi_UnlockMultiTaskMutex(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord);
BioAPI_RETURN BioAPI
bioapi_LockMultiTaskMutexByHandle(BioAPI_HANDLE ModuleHandle);
BioAPI_RETURN BioAPI
bioapi_UnlockMultiTaskMutexByHandle(BioAPI_HANDLE ModuleHandle);
BioAPI_RETURN BioAPI
bioapi_GetAttachRecordAndLockMultiTask(BioAPI_HANDLE hAddIn,
									 void **CallBack,
									 bioapi_INTERNAL_ATTACH_LIST_PTR *AttachInfoPtr);
BioAPI_RETURN BioAPI
bioapi_ReleaseAttachRecordAndUnlockMultiTask(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord);


/* internal list additions/deletions */
BioAPI_RETURN BioAPI
bioapi_CleanInternalModuleRecord(bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
							   CSSM_LIB_HANDLE *MainLibHandle);

void BioAPI
bioapi_CleanInternalDeviceRecord(bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord,
								bioapi_INTERNAL_DEVICE_LIST_PTR *DeviceRecord);

void BioAPI
bioapi_CleanInternalAttachRecord(bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord,
							   bioapi_INTERNAL_ATTACH_LIST_PTR *AttachRecord);


BioAPI_RETURN
bioapi_InsertAttachRecord(const BioAPI_MEMORY_FUNCS *MemoryFunc,
						const BioAPI_VERSION *Version,
						uint32 Reserved,
						uint32 Reserved1,
						uint32 Reserved2,
						const bioapi_INTERNAL_MODULE_LIST *ModuleRecord,
						bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord,
						BioAPI_HANDLE *NewAttachHandle,
						bioapi_INTERNAL_ATTACH_LIST_PTR *NewAttachRecord);

BioAPI_RETURN
bioapi_InsertDeviceRecord(uint32 DeviceID,
						 bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry,
						 bioapi_INTERNAL_DEVICE_LIST_PTR *pDeviceEntry);

BioAPI_RETURN bioapi_DeleteDeviceRecord( bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry,
							   bioapi_INTERNAL_DEVICE_LIST_PTR pDeviceEntry );

BioAPI_RETURN BioAPI bioapi_UnloadModule(const BioAPI_UUID *BSPUuid,
									  void* AppNotifyCallbackCtx);


BioAPI_RETURN BioAPI bioapi_ModuleEventHandler(
							const BioAPI_UUID *BSPUuid,
							void* AppNotifyCallbackCtx,
							BioAPI_DEVICE_ID DeviceID,
							uint32 Reserved,
							BioAPI_MODULE_EVENT EventType);


/* Management interface functions */
void BioAPI bioapi_CleanModuleRecord (const BioAPI_UUID *UUID);



BioAPI_RETURN BioAPI bioapi_RemoveModuleRecord (const BioAPI_UUID *UUID);
BioAPI_RETURN BioAPI
bioapi_NewModuleRecord(const BioAPI_UUID *UUID,
					 bioapi_INTERNAL_MODULE_LIST_PTR *NewModuleRecord);

BioAPI_RETURN BioAPI bioapi_UpdateModuleRecord (
								 const BioAPI_UUID *UUID,
								 const BioAPI_MODULE_FUNCS_PTR FunctionTable);

BioAPI_RETURN BioAPI bioapi_LoadAddInByName(char * pAddInName,
							uint32 AddInType,
							BioAPI_HANDLE *phAddIn);
BioAPI_RETURN BioAPI bioapi_UnloadAddInByHandle(BioAPI_HANDLE hAddIn);



BioAPI_RETURN BioAPI bioapi_AddInDetach (BioAPI_HANDLE AddInHandle);


uint32 bioapi_Rand();
BioAPI_RETURN bioapi_CheckAPIMemFuncs (
							  const BioAPI_MEMORY_FUNCS *MemoryFunctions);
BioAPI_RETURN BioAPI bioapi_GetUsageMask(BioAPI_HANDLE ModuleHandle);
BioAPI_RETURN BioAPI bioapi_GetUUIDFromHandle(BioAPI_HANDLE ModuleHandle,
										   BioAPI_UUID_PTR UuidPtr);

UNIX_HANDLE BioAPI bioapi_GetSWMRLockHandle (void *ModuleInfo);

BioAPI_RETURN bioapi_CleanUp (void);



/* event notify callback list functions */
BioAPI_RETURN bioapi_PruneCallbackList(
							bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry,
							BioAPI_ModuleEventHandler AppCallback,
							const void *AppCallbackCtx );
BioAPI_RETURN bioapi_UpdateCallbackList(
							bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry,
							BioAPI_ModuleEventHandler AppCallback,
							const void *AppCallbackCtx );
BioAPI_RETURN bioapi_BroadcastModuleEvent(const BioAPI_UUID *BSPUuid,
								uint32 DeviceID,
								BioAPI_MODULE_EVENT EventType );

BioAPI_RETURN bioapi_GetFunctionTable(BioAPI_FUNC_NAME_ADDR_PTR FuncNameAddrPtr,
								  uint32 NumOfFuncNameAddre,
								  BioAPI_HANDLE hAddIn);


BioAPI_RETURN BioAPI
bioapi_UUIDToStr( const BioAPI_UUID *Uuid,	char *ID);

BioAPI_DATA_PTR BioAPI
Base64_Decode(BioAPI_HANDLE AppHandle,
			  const BioAPI_UPCALLS *MemoryFunctions,
			  const BioAPI_DATA *pEncodedData);

BioAPI_RETURN BioAPI
bioapi_GetModuleInfo_func(BioAPI_HANDLE Module,
						BioAPI_UUID_PTR Uuid,
						BioAPI_VERSION *Version,
						BioAPI_FUNC_NAME_ADDR_PTR FunctionTable,
						uint32 *NumFunctions);

BioAPI_RETURN BioAPI
bioapi_AttachCommon(const BioAPI_UUID *ModuleUuid,
					const BioAPI_VERSION *Version,
					const BioAPI_MEMORY_FUNCS *MemoryFuncs,
					uint32 SubserviceID,
					uint32 Reserved,
					uint32 Reserved1,
					uint32 Reserved2,
					/* Linux-Port: replaced void * with BioAPI_UUID */
					const BioAPI_UUID *CallerUuid,
					//const void * CallerUuid,
					BioAPI_FUNC_NAME_ADDR *FunctionTable,
					uint32 NumFunctionTable,
					const void * Reserved4,
					BioAPI_HANDLE_PTR NewModuleHandle,
					bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
					bioapi_INTERNAL_DEVICE_LIST_PTR *ServiceRecord,
					bioapi_INTERNAL_ATTACH_LIST_PTR *AttachRecord);

BioAPI_RETURN BioAPI
bioapi_ModuleAttachNoInteg(const BioAPI_UUID *ModuleUuid,
						 const BioAPI_VERSION *Version,
						 const BioAPI_MEMORY_FUNCS *MemoryFuncs,
						 uint32 SubserviceID,
						 uint32 Reserved,
						 uint32 Reserved1,
					   /* Linux-Port: replaced void * with BioAPI_UUID */
						 const BioAPI_UUID *CallerUuid,
						 //const void * CallerUuid,
						 uint32 Reserved2,
						 BioAPI_FUNC_NAME_ADDR *FunctionTable,
						 uint32 NumFunctionTable,
						 const void * Reserved3,
						 BioAPI_HANDLE_PTR NewModuleHandle);

BioAPI_RETURN
bioapi_AttachModuleNoKeys(const BioAPI_UUID *ModuleUuid,
						  const BioAPI_VERSION *Version,
						  uint32 DeviceId,
						  uint32 Reserved1,
						  uint32 Reserved2,
						  BioAPI_HANDLE ModuleHandle,
						  uint32 Reserved3,
						  const void *Reserved4,
						  BioAPI_MODULE_FUNCS_PTR *AttachFuncTbl,
						  bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord,
						  bioapi_INTERNAL_DEVICE_LIST_PTR ServiceRecord);

BioAPI_RETURN
bioapi_DeleteModuleRecordonLoadFail(const BioAPI_UUID *ModuleUuid,
								  BioAPI_ModuleEventHandler AppNotifyCallback,
								  const void* AppNotifyCallbackCtx);


#ifdef __cplusplus
}
#endif

#endif
