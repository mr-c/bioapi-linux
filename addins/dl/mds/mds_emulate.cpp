/*-----------------------------------------------------------------------
 * File: MDS_EMULATE.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/*
 * This file implements functions that emulate parts of the DAL and MAF for MDS
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

#include "port/bioapi_port.h"
#include "port/bioapi_lock.h"
#include "bioapi_type.h"
#include "bioapi_err.h"

#include "maf_collectn.h"
#include "maf_util.h"
#include "mds.h"
#include "mds_internal.h"

#define DAL_SOURCE_FILE
#include "dal_classes.h"

struct mds_attach_tracker
{
#ifdef _DEBUG
	uint32 debug_MagicNumber;
#endif



	MDS_HANDLE MdsHandle;
	CSSM_MEMORY_FUNCS MemFuncs;
	DAL_DATABASE_INFO_LIST DatabaseList;

	mds_attach_tracker();
};

typedef struct mds_attach_tracker MDS_ATTACH_TRACKER;

 /*************************************************************************
 *************************************************************************
 ***** Begin thread local storage abstractions ***************************
 *************************************************************************
 *************************************************************************/

/*************************************************************************
 *************************************************************************
 ***** Static Varaibles in mds_emulate.cpp *******************************
 *************************************************************************
 *************************************************************************/

/* Thread local storage index value for keeping track of the operating
 * context of the SPI call (which CSSM called it).
 */
static PORT_TLS_INDEX s_tlsThreadContext = PORT_INVALID_TLS_INDEX;

/* List for maintaining a record of attaches.
 */
static MAF_LIST_COLLECTION s_lcAttachList;

/* Are we between the end of _init and the beginning of _fini? */
CSSM_BOOL g_initComplete = CSSM_FALSE;

/* mutex  */
static PORT_MUTEX_HANDLE s_InitMutex = NULL;
#ifdef POSIX
static pthread_mutex_t initMutex = PORT_STATIC_MUTEX_INITIALIZER;
#endif

/*************************************************************************
 *************************************************************************
 ***** Private Interface declaration *************************************
 *************************************************************************
 *************************************************************************/

static int MLC_FASTCALL mds_FindAttachByHandle( void *pItem, void *pKey );

#ifdef _DEBUG /* Debug version */
#define ATTACH_TRACKER_MAGIC_NUMBER(pTracker) (~((uint32)(pTracker))+1)
static void ATTACH_TRACKER_ASSERT_VALID(const MDS_ATTACH_TRACKER * pTracker)
{
	ASSERT(port_IsBadReadPtr(pTracker, sizeof(MDS_ATTACH_TRACKER)) == CSSM_FALSE);
	ASSERT((pTracker)->debug_MagicNumber == ATTACH_TRACKER_MAGIC_NUMBER(pTracker));
}
#else /* Release version */
#define ATTACH_TRACKER_MAGIC_NUMBER(pTracker)
#define ATTACH_TRACKER_ASSERT_VALID(pTracker)
#endif

#ifdef _DEBUG
static void mds_SetCurrentAttach(const MDS_ATTACH_TRACKER * pContext)
{
    port_SetTlsValue( s_tlsThreadContext, (void *)pContext );
}
#else
#define mds_SetCurrentAttach(pContext) port_SetTlsValue( s_tlsThreadContext, (void *)pContext )
#endif

static const MDS_ATTACH_TRACKER * mds_GetCurrentAttach(void);
#define mds_ClearCurrentAttach() port_SetTlsValue( s_tlsThreadContext, NULL)
#define ASSERT_NO_CURRENT_ATTACH() \
{ \
    void *TempTlsValue = NULL; \
    port_GetTlsValue( s_tlsThreadContext, &TempTlsValue ); \
    ASSERT( TempTlsValue == NULL ); \
}


/* Application's memory allocation functions */
void* BioAPI _App_Malloc(
						CSSM_MODULE_HANDLE MdsHandle,
						uint32 size
#ifdef BioAPI_MEMTRACK_ON
						, const char *szFilename,
						uint32 u32LineNumber
#endif
						)
{
	const MDS_ATTACH_TRACKER * pTracker = mds_GetCurrentAttach();

	if ( pTracker == NULL )
	{
		/* Did someone forget to call Addin_SPIBegin? */
		return NULL;
	}

	ASSERT(pTracker->MdsHandle == MdsHandle);
	return pTracker->MemFuncs.malloc_func( size,
										pTracker->MemFuncs.AllocRef
#ifdef BioAPI_MEMTRACK_ON
									, szFilename,
									u32LineNumber
#endif
									);
}

