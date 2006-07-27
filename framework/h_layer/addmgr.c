/*-----------------------------------------------------------------------
 * File: ADDMGR.C
 *
 *-----------------------------------------------------------------------
 */

/*
 * This file contains the code used to manage plug-in modules by the H Layer.
 */

#include "precomp.h"

/*
 * bioapi_NewModuleRecord
 *
 * Description: This function creates a new module record for a given UUID and
 *		places this record in the global module record list.  It is assumed
 *		that the caller does not hold a lock on the global list when this
 *		function is called.
 *
 *		The new module record is returned with a writer lock held.
 *
 *		This function considers it to be an error to call this function when
 *		a module with this UUID (same module) is already present in the global
 *		list.
 *
 * Parameters:
 *		Uuid (input) : UUID of the module to be added to the record list.
 *		NewModuleRecord (output) : The new record with the writer lock held.
 *
 * Returns:
 *		BioAPI_OK - success
 *		All other returns indicate error.
 */

BioAPI_RETURN BioAPI
bioapi_NewModuleRecord(const BioAPI_UUID *Uuid,
					 bioapi_INTERNAL_MODULE_LIST_PTR *NewModuleRecord)
{
	bioapi_INTERNAL_MODULE_LIST_PTR tempRecord = NULL;
	bioapi_INTERNAL_MODULE_LIST_PTR HandleSearcher = NULL;
	uint32 HandleIndex = 0;
	BioAPI_RETURN ret = BioAPI_OK;




	/* now we need to lock the global list */
	if( (ret = bioapi_GetModuleListLock(BioAPI_WRITER_LOCK)) == BioAPI_OK)
	{
		/* We need to check to make sure this module hasn't already been
		   loaded.
		*/
		tempRecord = hModuleListHead;

		while((NULL != tempRecord) &&
			  (BioAPI_OK == ret))
		{
			/* Test to see if the UUIDs match */
			if(BioAPI_TRUE == BioAPI_CompareUuids((*Uuid),
											  tempRecord->UUID))
			{
				ERR(ret = BioAPI_ERRCODE_INTERNAL_ERROR);
			}
			else
			{
				/* Move to the next record */
				tempRecord = tempRecord->Next;
			}
		}

		if(BioAPI_OK == ret)
		{
			/* Allocate memory for the new record */
			if ((tempRecord =
				(bioapi_INTERNAL_MODULE_LIST_PTR)local_calloc((uint32)sizeof(bioapi_INTERNAL_MODULE_LIST),
															1,
															0)) == NULL)
			{
				ERR(ret = BioAPI_ERRCODE_MEMORY_ERROR);
			}
			else
			{
				/* copy the input UUID */
				memcpy(&(tempRecord->UUID),
					   Uuid,
					   sizeof(BioAPI_UUID));

				/* generate index for this new record */

				/* is this the first record */
				if(NULL == hModuleListTail)
				{
					HandleSearcher = NULL;
					HandleIndex = 0;
				}
				else
				{
					/* if this isn't the rollover mark */
					if(0xFF800000 > bioapi_ExtractModuleIndex(hModuleListTail->ModuleHandleIndex))
					{
						HandleIndex = bioapi_ExtractModuleIndex(hModuleListTail->ModuleHandleIndex) + 0x00800000;
						HandleSearcher = hModuleListTail;
					}
					else
					{
						/* just making sure the first slot isn't available */
						if(0 != bioapi_ExtractModuleIndex(hModuleListHead->ModuleHandleIndex))
						{
							HandleSearcher = NULL;
							HandleIndex = 0;
						}
						else
						{
							/* Now we need to search the list in hipes of
							   finding an emtpy slot to insert this record in.
							*/
							HandleSearcher = hModuleListHead;
							while((NULL != HandleSearcher->Next) &&
								((bioapi_ExtractModuleIndex(HandleSearcher->ModuleHandleIndex) + 0x00800000) ==
								(bioapi_ExtractModuleIndex(HandleSearcher->Next->ModuleHandleIndex))))
							{
								HandleSearcher = HandleSearcher->Next;
							}

							/* make sure we found a spot to add things */
							if(HandleSearcher == hModuleListTail)
							{
								/* all 512 possible modules are loaded */
								ERR(ret = BioAPI_ERRCODE_INTERNAL_ERROR);
							}
							else
							{
								/* We are inserting after HandleSearcher,
								so we know the handle value.
								*/
								HandleIndex = bioapi_ExtractModuleIndex(HandleSearcher->ModuleHandleIndex) + 0x00800000;
							}
						}
					}
				}

				if(BioAPI_OK == ret)
				{
					/* assign the handle into the record entry */
					tempRecord->ModuleHandleIndex = HandleIndex;

					/* now we generate the SWMR lock for this record */
					if(ERR(ret = bioapi_SWMRInit(&tempRecord->hModuleSWMRLock)) == BioAPI_OK)
					{
						/* Now that we successfully have a new record, we'll
						   lock it, set the global list, and then return the
						   record for updating.
						*/
						if ((ret = bioapi_WriterLock (tempRecord->hModuleSWMRLock)) == BioAPI_OK)
						{
							/* now we insert the record into the list */
							if(NULL == HandleSearcher)
							{
								/* we're inserting at the head */
								tempRecord->Next = hModuleListHead;
								hModuleListHead = tempRecord;

								/* if this is the only record */
								if(NULL == hModuleListTail)
								{
									hModuleListTail = tempRecord;
								}
							}
							else
							{
								tempRecord->Next = HandleSearcher->Next;
								HandleSearcher->Next = tempRecord;

								/* check if we need to move the tail */
								if(hModuleListTail == HandleSearcher)
								{
									hModuleListTail = tempRecord;
								}
							}
						}

						if(BioAPI_OK != ret)
						{
							/* free up the lock memory - we know we don't
							   hold the writer lock here
							*/
							bioapi_SWMRDelete(tempRecord->hModuleSWMRLock);
						}
					}
				}

				if(BioAPI_OK != ret)
				{
					local_free(tempRecord,
							   NULL);
				}
			}
		}

		/* unlock the global lock whether we had success or not */
		bioapi_ReleaseModuleListLock(BioAPI_WRITER_LOCK);
	}

	if(BioAPI_OK == ret)
	{
		*NewModuleRecord = tempRecord;
	}

	return ret;
}


