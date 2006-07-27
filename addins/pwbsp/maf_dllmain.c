/*-----------------------------------------------------------------------
 * File: MAF_DLLMAIN.C
 *-----------------------------------------------------------------------
 */

#define BioAPI_INITUUID
#include "maf_include.h"
#include "bioapi_util.h"

/* Microsoft specific headers for debug heap usage */
#if defined(_DEBUG) && defined(WIN32)
#include <crtdbg.h> /* This header needed to use the debug heap functions
					 * directly to change the apparent calling location.
					 */
#endif

/* *******************************************************************
 * Check the validity of maf_config.h
 * *******************************************************************/

/* *******************************************************************
 * BSP options
 * *******************************************************************/
#ifndef BSP_UUID_INITIALIZER
 #error BSP_UUID_INITIALIZER not defined in maf_config.h
#endif
#ifndef BSP_DEVICE_ID
 #error BSP_DEVICE_ID not defined in maf_config.h
#endif
#ifndef BSP_SELF_CHECK_SECTION
 #error BSP_SELF_CHECK_SECTION not defined in maf_config.h
#endif
#ifndef BSP_VERSION_MAJOR
 #error BSP_VERSION_MAJOR not defined in maf_config.h
#endif
#ifndef BSP_VERSION_MINOR
 #error BSP_VERSION_MAJOR not defined in maf_config.h
#endif
#ifndef BSP_VENDOR_NAME
 #error BSP_VENDOR_NAME not defined in maf_config.h
#endif

#ifndef BSP_NUM_SUPPORTED_FORMATS
 #error BSP_NUM_SUPPORTED_FORMATS not defined in maf_config.h
#endif
#if BSP_NUM_SUPPORTED_FORMATS /* only require an initializer if there are formats */
 #ifndef BSP_SUPPORTED_FORMATS_INITIALIZER
  #error BSP_NUM_SUPPORTED_FORMATS defined, but BSP_SUPPORTED_FORMATS_INITIALIZER not defined in maf_config.h
 #endif
#endif

#ifndef BSP_FACTORS_MASK
 #error BSP_FACTORS_MASK not defined in maf_config.h
#endif
#if !BSP_FACTORS_MASK
 #error BSP_FACTORS_MASK cannot be zero
#endif
#ifndef BSP_OPERATIONS
 #error BSP_OPERATIONS not defined in maf_config.h
#endif
#if !BSP_OPERATIONS
 #error BSP_OPERATIONS cannot be zero
#endif
#ifndef BSP_OPTIONS
 #error BSP_OPTIONS not defined in maf_config.h
#endif

#ifndef BSP_PAYLOAD_POLICY
 #error BSP_PAYLOAD_POLICY not defined in maf_config.h
#endif
#ifndef BSP_MAX_PAYLOAD
 #error BSP_MAX_PAYLOAD not defined in maf_config.h
#endif
#ifndef BSP_VERIFY_TIMEOUT
 #error BSP_VERIFY_TIMEOUT not defined in maf_config.h
#endif
#ifndef BSP_IDENTIFY_TIMEOUT
 #error BSP_IDENTIFY_TIMEOUT not defined in maf_config.h
#endif
#ifndef BSP_CAPTURE_TIMEOUT
 #error BSP_CAPTURE_TIMEOUT not defined in maf_config.h
#endif
#ifndef BSP_ENROLL_TIMEOUT
 #error BSP_ENROLL_TIMEOUT not defined in maf_config.h
#endif
#ifndef BSP_MAX_DB_SIZE
 #error BSP_MAX_DB_SIZE not defined in maf_config.h
#endif
#ifndef BSP_MAX_IDENTIFY
 #error BSP_MAX_IDENTIFY not defined in maf_config.h
#endif
#ifndef BSP_DESCRIPTION
 #error BSP_DESCRIPTION not defined in maf_config.h
#endif


/* *******************************************************************
 * Device options
 * *******************************************************************/
#ifndef DEVICE_UUID_INITIALIZER
 #error DEVICE_UUID_INITIALIZER not defined in maf_config.h
#endif

#ifndef DEVICE_NUM_SUPPORTED_FORMATS
 #error DEVICE_NUM_SUPPORTED_FORMATS not defined in maf_config.h
#endif
#if DEVICE_NUM_SUPPORTED_FORMATS /* only require an initializer if there are formats */
 #ifndef DEVICE_SUPPORTED_FORMATS_INITIALIZER
  #error DEVICE_NUM_SUPPORTED_FORMATS defined, but DEVICE_SUPPORTED_FORMATS_INITIALIZER not defined in maf_config.h
 #endif
#endif

#ifndef DEVICE_SUPPORTED_EVENTS
 #error DEVICE_SUPPORTED_EVENTS not defined in maf_config.h
#endif
#ifndef DEVICE_VENDOR_NAME
 #error DEVICE_VENDOR_NAME not defined in maf_config.h
#endif
#ifndef DEVICE_DESCRIPTION
 #error DEVICE_DESCRIPTION not defined in maf_config.h
#endif
#ifndef DEVICE_HW_VERSION_MAJOR
 #error DEVICE_HW_VERSION_MAJOR not defined in maf_config.h
#endif
#ifndef DEVICE_HW_VERSION_MINOR
 #error DEVICE_HW_VERSION_MINOR not defined in maf_config.h
#endif
#ifndef DEVICE_FW_VERSION_MAJOR
 #error DEVICE_FW_VERSION_MAJOR not defined in maf_config.h
#endif
#ifndef DEVICE_FW_VERSION_MINOR
 #error DEVICE_FW_VERSION_MINOR not defined in maf_config.h
#endif
#ifndef DEVICE_SERIAL_NUMBER
 #error DEVICE_SERIAL_NUMBER not defined in maf_config.h
#endif
#ifndef DEVICE_AUTHENTICATED
 #error DEVICE_AUTHENTICATED not defined in maf_config.h
#endif

/* *******************************************************************
 * General options
 * *******************************************************************/
#ifndef BSP_THREAD_SAFE
 #error BSP_THREAD_SAFE not defined in maf_config.h
#endif
#ifndef BSP_NEED_LOAD_WRITER_LOCK
 #error BSP_NEED_LOAD_WRITER_LOCK not defined in maf_config.h
#endif
#ifndef BSP_NEED_ATTACH_WRITER_LOCK
 #error BSP_NEED_ATTACH_WRITER_LOCK not defined in maf_config.h
#endif
#ifndef ADDIN_NEED_ADDIN_LOAD_STRUCT
 #error ADDIN_NEED_ADDIN_LOAD_STRUCT not defined in maf_config.h
#endif
#ifndef ADDIN_NEED_ADDIN_ATTACH_STRUCT
 #error ADDIN_NEED_ADDIN_ATTACH_STRUCT not defined in maf_config.h
#endif
#ifndef BSP_IS_VALID_DEVICE
 #error BSP_IS_VALID_DEVICE not defined in maf_config.h
#endif

/*************************************************************************
 *************************************************************************
 ***** Global constants for MAF and SP implementation ********************
 *************************************************************************
 *************************************************************************/
//const BioAPI_UUID BSP_UUID = BSP_UUID_INITIALIZER;


/* Application's memory allocation functions */

void* BioAPI _App_Malloc(
						BioAPI_HANDLE AddinHandle,
						uint32 size
#ifdef BioAPI_MEMTRACK_ON
						, const char *szFilename,
						uint32 u32LineNumber
#endif
						)
{
	const MAF_MODULE_ATTACH_TRACKER *pTracker = Addin_GetAttachTracker();

	if ( pTracker == NULL )
	{
		/* Did someone forget to call Addin_SPIBegin? */
		return NULL;
	}

	return pTracker->AppUpcalls.Malloc_func(
									AddinHandle,
									size
#ifdef BioAPI_MEMTRACK_ON
									, szFilename,
									u32LineNumber
#endif
									);
}


void* BioAPI _App_Realloc(
						BioAPI_HANDLE AddinHandle,
						void *Ptr,
						uint32 size
#ifdef BioAPI_MEMTRACK_ON
						, const char * szFilename,
						uint32 u32LineNumber
#endif
						)
{
	const MAF_MODULE_ATTACH_TRACKER *pTracker = Addin_GetAttachTracker();

	if ( pTracker == NULL )
	{
		/* Did someone forget to call Addin_SPIBegin? */
		return NULL;
	}

	return pTracker->AppUpcalls.Realloc_func(
									AddinHandle,
									Ptr,
									size
#ifdef BioAPI_MEMTRACK_ON
									, szFilename,
									u32LineNumber
#endif
									);
}


void BioAPI _App_Free(
					   BioAPI_HANDLE AddinHandle,
					   void *Ptr
#ifdef BioAPI_MEMTRACK_ON
					   , const char *szFilename,
					   uint32 u32LineNumber
#endif
					   )
{
	const MAF_MODULE_ATTACH_TRACKER *pTracker = Addin_GetAttachTracker();

	if ( pTracker == NULL )
	{
		/* Did someone forget to call Addin_SPIBegin? */
		return;
	}

	pTracker->AppUpcalls.Free_func( AddinHandle,
									Ptr
#ifdef BioAPI_MEMTRACK_ON
									, szFilename,
									u32LineNumber
#endif
									);
}


