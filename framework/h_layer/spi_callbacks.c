/*-----------------------------------------------------------------------
 * File: spi_callbacks.c
 *
 *-----------------------------------------------------------------------
 */

#include "precomp.h"

/* The upper bound of the device handle space. Range 0 to 63. */
#define MAX_DEVICE_INDEX_VALUE ( ( 2 << NUM_DEVICE_INDEX_BITS ) - 1 )


#ifndef BioAPI_MEMTRACK_ON

/*---------------------------------------------------------------
*
*Name: BioAPI bioapi_SpiMalloc
*
*Description:
*	All add-in mallocs come through here to get memory from an app
*
*Parameters:
*	handle - describe the application that supplied memory fxs
*	size - the number of bytes to malloc
*
*Returns:
*	NULL - error in allocating memory
*	not NULL - pointer to memory
*
*-------------------------------------------------------------*/
void * BioAPI
BioAPI bioapi_SpiMalloc(BioAPI_HANDLE handle,
			   uint32 size)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_MEMORY_FUNCS_PTR mem_ptr = NULL;
	void* ptr = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;

	ERR(err = bioapi_FindAttachAndMultiLock(handle,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  &AttachRecord,
										  BioAPI_READER_LOCK));
	if(BioAPI_OK == err)
	{
		ERR(err = bioapi_GetAppMemoryFunctions(AttachRecord,
											 &mem_ptr));

		if((BioAPI_OK == err) &&
		   (NULL != mem_ptr))
		{
			ptr = mem_ptr->Malloc_func(size,
									   mem_ptr->AllocRef);
		}

		/* release the attach record */
		bioapi_ReleaseAttachLock (AttachRecord, BioAPI_READER_LOCK);
	}

	return ptr;
}

/*---------------------------------------------------------------
*
*Name: BioAPI bioapi_SpiFree
*
*Description:
*	All add-in frees come through here to get memory from an app
*
*Parameters:
*	handle - describe the application that supplied memory fxs
*	mem - pointer to memory for freeing
*
*Returns:
*	None
*
*-------------------------------------------------------------*/
void BioAPI
BioAPI bioapi_SpiFree(BioAPI_HANDLE handle,
			 void *mem)
{
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_MEMORY_FUNCS_PTR mem_ptr = NULL;

	ERR(err = bioapi_FindAttachAndMultiLock(handle,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  &AttachRecord,
										  BioAPI_READER_LOCK));
	if(err == BioAPI_OK)
	{
		ERR(err = bioapi_GetAppMemoryFunctions (AttachRecord, &mem_ptr));

		if((BioAPI_OK == err) &&
		   (NULL != mem_ptr))
		{
			mem_ptr->Free_func(mem,
							   mem_ptr->AllocRef);
		}

		/* release the attach record */
		bioapi_ReleaseAttachLock (AttachRecord, BioAPI_READER_LOCK);
	}

	return;
}

/*---------------------------------------------------------------
*
*Name: BioAPI bioapi_SpiRealloc
*
*Description:
*	All add-in reallocs come through here to get memory from an app
*
*Parameters:
*	handle - describe the application that supplied memory fxs
*	mem - pointer to previously allocated memory
*	size - the number of bytes to malloc
*
*Returns:
*	NULL - error in allocating memory
*	non NULL - pointer to memory
*
*-------------------------------------------------------------*/
void * BioAPI
BioAPI bioapi_SpiRealloc(BioAPI_HANDLE handle,
				void *mem,
				uint32 size)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_MEMORY_FUNCS_PTR mem_ptr = NULL;
	void* ptr = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;

	ERR(err = bioapi_FindAttachAndMultiLock(handle,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  &AttachRecord,
										  BioAPI_READER_LOCK));
	if(BioAPI_OK == err)
	{
		ERR(err = bioapi_GetAppMemoryFunctions (AttachRecord, &mem_ptr));

		if((BioAPI_OK == err) &&
		   (NULL != mem_ptr))
		{
			ptr = mem_ptr->Realloc_func(mem,
										size,
										mem_ptr->AllocRef);
		}

		/* release the attach record */
		bioapi_ReleaseAttachLock (AttachRecord, BioAPI_READER_LOCK);
	}

	return ptr;
}