/*-----------------------------------------------------------------------
*
*Name: bioapi_InsertAttachRecord
*
*Description:
* Used to generate an attach record and insert it in the attach list
* contained in DeviceRecord.  Function also generates the random handle
* used to refer to this attach record.	Writer lock needs to be set on
* DeviceRecord by the calling function.	 The ModuleRecord needs to have
* at least a reader lock applied when this function is called.
*
*Parameters:
*	MemoryFunc: Application memory functions to be stored in the attach record
*	Version: BioAPI version passed at attach time - to be stored in attach record
*	DeviceType: type of device being attached to
*	AttachFlags: flags for attach - store in attach record
*	KeyHierarchy: key hierarchy of attach - stored in attach record
*	ModuleRecord: record for module,
*	DeviceRecord: record for device
*	NewAttachHandle: new attach value which is to be used
*	NewAttachRecord: new attach record
*
*Returns:
*	BioAPI_OK - Success
*	All other returns are errors
*---------------------------------------------------------------------*/
BioAPI_RETURN
bioapi_InsertAttachRecord(const BioAPI_MEMORY_FUNCS *MemoryFunc,
						const BioAPI_VERSION *Version,
						uint32 Reserved,
						uint32 Reserved1,
						uint32 Reserved2,
						const bioapi_INTERNAL_MODULE_LIST *ModuleRecord,
						bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord,
						BioAPI_HANDLE *NewAttachHandle,
						bioapi_INTERNAL_ATTACH_LIST_PTR *NewAttachRecord)
{
	bioapi_INTERNAL_ATTACH_LIST_PTR TempRecord;
	bioapi_INTERNAL_ATTACH_LIST_PTR ListIndexPtr;
	BioAPI_RETURN ret = BioAPI_OK;
	uint32 RandomBits = 0;
	uint32 tempNewHandle = 0;

	/* allocate memory for attach record */
	TempRecord = (bioapi_INTERNAL_ATTACH_LIST_PTR)local_calloc((uint32)sizeof(bioapi_INTERNAL_ATTACH_LIST),
															 1,
															 0);
	if(TempRecord == NULL)
	{
		return(BioAPI_ERRCODE_MEMORY_ERROR);
	}

	TempRecord->MemoryFunctions = (BioAPI_MEMORY_FUNCS_PTR)local_calloc((uint32)sizeof(BioAPI_MEMORY_FUNCS),
																		  1,
																		  0);
	if(TempRecord->MemoryFunctions == NULL)
	{
		local_free (TempRecord, 0);
		return (BioAPI_ERRCODE_MEMORY_ERROR);
	}

	/* Generate a handle for the library; make sure it is unique */
	while(0 == tempNewHandle)
	{
		RandomBits = bioapi_Rand();
		ListIndexPtr = DeviceRecord->AttachList;

		/* strip the random bits to the 17 bits needed for the attach record */
		tempNewHandle = bioapi_ExtractDeviceIndex(RandomBits) ^
			bioapi_ExtractAttachIndex(RandomBits);

		while(NULL != ListIndexPtr)
		{
			if ((ret = bioapi_ReaderLock (ListIndexPtr->hAttachSWMRLock)) == BioAPI_OK)
			{

				if(tempNewHandle == bioapi_ExtractAttachIndex(ListIndexPtr->AttachHandle))
				{
					RandomBits = 0;
					tempNewHandle = 0;
				}

				bioapi_ReaderUnlock(ListIndexPtr->hAttachSWMRLock);
			}
			else
			{
				local_free(TempRecord->MemoryFunctions, 0);
				local_free (TempRecord, 0);
				return(ret);
			}
			ListIndexPtr = ListIndexPtr->Next;
		}
	}

	/* copy the handle into the new record */
	TempRecord->AttachHandle = bioapi_ExtractModuleIndex(DeviceRecord->DeviceHandleIndex) |
		bioapi_ExtractDeviceIndex(DeviceRecord->DeviceHandleIndex) |
		tempNewHandle;

	/* copy app memory functions for the new record */
	TempRecord->MemoryFunctions->Malloc_func = MemoryFunc->Malloc_func;
	TempRecord->MemoryFunctions->Realloc_func = MemoryFunc->Realloc_func;
	TempRecord->MemoryFunctions->Free_func = MemoryFunc->Free_func;
	TempRecord->MemoryFunctions->Calloc_func = MemoryFunc->Calloc_func;
	TempRecord->MemoryFunctions->AllocRef = MemoryFunc->AllocRef;

    /* copy attach record options */
    TempRecord->Version = *Version;

	/*
	* Create the multiple-reader-single-write synchronization object.
	*/
	if((ret = bioapi_SWMRInit (&TempRecord->hAttachSWMRLock)) != BioAPI_OK)
	{
		local_free(TempRecord->MemoryFunctions, 0);
		local_free (TempRecord, 0);
		return (ret);
	}

	if ((ret = bioapi_WriterLock (TempRecord->hAttachSWMRLock)) != BioAPI_OK)
	{
		local_free(TempRecord->MemoryFunctions, 0);
		local_free(TempRecord, 0);
		return (ret);
	}

	TempRecord->AttachComplete = BioAPI_FALSE;

	*NewAttachHandle = TempRecord->AttachHandle;
	*NewAttachRecord = TempRecord;

	/* insert at the beginning of the attach list */
	if(NULL != DeviceRecord->AttachList)
		TempRecord->Next = DeviceRecord->AttachList;

	DeviceRecord->AttachList = TempRecord;


	return(BioAPI_OK);

}



/*-----------------------------------------------------------------------
*
*Name: bioapi_GetAppMemoryFunctions
*
*Description:
*	This function gets the application's memory function out of the attach
*	record.
*
*Parameters:
*	AttachRecord: The attach record to get the functions from
*	MemFuncs: The memory functions that were given when hAddIn was generated
*
*Returns:
*	BioAPI_OK: Success
*	BioAPI_ERRCODE_INTERNAL_ERROR: There weren't memory functions in the attach
*		record.	 This should've been caught earlier.
*---------------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_GetAppMemoryFunctions(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord,
						   BioAPI_MEMORY_FUNCS_PTR *MemFuncs)
{
	BioAPI_RETURN err = BioAPI_OK;

	if(NULL != AttachRecord->MemoryFunctions)
	{
		*MemFuncs = AttachRecord->MemoryFunctions;
	}
	else
	{
		ERR(err = BioAPI_ERRCODE_INTERNAL_ERROR);
	}

	return BioAPI_OK;
}



/*-----------------------------------------------------------------------
*
*Name: bioapi_GetAddInCallback
*
*Description:
* Interface function that allows an API function to get the callback
* into an AddIn given its info pointer.
*
*Parameters:
* ModuleInfo(input) - Pointer to the info for the addin.
*  the callback registered with the framework
*
*Returns:
* Pointer to the the structure containg the callback functions for the addin.
* NULL - if invalid addin
*---------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_GetAddInCallBack(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord,
							void** CallBack)
{
	BioAPI_RETURN ret = BioAPI_OK;



	if( (NULL != AttachRecord) && (NULL != CallBack) )
	{
		if(NULL != AttachRecord->AddInFunctions)
		{
                *CallBack = (void *)AttachRecord->AddInFunctions->ServiceFuncs;
		}

		if(NULL == *CallBack)
		{
			ret = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
		}

	}
	else
	{
		ret = BioAPI_ERRCODE_INVALID_POINTER;
	}

	return ret;
}



BioAPI_RETURN BioAPI
bioapi_AttachCommon( const BioAPI_UUID *ModuleUuid,
						 const BioAPI_VERSION *Version,
						 const BioAPI_MEMORY_FUNCS *MemoryFuncs,
						 uint32 DeviceID,
						 uint32 Reserved,
						 uint32 Reserved1,
						 uint32 Reserved2,
						 const BioAPI_UUID *CallerUuid,
						 BioAPI_FUNC_NAME_ADDR *FunctionTable,
						 uint32 NumFunctionTable,
						 const void * Reserved4,
						 BioAPI_HANDLE_PTR NewModuleHandle,
						 bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
						 bioapi_INTERNAL_DEVICE_LIST_PTR *DeviceRecord,
						 bioapi_INTERNAL_ATTACH_LIST_PTR *AttachRecord)
{
	BioAPI_RETURN dwRtn = BioAPI_OK;


	/*
	* Get the writer lock on the correct device list entry
	*/
	if((dwRtn = bioapi_FindDeviceByUUIDAndDeviceAndMultiLock(ModuleUuid,
		DeviceID,
		BioAPI_NO_LOCK,
		ModuleRecord,
		BioAPI_READER_LOCK,
		DeviceRecord,
		BioAPI_WRITER_LOCK)) != BioAPI_OK)
	{
		return (dwRtn);
	}

	/* create an entry in the attach list for this device record */
	if((dwRtn = bioapi_InsertAttachRecord(MemoryFuncs,
		Version,
		Reserved,
		Reserved1,
		Reserved2,
		*ModuleRecord,
		*DeviceRecord,
		NewModuleHandle,
		AttachRecord)) != BioAPI_OK)
	{
		bioapi_ReleaseModuleLock(*ModuleRecord,
			BioAPI_READER_LOCK);
		bioapi_ReleaseDeviceLock(*DeviceRecord,
			BioAPI_WRITER_LOCK);
		return (dwRtn);
	}

	/* we're done with the module record now so we release the lock */
	bioapi_ReleaseModuleLock(*ModuleRecord,
		BioAPI_READER_LOCK);
	return (dwRtn);
}


BioAPI_RETURN BioAPI
bioapi_ModuleAttachNoInteg(const BioAPI_UUID *ModuleUuid,
						 const BioAPI_VERSION *Version,
						 const BioAPI_MEMORY_FUNCS *MemoryFuncs,
						 uint32 DeviceID,
						 uint32 Reserved,
						 uint32 Reserved1,
						 /* Linux-Port: replaced void * with BioAPI_UUID */ 
						 const BioAPI_UUID *CallerUuid,
						 //const void * CallerUuid,
						 uint32 Reserved2,
						 BioAPI_FUNC_NAME_ADDR *FunctionTable,
						 uint32 NumFunctionTable,
						 const void * Reserved3,
						 BioAPI_HANDLE_PTR NewModuleHandle)
{
	bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord = NULL;
	bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN dwRtn = BioAPI_OK;


	ERR(dwRtn = bioapi_AttachCommon(ModuleUuid,
								  Version,
								  MemoryFuncs,
								  DeviceID,
								  Reserved,
								  Reserved1,
								  Reserved2,
								  CallerUuid,
								  FunctionTable,
								  NumFunctionTable,
								  Reserved3,
								  NewModuleHandle,
								  &ModuleRecord,
								  &DeviceRecord,
								  &AttachRecord));

	if(BioAPI_OK == dwRtn)
	{
		ERR(dwRtn = bioapi_AttachModuleNoKeys(ModuleUuid,
											Version,
											DeviceID,
											Reserved,
											Reserved1,
											*NewModuleHandle,
											Reserved2,
											CallerUuid,
											&AttachRecord->AddInFunctions,
											AttachRecord,
											DeviceRecord));

		if(BioAPI_OK != dwRtn)
		{
			/* We've failed the attach so we'll remove the attach record */
			bioapi_CleanInternalAttachRecord(DeviceRecord,
										   &AttachRecord);
		}

		/* At this point we know we're done with the device record, so we'll
		   unlock it here.
		*/
		bioapi_ReleaseDeviceLock(DeviceRecord,
								BioAPI_WRITER_LOCK);

		if(BioAPI_OK == dwRtn)
		{
			/* We've successfully completed all attach requirements - unless we
			   use KR.	In that case, the attach lock will have to be
			   re-obtained.
			*/

			AttachRecord->AttachComplete = BioAPI_TRUE;
			bioapi_ReleaseAttachLock(AttachRecord,
								   BioAPI_WRITER_LOCK);

			/* If the caller wants a function table back, this is where we set
			   it.
			*/
			if((NULL != FunctionTable) &&
			   (0 != NumFunctionTable))
			{
				bioapi_GetFunctionTable(FunctionTable,
									  NumFunctionTable,
									  *NewModuleHandle);
			}
		}
	}


	return (dwRtn);
}