void* BioAPI _App_Calloc(

						BioAPI_HANDLE AddinHandle,
						uint32 size,
						uint32 count
#ifdef BioAPI_MEMTRACK_ON
						, const char * szFilename,
						uint32 u32LineNumber
#endif
						)
{
	const MAF_MODULE_ATTACH_TRACKER *pTracker = Addin_GetAttachTracker();

	if ( pTracker == NULL )
	{
		/* Did someone forget to call Addin_SPIBegin? */
		return NULL;
	}

	return pTracker->AppUpcalls.Calloc_func(
									AddinHandle,
									size,
									count
#ifdef BioAPI_MEMTRACK_ON
									, szFilename,
									u32LineNumber
#endif
									);
}


/*************************************************************************
 *************************************************************************
 ***** state tracking definitions and declarations ***********************
 *************************************************************************
 *************************************************************************/

/* Mutex used to protect service provider resource initialization and
 * destruction. This mutex is locked BEFORE checking to see if the SP is
 * initialized (indicated by s_bInitialized). If not, the lock is held
 * throughout the load process. Otherwise, the lock is only held until the
 * the proper level of use protection can be aquired on the SWMR lock
 * s_hUseLock.
 */
static PORT_MUTEX_HANDLE s_hLoadLock = NULL;
#ifdef POSIX
static pthread_mutex_t s_LoadMutex;
#endif

/* Indicates whether library initialization has taken place. */
static BioAPI_BOOL s_bInitialized = BioAPI_FALSE;

/* Macro to abstract initialization indicator */
#define addin_IsInitialized() s_bInitialized

/* Thread local storage index value for keeping track of the operating
 * context of the SPI call (which BioAPI framework called it).
 */
static PORT_TLS_INDEX s_hThreadContext = PORT_INVALID_TLS_INDEX;

/* Single-writer/multiple-reader lock used to enforce initialization order
 * on public interfaces and to insure that the module unload does not take
 * place before all SPI calls are done servicing.
 */
static cssm_SWMRLock s_hUseLock = PORT_SWMR_LOCK_INITIALIZER;

/* List for maintaining a record of the frameworks that have loaded this module.
 */
static MAF_LIST_COLLECTION s_lcLoadList;

/* List for maintaining a record of the attaches that have been made to
 * this module by all frameworks.
 */
static MAF_LIST_COLLECTION s_lcAttachList;

/*************************************************************************
 *************************************************************************
 ***** Begin internal function declarations ******************************
 *************************************************************************
 *************************************************************************/

static int MLC_FASTCALL addin_FindLoadByUUID(
						const MAF_MODULE_LOAD_TRACKER *pLoadTracker, /* void *pItem */
						const BioAPI_UUID *pUuid /* void *pKey */ );
static int MLC_FASTCALL addin_FindAttachByHandle(
						const MAF_MODULE_ATTACH_TRACKER *pAttachTracker, /* void *pItem */
						const BioAPI_HANDLE *pHandle /* void *pKey */ );

/* Validates the identity of a MAF_MODULE_ATTACH_TRACKER structure */
#ifdef _DEBUG /* Debug version */
#define ATTACH_TRACKER_MAGIC_NUMBER( pTracker ) (~((unsigned long)(pTracker)))
static void ATTACH_TRACKER_ASSERT_VALID(
						const MAF_MODULE_ATTACH_TRACKER *pTracker )
{
	assert( !port_IsBadReadPtr( pTracker,
								sizeof(MAF_MODULE_ATTACH_TRACKER) ) );
	assert( pTracker->debug_MagicNumber ==
								ATTACH_TRACKER_MAGIC_NUMBER( pTracker ) );
}
#else /* Release version */
#define ATTACH_TRACKER_MAGIC_NUMBER( pTracker )
#define ATTACH_TRACKER_ASSERT_VALID( pTracker )
#endif

/* Validates the identity of a MAF_MODULE_LOAD_TRACKER structure */
#ifdef _DEBUG /* Debug version */
#define LOAD_TRACKER_MAGIC_NUMBER( pTracker ) ((unsigned long)(pTracker) - 5123)
static void LOAD_TRACKER_ASSERT_VALID(
						const MAF_MODULE_LOAD_TRACKER *pTracker )
{
	assert( !port_IsBadReadPtr( pTracker,
								sizeof(MAF_MODULE_LOAD_TRACKER) ) );
	assert( pTracker->debug_MagicNumber ==
								LOAD_TRACKER_MAGIC_NUMBER( pTracker ) );
}
#else /* Release Version */
#define LOAD_TRACKER_ASSERT_VALID( pTracker )
#define LOAD_TRACKER_MAGIC_NUMBER( pTracker )
#endif

/* Sets the current operating context of the running thread */
#ifdef _DEBUG
void addin_SetCurrentAttach(
						MAF_MODULE_ATTACH_TRACKER *pAttachTracker )
{
	ATTACH_TRACKER_ASSERT_VALID( pAttachTracker );
	port_SetTlsValue( s_hThreadContext, (void *)pAttachTracker );
}
#else
#define addin_SetCurrentAttach( pAttachTracker ) \
	port_SetTlsValue( s_hThreadContext, (void *)pAttachTracker )
#endif

/* Clears the current operating context of the running thread */
#define addin_ClearCurrentAttach() \
	port_SetTlsValue( s_hThreadContext, NULL );

/* Validate that the operating context is well known... none */
#define ASSERT_NO_CURRENT_ATTACH() \
{ \
	void *TempTlsValue = NULL; \
	port_GetTlsValue( s_hThreadContext, &TempTlsValue ); \
	assert( TempTlsValue == NULL ); \
}

/* Abstracts the operation of synchronizing access to the service provider.
 * Must be called when entering and SPI. Returns a CSSM_OBJECT value defined
 * in cssmport.h.
 */
#if (BSP_THREAD_SAFE)
#define BSP_LOCK_FOR_SPI() \
		cssm_SWMRLockWaitToRead( &s_hUseLock, CSSM_INFINITE_WAIT )
#else
#define BSP_LOCK_FOR_SPI() \
		cssm_SWMRLockWaitToWrite( &s_hUseLock, CSSM_INFINITE_WAIT )
#endif

/* Abstracts the operation of synchronizing access to the service provider.
 * Must be called when leaving an SPI. Does not return a value.
 */
#if (BSP_THREAD_SAFE)
#define BSP_UNLOCK_FROM_SPI() \
		cssm_SWMRLockDoneReading( &s_hUseLock )
#else
#define BSP_UNLOCK_FROM_SPI() \
		cssm_SWMRLockDoneWriting( &s_hUseLock )
#endif

/*************************************************************************
 *************************************************************************
 ***** Exported Functions ************************************************
 *************************************************************************
 *************************************************************************/

/* Lock type constants for SPI handlers */
#if (BSP_NEED_LOAD_WRITER_LOCK)
#define _MAF_LOAD_LOCK_TYPE MLC_WRITE_LOCK
#else
#define _MAF_LOAD_LOCK_TYPE MLC_READ_LOCK
#endif

#if (BSP_NEED_ATTACH_WRITER_LOCK)
#define _MAF_ATTACH_LOCK_TYPE MLC_WRITE_LOCK
#else
#define _MAF_ATTACH_LOCK_TYPE MLC_READ_LOCK
#endif

/*-----------------------------------------------------------------------------
 * Name: Addin_SPIBegin
 *
 * Description:
 * Function that is called at the begining of every protected interface, finds
 * the context (which framework are we talking to) and stores information in thread
 * local storage
 *
 * Note: SPIBegin/End operate in a multi-framework environment. It follows these basic steps.
 *
 * 1) Lock the module for reading to prevent the final unload
 *	  from happening while the SPI is being serviced.
 * 2) Find the tracking record for the given handle. This gives
 *	  the proper set of callbacks into the calling framework as
 *	  well as enabling strong validation of all SPI calls.
 *	  A) Get a read lock on the tracking record to prevent a
 *		 detach in another thread from taking place while the
 *		 call is being serviced.
 *	  B) Get a write lock on the tracking record fs the SPI
 *		 needs to information in the record.
 * 3) Get a tracking record for the framework that the addin handle belongs to
 * 4) Verify that the return address belongs to the same module
 *	  that originally attached to the addin. This prevents
 *	  another module from masquerading as the original.
 * 5) Perform the SPI functions.
 * 6) Release the locks on the tracking record and the library.
 *
 * Parameters:
 * AddinHandle (input) : Handle of the addin
 * ReturnAddress (input) : Address of the caller
 *
 * Return Code:
 * NULL if the function failed, otherwise a lock reference that is returned to
 * Addin_SPIEnd
 *-------------------------------------------------------------*/

BioAPI_RETURN Addin_SPIBegin(

						BioAPI_HANDLE AddinHandle,
						MLC_LOCK_REF *pLockRef )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_MODULE_ATTACH_TRACKER * pAttachTracker = NULL;
	MAF_MODULE_LOAD_TRACKER * pLoadTracker = NULL;
	MLC_LOCK_REF LockRef = NULL;


#ifdef DBG_SPIBEGIN
	printf("maf_dllmain.c:  Addin_SPIBegin() called-------\n");
