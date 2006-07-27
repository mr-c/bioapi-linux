/*-----------------------------------------------------------------------
 * File: ADDUTIL.C
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the functions that are contained in the add-in util
 * of the BioAPI functions.
 */

#include "precomp.h"

/*----------------------------------------------------------
*Name: bioapi_Rand
*
*Description:
* Generate a 32 bit random number.	Note: this function no
* longer guarantees uniqueness for handles.
*
*Parameters:
* None
*
*Returns:
* 32 bit random number
*
*--------------------------------------------------------*/
uint32 bioapi_Rand ()
{
	static uint32 value=0;

	/* use C runtime functions */
	srand ((uint32)time (NULL));
	value ^= (uint32)rand();
	value <<= 16;
	srand (value);
	value ^= (uint32)rand();

	return value;
}

/*----------------------------------------------------------
*Name: bioapi_CheckAPIMemFuncs
*
*Description:
* Check to make sure that memory functions are initialize
*
*Parameters:
* MemoryFunctions (input) - pointer to memory function structure
*
*Returns:
* BioAPI_FAIL - bad pointer somewhere
* BioAPI_OK - all pointers OK
*
*--------------------------------------------------------*/
BioAPI_RETURN bioapi_CheckAPIMemFuncs (
								   const BioAPI_MEMORY_FUNCS *MemoryFunctions)
{
	/* Check validity of pointers passed in */
	if (port_IsBadReadPtr (MemoryFunctions, (uint32)sizeof (BioAPI_MEMORY_FUNCS))) {
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	/* Check validity of callbacks */
	if (port_IsBadCodePtr((BioAPI_PROC_ADDR)MemoryFunctions->Malloc_func)) {
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	if (port_IsBadCodePtr((BioAPI_PROC_ADDR)MemoryFunctions->Realloc_func)) {
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	if (port_IsBadCodePtr((BioAPI_PROC_ADDR)MemoryFunctions->Free_func)) {
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	if (port_IsBadCodePtr((BioAPI_PROC_ADDR)MemoryFunctions->Calloc_func)) {
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	return BioAPI_OK;
}


BioAPI_RETURN
bioapi_FindModuleAndMultiLock(BioAPI_HANDLE hModule,
							BioAPI_LOCK_TYPE ModuleListLock,
							bioapi_INTERNAL_MODULE_LIST_PTR *ppModule,
							BioAPI_LOCK_TYPE ModuleEntryLock)
{
	BioAPI_RETURN retCode = BioAPI_OK;
	BioAPI_RETURN LockRet;
	BioAPI_HANDLE hModuleIndex = bioapi_ExtractModuleIndex(hModule);
	bioapi_INTERNAL_MODULE_LIST_PTR pModule = NULL;
	BioAPI_BOOL ModuleFound;

	/* Check return parameter */
	if(ppModule == NULL)
	{
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	*ppModule = NULL;

	/* Get a reader lock on the module list */
	if(BioAPI_WRITER_LOCK == ModuleListLock)
	{
		LockRet = bioapi_GetModuleListLock(BioAPI_WRITER_LOCK);
	}
	else
	{
		LockRet = bioapi_GetModuleListLock(BioAPI_READER_LOCK);
	}

	if(BioAPI_OK != LockRet)
	{
		retCode = LockRet;
		goto bailout;
	}

	/* Iterate down the list and find the record with a matching handle */
	pModule = hModuleListHead;
	ModuleFound = BioAPI_FALSE;
	while((NULL != pModule) &&
		(BioAPI_FALSE == ModuleFound))
	{
		/* Test to see if the UUIDs match */
		if(pModule->ModuleHandleIndex == hModuleIndex)
		{
			ModuleFound = BioAPI_TRUE;
		}
		else
		{
			/* Move to the next record */
			pModule = pModule->Next;
		}
	}

	/* Did we find something? */
	if(NULL == pModule)
	{
		retCode = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
		goto droplock_bailout;
	}

	/* Obtain the proper lock on the record */
	if(BioAPI_READER_LOCK == ModuleEntryLock)
	{
		/* Get a reader lock on this entry in the list */
		if ((retCode = bioapi_ReaderLock (pModule->hModuleSWMRLock)) != BioAPI_OK)
		{
			goto droplock_bailout;
		}
		else
		{
			/* make sure another thread hasn't changed the index value on us */
			if(pModule->ModuleHandleIndex != hModuleIndex)
			{
				bioapi_ReaderUnlock(pModule->hModuleSWMRLock);
				retCode = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
			}
		}
	}
	else
	{
		if(BioAPI_WRITER_LOCK == ModuleEntryLock)
		{
			/* Get a writer lock on the record */
			if ((retCode = bioapi_WriterLock (pModule->hModuleSWMRLock)) != BioAPI_OK)
			{
				goto droplock_bailout;
			}
			else
			{
				/* make sure another thread hasn't changed the index value on us */
				if(pModule->ModuleHandleIndex != hModuleIndex)
				{
					bioapi_WriterUnlock(pModule->hModuleSWMRLock);
					retCode = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
				}
			}
		}
	}

	*ppModule = pModule;

droplock_bailout:
	if(BioAPI_NO_LOCK == ModuleListLock)
	{
		/* Drop the reader lock on the module list. */
		bioapi_ReaderUnlock(hModuleListSWMRLock);
	}
	else
	{
		if(BioAPI_OK != retCode)
		{
			/* now we drop either a reader or writer lock */
			if(BioAPI_READER_LOCK == ModuleListLock)
			{
				bioapi_ReaderUnlock(hModuleListSWMRLock);
			}
			else
			{
				bioapi_WriterUnlock(hModuleListSWMRLock);
			}
		}
	}

bailout:

	/* Return the error code */

	return retCode;
}


BioAPI_RETURN
bioapi_FindModuleByUUIDAndMultiLock(const BioAPI_UUID *UuidPtr,
								  BioAPI_LOCK_TYPE ModuleListLock,
								  bioapi_INTERNAL_MODULE_LIST_PTR *ppModule,
								  BioAPI_LOCK_TYPE ModuleEntryLock)
{
	BioAPI_RETURN retCode = BioAPI_OK;
	BioAPI_RETURN LockRet;
	bioapi_INTERNAL_MODULE_LIST_PTR pModule = NULL;
	BioAPI_BOOL ModuleFound;

	/* Check return parameter */
	if(ppModule == NULL)
	{
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	*ppModule = NULL;

	/* Get a reader lock on the module list */
	if(BioAPI_WRITER_LOCK == ModuleListLock)
	{
		LockRet = bioapi_GetModuleListLock(BioAPI_WRITER_LOCK);
	}
	else
	{
		LockRet = bioapi_GetModuleListLock(BioAPI_READER_LOCK);
	}

	if(BioAPI_OK != LockRet)
	{
		retCode = LockRet;
		goto bailout;
	}

	/* Iterate down the list and find the record with a matching handle */
	pModule = hModuleListHead;
	ModuleFound = BioAPI_FALSE;
	while((NULL != pModule) && (BioAPI_FALSE == ModuleFound))
	{
		/* Test to see if the UUIDs match */
		if(BioAPI_TRUE == BioAPI_CompareUuids((*UuidPtr), pModule->UUID))
		{
			ModuleFound = BioAPI_TRUE;
		}
		else
		{
			/* Move to the next record */
			pModule = pModule->Next;
		}
	}

	/* Did we find something? */
	if(NULL == pModule)
	{
		retCode = BioAPIERR_H_FRAMEWORK_INVALID_UUID;
		goto droplock_bailout;
	}

	/* Obtain the proper lock on the record */
	if(BioAPI_READER_LOCK == ModuleEntryLock)
	{
		/* Get a reader lock on this entry in the list */
		if ((retCode = bioapi_ReaderLock (pModule->hModuleSWMRLock)) != BioAPI_OK)
		{
			goto droplock_bailout;
		}
		else
		{
			/* make sure another thread hasn't changed the index value on us */
			if(BioAPI_TRUE != BioAPI_CompareUuids((*UuidPtr), pModule->UUID))
			{
				bioapi_ReaderUnlock(pModule->hModuleSWMRLock);
				retCode = BioAPIERR_H_FRAMEWORK_INVALID_UUID;
			}
		}
	}
	else
	{
		if(BioAPI_WRITER_LOCK == ModuleEntryLock)
		{
			/* Get a writer lock on the record */
			if ((retCode = bioapi_WriterLock (pModule->hModuleSWMRLock)) != BioAPI_OK)
			{
				goto droplock_bailout;
			}
			else
			{
				/* make sure another thread hasn't changed the index value on us */
				if(BioAPI_TRUE != BioAPI_CompareUuids((*UuidPtr),
					pModule->UUID))
				{
					bioapi_WriterUnlock(pModule->hModuleSWMRLock);
					retCode = BioAPIERR_H_FRAMEWORK_INVALID_UUID;
				}
			}
		}
	}

	*ppModule = pModule;

droplock_bailout:
	if(BioAPI_NO_LOCK == ModuleListLock)
	{
		/* Drop the reader lock on the module list. */
		bioapi_ReaderUnlock(hModuleListSWMRLock);
	}
	else
	{
		if(BioAPI_OK != retCode)
		{
			/* now we drop either a reader or writer lock */
			if(BioAPI_READER_LOCK == ModuleListLock)
			{
				bioapi_ReaderUnlock(hModuleListSWMRLock);
			}
			else
			{
				bioapi_WriterUnlock(hModuleListSWMRLock);
			}
		}
	}

bailout:

	/* Return the error code */

	return retCode;
}


BioAPI_RETURN
bioapi_FindDeviceAndMultiLock(BioAPI_HANDLE hDevice,
							 BioAPI_LOCK_TYPE ModuleListLock,
							 bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
							 BioAPI_LOCK_TYPE ModuleEntryLock,
							 bioapi_INTERNAL_DEVICE_LIST_PTR *ppDevice,
							 BioAPI_LOCK_TYPE DeviceEntryLock)
{
	BioAPI_RETURN retCode = BioAPI_OK;
	BioAPI_BOOL MatchFound;
	BioAPI_HANDLE hDeviceIndex = bioapi_ExtractDeviceIndex(hDevice);
	bioapi_INTERNAL_MODULE_LIST_PTR pModule = NULL;
	bioapi_INTERNAL_DEVICE_LIST_PTR pDevice = NULL;
	BioAPI_LOCK_TYPE LockForModule;

	/* Check return parameter */
	if(NULL == ppDevice)
	{
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	/* We need at least a reader lock here, but that may not be what the caller
	wants to have returned.
	*/
	if(BioAPI_NO_LOCK != ModuleEntryLock)
	{
		LockForModule = ModuleEntryLock;
	}
	else
	{
		LockForModule = BioAPI_READER_LOCK;
	}

	/* Fetch the module entry - locked as case demands */
	retCode = bioapi_FindModuleAndMultiLock(hDevice,
		ModuleListLock,
		&pModule,
		LockForModule);
	if(BioAPI_OK != retCode)
	{
		return BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
	}

	/* Iterate down the list and find the record with a matching handle */
	pDevice = pModule->DeviceList;
	MatchFound = BioAPI_FALSE;
	while((NULL != pDevice) && (BioAPI_FALSE == MatchFound))
	{
		/* test to see if we match indices */
		if(pDevice->DeviceHandleIndex == hDeviceIndex)
		{
			MatchFound = BioAPI_TRUE;
		}
		else
		{
			pDevice = pDevice->Next;
		}
	}

	/* Did we find something? */
	if(NULL != pDevice)
	{
		/* Obtain the proper lock on the record */
		if(BioAPI_READER_LOCK == DeviceEntryLock)
		{
			/* Get a reader lock on this entry in the list */
			if ((retCode = bioapi_ReaderLock (pDevice->hDeviceSWMRLock)) == BioAPI_OK)
			{
				/* make sure another thread hasn't changed the index value on us */
				if(pDevice->DeviceHandleIndex != hDeviceIndex)
				{
					bioapi_ReaderUnlock(pDevice->hDeviceSWMRLock);
					retCode = BioAPIERR_H_FRAMEWORK_INVALID_DEVICE_ID;
				}
			}
		}
		else
		{
			if(BioAPI_WRITER_LOCK == DeviceEntryLock)
			{
				/* Get a writer lock on the record */
				if ((retCode = bioapi_WriterLock (pDevice->hDeviceSWMRLock)) == BioAPI_OK)
				{
					/* make sure another thread hasn't changed the index value on us */
					if(pDevice->DeviceHandleIndex != hDeviceIndex)
					{
						bioapi_WriterUnlock(pDevice->hDeviceSWMRLock);
						retCode = BioAPIERR_H_FRAMEWORK_INVALID_DEVICE_ID;
					}
				}
			}
		}
	}
	else
	{
		retCode = BioAPIERR_H_FRAMEWORK_INVALID_DEVICE_ID;
	}

	if(BioAPI_OK == retCode)
	{
		*ppDevice = pDevice;

		/* if they want a module record, we'll set it */
		if(NULL != ModuleRecord)
		{
			*ModuleRecord = pModule;
		}

		/* they may not want this entry locked */
		if(BioAPI_NO_LOCK == ModuleEntryLock)
		{
			bioapi_ReleaseModuleLock(pModule, LockForModule);
		}
	}
	else
	{
		/* error case - release all locks */
		bioapi_ReleaseModuleLock(pModule, LockForModule);
		bioapi_ReleaseModuleListLock(ModuleListLock);
	}

	/* Return the error code */
	return retCode;
}


BioAPI_RETURN
bioapi_FindDeviceByUUIDAndDeviceAndMultiLock(const BioAPI_UUID *pUUID,
										  uint32 DeviceID,
										  BioAPI_LOCK_TYPE ModuleListLock,
										  bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
										  BioAPI_LOCK_TYPE ModuleEntryLock,
										  bioapi_INTERNAL_DEVICE_LIST_PTR *ppDevice,
										  BioAPI_LOCK_TYPE DeviceEntryLock)
{
	BioAPI_RETURN retCode = BioAPI_OK;
	BioAPI_BOOL MatchFound;
	bioapi_INTERNAL_MODULE_LIST_PTR pModule = NULL;
	bioapi_INTERNAL_DEVICE_LIST_PTR pDevice = NULL;
	BioAPI_LOCK_TYPE LockForModule;

	/* Check return parameter */
	if(NULL == ppDevice)
	{
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	/* We need at least a reader lock here, but that may not be what the caller
	wants to have returned.
	*/
	if(BioAPI_NO_LOCK != ModuleEntryLock)
	{
		LockForModule = ModuleEntryLock;
	}
	else
	{
		LockForModule = BioAPI_READER_LOCK;
	}

	/* Fetch the module entry - locked as case demands */
	retCode = bioapi_FindModuleByUUIDAndMultiLock(pUUID,
		ModuleListLock,
		&pModule,
		LockForModule);
	if(BioAPI_OK != retCode)
	{
		return BioAPIERR_H_FRAMEWORK_MODULE_NOT_LOADED;
	}

	/* Iterate down the list and find the record with a matching handle */
	pDevice = pModule->DeviceList;
	MatchFound = BioAPI_FALSE;
	while((NULL != pDevice) && (BioAPI_FALSE == MatchFound))
	{
		/* test to see if we match indeces */
		if(pDevice->DeviceID == DeviceID)
		{
			MatchFound = BioAPI_TRUE;
		}
		else
		{
			pDevice = pDevice->Next;
		}
	}

	/* Did we find something? */
	if(NULL != pDevice)
	{
		/* Obtain the proper lock on the record */
		if(BioAPI_READER_LOCK == DeviceEntryLock)
		{
			if ((retCode = bioapi_ReaderLock (pDevice->hDeviceSWMRLock)) == BioAPI_OK)
			{
				/* make sure another thread hasn't changed the index value on us */
				if(pDevice->DeviceID != DeviceID)
				{
					bioapi_ReaderUnlock(pDevice->hDeviceSWMRLock);
					ERR(retCode = BioAPIERR_H_FRAMEWORK_INVALID_DEVICE_ID);
				}
			}
		}
		else
		{
			if(BioAPI_WRITER_LOCK == DeviceEntryLock)
			{
				/* Get a writer lock on the record */

				if ((retCode = bioapi_WriterLock (pDevice->hDeviceSWMRLock)) == BioAPI_OK)
				{
					/* make sure another thread hasn't changed the index value on us */
					if(pDevice->DeviceID != DeviceID)
					{
						bioapi_WriterUnlock(pDevice->hDeviceSWMRLock);
						ERR(retCode = BioAPIERR_H_FRAMEWORK_INVALID_DEVICE_ID);
					}
				}
			}
		}
	}
	else
	{
		ERR(retCode = BioAPIERR_H_FRAMEWORK_INVALID_DEVICE_ID);
	}

	if(BioAPI_OK == retCode)
	{
		*ppDevice = pDevice;

		/* if they want a module record, we'll set it */
		if(NULL != ModuleRecord)
		{
			*ModuleRecord = pModule;
		}

		/* they may not want this entry locked */
		if(BioAPI_NO_LOCK == ModuleEntryLock)
		{
			bioapi_ReleaseModuleLock(pModule, LockForModule);
		}
	}
	else
	{
		/* error case - release all locks */
		bioapi_ReleaseModuleLock(pModule, LockForModule);
		bioapi_ReleaseModuleListLock(ModuleListLock);
	}


	/* Return the error code */
	return retCode;
}



BioAPI_RETURN
bioapi_FindAttachAndMultiLock(BioAPI_HANDLE hAttach,
							BioAPI_LOCK_TYPE ModuleListLock,
							bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
							BioAPI_LOCK_TYPE ModuleEntryLock,
							bioapi_INTERNAL_DEVICE_LIST_PTR *DeviceRecord,
							BioAPI_LOCK_TYPE DeviceEntryLock,
							bioapi_INTERNAL_ATTACH_LIST_PTR *ppAttach,
							BioAPI_LOCK_TYPE AttachEntryLock)
{
	BioAPI_RETURN retCode = BioAPI_OK;
	BioAPI_BOOL MatchFound;
	bioapi_INTERNAL_MODULE_LIST_PTR pModule = NULL;
	bioapi_INTERNAL_DEVICE_LIST_PTR pDevice = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR pAttach = NULL;
	BioAPI_LOCK_TYPE LockForDevice;

	/* Check return parameter */
	if(NULL == ppAttach)
	{
		return BioAPIERR_H_FRAMEWORK_INVALID_POINTER;
	}

	*ppAttach = NULL;

	/* We need at least a reader lock here, but that may not be what the caller
	wants to have returned.
	*/
	if(BioAPI_NO_LOCK != DeviceEntryLock)
	{
		LockForDevice = DeviceEntryLock;
	}
	else
	{
		LockForDevice = BioAPI_READER_LOCK;
	}

	/* Fetch the device entry - locked as case demands */
	retCode = bioapi_FindDeviceAndMultiLock(hAttach,
		ModuleListLock,
		&pModule,
		ModuleEntryLock,
		&pDevice,
		LockForDevice);
	if(BioAPI_OK != retCode)
	{
		return BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
	}

	/* Iterate down the list and find the record with a matching handle */
	pAttach = pDevice->AttachList;
	MatchFound = BioAPI_FALSE;
	while((NULL != pAttach) && (BioAPI_FALSE == MatchFound))
	{
		/* test to see if we match indices */
		if(pAttach->AttachHandle == hAttach)
		{
			MatchFound = BioAPI_TRUE;
		}
		else
		{
			pAttach = pAttach->Next;
		}
	}

	/* Did we find something? */
	if(NULL != pAttach)
	{
		/* Obtain the proper lock on the record */
		if(BioAPI_READER_LOCK == AttachEntryLock)
		{
			/* Get a reader lock on this entry in the list */
			if ((retCode = bioapi_ReaderLock (pAttach->hAttachSWMRLock)) == BioAPI_OK)
			{
				/* make sure another thread hasn't changed the index value on us */
				if(pAttach->AttachHandle != hAttach)
				{
					bioapi_ReaderUnlock(pAttach->hAttachSWMRLock);
					retCode = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
				}
			}
		}
		else
		{
			if(BioAPI_WRITER_LOCK == AttachEntryLock)
			{
				/* Get a writer lock on the record */
				if ((retCode = bioapi_WriterLock (pAttach->hAttachSWMRLock)) == BioAPI_OK)
				{
					/* make sure another thread hasn't changed the index value on us */
					if(pAttach->AttachHandle != hAttach)
					{
						bioapi_WriterUnlock(pAttach->hAttachSWMRLock);
						retCode = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
					}
				}
			}
		}
	}
	else
	{
		retCode = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
	}

	if(BioAPI_OK == retCode)
	{
		*ppAttach = pAttach;

		/* if they want a module record, we'll set it */
		if(NULL != ModuleRecord)
		{
			*ModuleRecord = pModule;
		}

		if(NULL != DeviceRecord)
		{
			*DeviceRecord = pDevice;
		}

		/* they may not want this entry locked */
		if(BioAPI_NO_LOCK == DeviceEntryLock)
		{
			bioapi_ReleaseDeviceLock(pDevice, LockForDevice);
		}

	}
	else
	{
		/* error case - release all locks */
		bioapi_ReleaseDeviceLock(pDevice, LockForDevice);
		bioapi_ReleaseModuleLock(pModule, ModuleEntryLock);
		bioapi_ReleaseModuleListLock(ModuleListLock);
	}

	/* Return the error code */
	return retCode;
}


void bioapi_ReleaseModuleLock(bioapi_INTERNAL_MODULE_LIST_PTR pModule,
							BioAPI_LOCK_TYPE ltLockType)
{
	if(BioAPI_READER_LOCK ==  ltLockType)
	{
		bioapi_ReaderUnlock(pModule->hModuleSWMRLock);
	}
	else
	{
		if(BioAPI_WRITER_LOCK ==  ltLockType)
		{
			bioapi_WriterUnlock(pModule->hModuleSWMRLock);
		}
	}
}


void bioapi_ReleaseDeviceLock(bioapi_INTERNAL_DEVICE_LIST_PTR pDevice,
							 BioAPI_LOCK_TYPE ltLockType)
{
	if(BioAPI_READER_LOCK == ltLockType)
	{
		bioapi_ReaderUnlock(pDevice->hDeviceSWMRLock);
	}
	else
	{
		if(BioAPI_WRITER_LOCK == ltLockType)
		{
			bioapi_WriterUnlock(pDevice->hDeviceSWMRLock);
		}
	}
}


void bioapi_ReleaseAttachLock(bioapi_INTERNAL_ATTACH_LIST_PTR pAttach,
							BioAPI_LOCK_TYPE ltLockType)
{
	if(BioAPI_READER_LOCK ==  ltLockType)
	{
		bioapi_ReaderUnlock(pAttach->hAttachSWMRLock);
	}
	else
	{
		if(BioAPI_WRITER_LOCK == ltLockType)
		{
			bioapi_WriterUnlock(pAttach->hAttachSWMRLock);
		}
	}
}


BioAPI_RETURN bioapi_GetModuleListLock(BioAPI_LOCK_TYPE ltLockType)
{
	BioAPI_RETURN retCode = BioAPI_OK;

	if(BioAPI_READER_LOCK == ltLockType)
	{
		retCode = bioapi_ReaderLock (hModuleListSWMRLock);
	}
	else
	{
		if(BioAPI_WRITER_LOCK ==  ltLockType)
		{
			retCode = bioapi_WriterLock (hModuleListSWMRLock);
		}
		else
		{
			if(BioAPI_NO_LOCK != ltLockType)
			{
				retCode = BioAPIERR_H_FRAMEWORK_INTERNAL_ERROR;
			}
		}
	}

	return retCode;
}


void bioapi_ReleaseModuleListLock(BioAPI_LOCK_TYPE ltLockType)
{
	if(BioAPI_READER_LOCK ==  ltLockType)
	{
		bioapi_ReaderUnlock(hModuleListSWMRLock);
	}
	else
	{
		if(BioAPI_WRITER_LOCK == ltLockType)
		{
			bioapi_WriterUnlock(hModuleListSWMRLock);
		}
	}
}



/*
 * bioapi_GetFunctionTable
 *
 * Description: This function takes a function table (FuncNameAddrPtr) and
 *		fills the addresses with the API level function address corresponding
 *		to the string name of the function.
 *
 * Parameters:
 *		FuncNameAddrPtr - the function table to be filled with function
 *							addresses
 *		NumOfFuncNameAddr - the number of entries in FuncNameAddrPtr
 *		hAddIn - Attach handle for the function table.	This is used to
 *					determine if we'll need to contact an EMM for filling the
 *					table.
 *
 * Returns:
 *		BioAPI_OK - Everything functioned correctly.
 *		Other error codes come from other functions and may or may not be
 *		fatal.
 */

BioAPI_RETURN
bioapi_GetFunctionTable(BioAPI_FUNC_NAME_ADDR_PTR FuncNameAddrPtr,
						uint32 NumOfFuncNameAddr,
						BioAPI_HANDLE hAddIn)
{
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	uint32 i;
	CSSM_RETURN err = CSSM_OK;

	ERR(err = bioapi_FindAttachAndMultiLock(hAddIn,
											BioAPI_NO_LOCK,	  /* global lock */
											NULL,		  /* module record */
											BioAPI_NO_LOCK,	  /* module lock */
											NULL,		 /* service record */
											BioAPI_NO_LOCK, /* service lock	 */
											&AttachRecord,
											BioAPI_READER_LOCK));
	if(BioAPI_OK == err)
	{
		/* since this is a regular addin, we can just give the function
		   pointers from this library
		*/
		for(i = 0; i < NumOfFuncNameAddr; i++)
		{
			ERR(port_GetProcAddress(HLayer_Library,
									FuncNameAddrPtr[i].Name,
									&(FuncNameAddrPtr[i].Address)));
		}

		bioapi_ReleaseAttachLock(AttachRecord,
								 BioAPI_READER_LOCK);
	}

	return err;
}