/*---------------------------------------------------------------
*
*Name: bioapi_ModuleAttach
*
*Description:
*	This function is called from the BioAPI_ModuleAttach api in bioapi.c.
*	If it is the first attach, it will add a ned addin module record, do
*	the integrity check of the module as well as cross check and do the
*	initializtion of the module. If it is an emm addin module, the emm
*	 manager module is loaded before the addin module load.
*	If it is not th efirst time attach, then a new addin handle is generated
*	and a new HandleInfo is added to the internal add in module record list.
*
*
*Parameters:
*	pModuleUUID - uuid of the add-in module
*	Version - version number of requested add-in
*	pMemoryFunc - application's memory functions
*	DeviceID - DeviceID
*	DeviceFlags - DeviceFlags
*	Application - Application context nonce for use with the callback
*	Notification - Callback used to tell the application when an
*				   add-in event takes place.
*	CallerUuid - Uuid of the calling application
*	USEERequest - USEE exemption request from the app
*	eventType - Event code passed to the add-in after attach
*	detachEventType - Event code passed to the add-in when a load
*					  failure occures.
*
*Returns:
*  0 - unable to attach to the add-in module
*  non 0 - handle to the add-in module
*
*----------------------------------------------------------------*/
BioAPI_RETURN
bioapi_ModuleAttach(const BioAPI_UUID *ModuleUuid,
				  const BioAPI_VERSION *Version,
				  const BioAPI_MEMORY_FUNCS *MemoryFuncs,
				  BioAPI_DEVICE_ID DeviceID,
				  uint32 Reserved,
				  uint32 Reserved1,
				  uint32 Reserved2,
				  BioAPI_FUNC_NAME_ADDR_PTR FunctionTable,
				  uint32 NumFunctionTable,
				  const void * Reserved3,
				  BioAPI_HANDLE_PTR NewModuleHandle,
					/* Linux-Port: replaced void * with BioAPI_UUID */
				  const BioAPI_UUID *CallerUuid)
				  //const void * CallerUuid)
{
	BioAPI_RETURN ret = BioAPI_OK;

	ret = bioapi_ModuleAttachNoInteg( ModuleUuid,
									  Version,
									  MemoryFuncs,
									  DeviceID,
									  Reserved,
									  Reserved1,
									  CallerUuid,
									  Reserved2,
									  FunctionTable,
									  NumFunctionTable,
									  Reserved3,
									  NewModuleHandle);

	return (ret);
}


/*---------------------------------------------------------------
*
*Name: bioapi_CleanInternalModuleRecord
*
*Description:
*	Deletes frees the internal structures under a module record.
*	This routine is called with a writer lock on the ModuleRecord.
*	The writer lock on the global module list should also be held.
*	This function will not release the writer lock on the ModuleRecord.
*
*Parameters:
*	ModuleRecord - the record from which everything should be cleaned.
*
*Returns:
*  BioAPI_FAIL - unable to delete internal add-in record
*  BioAPI_OK - add-in record is removed
*
*----------------------------------------------------------------*/
BioAPI_RETURN BioAPI
bioapi_CleanInternalModuleRecord(bioapi_INTERNAL_MODULE_LIST_PTR *ModuleRecord,
							   CSSM_LIB_HANDLE *MainLibHandle)
{
	bioapi_INTERNAL_MODULE_LIST_PTR LeadSearcher = NULL;
	bioapi_INTERNAL_MODULE_LIST_PTR TrailSearcher = NULL;
	bioapi_INTERNAL_DEVICE_LIST_PTR TempDevice = NULL;
	BioAPI_RETURN err = BioAPI_OK;



	if(NULL != ModuleRecord)
		LeadSearcher = hModuleListHead;

	/* first, let's take our entry out of the global list */
	while((NULL != LeadSearcher) && (ModuleRecord != NULL) &&
		(LeadSearcher->ModuleHandleIndex != (*ModuleRecord)->ModuleHandleIndex))
	{
		TrailSearcher = LeadSearcher;
		LeadSearcher = LeadSearcher->Next;
	}

	/* check if we've found the ModuleRecord */
	if(NULL != LeadSearcher)
	{
		/* update the global list */
		if(NULL == TrailSearcher)
		{
			hModuleListHead = LeadSearcher->Next;

			if(LeadSearcher == hModuleListTail)
			{
				hModuleListTail = NULL;
			}
		}
		else
		{
			TrailSearcher->Next = LeadSearcher->Next;

			if(LeadSearcher == hModuleListTail)
			{
				hModuleListTail = TrailSearcher;
			}
		}

		/* We've now remove this entry from the global list.  It's now time to,
		get rid of any and all device entries under this module record.
		*/
		if(NULL != ModuleRecord)
		{
			TempDevice = (*ModuleRecord)->DeviceList;
			while(NULL != TempDevice)
			{
				bioapi_WriterLock (TempDevice->hDeviceSWMRLock);
				if(ModuleRecord != NULL)
				{
					bioapi_CleanInternalDeviceRecord(*ModuleRecord,
													&TempDevice);
					TempDevice = (*ModuleRecord)->DeviceList;
				}
			}

			*MainLibHandle = (*ModuleRecord)->LibHandle;

			/* free locks */
			if(NULL != (*ModuleRecord)->hModuleSWMRLock)
			{
				bioapi_WriterUnlock((*ModuleRecord)->hModuleSWMRLock);
				bioapi_SWMRDelete((*ModuleRecord)->hModuleSWMRLock);
			}


			/* free the last of the memory */
			local_free(*ModuleRecord,0);
			*ModuleRecord = NULL;
		}
	}
	else
	{
		ERR(err = BioAPI_ERRCODE_INTERNAL_ERROR);
	}

	return (err);
}


void BioAPI
bioapi_CleanInternalDeviceRecord(bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord,
								bioapi_INTERNAL_DEVICE_LIST_PTR *DeviceRecord)
{
	bioapi_INTERNAL_DEVICE_LIST_PTR LeadSearcher = NULL;
	bioapi_INTERNAL_DEVICE_LIST_PTR TrailSearcher = NULL;
	bioapi_INTERNAL_DEVICE_LIST_PTR TempDevice = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR TempAttach = NULL;

	if(NULL != DeviceRecord)
		LeadSearcher = ModuleRecord->DeviceList;

	/* first, let's take our entry out of the global list */
	while((NULL != LeadSearcher) && (DeviceRecord != NULL) &&
		(LeadSearcher->DeviceHandleIndex != (*DeviceRecord)->DeviceHandleIndex))
	{
		TrailSearcher = LeadSearcher;
		LeadSearcher = LeadSearcher->Next;
	}

	/* check if we've found the ModuleRecord */
	if(NULL != LeadSearcher)
	{
		/* update the global list */
		if(NULL == TrailSearcher)
		{
			ModuleRecord->DeviceList = LeadSearcher->Next;

			if(LeadSearcher == ModuleRecord->DeviceList)
			{
				ModuleRecord->DeviceList = NULL;
			}
		}
		else
		{
			TrailSearcher->Next = LeadSearcher->Next;

			if(LeadSearcher == TrailSearcher)
			{
				ModuleRecord->DeviceList = TrailSearcher;
			}
		}
	}
	if(DeviceRecord != NULL)
	{
		TempDevice = *DeviceRecord;
		if ( TempDevice != NULL )
		{
			TempAttach = TempDevice->AttachList;
			while(NULL != TempAttach)
			{
				bioapi_WriterLock (TempAttach->hAttachSWMRLock);

				bioapi_CleanInternalAttachRecord( TempDevice,
					&TempAttach);
				TempAttach = TempDevice->AttachList;
			}

			if(TempDevice->hDeviceSWMRLock != NULL)
			{
				bioapi_WriterUnlock(TempDevice->hDeviceSWMRLock);
				bioapi_SWMRDelete(TempDevice->hDeviceSWMRLock);
			}
			local_free( TempDevice, NULL);
		}
	}
}