#endif

	if ( !addin_IsInitialized() )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
	}
	else
	{
		ASSERT_NO_CURRENT_ATTACH();

		/* Get a lock on the library */
		if ( BSP_LOCK_FOR_SPI() != BioAPI_OK )
		{
			ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
		}
		else
		{
			/* Find the tracker for this attach */
			if ( MLC_FindItem( &s_lcAttachList,
							   (MLC_FIND_FUNC)addin_FindAttachByHandle,
							   (void*)&AddinHandle,
							   _MAF_ATTACH_LOCK_TYPE,
							   &LockRef,
							   (void**)&pAttachTracker ) != BioAPI_OK )
			{
				/* Doesn't exist. What is the framework thinking? */
				BSP_UNLOCK_FROM_SPI();

				ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE );
			}
			else
			{
				ATTACH_TRACKER_ASSERT_VALID( pAttachTracker );

				/* reaquire the proper lock on the load record */
				if ( MLC_RelockItem( _MAF_LOAD_LOCK_TYPE,
									 pAttachTracker->LoadLockRef,
									(void**)&pLoadTracker ) != BioAPI_OK )
				{
					/* Doesn't exist. What is the framework thinking? */
					MLC_ReleaseItem( _MAF_ATTACH_LOCK_TYPE, LockRef );
					pAttachTracker = NULL;
					BSP_UNLOCK_FROM_SPI();
					ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
				}
				else
				{
					LOAD_TRACKER_ASSERT_VALID( pLoadTracker );

					/* Set the execution context for the thread so that we call the correct
					 * API/SPI functions.
					 */
					addin_SetCurrentAttach( pAttachTracker );
				} /* if pLoadTracker == NULL */
			} /* if pAttachTracker == NULL */
		} /* BSP_LOCK_FOR_SPI != BioAPI_OBJECT_LOCKED */
	} /* if !s_bInitialized */

	/* Set the attach lock reference for later. Clear it if there was and
	 * error.
	 */
	if ( rv == BioAPI_OK )
	{
		*pLockRef = LockRef;
	}
	else
	{
		*pLockRef = NULL;
	}

	return rv;
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
void Addin_SPIEnd( MLC_LOCK_REF LockRef )
{
	/* Done servicing the SPI. Release the library and the tracker. */
	const MAF_MODULE_ATTACH_TRACKER *pTracker = Addin_GetAttachTracker();

	addin_ClearCurrentAttach();
	MLC_ReleaseItem( _MAF_LOAD_LOCK_TYPE, pTracker->LoadLockRef );
	MLC_ReleaseItem( _MAF_ATTACH_LOCK_TYPE, LockRef );

	BSP_UNLOCK_FROM_SPI();
}

/*-----------------------------------------------------------------------------
 * Name: BioAPI_GetAttachContext
 *
 * Description:
 * Returns a reference, locked as specified, to the attach information
 * associated with AttachHandle. Use the MLC_ReleaseItem function to unlock
 * the context when finished. Use the Addin_GetAttachTrackerFromLock or
 * Addin_GetAttachDataFromLock function to access the attach context data.
 *
 * Parameters:
 * AttachHandle (input) : Attach handle used to lookup information.
 * ltLockType (input) : How to lock the context before it is returned.
 * pLockRef (ouput) : Pointer to a void* that will receive the value of a
 *					  lock reference. The reference is to the node containing
 *					  the requested attah information.
 *
 * Return Code:
 * BioAPI_OK : Attach context found
 * BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE : AttachHandle does not contain a valid attach
 *		handle value.
 *---------------------------------------------------------------------------*/
BioAPI_RETURN BioAPI_GetAttachContext( BioAPI_HANDLE AttachHandle,
									MLC_LOCK_TYPE ltLockType,
									MLC_LOCK_REF *pLockRef )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_MODULE_ATTACH_TRACKER * pAttachTracker = NULL;

	/* Find the tracker for this attach */
	if ( MLC_FindItem( &s_lcAttachList,
					   (MLC_FIND_FUNC)addin_FindAttachByHandle,
					   (void*)&AttachHandle,
					   ltLockType,
					   pLockRef,
					   (void**)pAttachTracker ) != BioAPI_OK )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE );
	}
	else
	{
		ATTACH_TRACKER_ASSERT_VALID( pAttachTracker );
	}

	return rv;
}

#if ADDIN_NEED_ADDIN_LOAD_STRUCT
/*-----------------------------------------------------------------------------
 * Name: BioAPI_GetLoadData
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
ADDIN_LOAD_DATA* BioAPI_GetLoadData()
{
	MAF_MODULE_LOAD_TRACKER *pTracker = (MAF_MODULE_LOAD_TRACKER *)BioAPI_GetLoadTracker();
	if ( pTracker == NULL )
	{
		return NULL;
	}
	return &pTracker->AddinLoadData;
}
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
ADDIN_ATTACH_DATA* Addin_GetAttachData()
{
	MAF_MODULE_ATTACH_TRACKER *pTracker = (MAF_MODULE_ATTACH_TRACKER *)Addin_GetAttachTracker();
	if ( pTracker == NULL )
	{
		return NULL;
	}
	return &pTracker->AddinAttachData;
}

/*-----------------------------------------------------------------------------
 * Name: Addin_GetAttachTrackerFromLock
 *
 * Description:
 * Returns the MAF_MODULE_ATTACH_TRACKER structure from a lock reference.
 * Caller must insure that the lock reference is appropriately locked.
 *
 * Parameters:
 * LockRef (input) : Lock reference to query.
 * ppAttachTracker (ouput) : Pointer that will receive the address of the
 *							 attach tracker record.
 *
 * Return Code:
 * BioAPI_OK : Attach tracker returned.
 * BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE : LockRef does not protect an attach tracker record.
 *---------------------------------------------------------------------------*/
BioAPI_RETURN Addin_GetAttachTrackerFromLock(
						MLC_LOCK_REF LockRef,
						const MAF_MODULE_ATTACH_TRACKER **ppAttachTracker )
{
	BioAPI_RETURN rv = BioAPI_OK;
	const MAF_MODULE_ATTACH_TRACKER *pAttachTracker = NULL;

	assert( ppAttachTracker != NULL );

	/* "relock" the reference with a "no lock" to get the item from the lock */
	if ( MLC_RelockItem( MLC_NO_LOCK,
						 LockRef,
						 (void**)pAttachTracker ) != BioAPI_OK )
	{
		*ppAttachTracker = NULL;
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE );
	}
	else
	{
		ATTACH_TRACKER_ASSERT_VALID( pAttachTracker );

		/* Return the item */
		*ppAttachTracker = pAttachTracker;
	}

	return rv;
}

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
 * BioAPI_OK : Attach tracker returned.
 * BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE : LockRef does not protect an attach tracker record.
 *---------------------------------------------------------------------------*/
BioAPI_RETURN Addin_GetAttachDataFromLock(
							MLC_LOCK_REF LockRef,
							ADDIN_ATTACH_DATA **ppAttachData )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_MODULE_ATTACH_TRACKER *pAttachTracker = NULL;

	assert( ppAttachData != NULL );

	/* "relock" the reference with a "no lock" to get the item from the lock */
	if ( MLC_RelockItem( MLC_NO_LOCK,
						 LockRef,
						 (void**)&pAttachTracker ) != BioAPI_OK )
	{
		*ppAttachData = NULL;
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE );
	}
	else
	{
		ATTACH_TRACKER_ASSERT_VALID( pAttachTracker );

		/* Return the item */
		*ppAttachData = &pAttachTracker->AddinAttachData;
	}

	return rv;
}

#endif

/*-----------------------------------------------------------------------------
 * Name: BioAPI_GetLoadTracker
 *
 * Description:
 * Function that returns the MAF_MODULE_LOAD_TRACKER structure for the current
 * thread. Addin must call Addin_SPIBegin before calling this function. The
 * structure returned should not be modified.
 *
 * Parameters:
 * None.
 *
 * Return Code:
 * On success, a pointer to a MAF_MODULE_LOAD_TRACKER, otherwise NULL
 *---------------------------------------------------------------------------*/
const MAF_MODULE_LOAD_TRACKER* BioAPI_GetLoadTracker()
{
	const MAF_MODULE_ATTACH_TRACKER * pAttachTracker = NULL;
	const MAF_MODULE_LOAD_TRACKER * pLoadTracker = NULL;

	port_GetTlsValue( s_hThreadContext, (void**)&pAttachTracker );

	if ( pAttachTracker == NULL )
	{
		return NULL;
	}
	ATTACH_TRACKER_ASSERT_VALID( pAttachTracker );

	MLC_RelockItem( MLC_NO_LOCK,
					pAttachTracker->LoadLockRef,
					(void**)&pLoadTracker );
	LOAD_TRACKER_ASSERT_VALID( pLoadTracker );

	return pLoadTracker;
}


/*-----------------------------------------------------------------------------
 * Name: Addin_GetAttachTracker
 *
 * Description:
 * Function that returns the MAF_MODULE_ATTACH_TRACKER structure for the current
 * thread. Addin must call Addin_SPIBegin before calling this function. The
 * structure returned should not be modified.
 *
 * Parameters:
 * None.
 *
 * Return Code:
 * On success, a pointer to a MAF_MODULE_ATTACH_TRACKER, otherwise NULL
 *---------------------------------------------------------------------------*/