/*---------------------------------------------------------------
*
*Name: BioAPI bioapi_SpiCalloc
*
*Description:
*	All add-in callocs come through here to get memory from an app
*
*Parameters:
*	handle - describe the application that supplied memory fxs
*	num - value to initialize memory
*	size - the number of bytes to malloc
*
*Returns:
*	NULL - error in allocating memory
*	non NULL - pointer to memory
*
*-------------------------------------------------------------*/
void * BioAPI
BioAPI bioapi_SpiCalloc(BioAPI_HANDLE handle,
			   uint32 num,
			   uint32 size)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_MEMORY_FUNCS_PTR mem_ptr = NULL;
	void* ptr = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;

	ERR(err = bioapi_FindAttachAndMultiLock(handle,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  &AttachRecord,
										  BioAPI_READER_LOCK));
	if(BioAPI_OK == err)
	{
		ERR(err = bioapi_GetAppMemoryFunctions (AttachRecord, &mem_ptr));

		if((BioAPI_OK == err) &&
		   (NULL != mem_ptr))
		{
			ptr = mem_ptr->Calloc_func(num,
									   size,
									   mem_ptr->AllocRef);
		}

		/* release the attach record */
		bioapi_ReleaseAttachLock (AttachRecord, BioAPI_READER_LOCK);
	}

	return ptr;
}

#else	/* BioAPI_MEMTRACK_ON */


/*---------------------------------------------------------------
*
*Name: BioAPI bioapi_SpiMalloc
*
*Description:
*	All add-in mallocs come through here to get memory from an app
*
*Parameters:
*	handle - describe the application that supplied memory fxs
*	size - the number of bytes to malloc
*
*Returns:
*	NULL - error in allocating memory
*	not NULL - pointer to memory
*
*-------------------------------------------------------------*/
void * BioAPI
BioAPI bioapi_SpiMalloc(BioAPI_HANDLE handle,
			   uint32 size,
			   const char* File,
			   uint32 Line)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_MEMORY_FUNCS_PTR mem_ptr = NULL;
	void* ptr = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;

	ERR(err = bioapi_FindAttachAndMultiLock(handle,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  &AttachRecord,
										  BioAPI_READER_LOCK));
	if(BioAPI_OK == err)
	{
		ERR(err = bioapi_GetAppMemoryFunctions(AttachRecord,
											 &mem_ptr));

		if((BioAPI_OK == err) &&
		   (NULL != mem_ptr))
		{
			ptr = mem_ptr->Malloc_func(size,
									   mem_ptr->AllocRef,
									   File,
									   Line);
		}

		/* release the attach record */
		bioapi_ReleaseAttachLock (AttachRecord, BioAPI_READER_LOCK);
	}

	return ptr;
}

/*---------------------------------------------------------------
*
*Name: BioAPI bioapi_SpiFree
*
*Description:
*	All add-in frees come through here to get memory from an app
*
*Parameters:
*	handle - describe the application that supplied memory fxs
*	mem - pointer to memory for freeing
*
*Returns:
*	None
*
*-------------------------------------------------------------*/
void BioAPI
BioAPI bioapi_SpiFree(BioAPI_HANDLE handle,
			 void *mem,
			 const char* File,
			 uint32 Line)
{
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_MEMORY_FUNCS_PTR mem_ptr = NULL;

	ERR(err = bioapi_FindAttachAndMultiLock(handle,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  &AttachRecord,
										  BioAPI_READER_LOCK));
	if(err == BioAPI_OK)
	{
		ERR(err = bioapi_GetAppMemoryFunctions (AttachRecord, &mem_ptr));

		if((BioAPI_OK == err) &&
		   (NULL != mem_ptr))
		{
			mem_ptr->Free_func(mem,
							   mem_ptr->AllocRef,
							   File,
							   Line);
		}

		/* release the attach record */
		bioapi_ReleaseAttachLock (AttachRecord, BioAPI_READER_LOCK);
	}

	return;
}

/*---------------------------------------------------------------
*
*Name: BioAPI bioapi_SpiRealloc
*
*Description:
*	All add-in reallocs come through here to get memory from an app
*
*Parameters:
*	handle - describe the application that supplied memory fxs
*	mem - pointer to previously allocated memory
*	size - the number of bytes to malloc
*
*Returns:
*	NULL - error in allocating memory
*	non NULL - pointer to memory
*
*-------------------------------------------------------------*/
void * BioAPI
BioAPI bioapi_SpiRealloc(BioAPI_HANDLE handle,
				void *mem,
				uint32 size,
				const char* File,
				uint32 Line)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_MEMORY_FUNCS_PTR mem_ptr = NULL;
	void* ptr = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;

	ERR(err = bioapi_FindAttachAndMultiLock(handle,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  &AttachRecord,
										  BioAPI_READER_LOCK));
	if(BioAPI_OK == err)
	{
		ERR(err = bioapi_GetAppMemoryFunctions (AttachRecord, &mem_ptr));

		if((BioAPI_OK == err) &&
		   (NULL != mem_ptr))
		{
			ptr = mem_ptr->Realloc_func(mem,
										size,
										mem_ptr->AllocRef,
										File,
										Line);

		}

		/* release the attach record */
		bioapi_ReleaseAttachLock (AttachRecord, BioAPI_READER_LOCK);
	}

	return ptr;
}