void BioAPI
bioapi_CleanInternalAttachRecord(bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord,
							   bioapi_INTERNAL_ATTACH_LIST_PTR *AttachRecord)
{
	bioapi_INTERNAL_ATTACH_LIST_PTR LeadSearcher = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR TrailSearcher = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR TempAttach = NULL;

	if((NULL != AttachRecord) &&
	   (NULL != DeviceRecord))
		LeadSearcher = DeviceRecord->AttachList;

	/* first, let's take our entry out of the global list */
	while((NULL != LeadSearcher) && (AttachRecord != NULL) &&
		(LeadSearcher->AttachHandle != (*AttachRecord)->AttachHandle))
	{
		TrailSearcher = LeadSearcher;
		LeadSearcher = LeadSearcher->Next;
	}

	/* check if we've found the ModuleRecord */
	if(NULL != LeadSearcher)
	{
		/* update the global list */
		if(NULL == TrailSearcher)
		{
			DeviceRecord->AttachList = LeadSearcher->Next;

			if(LeadSearcher == DeviceRecord->AttachList)
			{
				DeviceRecord->AttachList = NULL;
			}
		}
		else
		{
			TrailSearcher->Next = LeadSearcher->Next;

			if(LeadSearcher == TrailSearcher)
			{
				DeviceRecord->AttachList = TrailSearcher;
			}
		}
	}
	if( AttachRecord != NULL)
	{
		TempAttach = *AttachRecord;
		if ( TempAttach != NULL)


		{

			/* now we call the SPI_ModuleDetach for this addin */
			if(BioAPI_TRUE == TempAttach->AttachComplete)
				bioapi_ModuleDetach(TempAttach,DeviceRecord);

			if( TempAttach->MemoryFunctions != NULL)
				local_free( TempAttach->MemoryFunctions, NULL);
			if( TempAttach->AddInFunctions != NULL)
			{
				if( TempAttach->AddInFunctions->ServiceFuncs != NULL)
					local_free( (void *)TempAttach->AddInFunctions->ServiceFuncs, NULL );
				local_free( TempAttach->AddInFunctions, NULL);
			}
			if(NULL != TempAttach->hAttachSWMRLock)
			{
				bioapi_WriterUnlock(TempAttach->hAttachSWMRLock);
				bioapi_SWMRDelete(TempAttach->hAttachSWMRLock);
			}
			local_free( TempAttach, NULL);
		}
	}
}



BioAPI_RETURN
bioapi_GetAttachFunctions(BioAPI_HANDLE hAddIn,
						uint32 Reserved,
						void **CallBack,
						BioAPI_UUID_PTR Uuid)
{
	BioAPI_RETURN Ret = BioAPI_OK;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord;
	bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord;

	if(NULL == CallBack)
	{
		Ret = BioAPI_ERRCODE_INVALID_POINTER;
	}
	else
	{
		if(( Ret = bioapi_FindAttachAndMultiLock(hAddIn,
			BioAPI_NO_LOCK,
			NULL,
			BioAPI_NO_LOCK,
			NULL,
			BioAPI_NO_LOCK,
			&AttachRecord,
			BioAPI_READER_LOCK)) == BioAPI_OK)
		{
			Ret = bioapi_GetAddInCallBack(AttachRecord, CallBack);
		}
		else
		{
			/* the search didn't find anything using this handle */
			Ret = BioAPI_ERRCODE_INTERNAL_ERROR;
		}

		/* If everything's ok and the user wants to know if it's threadsafe,
		we find out.
		*/
		if( (BioAPI_OK == Ret) &&  (NULL != Uuid) )
		{
			if(BioAPI_OK == bioapi_FindModuleAndMultiLock(hAddIn,
				BioAPI_NO_LOCK,
				&ModuleRecord,
				BioAPI_READER_LOCK))
			{
				/* copy the UUID, if it's desired */
				if(NULL != Uuid)
				{
					/* copy the UUID */
					port_memcpy ( Uuid, ModuleRecord->UUID, sizeof(BioAPI_UUID));
				}

				bioapi_ReleaseModuleLock(ModuleRecord,
					BioAPI_READER_LOCK);
			}
			else
			{
				bioapi_ReleaseAttachLock(AttachRecord,
					BioAPI_READER_LOCK);
				Ret = BioAPI_ERRCODE_INTERNAL_ERROR;
			}
		}
	}

	return Ret;
}


void
bioapi_ReleaseAttachFunctions(BioAPI_HANDLE hAddIn)
{
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord;

	if(BioAPI_OK == bioapi_FindAttachAndMultiLock(hAddIn,
		BioAPI_NO_LOCK,
		NULL,
		BioAPI_NO_LOCK,
		NULL,
		BioAPI_NO_LOCK,
		&AttachRecord,
		BioAPI_READER_LOCK))
	{
	/* We now need to free 2 reader locks - the one we just got and the
	one we were initially called to free.
		*/
		bioapi_ReleaseAttachLock(AttachRecord,
			BioAPI_READER_LOCK);
		bioapi_ReleaseAttachLock(AttachRecord,
			BioAPI_READER_LOCK);
	}
}



/*-----------------------------------------------------------------------
*
*Name: bioapi_ModuleDetach
*
*Description:
* This function assumes a lock has been set on AttachRecord and calls the
* addin module's SPI_ModuleDetach.
*Parameters:
* AttachRecord (input/output): record for the attach instance which we
*							   are going to detach.
*
*Returns:
* BioAPI_OK - Module detached.
* BioAPI_FAIL - in case of failures.
*---------------------------------------------------------------------*/
BioAPI_RETURN
bioapi_ModuleDetach(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord,bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord)
{
	BioAPI_RETURN dwRtn = BioAPI_OK;
	SPI_DETACH_FUNC_PTR DetachFunc = NULL;
	char szModuleDetach[] = "BioSPI_ModuleDetach";


	/* Check if we have an attach record - if so, we assume it already has
	a writer lock set.
	*/
	if(NULL != AttachRecord)
	{
	/* Make sure the attach completed and we actually have to call
	detach.	 We return BioAPI_OK even if this call was unecessary.
		*/
		if(BioAPI_TRUE == AttachRecord->AttachComplete)
		{
			/* get the function pointer */
			dwRtn = port_GetProcAddress(DeviceRecord->ModuleLibHandle,
										szModuleDetach,
										(BioAPI_PROC_ADDR*)&DetachFunc);

			/* make sure we actually got a function */
			if( (dwRtn == BioAPI_OK) && (DetachFunc != NULL) )
			{
				dwRtn = DetachFunc(AttachRecord->AttachHandle);
				AttachRecord->AttachComplete = BioAPI_FALSE;
			}
			else
			{
				dwRtn = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED;
			}
		}
	}

	return (dwRtn);
}


/*---------------------------------------------------------------
*
*Name: bioapi_GetUUIDFromHandle
*
*Description:
*  Locate the UUID of the add-in module given a handle.
*
*Parameters:
*  ModuleHandle - Handle to module assigned by BioAPI
*
*Returns:
*  Non NULL - pointer to the UUID structure
*  NULL - error in retreiving the UUID
*
*----------------------------------------------------------------*/
BioAPI_RETURN BioAPI
bioapi_GetUUIDFromHandle(BioAPI_HANDLE ModuleHandle, BioAPI_UUID_PTR UuidPtr)
{
	bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord;

	if(port_IsBadWritePtr(UuidPtr, sizeof(BioAPI_UUID)))
	{
		return BioAPI_ERRCODE_INVALID_POINTER;
	}

	/*
	* Get the ModuleInfo record associated with the handle.	 We search down
	* to the attach record to make sure the handle is valid even though the
	* information we need is actually just in the ModuleRecord
	*/
	if(BioAPI_OK != bioapi_FindAttachAndMultiLock(ModuleHandle,
											  BioAPI_NO_LOCK,
											  &ModuleRecord,
											  BioAPI_READER_LOCK,
											  NULL,
											  BioAPI_NO_LOCK,
											  &AttachRecord,
											  BioAPI_NO_LOCK))
	{
		port_memset ( UuidPtr, 0, sizeof(BioAPI_UUID));
		return BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE;
	}
	else
	{
		/* copy the uuid to return pointer */
		port_memcpy ( UuidPtr, &ModuleRecord->UUID, sizeof(BioAPI_UUID));

		/*
		* Release the Module Record.
		*/
		bioapi_ReleaseModuleLock(ModuleRecord,
			BioAPI_READER_LOCK);
	}

	return BioAPI_OK;
}