const MAF_MODULE_ATTACH_TRACKER * Addin_GetAttachTracker()
{
	const MAF_MODULE_ATTACH_TRACKER * pAttachTracker;

	port_GetTlsValue( s_hThreadContext, (void**)&pAttachTracker );

	if ( pAttachTracker != NULL )
	{
		ATTACH_TRACKER_ASSERT_VALID( pAttachTracker );
		return pAttachTracker;
	}
	else
	{
		return NULL;
	}
}

/* Internal structure type used by the __BioAPI_SendEvent and
 * BioAPI_SendModuleEvent functions to pass data to the MLC_ForEach iterator.
 */
typedef struct _bsp_send_event
{
	BioAPI_UUID ModuleUuid;
	BioAPI_DEVICE_ID uDeviceId;
	BioAPI_MODULE_EVENT meEvent;
}
__BSP_SEND_EVENT;

/*-----------------------------------------------------------------------------
 * Name: __BioAPI_SendEvent
 *
 * Description:
 *	Helper function used with the MLC_ForEach interator to send the message to
 *	all frameworks in the list.
 *
 * Parameters:
 *	pItem (input) - Void pointer that will be cast to a load tracker record.
 *	pParam (input) - Void pointer that will be cast to an __BSP_SEND_EVENT
 *	 structure.
 *
 * Return value:
 *	non-zero if the function succedes and the ForEach iterator should continue,
 *	zero otherwise.
 *----------------------------------------------------------------------------*/
static int MLC_FASTCALL __BioAPI_SendEvent(
						const MAF_MODULE_LOAD_TRACKER *pTracker, /* void *pItem */
						const __BSP_SEND_EVENT *pEventRec /* void *pParam */ )
{
	if ( !port_IsBadCodePtr( (BioAPI_PROC_ADDR)pTracker->ModuleEventHandler ) )
	{
		pTracker->ModuleEventHandler( &pEventRec->ModuleUuid,
									  (void*)pTracker->ModuleEventHandlerCtx,
									  pEventRec->uDeviceId,
									  0,	/* reserved */
									  pEventRec->meEvent );
	}

	return 1;
}

/*-----------------------------------------------------------------------------
 * Name: BioAPI_SendModuleEvent
 *
 * Description:
 *	Iterates through the load list and sends the specified message to all of
 *	the frameworks being tracked.
 *
 * Parameters:
 *	uDeviceId (input) - Device ID to from which the event originated.
 *	meEvent (input) - Event code to send.
 *
 * Return value:
 *	~(BioAPI_OK), if the message could not be sent
 *	BioAPI_OK, if the message was sent successfully
 *----------------------------------------------------------------------------*/
BioAPI_RETURN BioAPI_SendModuleEvent( BioAPI_DEVICE_ID uDeviceId,
								   BioAPI_MODULE_EVENT meEvent )
{
	BioAPI_RETURN rv = BioAPI_OK;
	__BSP_SEND_EVENT EventStruct;

	/* Fill in the event structure */
	memcpy (EventStruct.ModuleUuid, BSP_UUID, sizeof (BioAPI_UUID));
	EventStruct.uDeviceId = uDeviceId;
	EventStruct.meEvent = meEvent;

	rv = MLC_ForEach( &s_lcLoadList,
					  (MLC_ITEM_ITERATOR_FUNC)__BioAPI_SendEvent,
					  (void*)&EventStruct,
					  MLC_READ_LOCK );

	return rv;
}


/*************************************************************************
 *************************************************************************
 ***** Begin attach tracker functions ************************************
 *************************************************************************
 *************************************************************************/

/*-----------------------------------------------------------------------------
 * Name: addin_AttachTrackerNew
 *
 * Description:
 * Creates a new MAF_MODULE_ATTACH_TRACKER instanace
 *
 * Parameters:
 * FuntionName (input) - Name of function to be imported
 * ppFunction (output) - Pointer to function retrieved
 * hFramework (input) - Handle to framework used to extract functions from
 *
 * Return value:
 * BioAPI_FAIL, if the function pointer could not be retrieved
 * BioAPI_OK, if the funtion pointer was retrieved successfully
 *----------------------------------------------------------------------------*/
static BioAPI_RETURN addin_AttachTrackerNew(
						BioAPI_DEVICE_ID DeviceID,
						BioAPI_HANDLE ModuleHandle,
						const BioAPI_UPCALLS *pAppUpcalls,
						MLC_LOCK_REF LoadLockRef,
						BioAPI_HANDLE hFramework,
						MAF_MODULE_ATTACH_TRACKER **ppNewTracker )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_MODULE_ATTACH_TRACKER *pTracker = NULL;

	assert( ModuleHandle );
	assert( pAppUpcalls );
	assert( LoadLockRef );

	pTracker = (MAF_MODULE_ATTACH_TRACKER*)BioAPI_malloc(
										sizeof(MAF_MODULE_ATTACH_TRACKER),
										NULL );
	if ( pTracker == NULL )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_MEMORY_ERROR );
	}
	else
	{
		/* Wipe the structure */
		memset( pTracker, 0, sizeof(MAF_MODULE_ATTACH_TRACKER) );

#ifdef _DEBUG
		/* Magic Number that is used to verify the integrity of the structure */
		pTracker->debug_MagicNumber = ATTACH_TRACKER_MAGIC_NUMBER( pTracker );
#endif

		/* Parameter values passed by BioAPI during attach */
		pTracker->DeviceID = DeviceID;
		pTracker->ModuleHandle = ModuleHandle;
		pTracker->AppUpcalls = *pAppUpcalls;
		pTracker->LoadLockRef = LoadLockRef;
		pTracker->hFramework = hFramework;

#if ADDIN_NEED_ADDIN_ATTACH_STRUCT
		addin_SetCurrentAttach( pTracker );
		if ( ( rv = Addin_callout_AttachDataConstruct(
								&pTracker->AddinAttachData) ) != BioAPI_OK )
		{
			memset( pTracker, 0, sizeof(MAF_MODULE_ATTACH_TRACKER) );
			BioAPI_free( pTracker, NULL );
			pTracker = NULL;
		}
		addin_ClearCurrentAttach();
#endif
	}

	/* Set the return parameter(s) */
	*ppNewTracker = pTracker;

	return rv;
}

static BioAPI_RETURN addin_AttachTrackerDelete(
						MAF_MODULE_ATTACH_TRACKER *pTracker )
{
	BioAPI_RETURN rv = BioAPI_OK;

	ATTACH_TRACKER_ASSERT_VALID( pTracker );

#if ADDIN_NEED_ADDIN_ATTACH_STRUCT
	addin_SetCurrentAttach( pTracker );
	Addin_callout_AttachDataDestroy( &pTracker->AddinAttachData );
	addin_ClearCurrentAttach();
#endif

	memset( pTracker, 0, sizeof(MAF_MODULE_ATTACH_TRACKER) );
	BioAPI_free( pTracker, NULL );

	return rv;
}

/*************************************************************************
 *************************************************************************
 ***** Begin load tracker functions **************************************
 *************************************************************************
 *************************************************************************/

static BioAPI_RETURN addin_LoadTrackerNew(
						const BioAPI_UUID *pUuidFramework,
						BioSPI_ModuleEventHandler BioAPIModuleEventHandler,
						const void *BioAPIModuleEventHandlerCtx,
						BioAPI_HANDLE hFramework,
						MAF_MODULE_LOAD_TRACKER **ppNewTracker )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_MODULE_LOAD_TRACKER * pTracker;

	assert( pUuidFramework );

	pTracker = (MAF_MODULE_LOAD_TRACKER*)BioAPI_malloc(
										sizeof(MAF_MODULE_LOAD_TRACKER),
										NULL );
	if ( pTracker == NULL )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_MEMORY_ERROR );
	}
	else
	{
		/* Wipe the load tracker structure */
		memset( pTracker, 0, sizeof(MAF_MODULE_LOAD_TRACKER) );

#ifdef _DEBUG
		/* Magic Number that is used to verify the integrity of the structure */
		pTracker->debug_MagicNumber = LOAD_TRACKER_MAGIC_NUMBER(pTracker);
#endif

		memcpy (pTracker->FrameworkUuid, *pUuidFramework, sizeof (BioAPI_UUID));
		pTracker->ModuleEventHandler = BioAPIModuleEventHandler;
		pTracker->ModuleEventHandlerCtx = BioAPIModuleEventHandlerCtx;
		pTracker->NumLoads = 1;
		pTracker->hFramework = hFramework;

#if ADDIN_NEED_ADDIN_LOAD_STRUCT
		ASSERT_NO_CURRENT_ATTACH();
		addin_ClearCurrentAttach();
		if ( ( rv = Addin_callout_LoadDataConstruct(
									(ADDIN_MODULE_HANDLE) hFramework,
									&pTracker->AddinLoadData) ) != BioAPI_OK )
		{
			/* Wipe and deallocate the tracker structure */
			memset( pTracker, 0, sizeof(MAF_MODULE_LOAD_TRACKER) );
			BioAPI_free( pTracker, NULL );
			pTracker = NULL;
		}
#endif
	}

	/* Set the return parameter(s) */
	*ppNewTracker = pTracker;

	return rv;
}