/*---------------------------------------------------------------
*
*Name: BioAPI bioapi_SpiCalloc
*
*Description:
*	All add-in callocs come through here to get memory from an app
*
*Parameters:
*	handle - describe the application that supplied memory fxs
*	num - value to initialize memory
*	size - the number of bytes to malloc
*
*Returns:
*	NULL - error in allocating memory
*	non NULL - pointer to memory
*
*-------------------------------------------------------------*/
void * BioAPI
BioAPI bioapi_SpiCalloc(BioAPI_HANDLE handle,
			   uint32 num,
			   uint32 size,
			   const char* File,
			   uint32 Line)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_MEMORY_FUNCS_PTR mem_ptr = NULL;
	void* ptr = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;

	ERR(err = bioapi_FindAttachAndMultiLock(handle,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  NULL,
										  BioAPI_NO_LOCK,
										  &AttachRecord,
										  BioAPI_READER_LOCK));
	if(BioAPI_OK == err)
	{
		ERR(err = bioapi_GetAppMemoryFunctions (AttachRecord, &mem_ptr));

		if((BioAPI_OK == err) &&
		   (NULL != mem_ptr))
		{
			ptr = mem_ptr->Calloc_func(num,
									   size,
									   mem_ptr->AllocRef,
									   File,
									   Line);
		}

		/* release the attach record */
		bioapi_ReleaseAttachLock (AttachRecord, BioAPI_READER_LOCK);
	}

	return ptr;
}




#endif	/* BioAPI_MEMTRACK_ON */

/*---------------------------------------------------------------
 *
 *Name: BioAPI bioapi_PruneCallbackList
 *
 *Description:
 *	 Walks the module list node's callback list and either updates
 *	 the reference count to an existing callback and context pair,
 *	 or removes the pair from the list. Pairs are removed if the
 *	 reference count reaches 0. Assumes a writer lock on
 *	 pModuleEntry.
 *
 *Parameters:
 *	pModuleEntry - Module entry to which the callback applies.
 *	AppCallback - Callback function registered for the load.
 *	AppCallbackCtx - Callback context that is passed as a
 *		parameter to the AppCallback function.
 *
 *Returns:
 *	 BioAPI_OK - callback list updated correctly.
 *	 BioAPI_FAIL - memory or parameter error.
 *
 *-------------------------------------------------------------*/
BioAPI_RETURN bioapi_PruneCallbackList(
							bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry,
							BioAPI_ModuleEventHandler AppCallback,
							const void *AppCallbackCtx )
{
	BioAPI_RETURN ret = BioAPI_OK;
	BioAPI_BOOL bSuccess = BioAPI_FALSE;

	bioapi_APP_NOTIFY_CALLBACK_LIST_PTR pCallbackEntry =
							pModuleEntry->EventNotificationCallbackList;

	/* Do we know about this callback combination? */
	while ( pCallbackEntry != NULL )
	{
		if ( ( pCallbackEntry->AppCallbackFunc == AppCallback ) &&
			 ( pCallbackEntry->AppCallbackCtx == AppCallbackCtx ) )
		{
			/* Already known, bump down the reference count */
			pCallbackEntry->RefCount--;

			/* Was that the last reference to this set of callbacks? */
			if ( pCallbackEntry->RefCount == 0 )
			{
				/* Remove it from the list */
				if ( pCallbackEntry ==
					 pModuleEntry->EventNotificationCallbackList )
				{
					/* First record in the list */
					pModuleEntry->EventNotificationCallbackList =
						pCallbackEntry->Next;
				}
				else
				{
					bioapi_APP_NOTIFY_CALLBACK_LIST_PTR pHunter =
						pModuleEntry->EventNotificationCallbackList;

					/* Find the predecessor in the list */
					while ( pHunter->Next != pCallbackEntry )
					{
						pHunter = pHunter->Next;
					}

					/* Unlink the record */
					pHunter->Next = pCallbackEntry->Next;
				}

				/* Free the record */
				local_free( pCallbackEntry, NULL );
				pCallbackEntry = NULL;
			}
			bSuccess = BioAPI_TRUE;
			break;
		}
		else
		{
			pCallbackEntry = pCallbackEntry->Next;
		}
	}
	if( !bSuccess )
	/* Couldn't find the callback, this is an error */
		ret = BioAPIERR_H_FRAMEWORK_LIB_REF_NOT_FOUND;
	return	ret ;
}