/*---------------------------------------------------------------
*
*Name: bioapi_QueryDevice
*
*Description:
*  Return the unique identifier of the attached module subservice, as identified
*	by the input handle.  .
*
*Parameters:
*  ModuleHandle - Handle to module assigned by BioAPI
*	MemoryFuncs - memory functions to be used for allocation.
*
*Returns:
*  Non NULL - pointer to the SERVICE_UID structure
*  NULL - error in retreiving the device UID
*
*----------------------------------------------------------------*/
BioAPI_RETURN
bioapi_QueryDevice(BioAPI_HANDLE ModuleHandle,
								BioAPI_SERVICE_UID_PTR UidPtr)
{
	bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord = NULL;
	bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN rc = BioAPI_OK;

	if((UidPtr == NULL ) ||
	   (port_IsBadWritePtr( UidPtr, sizeof( BioAPI_SERVICE_UID))) )
	{
		ERR(rc = BioAPI_ERRCODE_INVALID_POINTER);
	}
	else
	{
		/*
		 * Get the module list record associated with the handle.
		 */
		if(ERR(rc = bioapi_FindAttachAndMultiLock(ModuleHandle,
												BioAPI_NO_LOCK,
												&ModuleRecord,
												BioAPI_READER_LOCK,
												&DeviceRecord,
												BioAPI_READER_LOCK,
												&AttachRecord,
												BioAPI_READER_LOCK)) == BioAPI_OK)
		{
			port_memcpy( UidPtr->Uuid, &ModuleRecord->UUID, sizeof(BioAPI_UUID));
			UidPtr->Version = AttachRecord->Version;
			UidPtr->DeviceId = DeviceRecord->DeviceID;

			/* release module record */
			bioapi_ReleaseModuleLock(ModuleRecord,
								   BioAPI_READER_LOCK);

			/* release device record */
			bioapi_ReleaseDeviceLock(DeviceRecord,
								   BioAPI_READER_LOCK);

			/* release attach record */
			bioapi_ReleaseAttachLock(AttachRecord,
								   BioAPI_READER_LOCK);
		}
		else
		{
			ERR(rc = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_HANDLE);
		}
	}

	return rc;
}


/*---------------------------------------------------------------
*
*Name: bioapi_CleanUp
*
*Description:
*  Clean up internal structures.  This function is called just
*  before the framework is unloaded.  Fix memory leaks problems
*  created by ill behaved addins and apps.
*
*Parameters:
*  none
*
*Returns:
*  none
*
*----------------------------------------------------------------*/
BioAPI_RETURN
bioapi_CleanUp (void)
{
	bioapi_INTERNAL_MODULE_LIST_PTR TempModule = NULL;
	UNLOAD_MODULE_FUNC_PTR SPI_Unload_Func = NULL;
	char szModuleUnload[] = "BioSPI_ModuleUnload";
	BioAPI_RETURN rc = BioAPI_OK;
	CSSM_LIB_HANDLE MainLibHandle;

	if(BioAPI_OK == bioapi_GetModuleListLock(BioAPI_WRITER_LOCK))
	{
		while(NULL != hModuleListHead)
		{
			TempModule = hModuleListHead;

			/* lock the record we'r about to delete */
			if (bioapi_WriterLock (TempModule->hModuleSWMRLock) == BioAPI_OK)
			{
				/* Linux-Port: added the const keyword */
				const BioAPI_UUID tmpUuid;
				
				memcpy(&tmpUuid, &TempModule->UUID, sizeof(BioAPI_UUID));

				/* Delete everything under the record - the function will move
				   hModuleListHead for us.	We'll also get the lib handle for
				   the module that implements SPI_ModuleUnload
				*/
				/**********************************************************/

				ERR(rc = bioapi_CleanInternalModuleRecord(&TempModule,
														&MainLibHandle));

				if(BioAPI_OK == rc)
				{
						rc = port_GetProcAddress(MainLibHandle,
												 szModuleUnload,
												 (BioAPI_PROC_ADDR *)&SPI_Unload_Func);

						if((BioAPI_OK == rc) &&
						   (NULL != SPI_Unload_Func))
						{
							/* We've taken the record out of the internal
							   lists, now we'll call the addin to notify it
							   of the unload.
							*/
							ERR(rc = SPI_Unload_Func(NULL,
													 &tmpUuid,
													 (BioSPI_ModuleEventHandler)bioapi_ModuleEventHandler,
													 NULL));


						}
				}
				else
				{
					rc = BioAPI_ERRCODE_INTERNAL_ERROR;
				}


				/************************************************************/
				ERR(port_FreeLibrary(MainLibHandle));
			}
		}

		/* delete the global module list lock */
		bioapi_WriterUnlock(hModuleListSWMRLock);
		bioapi_SWMRDelete(hModuleListSWMRLock);
	}

	/* re-init these variables in case they are somehow used */
	hModuleListSWMRLock = NULL;
	hModuleListTail = NULL;

	return (rc);
}


/*-----------------------------------------------------------------------
*
*Name: bioapi_GetAttachRecordAndLockMultiTask
*
*Description:
*	This function gets the attach infomation record given the module handle.
*	It locks the record for reading and returns a pointer to the attach info
*	and callback functions for the module. It also locks the multitask
*	mutex for the addin.
*
*Parameters:
* hAddIn(input) - Module handle of library to query
* CallBack(output) - Pointer to a function pointer that will hold
*  the callback registered with the BIOAPI.
*
*Returns:
* NULL - In case of failures.
* NON-NULL - Pointer to the module information for the addin.
*---------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_GetAttachRecordAndLockMultiTask(BioAPI_HANDLE hAddIn,
									 void **CallBack,
									 bioapi_INTERNAL_ATTACH_LIST_PTR *AttachInfo)
{
	BioAPI_RETURN err = BioAPI_OK;

	if((NULL == AttachInfo) ||
	   (port_IsBadWritePtr(AttachInfo,
						   (uint32)sizeof(bioapi_INTERNAL_ATTACH_LIST_PTR))) ||
	   (NULL == CallBack))
	{
		ERR(err = BioAPI_ERRCODE_INVALID_POINTER);
	}
	else
	{

		/*
		 * Get the moduleinfo record for the addin.
		 */

		if(ERR(err = bioapi_FindAttachAndMultiLock(hAddIn,
												 BioAPI_NO_LOCK,
												 NULL,
												 BioAPI_NO_LOCK,
												 NULL,
												 BioAPI_NO_LOCK,
												 AttachInfo,
												 BioAPI_READER_LOCK)) == BioAPI_OK)
		{
			/*
			 * Get the call back for this record using the AddinType.
			 */
			ERR(err	 = bioapi_GetAddInCallBack(*AttachInfo,
												   CallBack));

			if(BioAPI_OK != err)
			{
				/* error case, so we don't return values to caller */
				bioapi_ReleaseAttachLock(*AttachInfo,
									   BioAPI_READER_LOCK);
				*AttachInfo = NULL;
				*CallBack = NULL;
			}
		}
	}

	return (err);
}



BioAPI_RETURN BioAPI
bioapi_ReleaseAttachRecordAndUnlockMultiTask(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord)
{
	BioAPI_RETURN rc = BioAPI_OK;
	/* just to make sure we don't walk a NULL pointer */
	if(NULL != AttachRecord)
	{
		bioapi_ReleaseAttachLock(AttachRecord,
			BioAPI_READER_LOCK);
		return ( rc );
	}
	else
		return (BioAPI_ERRCODE_INVALID_POINTER );
}

/*-----------------------------------------------------------------------
*
*Name: bioapi_ReaderLock
*
*Description:
*	This routine locks the SWMRLock lock for reading.
*
*Parameters:
* SWMRLock(input) - Single-writer-multiple-reader lock to be locked for reading.
*
*Returns:
* BioAPI_TRUE - SWMRLock is locked.
* BioAPI_FALSE - Unable to lock SWMRLock.
*---------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_ReaderLock (cssm_SWMRLock_PTR SWMRLock)
{
	BioAPI_RETURN rc = BioAPI_OK;

	/*
	* Get the Reader lock on the Module Record.
	*/
	if (SWMRLock != NULL)
	{
		rc = cssm_SWMRLockWaitToRead (SWMRLock, CSSM_INFINITE_WAIT);
		return (rc);
	} else
	{
			return (BioAPI_ERRCODE_INVALID_POINTER);
		}
}