static BioAPI_RETURN addin_LoadTrackerDelete(
						MAF_MODULE_LOAD_TRACKER *pTracker )
{
	LOAD_TRACKER_ASSERT_VALID( pTracker );

#if ADDIN_NEED_ADDIN_LOAD_STRUCT
	ASSERT_NO_CURRENT_ATTACH();
	addin_ClearCurrentAttach();
	Addin_callout_LoadDataDestroy( &pTracker->AddinLoadData );
#endif

	/* Wipe and deallocate the tracker structure */
	memset( pTracker, 0, sizeof(MAF_MODULE_LOAD_TRACKER) );
	BioAPI_free( pTracker, NULL );

	return BioAPI_OK;
}

/*************************************************************************
 *************************************************************************
 ***** Begin list searching functions ************************************
 *************************************************************************
 *************************************************************************/

static int MLC_FASTCALL addin_FindLoadByUUID(
						const MAF_MODULE_LOAD_TRACKER *pLoadTracker, /* void *pItem */
						const BioAPI_UUID *pUuid /* void *pKey */ )
{
	assert( pUuid );
	LOAD_TRACKER_ASSERT_VALID( pLoadTracker );

	return (BioAPI_CompareUuids (&pLoadTracker->FrameworkUuid, pUuid) == BioAPI_FALSE);
}

static int MLC_FASTCALL addin_FindAttachByHandle(
						const MAF_MODULE_ATTACH_TRACKER *pAttachTracker, /* void *pItem */
						const BioAPI_HANDLE *pHandle /* void *pKey */ )
{
	assert( pHandle );
	ATTACH_TRACKER_ASSERT_VALID( pAttachTracker );

	return ( pAttachTracker->ModuleHandle - (*pHandle) );
}

/* Search function that matches everything */
int MLC_FASTCALL addin_FindFirst( void *pItem, void *pKey )
{
	return 0;
}

int MLC_FASTCALL addin_FindSelf( void *pItem, void *pKey )
{
	if ( pItem == pKey )
	{
		return 0;
	}

	return 1;
}


/*************************************************************************
 *************************************************************************
 ***** Begin SPI helper functions ****************************************
 *************************************************************************
 *************************************************************************/

BioAPI_RETURN addin_LockServiceProvider( BioAPI_BOOL bWriter )
{
	BioAPI_RETURN rv = BioAPI_OK;

	if ( bWriter )
	{
		if ( cssm_SWMRLockWaitToWrite( &s_hUseLock,
									   CSSM_INFINITE_WAIT ) != BioAPI_OK )
		{
			ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
		}
	}
	else
	{
		if ( cssm_SWMRLockWaitToRead( &s_hUseLock,
									  CSSM_INFINITE_WAIT ) != BioAPI_OK )
		{
			ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
		}
	}

	return rv;
}

BioAPI_RETURN addin_UnlockServiceProvider( BioAPI_BOOL bWriter )
{
	if ( bWriter )
	{
		cssm_SWMRLockDoneWriting( &s_hUseLock );
	}
	else
	{
		cssm_SWMRLockDoneReading( &s_hUseLock );
	}

	return BioAPI_OK;
}

BioAPI_BOOL addin_IsLastUnload()
{
	BioAPI_BOOL rv = BioAPI_FALSE;
	const MAF_MODULE_LOAD_TRACKER *pTracker = NULL;
	MLC_LOCK_REF LockRef = NULL;

	/* This is the last unload if there is only one load record, and that
	 * record has a reference count of 1.
	 */

	/* Is the list longer than 1? */
	if ( MLC_Size( s_lcLoadList ) > 1 )
	{
		rv = BioAPI_FALSE;
	}
	else
	{
		/* Is there only one reference to that load? */
		if ( MLC_FindItem( &s_lcLoadList,
						   addin_FindFirst,
						   (void*)1, /* Prevent assert */
						   MLC_READ_LOCK,
						   &LockRef,
						   (void**)&pTracker ) != BioAPI_OK )
		{
			assert( 0 ); /* Something bad happened in the call sequence */
			rv = BioAPI_FALSE;
		}
		else
		{
			if ( pTracker->NumLoads == 1 )
			{
				rv = BioAPI_TRUE;
			}

			MLC_ReleaseItem( MLC_READ_LOCK, LockRef );
		}
	}

	return rv;
}


/*************************************************************************
 *************************************************************************
 ***** Begin resource initialization and destruction *********************
 *************************************************************************
 *************************************************************************/

static void MLC_FASTCALL addin_TeardownLoad(
						MAF_MODULE_LOAD_TRACKER *pTracker /* void *pItem */ )
{
	addin_LoadTrackerDelete( pTracker );
}

static void MLC_FASTCALL addin_TeardownAttach(
						MAF_MODULE_ATTACH_TRACKER *pTracker /* void *pItem */ )
{
	addin_AttachTrackerDelete( pTracker );
}

static BioAPI_RETURN addin_InitResources()
{
	BioAPI_RETURN rv = BioAPI_OK;

	/* Catch improper locking situations */
	assert( !addin_IsInitialized() );

	/* Initialize the thread-local storage index for storing the
	 * current operating context.
	 */
	if ( ( rv = port_AllocateTlsIndex( &s_hThreadContext,
									   NULL ) ) != BioAPI_OK )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
	}
	else
	{
		/* Initialize the load tracker list */
		if ( MLC_Init( &s_lcLoadList,
					   (MLC_ITEM_TEARDOWN_FUNC)addin_TeardownLoad ) != BioAPI_OK )
		{
			ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
		}
		else
		{
			/* Initialize the attach tracker list */
			if ( MLC_Init( &s_lcAttachList,
						   (MLC_ITEM_TEARDOWN_FUNC)addin_TeardownAttach ) != BioAPI_OK )
			{
				ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
			}
			else
			{
				/* Call the SP specific initialization */
				if ( ( rv = Addin_callout_Initialize() ) == BioAPI_OK )
				{
					/* Everything is initialized, set the indicator */
					s_bInitialized = BioAPI_TRUE;
				}
				else
				{
					/* Custom init failed, terminate the attach list */
					MLC_Term( &s_lcAttachList );
				}
			}

			/* If something failed, terminate the load list */
			if ( rv != BioAPI_OK )
			{
				MLC_Term( &s_lcLoadList );
			}
		}

		/* If something failed, terminate the thread context index */
		if ( rv != BioAPI_OK )
		{
			port_DeleteTlsIndex( s_hThreadContext );
			s_hThreadContext = PORT_INVALID_TLS_INDEX;
		}
	}

	return rv;
}

static BioAPI_RETURN addin_TermResources()
{
	BioAPI_RETURN rv = BioAPI_OK;

	/* Perform custom termination */
	Addin_callout_Terminate();

	/* Reset the initialization indicator */
	s_bInitialized = BioAPI_FALSE;

	/* Terminate the attach tracker list */
	MLC_Term( &s_lcAttachList );

	/* Terminate the load tracker list */
	MLC_Term( &s_lcLoadList );

	/* Terminate the thread context index */
	port_DeleteTlsIndex( s_hThreadContext );

	return rv;
}

/*************************************************************************
 *************************************************************************
 ***** Begin main load/unload functionality ******************************
 *************************************************************************
 *************************************************************************/

static BioAPI_RETURN addin_PerformModuleLoad(
						const BioAPI_UUID *FrameworkUuid,
						const BioAPI_UUID *ModuleUuid,
						BioSPI_ModuleEventHandler BioAPINotifyCallback,
						const void* BioAPINotifyCallbackCtx,
						const BioAPI_HANDLE hFramework )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker = NULL;
	MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker2 = NULL;
	MLC_LOCK_REF LockRef = MLC_NULL_LOCK_REF;

	/* Make sure that we have called ourselves correctly */
	assert( FrameworkUuid && ModuleUuid && BioAPINotifyCallback );

	/* Find this load tracker if it already exists and lock it for
	 * writing.
	 */
	if ( ( rv = MLC_FindItem( &s_lcLoadList,
							  (MLC_FIND_FUNC)addin_FindLoadByUUID,
							  (void*)FrameworkUuid,
							  MLC_WRITE_LOCK,
							  &LockRef,
							  (void**)&pLoadTracker ) ) == BioAPI_OK )
	{
		LOAD_TRACKER_ASSERT_VALID( pLoadTracker );

		/* This module has been loaded by this framewoirk before */
		pLoadTracker->NumLoads++;
	}
	else if ( rv == MLC_ERR_NOT_FOUND )
	{
		/* First time being loaded by this framework. Insert a tracking record
		 * into the internal list.
		 */
		if ( ( rv = addin_LoadTrackerNew( FrameworkUuid,
										  BioAPINotifyCallback,
										  BioAPINotifyCallbackCtx,
										  hFramework,
										  &pLoadTracker ) ) == BioAPI_OK )
		{
			LOAD_TRACKER_ASSERT_VALID( pLoadTracker );

			/* Add the new item to the tracking list */
			if ( MLC_AddItem( &s_lcLoadList,
							  (void *)pLoadTracker,
							  MLC_WRITE_LOCK,
							  &LockRef ) != BioAPI_OK )
			{
				/* Couldn't add the item to the list */
				ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
				addin_LoadTrackerDelete( pLoadTracker );
			}
		} /* if addin_LoadTrackerNew succeeded */
	}
	else
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
	}

	/* If rv==BioAPI_OK at this point, then pLoadTracker is a valid item that
	 * has been inserted into the load list, and there is a write lock on
	 * the record. The lock reference for the record is in LockRef.
	 */

	/* Are we successful so far? */
	if ( rv == BioAPI_OK )
	{
		ASSERT_NO_CURRENT_ATTACH();
		addin_ClearCurrentAttach();
		rv = Addin_callout_ModuleLoad( pLoadTracker,
									   BioAPINotifyCallback,
									   BioAPINotifyCallbackCtx );

		/* Delete the new item if initialization failed */
		if ( rv != BioAPI_OK )
		{
			/* Remove the load record if this was the first load from this framework */
			if ( pLoadTracker->NumLoads == 1 )
			{
				/* Drop the writer lock on the new item */
				MLC_ReleaseItem( MLC_WRITE_LOCK, LockRef );

				/* Remove the new item from the list */
				MLC_DeleteItem( &s_lcLoadList,
								addin_FindSelf,
								(void*)pLoadTracker,
								(void**)pLoadTracker2 );
				assert( pLoadTracker == pLoadTracker2 );
				addin_LoadTrackerDelete( pLoadTracker );
				pLoadTracker = NULL;
			}
			else
			{
				/* Decrement the load count to the previous value */
				pLoadTracker->NumLoads--;

				/* Drop the writer lock on the new item */
				MLC_ReleaseItem( MLC_WRITE_LOCK, LockRef );
			}
		}
		else
		{
			MLC_ReleaseItem( MLC_WRITE_LOCK, LockRef );
		}
	}

	return rv;
}