/*---------------------------------------------------------------
 *
 *Name: BioAPI bioapi_UpdateCallbackList
 *
 *Description:
 *	 Walks the module list node's callback list and either updates
 *	 the reference count to an existing callback and context pair,
 *	 or adds the pair to the list. Assumes a writer lock on
 *	 pModuleEntry.
 *
 *Parameters:
 *	pModuleEntry - Module entry to which the callback applies.
 *	AppCallback - Callback function registered for the load.
 *	AppCallbackCtx - Callback context that is passed as a
 *		parameter to the AppCallback function.
 *
 *Returns:
 *	 BioAPI_OK - callback list updated correctly.
 *	 BioAPI_FAIL - memory or parameter error.
 *
 *-------------------------------------------------------------*/
BioAPI_RETURN bioapi_UpdateCallbackList(
							bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry,
							BioAPI_ModuleEventHandler AppCallback,
							const void *AppCallbackCtx )
{
	bioapi_APP_NOTIFY_CALLBACK_LIST_PTR pCallbackEntry =
							pModuleEntry->EventNotificationCallbackList;


	/* Do we already know about this callback combination? */
	while ( pCallbackEntry != NULL )
	{
		if ( ( pCallbackEntry->AppCallbackFunc == AppCallback ) &&
			 ( pCallbackEntry->AppCallbackCtx == AppCallbackCtx ) )
		{
			/* Already known, bump the reference count */
			pCallbackEntry->RefCount++;
			return BioAPI_OK;
		}
		else
		{
			pCallbackEntry = pCallbackEntry->Next;
		}

	}

	/* Add the new callback and context combination to the list */
	pCallbackEntry = (bioapi_APP_NOTIFY_CALLBACK_LIST_PTR)
						local_calloc( (uint32)sizeof(bioapi_APP_NOTIFY_CALLBACK_LIST), 1,
									 NULL );
	if ( pCallbackEntry == NULL )
	{
		return BioAPIERR_H_FRAMEWORK_MEMORY_ERROR;
	}
	pCallbackEntry->RefCount = 1;
	pCallbackEntry->AppCallbackFunc = AppCallback;
	pCallbackEntry->AppCallbackCtx = (void *)AppCallbackCtx;

	/* Link in the next structure */
	pCallbackEntry->Next = pModuleEntry->EventNotificationCallbackList;
	pModuleEntry->EventNotificationCallbackList = pCallbackEntry;

	return BioAPI_OK;
}

/*---------------------------------------------------------------
 *
 *Name: BioAPI bioapi_BroadcastModuleEvent
 *
 *Description:
 *	 Walks the module list and triggers a callback for each
 *	 registered function and context combination.
 *
 *Parameters:
 *	CssmNotifyCallbackCtx - Passed "just in case"
 *	BSPUuid - UUID of the module that originated the event.
 *	DeviceID - ID of the device that originated the event.
 *	DeviceType - type of the module that originated the event.
 *	EventType - event code sent by the device.
 *
 *Returns:
 *	 void
 *
 *-------------------------------------------------------------*/
BioAPI_RETURN bioapi_BroadcastModuleEvent(const BioAPI_UUID *BSPUuid,
								uint32 DeviceID,
								BioAPI_MODULE_EVENT EventType )
{
	BioAPI_RETURN retCode = BioAPI_OK;
	bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry = NULL;
	bioapi_APP_NOTIFY_CALLBACK_LIST_PTR pCallbackEntry = NULL;

	/* Get the module entry for the event */
	retCode = bioapi_FindModuleByUUIDAndMultiLock( BSPUuid,
											BioAPI_NO_LOCK,
											&pModuleEntry,
											BioAPI_READER_LOCK );
	if ( retCode == BioAPI_OK )
	{
		/* Walk the callback list and trigger each */
		pCallbackEntry = pModuleEntry->EventNotificationCallbackList;
		while ( pCallbackEntry )
		{
			if ( pCallbackEntry->AppCallbackFunc != NULL )
			{
				pCallbackEntry->AppCallbackFunc( BSPUuid,
												 pCallbackEntry->AppCallbackCtx,
												 DeviceID,
												 0,
												 EventType );
			}
			pCallbackEntry = pCallbackEntry->Next;
		}

		/* Release the list lock */
		bioapi_ReleaseModuleLock( pModuleEntry, BioAPI_READER_LOCK );
	}
	return retCode;
}