/*-----------------------------------------------------------------------
*
*Name: bioapi_ReaderUnlock
*
*Description:
*	This routine locks the SWMRLock lock for reading.
*
*Parameters:
* SWMRLock(input) - Single-writer-multiple-reader lock to be unlocked.
*
*Returns:
* void
*---------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_ReaderUnlock (cssm_SWMRLock_PTR SWMRLock)
{
	BioAPI_RETURN rc = BioAPI_OK;


	/*
	* Release the reader lock.
	*/
	if (SWMRLock != NULL)
		rc = cssm_SWMRLockDoneReading (SWMRLock);
	else
		rc = BioAPI_ERRCODE_INVALID_POINTER;
	return (rc);
}


/*-----------------------------------------------------------------------
*
*Name: bioapi_WriterLock
*
*Description:
*	This routine locks the SWMRLock lock for writing.
*
*Parameters:
* SWMRLock(input) - Single-writer-multiple-reader lock to be locked for writing.
*
*Returns:
* BioAPI_TRUE - SWMRLock is locked.
* BioAPI_FALSE - Unable to lock SWMRLock.
*---------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_WriterLock (cssm_SWMRLock_PTR SWMRLock)
{
	BioAPI_RETURN rc = BioAPI_OK;

	/*
	* Get the Writer lock on the Module Record.
	*/
	if (SWMRLock != NULL)
	{
		rc = cssm_SWMRLockWaitToWrite (SWMRLock, CSSM_INFINITE_WAIT);
		return (rc);
	}
	else
	{
		return (BioAPI_ERRCODE_INVALID_POINTER);
	}
}

/*-----------------------------------------------------------------------
*
*Name: bioapi_WriterUnlock
*
*Description:
*	This routine locks the SWMRLock lock for writing.
*
*Parameters:
* SWMRLock(input) - Single-writer-multiple-reader lock to be unlocked.
*
*Returns:
* void
*---------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_WriterUnlock (cssm_SWMRLock_PTR SWMRLock)
{
	BioAPI_RETURN rc = BioAPI_OK;

	/*
	* Release the writer lock.
	*/
	if (SWMRLock != NULL)
		rc = cssm_SWMRLockDoneWriting (SWMRLock);
	else
		rc = BioAPI_ERRCODE_INVALID_POINTER;
	return (rc);
}

/*-----------------------------------------------------------------------
*
*Name: bioapi_WriterToReader
*
*Description:
*	This routine changes a writer lock to the reader lock, so that no other
*	writers can get in before getting the reader lock,
*
*Parameters:
* SWMRLock(input) - Single-writer-multiple-reader lock to be locked for writing.
*
*Returns:
* BioAPI_TRUE - SWMRLock is locked.
* BioAPI_FALSE - Unable to lock SWMRLock.
*---------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_WriterToReader (cssm_SWMRLock_PTR SWMRLock)
{
	BioAPI_RETURN ret = BioAPI_OK;

	/*
	* Get the Writer lock on the Module Record.
	*/
	if (SWMRLock != NULL)
	{
		ret = cssm_SWMRLockWriteToRead (SWMRLock);
		return (ret);
	}
	else
	{
		return ( BioAPI_ERRCODE_INVALID_POINTER );
	}

}

/*-----------------------------------------------------------------------
*
*Name: bioapi_SWMRInit
*
*Description:
*	This routine creates and intializes the SWMRLock.
*
*Parameters:
* SWMRLock(input) - Single-writer-multiple-reader lock to be creates/intialized.
*
*Returns:
* BioAPI_TRUE - On succesful intialization
* BioAPI_FAIL - In case of failures.
*---------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_SWMRInit(cssm_SWMRLock_PTR *SWMRLock)
{
	BioAPI_RETURN err = BioAPI_OK;

	/*
	* Create the multiple-reader-single-write synchronization object.
	*/
	if(NULL != SWMRLock)
	{
		if(((*SWMRLock) = (cssm_SWMRLock_PTR)local_malloc((uint32)sizeof(cssm_SWMRLock),
														  NULL)) == NULL)
		{
			ERR(err = BioAPIERR_H_FRAMEWORK_MEMORY_ERROR);
		}
		else
		{
			/* create the lock */
			if(ERR(err = cssm_SWMRLockCreate(*SWMRLock,
											 NULL)) != BioAPI_OK)
			{
				local_free((*SWMRLock), NULL);
			}
		}
	}
	return (err);
}


/*-----------------------------------------------------------------------
*
*Name: bioapi_SWMRDelete
*
*Description:
*	This routine creates and intializes the SWMRLock.
*
*Parameters:
* SWMRLock(input) - Single-writer-multiple-reader lock to be deleted.
*
*Returns:
* none
*---------------------------------------------------------------------*/

BioAPI_RETURN BioAPI
bioapi_SWMRDelete ( cssm_SWMRLock_PTR SWMRLock )

{
/*
* Delete the multiple-reader-single-write synchronization object.
	*/
	if ( SWMRLock != NULL )
	{
		/* we want unnamed locks */
		cssm_SWMRLockDelete( SWMRLock );
		local_free( SWMRLock, NULL );
		return (BioAPI_OK);
	}
	else
		return (BioAPI_ERRCODE_INVALID_POINTER);
}


BioAPI_RETURN
bioapi_DeleteModuleRecordonLoadFail(const BioAPI_UUID *ModuleUuid,
								  BioAPI_ModuleEventHandler AppNotifyCallback,
								  const void* AppNotifyCallbackCtx)
{
	BioAPI_RETURN Ret = BioAPI_OK;
	bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord = NULL;
	CSSM_LIB_HANDLE MainLibHandle;


	if(NULL != ModuleUuid)
	{
		/* Even if the module is not found in the caller list, it is OK, because introduce may not
		have been called at this point.
		So we don't have to check the return code here*/
		/* get global lock because we might need it */

		if((Ret = bioapi_GetModuleListLock(BioAPI_WRITER_LOCK)) == BioAPI_OK)
		{
			/* Iterate down the list and find the record with a matching UUID */
			ModuleRecord = hModuleListHead;
			if(NULL != ModuleRecord)
			{
				while((NULL != ModuleRecord) &&
					(BioAPI_TRUE != BioAPI_CompareUuids((*ModuleUuid),
					ModuleRecord->UUID)))
				{
					ModuleRecord = ModuleRecord->Next;
				}

				if(NULL != ModuleRecord)
				{
					if (bioapi_WriterLock (ModuleRecord->hModuleSWMRLock) != BioAPI_OK)
					{
						ModuleRecord = NULL;
					}
					else
					{
					/* Make sure another thread didn't change these values
					on us.
						*/
						if(BioAPI_TRUE != BioAPI_CompareUuids((*ModuleUuid),
							ModuleRecord->UUID))
						{
							bioapi_ReleaseModuleLock(ModuleRecord,
								BioAPI_WRITER_LOCK);
							ModuleRecord = NULL;
						}
					}
				}


				/* ModuleRecord should now either have a writer lock on our
				record or have determined that our record doesn't exist.
				*/
				if(NULL == ModuleRecord)
				{
					bioapi_ReleaseModuleListLock(BioAPI_WRITER_LOCK);
					Ret = BioAPI_ERRCODE_INTERNAL_ERROR;
				}
				else
				{
					Ret = bioapi_PruneCallbackList(ModuleRecord,
						AppNotifyCallback,
						AppNotifyCallbackCtx);
					if(BioAPI_OK != Ret)
					{
						bioapi_ReleaseModuleLock(ModuleRecord,
							BioAPI_WRITER_LOCK);
						bioapi_ReleaseModuleListLock(BioAPI_WRITER_LOCK);
						Ret = BioAPI_ERRCODE_INTERNAL_ERROR;
					}
				}
			}
			else
			{
				bioapi_ReleaseModuleListLock(BioAPI_WRITER_LOCK);
				Ret = BioAPI_ERRCODE_INTERNAL_ERROR;
			}
		}

		/* If everything's ok here, then we just need to perform the unload.
		After that we only have to free the removed record - if it's to
		be freed.
		*/
		if(BioAPI_OK == Ret)
		{
			/* Check if we should free the record - we need to at least unlock
			   it.	If we should free it, we'll free memory associated with
			   the record and unload the library.
			*/
			if((NULL != ModuleRecord) &&
			   (NULL == ModuleRecord->EventNotificationCallbackList))
			{
				/* If that's all the load calls we've gotten then
				   it's time to remove this module from our list.
				*/
				ERR(Ret = bioapi_CleanInternalModuleRecord(&ModuleRecord,
														 &MainLibHandle));
				if(BioAPI_OK == Ret)
				{
					ERR(port_FreeLibrary(MainLibHandle));
				}
			}
			else
			{
				bioapi_ReleaseModuleLock(ModuleRecord,
					BioAPI_WRITER_LOCK);
			}

			/* release the module list lock in all cases */
			bioapi_ReleaseModuleListLock(BioAPI_WRITER_LOCK);
		}
	}
	return (Ret);

}