static BioAPI_RETURN addin_PerformModuleUnload(
						const BioAPI_UUID *FrameworkUuid,
						const BioAPI_UUID *ModuleUuid,
						BioSPI_ModuleEventHandler BioAPINotifyCallback,
						const void* BioAPINotifyCallbackCtx )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker = NULL;
	void *pLockRef = NULL;
	BioAPI_BOOL bLastUsage = BioAPI_FALSE;

	/* Find the tracker for this load */
	if ( MLC_FindItem( &s_lcLoadList,
					   (MLC_FIND_FUNC)addin_FindLoadByUUID,
					   (void*)FrameworkUuid,
					   MLC_WRITE_LOCK,
					   &pLockRef,
					   (void**)&pLoadTracker ) != BioAPI_OK )
	{
		ERR( rv = BioAPI_ERRCODE_FUNCTION_FAILED );
	}
	else
	{
		LOAD_TRACKER_ASSERT_VALID( pLoadTracker );

		/* Decrement the load count and check for a final unload situation */
		assert( pLoadTracker->NumLoads != 0 );
		pLoadTracker->NumLoads--;
		if ( pLoadTracker->NumLoads == 0 )
		{
			bLastUsage = BioAPI_TRUE;
		}

		ASSERT_NO_CURRENT_ATTACH();
		addin_ClearCurrentAttach();
		rv = Addin_callout_ModuleUnload( pLoadTracker,
										 BioAPINotifyCallback,
										 BioAPINotifyCallbackCtx );

		/* Drop the writer lock on the tracker record */
		MLC_ReleaseItem( MLC_WRITE_LOCK, pLockRef );
		pLoadTracker = NULL;

		/* If the record is no longer needed, delete it */
		if ( bLastUsage )
		{
			/* Delete the list linkage */
			if ( MLC_DeleteItem( &s_lcLoadList,
								 (MLC_FIND_FUNC)addin_FindLoadByUUID,
								 (void*)FrameworkUuid,
								 (void**)&pLoadTracker ) != BioAPI_OK )
			{
				/* We just had this record, something went wrong */
				ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
			}
			else
			{
				LOAD_TRACKER_ASSERT_VALID( pLoadTracker );

				/* Free the memory for the record */
				addin_LoadTrackerDelete( pLoadTracker );
			}
		}
	}

	return rv;
}

/*************************************************************************
 *************************************************************************
 ***** Begin standard SPI interface **************************************
 *************************************************************************
 *************************************************************************/

BioAPI_RETURN BioAPI BioSPI_ModuleLoad(
	const BioAPI_UUID *FrameworkUuid,
	const BioAPI_UUID *ModuleUuid,
	BioSPI_ModuleEventHandler BioAPINotifyCallback,
	void* BioAPINotifyCallbackCtx )
{
	BioAPI_RETURN rv = BioAPI_OK;
	BioAPI_BOOL bFirstLoad = BioAPI_FALSE;
	BioAPI_HANDLE hFramework = BioAPI_INVALID_HANDLE;


#ifdef SOLARIS
	/*        _init();*/
#endif

	/* Are the parameters valid pointers? */
	if ( port_IsBadReadPtr( FrameworkUuid, sizeof(BioAPI_UUID) ) ||
		 port_IsBadReadPtr( ModuleUuid, sizeof(BioAPI_UUID) ) ||
		 port_IsBadCodePtr( (BioAPI_PROC_ADDR)BioAPINotifyCallback ) )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_POINTER );
	}
	/* Is this the correct module being requested? */
	else if (BioAPI_CompareUuids ( ModuleUuid, &BSP_UUID) == BioAPI_FALSE )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_UUID );
	}
	/* Lock the load/unload process so we can determine our current state */
	else if ( port_LockMutex( s_hLoadLock, CSSM_INFINITE_WAIT ) != CSSM_OK )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
	}
	else
	{
		/* Determine whether or not resources have been initialized */
		if ( !addin_IsInitialized() )
		{
			bFirstLoad = BioAPI_TRUE;
			rv = addin_InitResources();
		}
		if ( rv == BioAPI_OK )
		{
			/* Make sure something hasn't gone wrong. If this assert triggers, then
			 * we have probably detected a deadlock situation because of a
			 * circular reference among modules.
			 */
			ASSERT_NO_CURRENT_ATTACH();

			/* Obtain the appropriate lock on the library. */
			if ( ( rv = addin_LockServiceProvider( bFirstLoad ) ) == BioAPI_OK )
			{
				/* Unlock the resource init/destruct lock if this isn't the
				 * first load. Allow additional load/unload operations to
				 * progress in parallel.
				 */
				if ( !bFirstLoad )
				{
					port_UnlockMutex( s_hLoadLock );
				}

				/* Lock states:
				 * if ( bFirstLoad )
				 * {
				 *	  This is the first time an SPI has been called in the
				 *	  SP. s_hLoadLock (mutex) is locked to protect any initial
				 *	  resource creation and initialization. s_hUseLock (SWMR)
				 *	  is locked for writing (via addin_LockServiceProvider).
				 *
				 *	  No SPI functions are executing, or are able to begin
				 *	  executing. Resource creation may proceed without
				 *	  worrying about multiple threads.
				 * }
				 * else
				 * {
				 *	  s_hLoadLock is unlocked to allow other load/unload
				 *	  operations to run in parallel. s_hUseLock is locked for
				 *	  reading (via addin_LockServiceProvider). SPI functions
				 *	  may be executing or invoked in parallel. All resources
				 *	  are guaranteed to exist until all existing SPIs have
				 *	  completed (including this one).
				 * }
				 */

				/* Perform SP defined behavior */

				rv = addin_PerformModuleLoad( FrameworkUuid,
											  ModuleUuid,
											  BioAPINotifyCallback,
											  BioAPINotifyCallbackCtx,
											  hFramework );

				/* Drop the service provider lock on the internals */
				addin_UnlockServiceProvider( bFirstLoad );
			}

			/* Drop the load/unload lock */
			if ( bFirstLoad )
			{
				port_UnlockMutex( s_hLoadLock );

				/* Destroy resources if the module load failed since this
				 * is the first load.
				 */
				if ( rv != BioAPI_OK )
				{
					addin_TermResources();
				}
			}
		} /* all resources initialized and valid */
		else
		{
			/* SP resource creation failed. so we can't go any further,
			 * but we still hold the library lock.
			 */
			port_UnlockMutex( s_hLoadLock );
		}
	} /* parameters are valid and lib is locked for load */

	return rv;
}