void* BioAPI _App_Realloc(
						CSSM_MODULE_HANDLE MdsHandle,
						void *Ptr,
						uint32 size
#ifdef BioAPI_MEMTRACK_ON
						, const char * szFilename,
						uint32 u32LineNumber
#endif
						)
{
	const MDS_ATTACH_TRACKER * pTracker = mds_GetCurrentAttach();

	if ( pTracker == NULL )
	{
		/* Did someone forget to call Addin_SPIBegin? */
		return NULL;
	}

	ASSERT(pTracker->MdsHandle == MdsHandle);
	return pTracker->MemFuncs.realloc_func( Ptr,
										size,
										pTracker->MemFuncs.AllocRef
#ifdef BioAPI_MEMTRACK_ON
										, szFilename,
										u32LineNumber
#endif
										);
}

void BioAPI _App_Free(
					   CSSM_MODULE_HANDLE MdsHandle,
					   void *Ptr
#ifdef BioAPI_MEMTRACK_ON
					   , const char *szFilename,
					   uint32 u32LineNumber
#endif
					   )
{
	const MDS_ATTACH_TRACKER * pTracker = mds_GetCurrentAttach();

	if ( pTracker == NULL )
	{
		/* Did someone forget to call Addin_SPIBegin? */
		return;
	}

	ASSERT(pTracker->MdsHandle == MdsHandle);
	pTracker->MemFuncs.free_func( Ptr,
								pTracker->MemFuncs.AllocRef
#ifdef BioAPI_MEMTRACK_ON
								, szFilename,
								u32LineNumber
#endif
								);
}

void* BioAPI _App_Calloc(
						CSSM_MODULE_HANDLE MdsHandle,
						uint32 size,
						uint32 count
#ifdef BioAPI_MEMTRACK_ON
						, const char * szFilename,
						uint32 u32LineNumber
#endif
						)
{
	const MDS_ATTACH_TRACKER * pTracker = mds_GetCurrentAttach();

	if ( pTracker == NULL )
	{
		/* Did someone forget to call Addin_SPIBegin? */
		return NULL;
	}

	ASSERT(pTracker->MdsHandle == MdsHandle);
	return pTracker->MemFuncs.calloc_func( size,
										count,
										pTracker->MemFuncs.AllocRef
#ifdef BioAPI_MEMTRACK_ON
										, szFilename,
										u32LineNumber
#endif
										);
}


/*************************************************************************
 *************************************************************************
 ***** Interface functions that are not from maf *************************
 *************************************************************************
 *************************************************************************/

