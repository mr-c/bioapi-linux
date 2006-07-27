/*-----------------------------------------------------------------------
 * File: MAF_UTIL.H
 *-----------------------------------------------------------------------
 */


#ifndef MAF_UTIL_H
#define MAF_UTIL_H

#ifndef _BIOAPITYPE_H_
#error bioapi_type.h must be included before this file
#endif
#ifndef __COLLECTN_H__
#error maf_collectn.h must be included before this file
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Error code base value for all MAF libraries that define internal errors
 * that are not reflected up through the module interface and must be
 * trapped. This number is really big to avoid conflicts with spec defined
 * errors in the forseable future.
 */
#define MAF_INTERNAL_ERROR_BASE		( 0x80000000 )
#define ASSERT_RETURNABLE_ERROR( error ) \
							{ assert( error < MAF_INTERNAL_ERROR_BASE ); }

/* Memory function table(s) passed to the CSSM_ModuleAttach API when
 * connecting to other modules, or passed to callback functions that
 * allocate memory.
 */
extern const BioAPI_MEMORY_FUNCS Addin_APIMemFuncs;

/* Constant with the value of the SP UUID.
 */
extern const BioAPI_UUID BSP_UUID;


/* The Application's upcall functions (replaces the CSSM_UPCALLS calls).
 * These functions should not be called directly. The shorter macros
 * with the same name (minus the leading underscore) should be used to
 * support memory tracking. All memory is allocated using the applications
 * heap using through the CSSM.
 */
void* BioAPI _App_Malloc(
						CSSM_MODULE_HANDLE AddinHandle,
						uint32 size
#ifdef BioAPI_MEMTRACK_ON
						, const char *szFilename,
						uint32 u32LineNumber
#endif
						);

void* BioAPI _App_Realloc(
						CSSM_MODULE_HANDLE AddinHandle,
						void *Ptr,
						uint32 size
#ifdef BioAPI_MEMTRACK_ON
						, const char *szFilename,
						uint32 u32LineNumber
#endif
						);

void BioAPI _App_Free( CSSM_MODULE_HANDLE AddinHandle,
						void *Ptr
#ifdef BioAPI_MEMTRACK_ON
						, const char *szFilename,
						uint32 u32LineNumber
#endif
						);

void* BioAPI _App_Calloc(
						CSSM_MODULE_HANDLE AddinHandle,
						uint32 size,
						uint32 count
#ifdef BioAPI_MEMTRACK_ON
						, const char * szFilename,
						uint32 u32LineNumber
#endif
						);

/* Macros defined to support memory tracking operations. These macros
 * should be used to keep code easy to read.
 */
#ifdef BioAPI_MEMTRACK_ON
#define App_Malloc( handle, size ) _App_Malloc( handle, size, __FILE__, __LINE__ )
#define App_Realloc( handle, buf, size ) _App_Realloc( handle, buf, size, __FILE__, __LINE__ )
#define App_Free( handle, buf ) _App_Free( handle, buf, __FILE__, __LINE__ )
#define App_Calloc( handle, size, count ) _App_Calloc( handle, size, count, __FILE__, __LINE__ )
#else
#define App_Malloc( handle, size ) _App_Malloc( handle, size )
#define App_Realloc( handle, buf, size ) _App_Realloc( handle, buf, size )
#define App_Free( handle, buf ) _App_Free( handle, buf )
#define App_Calloc( handle, size, count ) _App_Calloc( handle, size, count )
#endif

BioAPI_RETURN Addin_SendModuleEvent( BioAPI_DEVICE_ID uDeviceId,
								   BioAPI_MODULE_EVENT meEvent );

#define ADDIN_SPI_BEGIN( rv, AddinHandle, pLockRef ) \
{ \
	rv = Addin_SPIBegin( AddinHandle, &pLockRef ); \
}

/*-----------------------------------------------------------------------------
 * Name: Addin_SPIBegin
 *
 * Description:
 * Function that is called at the begining of every protected interface, finds
 * the context (which CSSM are we talking to) and stores information in thread
 * local storage
 *
 * Parameters:
 * AddinHandle (input) : Handle of the addin
 * ReturnAddress (input) : Address of the caller
 *
 * Return Code:
 * NULL if the function failed, otherwise a lock reference that is returned to
 * Addin_SPIEnd
 *---------------------------------------------------------------------------*/
BioAPI_RETURN Addin_SPIBegin(
						BioAPI_HANDLE AddinHandle,
						MLC_LOCK_REF *pLockRef );

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
void Addin_SPIEnd( MLC_LOCK_REF LockRef );

/*-----------------------------------------------------------------------------
 * Name: Addin_GetAttachContext
 *
 * Description:
 * Returns a reference, locked for writing, to the attach information
 * associated with AttachHandle. Use the CLC_ReleaseItem function to unlock
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
 * BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE : AttachHandle does not contain a
 *		valid attach handle value.
 *---------------------------------------------------------------------------*/
BioAPI_RETURN Addin_GetAttachContext(
						BioAPI_HANDLE AttachHandle,
						MLC_LOCK_TYPE ltLockType,
						MLC_LOCK_REF *pLockRef );


#ifdef __cplusplus
}
#endif

#endif