BioAPI_RETURN BioAPI BioSPI_ModuleUnload (
						const BioAPI_UUID * FrameworkUuid,
						const BioAPI_UUID * ModuleUuid,
						BioSPI_ModuleEventHandler BioAPINotifyCallback,
						void* BioAPINotifyCallbackCtx )
{
	BioAPI_RETURN rv = BioAPI_OK;
	BioAPI_BOOL bLastLoad = BioAPI_FALSE;

	/* Check pointer validity */
	if ( port_IsBadReadPtr( FrameworkUuid, sizeof(BioAPI_UUID) ) ||
		 port_IsBadReadPtr( ModuleUuid, sizeof(BioAPI_UUID)) )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_POINTER );
	}
	/* Is this the correct module being requested? */
	else if ( BioAPI_CompareUuids (ModuleUuid, &BSP_UUID) == BioAPI_FALSE )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_UUID );
	}
	/* Get a lock on the load/unload */
	else if ( port_LockMutex( s_hLoadLock, CSSM_INFINITE_WAIT ) != CSSM_OK )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
	}
	else if ( !addin_IsInitialized() )
	{
		/* Unlock the load/unload mutex */
		port_UnlockMutex( s_hLoadLock );
		ERR( rv = BioAPIERR_H_FRAMEWORK_NOT_INITIALIZED );
	}
	else
	{
		/* Make sure something hasn't gone wrong. If this assert triggers, then
		 * we have probably detected a deadlock situation because of a
		 * circular reference among modules.
		 */
		ASSERT_NO_CURRENT_ATTACH();

		addin_ClearCurrentAttach();

		/* Determine if this is the last load */
		bLastLoad = addin_IsLastUnload();

		/* Get a reader lock on the module. */
		if ( addin_LockServiceProvider( bLastLoad ) != BioAPI_OK )
		{
			ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
		}
		else
		{
			/* Unload the load/unload mutex if this is not the last unload */
			if ( !bLastLoad )
			{
				port_UnlockMutex( s_hLoadLock );
			}

			/* Lock states:
			 * if ( bLastLoad )
			 * {
			 *	  This is the last time an SPI will be called in the
			 *	  SP. s_hLoadLock (mutex) is locked to protect any final
			 *	  resource destruction. s_hUseLock (SWMR) is locked for
			 *	  writing (via addin_LockServiceProvider).
			 *
			 *	  No SPI functions are executing, or are able to begin
			 *	  executing. Resource destruction may proceed without
			 *	  worrying about multiple threads.
			 * }
			 * else
			 * {
			 *	  s_hLoadLock is unlocked to allow other load/unload
			 *	  operations to run in parallel. s_hUseLock is locked for
			 *	  reading (via addin_LockServiceProvider). SPI functions
			 *	  may be executing or invoked in parallel. All resources
			 *	  are guaranteed to exist until all existing SPIs have
			 *	  completed (including this one).
			 * }
			 */

			/* Perform SP defined behavior */
			rv = addin_PerformModuleUnload( FrameworkUuid,
											ModuleUuid,
											BioAPINotifyCallback,
											BioAPINotifyCallbackCtx );

			/* If this is the last unload operation, prepare the SP to
			 * be unloaded by destroying all of the resources.
			 */
			if ( bLastLoad )
			{
				addin_TermResources();
			}

			/* Drop the service provider lock */
			addin_UnlockServiceProvider( bLastLoad );
		}

		/* Unlock the load/unload mutex if this is the last unload */
		if ( bLastLoad )
		{
			port_UnlockMutex( s_hLoadLock );
		}
	}

#ifdef SOLARIS
	/*	_fini();*/
#endif
	return rv;
}

BioAPI_RETURN BioAPI BioSPI_ModuleAttach(
						const BioAPI_UUID *ModuleUuid,
						const BioAPI_VERSION *Version,
						BioAPI_DEVICE_ID DeviceId,
						uint32 Reserved1,
						uint32 Reserved2,
						BioAPI_HANDLE ModuleHandle,
						uint32 Reserved3,
						const void *Reserved4,
						const void *Reserved5,
						const void *Reserved6,
						const BioAPI_UPCALLS *Upcalls,
						BioAPI_MODULE_FUNCS_PTR *FuncTbl )

{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_MODULE_ATTACH_TRACKER_PTR pAttachTracker = NULL;
	MLC_LOCK_REF AttachLockRef = MLC_NULL_LOCK_REF;
	MAF_MODULE_LOAD_TRACKER_PTR pLoadTracker = NULL;
	MLC_LOCK_REF LoadLockRef = MLC_NULL_LOCK_REF;
	BioAPI_HANDLE hFramework = BioAPI_INVALID_HANDLE;


	/* Make sure the system is initialized */
	if ( !addin_IsInitialized() )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_NOT_INITIALIZED );
	}
	/* Perform basic parameter pointer checks */
	else if ( port_IsBadReadPtr( ModuleUuid, sizeof(BioAPI_UUID) ) ||
			  port_IsBadReadPtr( Version, sizeof(BioAPI_VERSION) ) ||
			  port_IsBadReadPtr( Upcalls, sizeof(BioAPI_UPCALLS) ) ||
			  port_IsBadWritePtr( FuncTbl, sizeof(BioAPI_MODULE_FUNCS_PTR) ) )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_POINTER );
	}
	/* Check for a valid module handle */
	else if ( ModuleHandle == BioAPI_INVALID_HANDLE )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE );
	}
	/* Check the upcall table pointers */
	else if ( port_IsBadCodePtr( (BioAPI_PROC_ADDR)Upcalls->Malloc_func ) ||
			  port_IsBadCodePtr( (BioAPI_PROC_ADDR)Upcalls->Free_func ) ||
			  port_IsBadCodePtr( (BioAPI_PROC_ADDR)Upcalls->Realloc_func ) ||
			  port_IsBadCodePtr( (BioAPI_PROC_ADDR)Upcalls->Calloc_func ) )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_POINTER );
	}
	/* Is this the correct module being requested? */
	else if ( memcmp( ModuleUuid, &BSP_UUID, sizeof(BioAPI_UUID) ) )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_UUID );
	}
	/* Check that this is a compatible version */
	else if ( ( Version->Major != BioAPI_MAJOR ) ||
			  ( Version->Minor != BioAPI_MINOR ) )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INCOMPATIBLE_VERSION );
	}
	/* Check to see that the device is supported */
	else if ( !BSP_IS_VALID_DEVICE(DeviceId) )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_DEVICE_ID );
	}
	else
	{
		/* Make sure something hasn't gone wrong. If this assert triggers, then
		 * we have probably detected a deadlock situation because of a
		 * circular reference among modules.
		 */
		ASSERT_NO_CURRENT_ATTACH();
		addin_ClearCurrentAttach();

		/* Get a reader lock on the library */
		if ( addin_LockServiceProvider( BioAPI_FALSE ) != BioAPI_OK )
		{
			ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
		}
		else
		{
			/* Make sure the framework has called load */
			if ( MLC_FindItem( &s_lcLoadList,
							   (MLC_FIND_FUNC)addin_FindLoadByUUID,
							   (void*)ref_hlayer_uuid,
							   MLC_READ_LOCK,
							   &LoadLockRef,
							   (void**)&pLoadTracker ) != CSSM_OK )
			{
				/* This framework has never called load */
				ERR( rv = BioAPI_ERRCODE_FUNCTION_FAILED );
			}
			else
			{
				/* Allocate a new attach tracker record */
				if ( ( rv = addin_AttachTrackerNew(
										DeviceId,
										ModuleHandle,
										Upcalls,
										LoadLockRef,
										hFramework,
										&pAttachTracker ) ) == BioAPI_OK )
				{
					/* Call custom functionality */
					addin_SetCurrentAttach( pAttachTracker );
					rv = Addin_callout_ModuleAttach(
										pLoadTracker,
										pAttachTracker,
										FuncTbl );
					addin_ClearCurrentAttach();
					if ( rv == BioAPI_OK )
					{
						/* Add the new record to the tracker list */
						if ( ( rv = MLC_AddItem( &s_lcAttachList,
										  (void *)pAttachTracker,
										  MLC_NO_LOCK,
										  &AttachLockRef ) ) != BioAPI_OK )
						{
							/* Couldn't add the item to the list */
							ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
						}
					} /* Custom attach stuff done */

					/* Clean-up the attach tracker in the event of an error */
					if ( rv != BioAPI_OK )
					{
						addin_AttachTrackerDelete( pAttachTracker );
					}

					/* Indicate that we are done with the attach tracker */
					pAttachTracker = NULL;
				} /* New tracker created */
			} /* Load record found */

			MLC_ReleaseItem( MLC_READ_LOCK, LoadLockRef );
			pLoadTracker = NULL;

			/* Drop the reader lock to signal that we are finished */
			addin_UnlockServiceProvider( BioAPI_FALSE );
		} /* Service provider locked for attach */
	} /* Parameters are valid */

	return rv;
}

/*---------------------------------------------------------------
 *
 *Name: Addin_SPI_ModuleDetach
 *
 *Description:
 *	 Public entry point used by frameworks to detach a single attach.
 *	 Default behavior is to find the attach tracking record for
 *	 the attach and verify that the same module that made the
 *	 original attach is making the detach call. It then tears
 *	 down the tracking record and frees the resources.
 *
 *Parameters:
 *	 Handle (input) - Framework specified handle representing the
 *		 attach session.
 *
 *Returns:
 *	 BioAPI_FAIL - Internal error, unknown handle, or bad caller.
 *	 BioAPI_OK - The detach succeded.
 *
 *-------------------------------------------------------------*/