BioAPI_RETURN
bioapi_ModuleLoad(const BioAPI_UUID *ModuleUuid,
				  uint32 Reserved,
				  BioAPI_ModuleEventHandler AppNotifyCallback,
				  const void* AppNotifyCallbackCtx)
{
	BioAPI_RETURN Ret = BioAPI_OK;
	bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord = NULL;
	LOAD_MODULE_FUNC_PTR SPI_Load_Func = NULL;
	char szModuleLoad[] = "BioSPI_ModuleLoad";
	CSSM_DATA tmpPath;

	CSSM_DATA ModulePath;
	CSSM_DATA tmpModuleName;
	CSSM_LIB_HANDLE LibHandle = NULL;

	/* First, we check if this module is already loaded */
	if(ERR(Ret = bioapi_FindModuleByUUIDAndMultiLock(
													 /* Linux-Port: removed the cast */
													 ModuleUuid,
													 // (BioAPI_UUID_PTR)ModuleUuid,
												   BioAPI_NO_LOCK,
												   &ModuleRecord,
												   BioAPI_WRITER_LOCK)) != BioAPI_OK)
	{
		/* Since this is the first call to load, we may need to
		   authenticate the module.
		*/

		/*
		* Get the credential, search path and section name from MDS
		*/
		if ( (Ret = mdsutil_GetModuleCredentialInfo (ModuleUuid, NULL, &tmpModuleName,
															&tmpPath)) != BioAPI_OK)
		{
			return (BioAPI_ERRCODE_INTERNAL_ERROR);
		}

		Ret = mdsutil_GetModulePath (tmpModuleName, tmpPath, &ModulePath);
		local_free (tmpModuleName.Data, NULL);
		local_free (tmpPath.Data, NULL);
		if (Ret != BioAPI_OK)
		{
			return (BioAPI_ERRCODE_INTERNAL_ERROR);
		}
		/* Load the library directly */
		Ret = port_LoadLibrary((const char *)ModulePath.Data, &LibHandle);
		local_free (ModulePath.Data, NULL);
		if ( (Ret != BioAPI_OK) && (LibHandle == NULL) )
			return (Ret);


		/* If all is ok, we've now loaded the module. */
		if( Ret == BioAPI_OK )
		{
			/* We now make an entry in the module list */
			if(BioAPI_OK != bioapi_NewModuleRecord(ModuleUuid,
				&ModuleRecord))
			{
				/* we may've failed because another thread loaded the module */
				if(BioAPI_OK == bioapi_FindModuleByUUIDAndMultiLock(ModuleUuid,
					BioAPI_NO_LOCK,
					&ModuleRecord,
					BioAPI_WRITER_LOCK))
				{
					Ret = bioapi_UpdateCallbackList(ModuleRecord,
						AppNotifyCallback,
						AppNotifyCallbackCtx);
					if(BioAPI_OK != Ret)
					{
						bioapi_ReleaseModuleLock(ModuleRecord,
							BioAPI_WRITER_LOCK);
					}
				}
				else
				{

					Ret = BioAPI_ERRCODE_INVALID_UUID;
				}
			}
			else
			{
			/* Our addition worked.	 Now we register the callback and
			either the new lib handle or the verified module ptr.
			After that call the SPI function.  Remember that the
			creation left the record in a writer_lock state.
				*/
				ModuleRecord->LibHandle = LibHandle;

				Ret = bioapi_UpdateCallbackList(ModuleRecord,
					AppNotifyCallback,
					AppNotifyCallbackCtx);
			}
		}
	}
	else
	{
		Ret = bioapi_UpdateCallbackList(ModuleRecord,
			AppNotifyCallback,
			AppNotifyCallbackCtx);

		if(BioAPI_OK != Ret)
		{
			bioapi_ReleaseModuleLock(ModuleRecord,
				BioAPI_WRITER_LOCK);
		}
	}

	if(BioAPI_OK == Ret)
	{
		/* Now we get the address for the function we're about to call. */
		Ret = port_GetProcAddress(ModuleRecord->LibHandle,
			szModuleLoad, (BioAPI_PROC_ADDR*)&SPI_Load_Func);
		if( BioAPI_OK == Ret )
		{
			/* Unlock the lock before the SPI call - if we don't we'll
			likely deadlock if the module is static.
			*/
			;
		}

		bioapi_ReleaseModuleLock(ModuleRecord,
			BioAPI_WRITER_LOCK);


		if((BioAPI_OK == Ret) &&
		   (NULL != SPI_Load_Func))
		{
			/* Now we call the addin's load function */
			Ret = SPI_Load_Func(&ref_hlayer_uuid,
				ModuleUuid,
				(BioSPI_ModuleEventHandler)bioapi_ModuleEventHandler,  /*BioAPI Notify Callback */
				AppNotifyCallbackCtx);						/*	Callback context */

			if(BioAPI_OK != Ret)
			{
				bioapi_DeleteModuleRecordonLoadFail(ModuleUuid,
												  AppNotifyCallback,
												  AppNotifyCallbackCtx);
			}
		}
		else
		{
			bioapi_DeleteModuleRecordonLoadFail(ModuleUuid,
											  AppNotifyCallback,
											  AppNotifyCallbackCtx);
			ERR(Ret = BioAPI_ERRCODE_INTERNAL_ERROR);
		}
	}
	return (Ret);
}



BioAPI_RETURN
bioapi_ModuleUnload(const BioAPI_UUID *ModuleUuid,
				  BioAPI_ModuleEventHandler AppNotifyCallback,
				  const void* AppNotifyCallbackCtx)
{
	BioAPI_RETURN Ret = BioAPI_OK;
	bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord = NULL;
	UNLOAD_MODULE_FUNC_PTR SPI_Unload_Func = NULL;
	char szModuleUnload[] = "BioSPI_ModuleUnload";
	CSSM_LIB_HANDLE MainLibHandle = 0;
	BioAPI_BOOL LastUnload = BioAPI_FALSE;


	if(NULL != ModuleUuid)
	{
		/* get global lock because we might need it */
		if((Ret = bioapi_GetModuleListLock(BioAPI_WRITER_LOCK)) == BioAPI_OK)
		{
			/* Iterate down the list and find the record with a matching UUID */
			ModuleRecord = hModuleListHead;
			if(NULL != ModuleRecord)
			{
				while((NULL != ModuleRecord) &&
					(BioAPI_TRUE != BioAPI_CompareUuids((*ModuleUuid),
					ModuleRecord->UUID)))
				{
					ModuleRecord = ModuleRecord->Next;
				}

				if(NULL != ModuleRecord)
				{
					if (bioapi_WriterLock (ModuleRecord->hModuleSWMRLock) != BioAPI_OK)
					{
						ModuleRecord = NULL;
					}
					else
					{
					/* Make sure another thread didn't change these values
					on us.
						*/
						if(BioAPI_TRUE != BioAPI_CompareUuids((*ModuleUuid),
							ModuleRecord->UUID))
						{
							bioapi_ReleaseModuleLock(ModuleRecord,
								BioAPI_WRITER_LOCK);
							ModuleRecord = NULL;
						}
					}
				}

				/* ModuleRecord should now either have a writer lock on our
				record or have determined that our record doesn't exist.
				*/
				if(NULL == ModuleRecord)
				{
					bioapi_ReleaseModuleListLock(BioAPI_WRITER_LOCK);
					Ret = BioAPIERR_H_FRAMEWORK_MODULE_LOAD_FAILED;
				}
				else
				{
					Ret = bioapi_PruneCallbackList(ModuleRecord,
						AppNotifyCallback,
						AppNotifyCallbackCtx);
					if(BioAPI_OK != Ret)
					{
						bioapi_ReleaseModuleLock(ModuleRecord,
							BioAPI_WRITER_LOCK);
						bioapi_ReleaseModuleListLock(BioAPI_WRITER_LOCK);
					}
				}
			}
			else
			{
				bioapi_ReleaseModuleListLock(BioAPI_WRITER_LOCK);
				Ret = BioAPIERR_H_FRAMEWORK_MODULE_UNLOAD_FAILED;
			}
		}

		/* If everything's ok here, then we just need to perform the unload.
		After that we only have to free the removed record - if it's to
		be freed.
		*/
		if(BioAPI_OK == Ret)
		{
		/* We need to find and locate the unload function and call it,
		whether or not this is the final unload for this module.
			*/
			if(ModuleRecord != NULL)
			{

				Ret = port_GetProcAddress(ModuleRecord->LibHandle,
										  szModuleUnload,
										  (BioAPI_PROC_ADDR *)&SPI_Unload_Func);
			}

			/* Check if we should free the record - we need to at least unlock
			it.	 If we should free it, we'll free memory associated with
			the record and unload the library.
			*/
			if((NULL != ModuleRecord) &&
			   (NULL == ModuleRecord->EventNotificationCallbackList))
			{
				/* If that's all the load calls we've gotten then
				   it's time to remove this module from our list.
				*/
				ERR(Ret = bioapi_CleanInternalModuleRecord(&ModuleRecord,
														 &MainLibHandle));
				LastUnload = BioAPI_TRUE;
			}
			else
			{
				bioapi_ReleaseModuleLock(ModuleRecord,
					BioAPI_WRITER_LOCK);
			}

			/* release the module list lock in all cases */
			bioapi_ReleaseModuleListLock(BioAPI_WRITER_LOCK);

			/* We've taken the record out of the internal lists, now we'll call
			the addin to notify it of the unload.
			*/
			if((BioAPI_OK == Ret) &&
			   (NULL != SPI_Unload_Func))
			{
				ERR(Ret = SPI_Unload_Func(ref_hlayer_uuid,
										  ModuleUuid,
										  (BioSPI_ModuleEventHandler)bioapi_ModuleEventHandler,
										  AppNotifyCallbackCtx));

			}
			else
			{
				Ret = BioAPIERR_H_FRAMEWORK_MODULE_UNLOAD_FAILED;
			}

			if(BioAPI_TRUE == LastUnload)
			{
				ERR(port_FreeLibrary(MainLibHandle));
			}
		}
	}
	return (Ret);
}