/*---------------------------------------------------------------
 *
 *Name: BioAPI bioapi_NextDeviceHandle
 *
 *Description:
 *	 Given a module list node, the next available device handle
 *	 is generated for that node's device list. Holes in a
 *	 sequence of handles will return the value of the first hole.
 *
 *	 The index and the address of the device record immediately
 *	 preceding the hole in the sequence are returned. If the hole
 *	 is at the start of the list, NULL is returned.
 *
 *	 NOTE: It is assumed that a writer lock is held on the
 *	 module node.
 *
 *Parameters:
 *	pModuleEntry - module list entry to search for a new device
 *		handle.
 *	pDeviceIndex - return parameter for the new device index.
 *	ppInsertLocation - receives the address of the record
 *		immediately preceding the hole in the device indexes.
 *
 *Returns:
 *	 BioAPI_OK - Handle successfully created.
 *	 BioAPI_FAIL - locking, parameter or full list problem.
 *
 *-------------------------------------------------------------*/
BioAPI_RETURN BioAPI bioapi_NextDeviceHandle(
						bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry,
						uint32 *pDeviceIndex,
						bioapi_INTERNAL_DEVICE_LIST_PTR *ppInsertLocation )
{
	bioapi_INTERNAL_DEVICE_LIST_PTR pDevice = pModuleEntry->DeviceList;
	uint32 uLastIndex = 0, uNewIndex = 0;
	BioAPI_RETURN ret = BioAPI_OK;

  /* Is this the first record in the list? If so the device index is
	 * the same as the module index.
	 */
	if ( pDevice == NULL )
	{
		*pDeviceIndex = pModuleEntry->ModuleHandleIndex;
		*ppInsertLocation = NULL;
		return BioAPI_OK;
	}
	if ((ret = bioapi_ReaderLock( pDevice->hDeviceSWMRLock )) != BioAPI_OK)
	{
		return ret ;
	}
	uLastIndex =  ( pDevice->DeviceHandleIndex ^
					bioapi_ExtractModuleIndex( pDevice->DeviceHandleIndex )
				  ) >> NUM_ATTACH_INDEX_BITS;
	bioapi_ReaderUnlock( pDevice->hDeviceSWMRLock );

	if ( uLastIndex != 0 )
	{
		/* A hole exists */
		*pDeviceIndex = pModuleEntry->ModuleHandleIndex;
		*ppInsertLocation = NULL;
		return	BioAPI_OK;
	}

	/* Search the list looking for a hole in the handle sequence. Use a
	 * one record look-ahead to preserve the insert location.
	 */
	while ( ( pDevice->Next != NULL ) &&
			( uLastIndex != MAX_DEVICE_INDEX_VALUE ) )
	{
		if (bioapi_ReaderLock( pDevice->Next->hDeviceSWMRLock ) != BioAPI_OK)
		{
			return ERR(BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR);
		}
		uNewIndex =	 ( pDevice->Next->DeviceHandleIndex ^
						bioapi_ExtractModuleIndex(
								pDevice->Next->DeviceHandleIndex )
					 ) >> NUM_ATTACH_INDEX_BITS;
		bioapi_ReaderUnlock( pDevice->Next->hDeviceSWMRLock );
		if ( ( uNewIndex - uLastIndex ) != 1 )
		{
			break;
		}

		uLastIndex = uNewIndex;
		pDevice = pDevice->Next;
	}

	/* Did the search end because we ran out of handles? */
	if ( uLastIndex == MAX_DEVICE_INDEX_VALUE )
	{
		return	BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR;
	}

	/* We found a good value. Calculate the new index value and return the
	 * insert pointer.
	 */
	*pDeviceIndex = pModuleEntry->ModuleHandleIndex |
						( ( uLastIndex + 1 ) << NUM_ATTACH_INDEX_BITS );
	*ppInsertLocation = pDevice;

	return	BioAPI_OK;
}

/*---------------------------------------------------------------
 *
 *Name: BioAPI bioapi_InsertDeviceRecord
 *
 *Description:
 *	 Given a module list node and a set of device list node
 *	 parameters, a corresponding device list node is contained
 *	 in the module list node's device list. The device list
 *	 node is created with writer lock.
 *
 *	 The new device record index is generated by finding the
 *	 first index in the range 0 to 2^NUM_DEVICE_INDEX_BITS-1.
 *	 The function fails if an index can not be found.
 *
 *	 NOTE: It is assumed that a writer lock is held on the
 *	 module node.
 *
 *Parameters:
 *	DeviceID - ID of the new device.
 *	DeviceType - device type mask to assign to the device.
 *	pModuleEntry - module list entry to receive the new device.
 *
 *Returns:
 *	 BioAPI_OK - Record successfully inserted.
 *	 BioAPI_FAIL - Memory, lock creation or parameter error.
 *
 *-------------------------------------------------------------*/