BioAPI_RETURN BioAPI BioSPI_ModuleDetach(BioAPI_HANDLE Handle)
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_MODULE_ATTACH_TRACKER_PTR pAttachTracker = NULL;
	MLC_LOCK_REF LockRef = NULL;

	/* Check to see that the SP has been initialized */
	if ( !addin_IsInitialized() )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_NOT_INITIALIZED );
	}
	/* Check for a presumably valid handle */
	else if ( Handle == BioAPI_INVALID_HANDLE )
	{
		ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE );
	}
	else
	{
		ASSERT_NO_CURRENT_ATTACH();
		addin_ClearCurrentAttach();

		/* Get a reader lock on the library */
		if ( addin_LockServiceProvider( BioAPI_FALSE ) != BioAPI_OK )
		{
			ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
		}
		else
		{
			/* Find the tracker for this attach */
			if ( MLC_FindItem( &s_lcAttachList,
							   (MLC_FIND_FUNC)addin_FindAttachByHandle,
							   (void*)&Handle,
							   MLC_READ_LOCK,
							   &LockRef,
							   (void**)&pAttachTracker ) != BioAPI_OK )
			{
				/* Doesn't exist. What is the framework thinking? */
				ERR( rv = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE );
			}
			else
			{
				ATTACH_TRACKER_ASSERT_VALID(pAttachTracker);

				/* Release the read lock so we can delete the record */
				MLC_ReleaseItem( MLC_READ_LOCK, LockRef );
				pAttachTracker = NULL;

				/* Unlink the tracker for this attach */
				if ( MLC_DeleteItem( &s_lcAttachList,
									 (MLC_FIND_FUNC)addin_FindAttachByHandle,
									 (void*)&Handle,
									 (void**)&pAttachTracker ) != BioAPI_OK )
				{
					/* Something went wrong here */
					ERR( rv = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR );
				}
				else
				{
					/* Teardown the attach record */
					ATTACH_TRACKER_ASSERT_VALID( pAttachTracker );
					addin_AttachTrackerDelete( pAttachTracker );
				} /* Item was removed from the list */
			} /* Attach record found */

			/* Drop the reader lock to signal that we are finished */
			addin_UnlockServiceProvider( BioAPI_FALSE );
		} /* Service provider locked */
	} /* Parameter validation passed */

	return rv;
}

#if defined (LINUX) || defined (SOLARIS)
int init() __attribute__ ((constructor));
int fini() __attribute__ ((destructor));
#endif

int init() {

  CSSM_BOOL rv = CSSM_TRUE;

  /* Initialize a tracing functionality */
  if ( port_InitMemoryTracking() == CSSM_OK ) {
  /* Initialize the library load and initialization mutex */
#ifdef POSIX
	s_hLoadLock = &s_LoadMutex;
#endif
  	if ( port_CreateMutex( NULL, &s_hLoadLock ) == CSSM_OK ) {
	 	  if ( cssm_SWMRLockCreate( &s_hUseLock, NULL ) != CSSM_OK ) {
	      port_CloseMutex( s_hLoadLock );
	      rv = CSSM_FALSE;
	 	  } else {
	      /* 
	       * Indicate the the service provider resources have not
	       * been allocated.
	       */
	      s_bInitialized = CSSM_FALSE;
	    }
		}
	  else {
	    rv = CSSM_FALSE;
	  }
  } else {
    rv = CSSM_FALSE;
  }

  return rv;
}

int fini()
{
    /* Dump heap leaks when we are unloaded */
    port_DumpMemoryLeaks( BSP_DESCRIPTION );

    /*
     * Here's why we moved the deletion of s_hUseLock from
     * addin_TermResources() to _fini():  CSSM_SPI_ModuleUnload()
     * needs to lock s_hUseLock while calling addin_TermResources(),
     * but s_hUseLock can't always be deleted while it's locked.
     * It can on WIN32, but it can't on UNIX where it's implemented
     * with POSIX rw locks.
     *
     * Since we moved the deletion, we also moved the creation from
     * addin_InitResources() to _init().
     */
    if ( cssm_SWMRLockDelete( &s_hUseLock ) != CSSM_OK )
    {
		assert( 0 );
    }

    /* Destroy the load mutex */
    if ( port_CloseMutex( s_hLoadLock ) != CSSM_OK )
    {
		assert( 0 );
    }
	
	return CSSM_OK;
}


#if 0
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
 *	 that only a single thread can be running this code. The framework
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
 *	 FALSE - Initialization failed. Loading fails.
 *	 TRUE - DLL is initialized for use.
 *
 *-------------------------------------------------------------*/
int _init()
{
	/*
	 * Open the MDS Object and CDSA directory.
	 */
	if (maf_open_mds_directory (&hDLDBBioAPI, &pMDSFunctions) != BioAPI_OK)
		return FALSE;

	if ( BioAPI_SelfCheck() != BioAPI_OK )
	{
		maf_close_mds_directory (hDLDBBioAPI);
		return FALSE;
	}

	/* Initialize the thread local storage for the module */
	s_hThreadContext = TLS_NewIndex();
	if ( s_hThreadContext == TLS_BAD_INDEX )
	{
		maf_close_mds_directory (hDLDBBioAPI);
		return FALSE;
	}

	/* Create the SWMR lock to gaurd the library resources */
	if ( cssm_SWMRLockCreate( &s_swmrInit, NULL, NULL, NULL )
			!= CSSM_TRUE )
	{
		maf_close_mds_directory (hDLDBBioAPI);
		TLS_DeleteIndex( s_hThreadContext );
		return FALSE;
	}

	/* Create the load/unload lock */
	if ( ( s_hLoadLock = cssm_CreateMutex( NULL ) ) == NULL )
	{
		cssm_SWMRLockDelete( &s_swmrInit );
		maf_close_mds_directory (hDLDBBioAPI);
		TLS_DeleteIndex( s_hThreadContext );
		return FALSE;
	}

	/* Initialize the locking on the load and attach tracking lists */
	if ( !CLC_Init( &s_lcLoadList, NULL ) )
	{
		cssm_SWMRLockDelete( &s_swmrInit );
		maf_close_mds_directory (hDLDBBioAPI);
		cssm_SWMRLockDelete( &s_swmrInit );
		TLS_DeleteIndex( s_hThreadContext );
		return FALSE;
	}
	else
	{
		if ( !CLC_Init( &s_lcAttachList, NULL ) )
		{
			cssm_SWMRLockDelete( &s_swmrInit );
			maf_close_mds_directory (hDLDBBioAPI);
			CLC_Term( &s_lcLoadList );
			cssm_SWMRLockDelete( &s_swmrInit );
			TLS_DeleteIndex( s_hThreadContext );
			return FALSE;
		}
	}

	ASSERT_NO_CURRENT_ATTACH();
	if (BioAPI_OK != Addin_callout_Initialize())
	{
		cssm_SWMRLockDelete( &s_swmrInit );
		maf_close_mds_directory (hDLDBBioAPI);
		CLC_Term( &s_lcAttachList );
		CLC_Term( &s_lcLoadList );
		cssm_SWMRLockDelete( &s_swmrInit );
		TLS_DeleteIndex( s_hThreadContext );
		return FALSE;
	}

	s_fInitialized = BioAPI_TRUE;
	return TRUE;
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
 *	 single thread can be running this code. The framework maintains
 *	 an exclusive lock on its internal tracking structure for
 *	 this library when this function is called.
 *
 *Parameters:
 *	 None.
 *
 *Returns:
 *	 FALSE - whoops.
 *	 TRUE - DLL is uninitialized for unloading
 *
 *-------------------------------------------------------------*/
int _fini()
{
	int retCode = TRUE;

	s_fInitialized = BioAPI_FALSE;

	if (BioAPI_OK != Addin_callout_Terminate())
		retCode = FALSE;

	ASSERT_NO_CURRENT_ATTACH();
	addin_ClearCurrentAttach();

	/* Destroy the SWMR lock gaurding the library resources */
	if (!CLC_CleanList(&s_lcAttachList, (CSSM_COLLECTION_ITEM_TEARDOWN_FUNC)addin_AttachTrackerDelete))
		retCode = FALSE;

	if (!CLC_CleanList(&s_lcLoadList, (CSSM_COLLECTION_ITEM_TEARDOWN_FUNC)addin_LoadTrackerDelete))
		retCode = FALSE;

	CLC_Term( &s_lcAttachList );
	CLC_Term( &s_lcLoadList );
	cssm_SWMRLockDelete( &s_swmrInit );
	cssm_CloseMutex( s_hLoadLock );

	/* Deallocate the thread local storage index */
	TLS_DeleteIndex( s_hThreadContext );

	/*
	 * Close the mds directory.
	 */
	maf_close_mds_directory (hDLDBBioAPI);

	/* Always succedes */
	return retCode;
}


#endif


#ifdef WIN32

/**---------------------------------------------------------------
 ** DllMain
 **
 ** Description:
 **	 Entry point for the DLL in Win32. This function is responsible for
 **	 initializing and destroying the s_LoadLock mutex and insuring that the
 **	 s_bInitialized variable is set to the known state BioAPI_FALSE. Memory
 **	 heap tracking is also initialized and terminated.
 **
 ** Parameters:
 **	 hInstance (input) - Instance handle of calling process
 **	 dwReason (input) - Reasons for receiving this call
 **	 lpReserved (input) - Reserved field
 **
 ** Returns:
 **	 FALSE - unable to initialize DLL.	This will cause the LoadLibrary
 **	   call to fail.
 **	 TRUE - DLL is initialize and ready to be called
 **------------------------------------------------------------------------
 **/
BOOL WINAPI DllMain(	HANDLE hInstance,
						DWORD dwReason,
						LPVOID lpReserved )
{
	BOOL rv = TRUE;

	switch ( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		{
			rv = _init();
			break;
		} /* DLL_PROCESS_ATTACH */
	case DLL_THREAD_ATTACH:
		{
			break;
		} /* DLL_THREAD_ATTACH */

	case DLL_THREAD_DETACH:
		{
			break;
		} /* DLL_THREAD_DETACH */
	case DLL_PROCESS_DETACH:
		{
			_fini();
			break;
		} /* DLL_PROCESS_DETACH */
	}

	return rv;
}

#endif /* ifdef WIN32 */