BioAPI_RETURN mds_SetFunctionPointers(
	const BioAPI_MEMORY_FUNCS * pMemFuncs,
	MDS_HANDLE *hMds)
{
	static MDS_HANDLE s_LastMdsHandle = 1;
	MLC_LOCK_REF LockRef = MLC_NULL_LOCK_REF;

	if (g_initComplete == CSSM_FALSE) {
		return CSSMERR_DL_FUNCTION_FAILED;
	}

	ASSERT_NO_CURRENT_ATTACH();
	mds_ClearCurrentAttach();

	/* Create a new attach node */
	MDS_ATTACH_TRACKER * pAttach = new MDS_ATTACH_TRACKER;

	if (pAttach == NULL)
		return CSSMERR_DL_MEMORY_ERROR;

	/* assign the pointers */
	pAttach->MemFuncs = *(const CSSM_MEMORY_FUNCS_PTR)pMemFuncs;

	/* generate a handle */
	if (CSSM_OK != port_LockMutex (s_InitMutex, CSSM_INFINITE_WAIT))
	{
		delete pAttach;
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	pAttach->MdsHandle = *hMds = s_LastMdsHandle;
	s_LastMdsHandle += 3;

	port_UnlockMutex(s_InitMutex);

	/* Initialize the database list */
	mds_SetCurrentAttach(pAttach);

	if (CSSM_OK != pAttach->DatabaseList.Initialize())
	{
		delete pAttach;
	}

	mds_ClearCurrentAttach();

	/* Add the item to the list */
	if (CSSM_OK != MLC_AddItem( &s_lcAttachList, (void *)pAttach , MLC_NO_LOCK, &LockRef) )
	{
		/* Couldn't add the item to the list */
		delete pAttach;
		return CSSMERR_DL_FUNCTION_FAILED;
	}
	pAttach = NULL;

	return CSSM_OK;
}

BioAPI_RETURN mds_Terminate(MDS_HANDLE Handle)
{
	MDS_ATTACH_TRACKER * pAttachTracker = NULL;

	if (g_initComplete == CSSM_FALSE)
		return (CSSMERR_DL_INVALID_DL_HANDLE);

	ASSERT_NO_CURRENT_ATTACH();
	mds_ClearCurrentAttach();

	if (Handle == 0) {
		return (CSSMERR_DL_INVALID_DL_HANDLE);
	}

	/* Unlink the tracker for this attach */
	if (MLC_DeleteItem( &s_lcAttachList,
						mds_FindAttachByHandle,
						(void*)&Handle,
						(void **)&pAttachTracker) != CSSM_OK)
	{
		return (CSSMERR_DL_INVALID_DL_HANDLE);
	}

	mds_ClearCurrentAttach();
	delete pAttachTracker;

	return CSSM_OK;
}

DAL_DATABASE_INFO_LIST * dal_GetDatabaseList()
{
    MDS_ATTACH_TRACKER * pAttachTracker = NULL;

    port_GetTlsValue( s_tlsThreadContext, (void **)&pAttachTracker );

	if (pAttachTracker != NULL)
	{
		ATTACH_TRACKER_ASSERT_VALID(pAttachTracker);
		return &pAttachTracker->DatabaseList;
	}
	else
		return NULL;
}

/*************************************************************************
 *************************************************************************
 ***** Emulated MAF functions ********************************************
 *************************************************************************
 *************************************************************************/


/*-----------------------------------------------------------------------------
 * Name: Addin_SPIBegin
 *
 * Description:
 * Function that is called at the begining of every protected interface, finds
 * the context (which CSSM are we talking to) and stores information in thread
 * local storage
 *
 * Note: SPIBegin/End operate in a multi-CSSM environment. It follows these basic steps.
 *
 * 1) Lock the module for reading to prevent the final unload
 *	  from happening while the SPI is being serviced.
 * 2) Find the tracking record for the given handle. This gives
 *	  the proper set of callbacks into the calling CSSM as well
 *	  as enabling strong validation of all SPI calls.
 *	  A) Get a read lock on the tracking record to prevent a
 *		 detach in another thread from taking place while the
 *		 call is being serviced.
 *	  B) Get a write lock on the tracking record fs the SPI
 *		 needs to information in the record.
 * 3) Get a tracking record for the CSSM that the mds handle belongs to
 * 4) Verify that the return address belongs to the same module
 *	  that originally attached to the mds. This prevents
 *	  another module from masquerading as the original.
 * 5) Perform the SPI functions.
 * 6) Release the locks on the tracking record and the library.
 *
 * Parameters:
 * MdsHandle (input) : Handle of the mds
 * ReturnAddress (input) : Address of the caller
 *
 * Return Code:
 * NULL if the function failed, otherwise a lock reference that is returned to
 * Addin_SPIEnd
 *-------------------------------------------------------------*/
BioAPI_RETURN Addin_SPIBegin(
			BioAPI_HANDLE MdsHandle,
			MLC_LOCK_REF *pLockRef)
{
	const MDS_ATTACH_TRACKER * pAttachTracker = NULL;

#ifdef DBG_SPIBEGIN
	printf("mds_emulate.cpp:  Addin_SPIBegin() called +++++++++\n");
#endif


	/* Determine if we are between the end of _init and the beginning of _fini */
	if (g_initComplete == CSSM_FALSE)
		return CSSMERR_DL_FUNCTION_FAILED;

	ASSERT_NO_CURRENT_ATTACH();

	/* Find the tracker for this attach */
	if (MLC_FindItem (&s_lcAttachList,
						mds_FindAttachByHandle,
						(void*)&MdsHandle,
						MLC_READ_LOCK,
						pLockRef,
						(void **)&pAttachTracker) != CSSM_OK)
	{
		/* Caller passed an invalid MdsHandle */
		return (CSSMERR_DL_INVALID_DL_HANDLE);
	}

	/* Set the execution context for the thread so that we call the correct
	 * API/SPI functions.
	 */
	mds_SetCurrentAttach(pAttachTracker);

	/* Error codes can be returned starting here since we have the function
	 * pointers and we know they point to the right place.
	 */
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: Addin_SPIEnd
 *
 * Description:
 * Function that is called at the end of every protected interface. This function
 * cleans up state created by Addin_SPIBegin
 *
 * Parameters:
 * pLockRef (input) : lock reference returned by SPIBegin
 *
 * Return Code:
 * None
 *---------------------------------------------------------------------------*/
void Addin_SPIEnd(MLC_LOCK_REF pLockRef)
{
	mds_ClearCurrentAttach();
	MLC_ReleaseItem( MLC_READ_LOCK, pLockRef );
}

/* In debug mode, the linker is not smart enough to realize that it doesn't need this func */


/*-----------------------------------------------------------------------------
 * Name: Addin_GetProcAddress
 *
 * Description:
 * Gets function pointer from CSSM.
 *
 * Parameters:
 * FuntionName (input) - Name of function to be imported
 * ppFunction (output) - Pointer to function retrieved
 * hCssm (input) - Handle to CSSM used to extract functions from
 *
 * Return value:
 * CSSM_FAIL, if the function pointer could not be retrieved
 * CSSM_OK, if the funtion pointer was retrieved successfully
 *----------------------------------------------------------------------------*/
BioAPI_RETURN Addin_GetProcAddress(
	const char * FunctionName,
	CSSM_PROC_ADDR * ppFunction,
	CSSM_MODULE_HANDLE hCssmModule)
{
	hCssmModule;
	FunctionName;
	*ppFunction = NULL;
	return CSSMERR_DL_INTERNAL_ERROR;
}

/*************************************************************************
 *************************************************************************
 ***** Private Interface functions ***************************************
 *************************************************************************
 *************************************************************************/

static int MLC_FASTCALL mds_FindAttachByHandle( void *pItem, void *pKey )
{
	const MDS_ATTACH_TRACKER * pAttachTracker = (const MDS_ATTACH_TRACKER *)pItem;
	const MDS_HANDLE *pHandle = (const MDS_HANDLE *)pKey;

	ASSERT(pKey);
	ATTACH_TRACKER_ASSERT_VALID(pAttachTracker);

	return ( pAttachTracker->MdsHandle - *pHandle );
}

static const MDS_ATTACH_TRACKER * mds_GetCurrentAttach(void)
{
    const MDS_ATTACH_TRACKER * pAttachTracker = NULL;

    port_GetTlsValue( s_tlsThreadContext, (void **)&pAttachTracker );

	if (pAttachTracker != NULL)
	{
		return pAttachTracker;
	}
	else
		return NULL;
}

mds_attach_tracker::mds_attach_tracker()
{
	memset(this, 0, sizeof(mds_attach_tracker));
#ifdef _DEBUG
	debug_MagicNumber = ATTACH_TRACKER_MAGIC_NUMBER(this);
#endif
}

#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
extern "C"
{
#elif defined (UNIX)
/*
 * Specifying C linkage lets ld look for these functions as _init and
 * _fini, not as mangled names.
 */
extern "C" {
	int _init();
	int _fini();
};
#endif

/*---------------------------------------------------------------
 *
 *Name: _init
 *
 *Description:
 *	 Entry point for the DLL in non-Win32 environments. This
 *	 function matches the conventions of the Solaris DDI
 *	 interface as well as the AIX standard interface. This
 *	 function should only be used initialization that must be
 *	 done atomically in an environment where there is a guarantee
 *	 that only a single thread can be running this code. The CSSM
 *	 maintains an exclusive lock on its internal tracking
 *	 structure for this library when this function is called.
 *	 NOTE: Activity in this function should be kept to a minimum
 *	 for performance reasons. It should be limited to clearing
 *	 data values and creating resource locks only!
 *
 *Parameters:
 *	 None.
 *
 *Returns:
 *   CSSM_FALSE - Initialization failed. Loading fails.
 *   CSSM_TRUE - DLL is initialized for use.
 *
 *-------------------------------------------------------------*/
#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
int _init_mds()
#else
int _init()
#endif
{
#if defined (WIN32)
	CSSM_RETURN ret = port_CreateMutex(NULL, &s_InitMutex);
	if (ret != CSSM_OK || s_InitMutex == NULL)
        return CSSM_FALSE;
#elif defined (POSIX)
    s_InitMutex = &initMutex;
#endif

	/* Initialize the thread local storage for the module */
    if ( port_AllocateTlsIndex( &s_tlsThreadContext, NULL ) != CSSM_OK )
	{
        return CSSM_FALSE;
	}

	/* Initialize the locking on the load and attach tracking lists */
	if (CSSM_OK != MLC_Init( &s_lcAttachList, NULL))
	{
        port_DeleteTlsIndex( s_tlsThreadContext );
        return CSSM_FALSE;
	}

	ASSERT_NO_CURRENT_ATTACH();

	if (CSSM_OK != dlbe_Initialize())
	{
        return CSSM_FALSE;
	}

	g_initComplete = CSSM_TRUE;
    return CSSM_TRUE;
}

/*---------------------------------------------------------------
 *
 *Name: _fini
 *
 *Description:
 *	 Entry point for the DLL in non-Win32 environments. This
 *	 function matches the conventions of the Solaris DDI
 *	 interface as well as the AIX standard interface. This
 *	 function should only be used for emergency clean-up for
 *	 unexpected shutdowns or uninitialization that must be done
 *	 in an environment where there is a guarantee that only a
 *	 single thread can be running this code. The CSSM maintains
 *	 an exclusive lock on its internal tracking structure for
 *	 this library when this function is called.
 *
 *Parameters:
 *	 None.
 *
 *Returns:
 *   CSSM_FALSE - whoops.
 *   CSSM_TRUE - DLL is uninitialized for unloading
 *
 *-------------------------------------------------------------*/

static void MLC_FASTCALL mds_AttachTrackerDelete(void *pItem)
{
	MDS_ATTACH_TRACKER *pTracker = (MDS_ATTACH_TRACKER*)pItem;
	ASSERT_NO_CURRENT_ATTACH();
	/*mds_SetCurrentAttach(pTracker);	*/
	delete pTracker;
	/*mds_ClearCurrentAttach();*/
}

#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
int _fini_mds()
#else
int _fini()
#endif
{
    int retCode = CSSM_TRUE;

	g_initComplete = CSSM_FALSE;

	ASSERT_NO_CURRENT_ATTACH();
	mds_ClearCurrentAttach();

	if (CSSM_OK != dlbe_Uninitialize())
	{
        retCode = CSSM_FALSE;
	}

	/* Destroy the SWMR lock gaurding the library resources */
	if (CSSM_OK != MLC_CleanList(&s_lcAttachList, mds_AttachTrackerDelete))
        retCode = CSSM_FALSE;

	port_CloseMutex(s_InitMutex);

	MLC_Term( &s_lcAttachList );

	/* Deallocate the thread local storage index */
    port_DeleteTlsIndex( s_tlsThreadContext );

	/* Always succedes */
	return retCode;
}

#ifdef _WIN32

/*---------------------------------------------------------------
 *
 *Name: DllMain
 *
 *Description:
 *	 Entry point for the DLL in Win32. This function is a Win32
 *	 formality. It just calls the _init and _fini functions which
 *	 are cross-platform.
 *
 *Parameters:
 *	 hInstance (input) - Instance handle of calling process
 *	 dwReason (input) - Reasons for receiving this call
 *	 lpReserved (input) - Reserved field
 *
 *Returns:
 *   CSSM_FALSE - unable to initialize DLL.  This will cause the LoadLibrary
 *			 call to fail.
 *   CSSM_TRUE - DLL is initialize and ready to be called
 *
 *-------------------------------------------------------------*/
BOOL WINAPI DllMain (HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	dwReason;
	lpReserved;

	switch ( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		{
			/* Call the cross-platform _init entry point to keep the
			 * code relatively cross-platform.
			 */

			return _init();

			break;
		}

	case DLL_THREAD_ATTACH:
		{
			break;
		}

	case DLL_THREAD_DETACH:
		{
			break;
		}

	case DLL_PROCESS_DETACH:
		{
			/* Call the cross platform _fini entry point to keep the
			 * code relatively cross-platform.
			 */
			int iStatus = _fini();

#ifdef _DEBUG
		port_DumpMemoryLeaks( "MDS" );
#endif

			return iStatus;

			break;
		}
	}

    return CSSM_TRUE;
}

#endif
#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
}
#endif