/*****************************************************************************
*	Name: bioapi_SetupStandardSPICall
*
*	Description: This function performs the standard initialization and return
*	address checks performed by the majority of the BioAPI APIs.  It acquires
*	all locks necessary for a safe call to a service provider, and acquires
*	the SPI function table for the call.  Note that the ModuleHandle must be
*	to a service provider as indicated by ModuleType.
*
*	Parameters:
*		ModuleHandle (input) : This handle is used to select a particular
*			attach record, ensuring that the attach will be around for the
*			duration of the SPI call.
*		CallBack (output) : The function table associated with the ModuleHandle
*			and the ModuleType.
*		AttachRecord (output) : The attach record referenced by ModuleHandle.
*
*	WARNING: This function should not be modified very often, as it is used by
*		almost all API functions which route to a service provider.	 Functions
*		like those in context.c and cspapi.c which require more to be done per
*		API call should either not use this function or use it in addition to
*		other code.	 In short, don't change this function unless you absolutely
*		have to and know exactly what you're doing.
*
*****************************************************************************/

BioAPI_RETURN
bioapi_SetupStandardSPICall(BioAPI_HANDLE ModuleHandle,
						  void** CallBack,
						  bioapi_INTERNAL_ATTACH_LIST_PTR *AttachRecord)
{
	BioAPI_RETURN err = BioAPI_OK;

	/* make sure that app has done a BioAPI_Init */
	if(BioAPI_OK == ERR(err = bioapi_CheckInit()))
	{

		if(BioAPI_OK == err)
		{
			/*
			 * Get the AttachRecord record and the callback pointers
			 * associated with the specified handle.
			 */
			ERR(err = bioapi_GetAttachRecordAndLockMultiTask(ModuleHandle,
														   CallBack,
														   AttachRecord));
		}
	}

	return err;
}



/*****************************************************************************
*	Name: bioapi_CleanupStandardSPICall
*
*	Description: This function performs cleanup performed by the majority of
*	the BioAPI APIs.  It releases the necessary locks.	It is assumed that this
*	function will be called after a successful call to
*	bioapi_SetupStandardSPICall.
*
*	Parameters:
*		AttachRecord (input) : The attach record associated with the handle
*			used to make the recent SPI call.  This should be returned by
*			bioapi_SetupStandardAPICall.
*
*	WARNING: This function should not be modified very often, as it is used by
*		almost all API functions which route to a service provider.	 Functions
*		like those in context.c and cspapi.c which require more to be done per
*		API call should either not use this function or use it in addition to
*		other code.	 In short, don't change this function unless you absolutely
*		have to and know exactly what you're doing.
*
*****************************************************************************/

BioAPI_RETURN
bioapi_CleanupStandardSPICall(bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord)
{
	BioAPI_RETURN err = BioAPI_OK;

	/* Release the AttachRecord Record.*/
	ERR(err = bioapi_ReleaseAttachRecordAndUnlockMultiTask(AttachRecord));

	return err;
}

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
						  bioapi_INTERNAL_DEVICE_LIST_PTR DeviceRecord)
{
	BioAPI_RETURN err = BioAPI_OK;
	SPI_ATTACH_FUNC_PTR pfnModuleAttach = NULL;
	uint32 NumFuncs;
	char szModuleAttach[] = "BioSPI_ModuleAttach";
	BioAPI_MODULE_FUNCS_PTR TempFuncTable;
	bioapi_INTERNAL_MODULE_LIST_PTR ModuleRecord;
	CSSM_LIB_HANDLE LibHandle;


	if(ERR(err = bioapi_FindModuleAndMultiLock(ModuleHandle,
											   BioAPI_NO_LOCK,
											   &ModuleRecord,
											   BioAPI_READER_LOCK)) == BioAPI_OK)
	{
		LibHandle = ModuleRecord->LibHandle;

		ERR(err = port_GetProcAddress(LibHandle,
									  szModuleAttach,
									  (BioAPI_PROC_ADDR*)&pfnModuleAttach));

		/* Release the module record now - we still have the attach record
		   locked, so we know no one can unload this on us.
		*/
		bioapi_ReleaseModuleLock(ModuleRecord,
								 BioAPI_READER_LOCK);

		if((BioAPI_OK == err) &&
		   (NULL != pfnModuleAttach))
		{
			/* Calls SPI_ModuleAttach function */
			ERR(err = pfnModuleAttach(ModuleUuid,
									  Version,
									  DeviceId,
									  Reserved1,
									  Reserved2,
									  ModuleHandle,
									  Reserved3,
									  Reserved4,
									  NULL,
									  NULL,
									  &InternalUpcalls,
									  &TempFuncTable));

			if(BioAPI_OK == err)
			{
				if((BioAPI_FALSE == port_IsBadReadPtr(TempFuncTable,
													sizeof(BioAPI_MODULE_FUNCS))) &&
				   (BioAPI_FALSE == port_IsBadReadPtr(TempFuncTable->ServiceFuncs,
													(TempFuncTable->NumberOfServiceFuncs * sizeof(BioAPI_PROC_ADDR)))))
				{
					NumFuncs = TempFuncTable->NumberOfServiceFuncs;

					if(0 != NumFuncs)
					{
						*AttachFuncTbl = (BioAPI_MODULE_FUNCS_PTR)local_calloc(sizeof(BioAPI_MODULE_FUNCS),
																			 1,
																			 NULL);
						if(NULL != *AttachFuncTbl)
						{
							(*AttachFuncTbl)->NumberOfServiceFuncs = NumFuncs;

							(*AttachFuncTbl)->ServiceFuncs = local_calloc((sizeof(BioAPI_PROC_ADDR) * NumFuncs),
																		  1,
																		  NULL);
							if(NULL != (*AttachFuncTbl)->ServiceFuncs)
							{
								/* Linux-Port: added the (void *) cast */
								memcpy((void *)(*AttachFuncTbl)->ServiceFuncs,
									   TempFuncTable->ServiceFuncs,
									   (sizeof(BioAPI_PROC_ADDR) * NumFuncs));
							}
							else
							{
								ERR(err = BioAPIERR_H_FRAMEWORK_MEMORY_ERROR);
								local_free((*AttachFuncTbl),
										   NULL);
								*AttachFuncTbl = NULL;
							}
						}
						else
						{
						ERR(err = BioAPIERR_H_FRAMEWORK_MEMORY_ERROR);
						}
					}
					else
					{
					ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_FUNCTION_TABLE);
					}
				}
				else
				{
					ERR(err = BioAPIERR_H_FRAMEWORK_INVALID_MODULE_FUNCTION_TABLE);
				}
			}
		}
		else
		{
			if(BioAPI_OK == err)
			{
				ERR(err = BioAPIERR_H_FRAMEWORK_FUNCTION_NOT_IMPLEMENTED);
			}
		}
	}

	return err;
}