BioAPI_RETURN
bioapi_InsertDeviceRecord(uint32 DeviceID,
						 bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry,
						 bioapi_INTERNAL_DEVICE_LIST_PTR *pDeviceEntry)
{
	bioapi_INTERNAL_DEVICE_LIST_PTR pTempDevice = NULL;
	bioapi_INTERNAL_DEVICE_LIST_PTR pInsertLocation = NULL;
	BioAPI_RETURN ret = BioAPI_OK;

	/* Allocate a device record */
	pTempDevice = (bioapi_INTERNAL_DEVICE_LIST_PTR)
						local_calloc((uint32)sizeof(bioapi_INTERNAL_DEVICE_LIST),1,
									 NULL);
	if ( pTempDevice == NULL )
	{
		return BioAPIERR_H_FRAMEWORK_MEMORY_ERROR;
	}

	/* Fill in the device record */
	pTempDevice->DeviceID = DeviceID;

	if((ret = bioapi_NextDeviceHandle( pModuleEntry,
							   &pTempDevice->DeviceHandleIndex,
							   &pInsertLocation )) != BioAPI_OK )
	{
		local_free(pTempDevice, NULL);
		return ret;
	}

	if (ERR(ret = bioapi_SWMRInit(&pTempDevice->hDeviceSWMRLock)) != BioAPI_OK)
	{
		local_free(pTempDevice, NULL);
		return ret;
	}

	if((ret = cssm_SWMRLockWaitToWrite(pTempDevice->hDeviceSWMRLock,
									   CSSM_INFINITE_WAIT)) != BioAPI_OK)
	{
		bioapi_SWMRDelete(pTempDevice->hDeviceSWMRLock);
		local_free(pTempDevice, NULL);
		return ret;
	}

	/* Insert the record into the list */
	if ( pInsertLocation == NULL )
	{
		/* Insert the record at the head of the list */
		pTempDevice->Next = pModuleEntry->DeviceList;
		pModuleEntry->DeviceList = pTempDevice;
	}
	else
	{
		pTempDevice->Next = pInsertLocation->Next;
		pInsertLocation->Next = pTempDevice;
	}

	pTempDevice->ModuleLibHandle = pModuleEntry->LibHandle;
	*pDeviceEntry = pTempDevice;

	return BioAPI_OK;
}

/*---------------------------------------------------------------
 *
 *Name: BioAPI bioapi_DeleteDeviceRecord
 *
 *Description:
 *	 Given a module list node and a device list node contained
 *	 in the module list node's device list, the device list
 *	 node is removed and destroyed. The proper internal teardown
 *	 function is called on the device list node to insure that
 *	 SPIs are called correctly.
 *
 *	 NOTE: It is assumed that writer locks are held on both
 *	 records and that the device list node is actually in the
 *	 module list node's device list.
 *
 *Parameters:
 *	pModuleEntry - module list node to search.
 *	pDeviceEntry - device list entry to remove.
 *
 *Returns:
 *	 void
 *
 *-------------------------------------------------------------*/
BioAPI_RETURN bioapi_DeleteDeviceRecord( bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry,
							   bioapi_INTERNAL_DEVICE_LIST_PTR pDeviceEntry )
{
	bioapi_INTERNAL_DEVICE_LIST_PTR pTempDevice = NULL;
	BioAPI_RETURN ret = BioAPI_OK;

	/* Tear down the internal structure of the device record */
	bioapi_CleanInternalDeviceRecord( pModuleEntry, &pDeviceEntry );

	/* Find the entry in the list */
	if ( pModuleEntry->DeviceList == pDeviceEntry )
	{
		/* Handle the head of the list case */
		pModuleEntry->DeviceList = pDeviceEntry->Next;
	}
	else
	{
		/* Start at the front of the device list */
		pTempDevice = pModuleEntry->DeviceList;

		/* Scan looking ahead one record for the target */
		while ( pTempDevice->Next != pDeviceEntry )
		{
			pTempDevice = pTempDevice->Next;
		}

		/* Relink the list */
		pTempDevice->Next = pTempDevice->Next->Next;
	}

	/* Free the device record */
	local_free( pDeviceEntry, 0 );
	return	ret;
}
/*---------------------------------------------------------------
 *
 *Name: BioAPI NotifyFaultThread
 *
 *Description:
 *	Function to handle the Fault call back event
 *Parameters:
 *	lpParam - void param
 *
 *Returns:
 *	 BioAPI_FAIL -
 *	 BioAPI_OK -
 *
 *-------------------------------------------------------------*/
BioAPI_RETURN BioAPI
NotifyFaultThread(void *Param)
{
	bioapi_THREAD_DATA_PTR threadDataPtr = (bioapi_THREAD_DATA_PTR)Param;
	BioAPI_RETURN  ret = BioAPI_OK;

	if( threadDataPtr != NULL )
	{
		/* Broadcast the fault to the applications */
		ret = bioapi_BroadcastModuleEvent( 
									 /* Linux-Port: add the (void *) cast */
									 (void *) &threadDataPtr->BSPUuid,
								   threadDataPtr->DeviceID,
								   threadDataPtr->EventType );
	}
	return 0;
}
/*---------------------------------------------------------------
 *
 *Name: BioAPI NotifyRemoveThread
 *
 *Description:
 *	Function to handle the Remove Token call back event
 *Parameters:
 *	lpParam - void param
 *
 *Returns:
 *	 BioAPI_FAIL -
 *	 BioAPI_OK -
 *
 *-------------------------------------------------------------*/

BioAPI_RETURN BioAPI
NotifyRemoveThread(void *Param)
{
	bioapi_INTERNAL_DEVICE_LIST_PTR pDeviceEntry = NULL;
	bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry = NULL;
	BioAPI_RETURN retCode = BioAPI_OK;
	bioapi_THREAD_DATA_PTR threadDataPtr = (bioapi_THREAD_DATA_PTR)Param;
	if( threadDataPtr != NULL )
	{
		/* Is the device record known? */
		retCode = bioapi_FindDeviceByUUIDAndDeviceAndMultiLock(
										/* Linux-Port: added the (void *) cast */
										(void *) &threadDataPtr->BSPUuid,
										threadDataPtr->DeviceID,
										BioAPI_NO_LOCK,
										NULL,
										BioAPI_NO_LOCK,
										&pDeviceEntry,
										BioAPI_WRITER_LOCK );
		if ( retCode != BioAPI_OK )
		{
			/* Can't remove something that wasn't inserted! */
			return	0;
		}


		/* Get a writer on the module record for this device */
		retCode = bioapi_FindModuleByUUIDAndMultiLock( 
										    /* Linux-Port: added the (void *) cast */
										    (void *) &threadDataPtr->BSPUuid,
												BioAPI_NO_LOCK,
												&pModuleEntry,
												BioAPI_WRITER_LOCK );
		if ( retCode != BioAPI_OK )
		{
			bioapi_ReleaseDeviceLock( pDeviceEntry, BioAPI_WRITER_LOCK );
			return	0;
		}

		/* Remove the device record from the list */
		bioapi_CleanInternalDeviceRecord( pModuleEntry, &pDeviceEntry );

		/* Release the locks */
		bioapi_ReleaseModuleLock( pModuleEntry, BioAPI_WRITER_LOCK );

		/* Broadcast the remove to the applications */
		bioapi_BroadcastModuleEvent(
								  /* Linux-Port: added the (void *) cast */
									(void *) &threadDataPtr->BSPUuid,
								  threadDataPtr->DeviceID,
								  threadDataPtr->EventType );
	}
	return 1;
}
/*---------------------------------------------------------------
 *
 *Name: bioapi_ModuleEventHandler
 *
 *Description:
 *	 Function passed to addins during the BioAPI_SPI_ModuleLoad SPI
 *	 call. Responsible for proxying events triggered by an addin
 *	 that signal a change in the module state
 *	 (insert/remove/fault).
 *
 *	 When an insert event is recieved, the new device is added
 *	 to the internal module information structures, and then the
 *	 event is reflected up to the application. If it already
 *	 exists in the module information (duplicate event), then it
 *	 is simply reflected up to the application.
 *
 *	 When a remove event is received, the corresponding device
 *	 is removed from the internal module information structure
 *	 and the events are reflected up to the application.
 *
 *	 Fault events cause the CSSM to perform an implicit
 *	 BioAPI_ModuleUnload on the notifying addin. The event is then
 *	 reflected up to the application.
 *
 *	 All unrecognized events are immediately reflected to the
 *	 application.
 *
 *Parameters:
 *	BSPUuid - uuid of addin sending the event.
 *	CssmNotifyCallbackCtx - CSSM specific callback context.
 *	DeviceID - ID number of the relevant device.
 *	DeviceType- the type of device.
 *	EventType - event code.
 *
 *Returns:
 *	 BioAPI_FAIL - internal error; FATAL!
 *	 BioAPI_OK - event handled and delivered.
 *
 *-------------------------------------------------------------*/
BioAPI_RETURN BioAPI bioapi_ModuleEventHandler(
							const BioAPI_UUID *BSPUuid,
							void* BioAPINotifyCallbackCtx,
							BioAPI_DEVICE_ID DeviceID,
							uint32 Reserved,
							BioAPI_MODULE_EVENT EventType)

{
	bioapi_INTERNAL_DEVICE_LIST_PTR pDeviceEntry = NULL;
	bioapi_INTERNAL_MODULE_LIST_PTR pModuleEntry = NULL;
	BioAPI_RETURN retCode = BioAPI_OK;

	/* Do we recognize this event code? */
	switch ( EventType )
	{
	case BioAPI_NOTIFY_INSERT:
		{
			/* Is the device record already known? */
			retCode = bioapi_FindDeviceByUUIDAndDeviceAndMultiLock(
													BSPUuid,
													DeviceID,
													BioAPI_NO_LOCK,
													NULL,
													BioAPI_NO_LOCK,
													&pDeviceEntry,
													BioAPI_READER_LOCK );
			if ( retCode != BioAPI_OK )
			{
				/* New device.*/
				/* Find the module record and lock it for writing */
				retCode = bioapi_FindModuleByUUIDAndMultiLock(	  BSPUuid,
														BioAPI_NO_LOCK,
														&pModuleEntry,
														BioAPI_WRITER_LOCK );
				if ( retCode != BioAPI_OK )
				{
					return	retCode;
				}

				/* Add it to the list */
				retCode = bioapi_InsertDeviceRecord( DeviceID,
													pModuleEntry,
													&pDeviceEntry);
				if ( retCode != BioAPI_OK )
				{
					bioapi_ReleaseModuleLock( pModuleEntry, BioAPI_WRITER_LOCK );
					return	retCode;
				}

				/* Release the lock on the device list entry */
				bioapi_ReleaseDeviceLock(pDeviceEntry, BioAPI_WRITER_LOCK);

				/* Release the lock on the module list */
				bioapi_ReleaseModuleLock( pModuleEntry, BioAPI_WRITER_LOCK );
			}
			else
			{
				/* No need to read the device any longer */
				bioapi_ReleaseDeviceLock( pDeviceEntry, BioAPI_READER_LOCK );
			}
			if( retCode == BioAPI_OK )
			{
				/* Broadcast the insert to the applications */
				retCode = bioapi_BroadcastModuleEvent( BSPUuid,
										   DeviceID,
										   EventType );
			}
			break;
		}
	case BioAPI_NOTIFY_REMOVE:
		{
			bioapi_THREAD_DATA_PTR threadDataPtr = local_malloc(  (uint32)sizeof( bioapi_THREAD_DATA), NULL);
			if( threadDataPtr == NULL)
				return BioAPIERR_H_FRAMEWORK_MEMORY_ERROR;
			port_memcpy(&threadDataPtr->BSPUuid,
						BSPUuid,
						sizeof(BioAPI_UUID));
			threadDataPtr->BioAPINotifyCallbackCtx = BioAPINotifyCallbackCtx;
			threadDataPtr->DeviceID = DeviceID;
			threadDataPtr->EventType = EventType;

			retCode = port_CreateThread(NotifyRemoveThread,	 /* thread func */
										(void*)threadDataPtr, /* func param */
										BioAPI_TRUE,	   /* detached thread */
										NULL);	/* don't need thread handle */
			break;
		}
	case BioAPI_NOTIFY_FAULT:
		{
			bioapi_THREAD_DATA_PTR threadDataPtr = local_malloc(  (uint32)sizeof( bioapi_THREAD_DATA), NULL);
			if( threadDataPtr == NULL)
				return BioAPIERR_H_FRAMEWORK_MEMORY_ERROR;
			port_memcpy(&threadDataPtr->BSPUuid,
						BSPUuid,
						sizeof(BioAPI_UUID));
			threadDataPtr->BioAPINotifyCallbackCtx = BioAPINotifyCallbackCtx;
			threadDataPtr->DeviceID = DeviceID;
			threadDataPtr->EventType = EventType;

			retCode = port_CreateThread(NotifyFaultThread,	 /* thread func */
										(void*)threadDataPtr, /* func param */
										BioAPI_TRUE,	   /* detached thread */
										NULL);	/* don't need thread handle */

			/* Tear down the internal state for the module */

			break;
		}
	default:
		{
			/* Module defined event type, just broadcast the event */
			retCode = bioapi_BroadcastModuleEvent(BSPUuid,
												 DeviceID,
												 EventType );

			break;
		}
	}

	return	retCode ;
}
