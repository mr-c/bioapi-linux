/*-----------------------------------------------------------------------
 * File: MAF_COLLECTN.C
 *-----------------------------------------------------------------------
 */

#include "maf_include.h"
#include "bioapi_util.h"

static BioAPI_RETURN mlc_LockIt(
						MLC_LOCK_TYPE ltLockType,
						cssm_SWMRLock *pLock )
{
	BioAPI_RETURN rv = BioAPI_OK;

	switch ( ltLockType )
	{
	case MLC_NO_LOCK:
		{
			// Do nothing
			break;
		}
	case MLC_WRITE_LOCK:
		{
			if ( cssm_SWMRLockWaitToWrite( pLock,
										   CSSM_INFINITE_WAIT) != CSSM_OK )
			{
				ERR( rv = MLC_ERR_LOCKING );
			}
			break;
		}
	case MLC_READ_LOCK:
		{
			if ( cssm_SWMRLockWaitToRead( pLock,
										  CSSM_INFINITE_WAIT ) != CSSM_OK )
			{
				ERR( rv = MLC_ERR_LOCKING );
			}
			break;
		}
	default:
		{
			assert( 0 );
			ERR( rv = MLC_ERR_LOCKING_MODE );
		}
	}

	return rv;
}

static BioAPI_RETURN mlc_UnlockIt(
						MLC_LOCK_TYPE ltLockType,
						cssm_SWMRLock *pLock )
{
	BioAPI_RETURN rv = BioAPI_OK;

	switch ( ltLockType )
	{
	case MLC_NO_LOCK:
		{
			// Do nothing
			break;
		}
	case MLC_WRITE_LOCK:
		{
			if ( cssm_SWMRLockDoneWriting( pLock ) != CSSM_OK )
			{
				ERR( rv = MLC_ERR_LOCKING );
			}
			break;
		}
	case MLC_READ_LOCK:
		{
			if ( cssm_SWMRLockDoneReading( pLock ) != CSSM_OK )
			{
				ERR( rv = MLC_ERR_LOCKING );
			}
			break;
		}
	default:
		{
			assert( 0 );
			ERR( rv = MLC_ERR_LOCKING_MODE );
		}
	}

	return rv;
}

/**------------------------------------------------------------------------
 ** MLC_Init
 **
 ** Description
 **	 Initializes an MLC_LIST_COLLECTION object for use. A list object can
 **	 not be used without initializing it first. Any existing contents of
 **	 the structure will be lost.
 **
 ** Parameters
 **	 pCollection (input) - List object to initialize.
 **	 TearDown (input) - Default object destruction function that will be
 **	   used to remove any remaining items in the list when MLC_Term is
 **	   called.
 **
 ** Return Value
 **	 BioAPI_OK - initialization succeeded
 **	 MLC_ERR_RESOURCE - The required locking resources could not be created
 **------------------------------------------------------------------------
 **/
BioAPI_RETURN MLC_Init(	  MAF_LIST_COLLECTION *pCollection,
						MLC_ITEM_TEARDOWN_FUNC TearDown )
{
	BioAPI_RETURN rv = BioAPI_OK;

	/* Perform a quick validation */
	assert( pCollection );

	/* Wipe the structure */
	pCollection->pHead = pCollection->pTail = NULL;
	pCollection->uNumNodes = 0;
	pCollection->TearDown = TearDown;

	/* Create the SWMR lock for the list */
	if ( cssm_SWMRLockCreate( &pCollection->ListLock,
							  NULL ) != CSSM_OK )
	{
		ERR( rv = MLC_ERR_RESOURCE );
	}

	return rv;
}

/**------------------------------------------------------------------------
 ** MLC_Term
 **
 ** Description
 **	 Terminates a MAF_LIST_COLLECTION object. If a function was registered
 **	 when the MLC_Init function was called, then the contents of the list
 **	 are destroyed. All locking resources are deallocated, and the object
 **	 is wiped clean.
 **
 ** Parameters
 **	 pCollection (input) - List object to terminate.
 **
 ** Return Value
 **	 BioAPI_OK - Termination succeeded
 **	 MLC_ERR_LOCKING - The list could not be locked to deallocate its
 **	   contents. The object may not have been properly initialized.
 **	 MLC_ERR_POSSIBLE_RES_LEAK - When destroying records in the list, one
 **	   of the records could not be locked before deallocating it. In this
 **	   case, the lock was not deallocated because it is assumed to be
 **	   invalid. The resources associated with the lock may have been lost.
 **------------------------------------------------------------------------
 **/
BioAPI_RETURN MLC_Term(	  MAF_LIST_COLLECTION *pCollection )
{
	BioAPI_RETURN rv = BioAPI_OK;

	/* Perform a quick validation */
	assert( pCollection );
//	assert( pCollection->pHead == NULL );

	/* Clean the list if anything is still there and we have a teardown */
	if ( pCollection->TearDown )
	{
		rv = MLC_CleanList( pCollection, pCollection->TearDown );
	}

	/* Delete the SWMR lock for the list */
	cssm_SWMRLockDelete( &pCollection->ListLock );

	/* Wipe the structure */
	memset( pCollection, 0, sizeof(*pCollection) );

	return rv;
}

/**------------------------------------------------------------------------
 ** MLC_AddItem
 **
 ** Description
 **	 Adds a new record to the end of the list. The caller may request that
 **	 the record is returned with any type of lock pre-aquired on the new
 **	 record. The caller is responsible for calling MLC_ReleaseItem to drop
 **	 the lock.
 **
 ** Parameters
 **	 pCollection (input) - List that will receive the new record.
 **
 ** Return Value
 **	 BioAPI_OK - The new record has been added to the list, and the requested
 **	   lock has been aquired.
 **	 BioAPI_ERRCODE_MEMORY_ERROR - Memory could not be allocated for the new list
 **	   node.
 **	 MLC_ERR_RESOURCE - Locking resources could not be created for the new
 **	   list node.
 **	 MLC_ERR_LOCKING - The list could not be locked to add the new record,
 **	   or the new record could not be locked.
 **	 MLC_ERR_LOCKING_MODE - An invalid locking mode was specified for the
 **	   new record.
 **------------------------------------------------------------------------
 **/
BioAPI_RETURN MLC_AddItem( MAF_LIST_COLLECTION *pCollection,
						 void *pNewItem,
						 MLC_LOCK_TYPE ltLockType,
						 MLC_LOCK_REF *pLockRef )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_LIST_COLLECTION_NODE *pNewNode = NULL;

	/* Perform a quick validation */
	assert( pCollection && pNewItem );

	/* Clear the lock reference return value */
	*pLockRef = MLC_NULL_LOCK_REF;

	/* Create the new record */
	pNewNode = (MAF_LIST_COLLECTION_NODE*)
					BioAPI_malloc( sizeof(MAF_LIST_COLLECTION_NODE), NULL  );
	if ( !pNewNode )
	{
		ERR( rv = BioAPI_ERRCODE_MEMORY_ERROR );
	}
	else
	{
		/* Fill in the new record */
		pNewNode->pNext = NULL;
		pNewNode->pPrev = NULL;
		pNewNode->pValue = pNewItem;

		/* Create the new node lock */
		if ( cssm_SWMRLockCreate( &pNewNode->NodeLock,
								  NULL ) != CSSM_OK )
		{
			/* Error condition, free memory */
			BioAPI_free( pNewNode, NULL );
			pNewNode = NULL;
			ERR( rv = MLC_ERR_RESOURCE );
		}
		/* Lock the new node as requested */
		else if ( ( rv = mlc_LockIt( ltLockType,
									 &pNewNode->NodeLock ) ) != BioAPI_OK )
		{
			/* Error condition. Delete everything */
			cssm_SWMRLockDelete( &pNewNode->NodeLock );
			BioAPI_free( pNewNode, NULL );
			pNewNode = NULL;
		}
		/* Get a writer lock on the list */
		else if ( cssm_SWMRLockWaitToWrite( &pCollection->ListLock,
											CSSM_INFINITE_WAIT ) != CSSM_OK )
		{
			/* Error condition. Unlock and delete everything */
			mlc_UnlockIt( ltLockType, &pNewNode->NodeLock );
			cssm_SWMRLockDelete( &pNewNode->NodeLock );
			BioAPI_free( pNewNode, NULL );
			pNewNode = NULL;
			ERR( rv = MLC_ERR_LOCKING );
		}
		else
		{
			/* Insert the item at the tail of the list */
			if ( pCollection->pTail == NULL )
			{
				pCollection->pTail = pCollection->pHead = pNewNode;
			}
			else
			{
				pNewNode->pPrev = pCollection->pTail;
				pCollection->pTail->pNext = pNewNode;
				pCollection->pTail = pNewNode;
			}

			/* Increment the list size */
			(pCollection->uNumNodes)++;

			/* Drop the lock on the list */
			cssm_SWMRLockDoneWriting( &pCollection->ListLock );

			/* Set the lock reference output parameter */
			*pLockRef = pNewNode;
		}
	}

	return rv;
}


/**------------------------------------------------------------------------
 ** MLC_DeleteItem
 **
 ** Description
 **	 Removes the first record that matches the conditions defined by the
 **	 ItemFind parameter. The pKey parameter is passed to the ItemFind
 **	 comparison function without modification. After the item is removed
 **	 from the list, it is returned to the caller.
 **
 ** Parameters
 **	 pCollection (input) - List that will be searched for a matching record
 **	 ItemFind (input) - Comparison function used to indicate the record to
 **	   remove from the list.
 **	 pKey (input) - opaque value that will have meaning to the ItemFind
 **	   function.
 **	 ppValue (output) - Will be set to a pointer to the record that was
 **	   removed from the list.
 **
 ** Return Value
 **	 BioAPI_OK - The record has been removed from the list.
 **	 MLC_ERR_LOCKING - The list could not be locked to remove the record,
 **	   or the record to be removed could not be locked. The list was not
 **	   modified.
 **	 MLC_ERR_NOT_FOUND - A record could not be found that matched the
 **	   ItemFind function.
 **------------------------------------------------------------------------
 **/
BioAPI_RETURN MLC_DeleteItem(
						MAF_LIST_COLLECTION *pCollection,
						MLC_FIND_FUNC ItemFind,
						void *pKey,
						void **ppValue )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_LIST_COLLECTION_NODE *pNode = NULL;

	/* Perform a quick validation */
	assert( pCollection && ItemFind && ppValue );

	/* Clear the ppValue output parameter */
	*ppValue = NULL;

	/* Get a writer lock on the list */
	if ( cssm_SWMRLockWaitToWrite( &pCollection->ListLock,
								   CSSM_INFINITE_WAIT ) != CSSM_OK )
	{
		ERR( rv = MLC_ERR_LOCKING );
	}
	else
	{
		/* Interate down the list applying ItemFind(<item>,pKey) until a
		 * match or end of list is found.
		 */
		pNode = pCollection->pHead;
		while ( pNode )
		{
			/* Compare the objects */
			if ( !ItemFind( pNode->pValue, pKey ) )
			{
				/* Get a writer lock on the candidate record */
				if ( cssm_SWMRLockWaitToWrite( &pNode->NodeLock,
											   CSSM_INFINITE_WAIT ) != CSSM_OK )
				{
					ERR( rv = MLC_ERR_LOCKING );
					pNode = NULL;
					break; /* Exit the while loop */
				}

				/* Recompare the values to insure it hasn't changed from the
				 * value before the lock was obtain, or is still a match.
				 */
				if ( !ItemFind( pNode->pValue, pKey ) )
				{
					break;
				}

				/* This wasn't the correct record after all, unlock it and
				 * continue down the list.
				 */
				/* Drop the writer lock on the record */
				cssm_SWMRLockDoneWriting( &pNode->NodeLock );
			}

			/* Move to the next record */
			pNode = pNode->pNext;
		} /* while */

		/* Did we find something? */
		if ( pNode == NULL )
		{
			/* Only set an error if one hasn't been set already */
			if ( rv == BioAPI_OK )
			{
				ERR( rv = MLC_ERR_NOT_FOUND );
			}
		}
		else
		{
			/* Unlink the list node */
			if ( ( pNode == pCollection->pHead ) &&
				 ( pNode == pCollection->pTail ) )
			{
				/* Only node in the list */
				pCollection->pHead = pCollection->pTail = NULL;
			}
			else if ( pNode == pCollection->pHead )
			{
				/* First item in the list */
				pCollection->pHead = pCollection->pHead->pNext;
				pCollection->pHead->pPrev = NULL;
			}
			else if ( pNode == pCollection->pTail )
			{
				/* Last item in the list */
				pCollection->pTail = pCollection->pTail->pPrev;
				pCollection->pTail->pNext = NULL;
			}
			else
			{
				/* Middle of the list */
				pNode->pNext->pPrev = pNode->pPrev;
				pNode->pPrev->pNext = pNode->pNext;
			}

			/* Update the node count */
			(pCollection->uNumNodes)--;

			/* Destroy the SWMR lock on the record */
			cssm_SWMRLockDoneWriting( &pNode->NodeLock );
			cssm_SWMRLockDelete( &pNode->NodeLock );

			/* Cache the old item */
			*ppValue = pNode->pValue;

			/* Free the lock memory */
			BioAPI_free( pNode, NULL );
		}

		/* Drop the writer lock on the list */
		cssm_SWMRLockDoneWriting( &pCollection->ListLock );
	}

	/* Return the old item to indicate success */
	return rv;
}

/**------------------------------------------------------------------------
 ** MLC_FindItem
 **
 ** Description
 **	 Returns the first record that matches the conditions defined by the
 **	 ItemFind parameter. The pKey parameter is passed to the ItemFind
 **	 comparison function without modification. After the item is found, it
 **	 is locked as specified by ltLockType. The lock reference and record
 **	 value are returned. The caller must call MLC_ReleaseItem with
 **	 the same ltLockType value to drop the lock aquired by calling
 **	 MLC_FindItem.
 **
 ** Parameters
 **	 pCollection (input) - List that will be searched for a matching record
 **	 ItemFind (input) - Comparison function used to indicate the record to
 **	   return from the list.
 **	 pKey (input) - opaque value that will have meaning to the ItemFind
 **	   function.
 **	 ltLockType (input) - Type of lock to aquire on the record.
 **	 pLockRef (output) - Lock reference for the record found in the list.
 **	 ppValue (output) - Will be set to a pointer to the record that was
 **	   returned from the list.
 **
 ** Return Value
 **	 BioAPI_OK - The record has been removed from the list.
 **	 MLC_ERR_LOCKING - The list could not be locked to search for the
 **	   record, or the record to be returned could not be locked as
 **	   requested.
 **	 MLC_ERR_LOCKING_MODE - An invalid locking mode was specified for the
 **	   returned record.
 **	 MLC_ERR_NOT_FOUND - A record could not be found that matched the
 **	   ItemFind function.
 **------------------------------------------------------------------------
 **/
BioAPI_RETURN MLC_FindItem(
						MAF_LIST_COLLECTION *pCollection,
						MLC_FIND_FUNC ItemFind,
						void *pKey,
						MLC_LOCK_TYPE ltLockType,
						MLC_LOCK_REF *pLockRef,
						void **ppValue )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_LIST_COLLECTION_NODE *pNode = NULL;

	/* Perform a quick validation */
	assert( pCollection && ItemFind && pKey && pLockRef && ppValue );

	/* Clear the return parameters */
	*pLockRef = MLC_NULL_LOCK_REF;
	*ppValue = NULL;

	/* Get a reader lock on the list */
	if ( cssm_SWMRLockWaitToRead( &pCollection->ListLock,
								  CSSM_INFINITE_WAIT ) != CSSM_OK )
	{
		ERR( rv = MLC_ERR_LOCKING );
	}
	else
	{
		/* Interate down the list applying ItemFind(<item>,pKey) until a
		 * match or end of list is found.
		 */
		pNode = pCollection->pHead;
		while ( pNode )
		{
			/* Compare the objects */
			if ( !ItemFind( pNode->pValue, pKey ) )
			{
				/* Obtain the requested lock on the item */
				if ( ( rv = mlc_LockIt( ltLockType,
										&pNode->NodeLock ) ) != BioAPI_OK )
				{
					pNode = NULL;
					break; /* Break out of the while loop */
				}

				/* Recompare the values to insure it hasn't changed from the
				 * value before the lock was obtain, or is still a match.
				 */
				if ( !ItemFind( pNode->pValue, pKey ) )
				{
					break;
				}

				/* This wasn't the correct record after all, unlock it and
				 * continue down the list.
				 */
				mlc_UnlockIt( ltLockType, &pNode->NodeLock );
			}

			/* Move to the next record */
			pNode = pNode->pNext;
		}

		/* Return the record value */
		if ( pNode != NULL )
		{
			/* Set the "lock reference" value */
			*pLockRef = (MLC_LOCK_REF)pNode;
			*ppValue = pNode->pValue;
		}
		else
		{
			/* Only set an error if it hasn't been set already */
			if ( rv == BioAPI_OK )
			{
				/* Don't wrap this case in the ERR() macro because there
				 * are just as many GOOD reasons for the search to fail,
				 * as bad reasons to fail (i.e. checking to make sure
				 * a record isn't there.
				 */
				 rv = MLC_ERR_NOT_FOUND;
			}
		}

		/* Drop the reader lock on list */
		cssm_SWMRLockDoneReading( &pCollection->ListLock );
	}

	return rv;
}

/**------------------------------------------------------------------------
 ** MLC_RelockItem
 **
 ** Description
 **	 Reaquires a lock on the lock reference, LockRef. The lock type is
 **	 determined by the ltLockType parameter. The record protected by the
 **	 lock reference must be guaranteed to exist in a MLC_LIST_COLLECTION
 **	 list, otherwise it could cause program fault.
 **
 ** Parameters
 **	 ltLockType (input) - Type of lock to aquire on the record.
 **	 LockRef (input) - Lock reference to relock.
 **	 ppValue (output) - Will be set to a pointer to the record protected by
 **	   LockRef
 **
 ** Return Value
 **	 BioAPI_OK - The lock has been reaquired.
 **	 MLC_ERR_LOCKING - The lock reference is invalid.
 **	 MLC_ERR_LOCKING_MODE - An invalid locking mode was specified.
 **------------------------------------------------------------------------
 **/
BioAPI_RETURN MLC_RelockItem( MLC_LOCK_TYPE ltLockType,
							MLC_LOCK_REF LockRef,
							void **ppValue )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_LIST_COLLECTION_NODE *pNode = (MAF_LIST_COLLECTION_NODE*)LockRef;

	/* Perform a quick validation */
	assert( LockRef && ppValue );

	/* Lock the node */
	if ( ( rv = mlc_LockIt( ltLockType, &pNode->NodeLock ) ) == BioAPI_OK )
	{
		*ppValue = pNode->pValue;
	}
	else
	{
		*ppValue = NULL;
	}

	/* Return a non-NULL value to indicate success */
	return rv;
}


/**------------------------------------------------------------------------
 ** MLC_ReleaseItem
 **
 ** Description
 **	 Releases a lock on a record that was locked by the MLC_AddItem,
 **	 MLC_FindItem, or MLC_RelockItem function. The value of ltLockType must
 **	 be the same as the value used to aquire the lock. The record pointer
 **	 returned with the lock reference should no longer be used until it is
 **	 relocked.
 **
 ** Parameters
 **	 ltLockType (input) - Type of lock to release on the record.
 **	 LockRef (input) - Lock reference to unlock.
 **
 ** Return Value
 **	 BioAPI_OK - The lock has been released.
 **	 MLC_ERR_LOCKING - The lock reference is invalid.
 **	 MLC_ERR_LOCKING_MODE - An invalid locking mode was specified.
 **------------------------------------------------------------------------
 **/
BioAPI_RETURN MLC_ReleaseItem( MLC_LOCK_TYPE ltLockType,
							 MLC_LOCK_REF LockRef )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_LIST_COLLECTION_NODE *pNode = (MAF_LIST_COLLECTION_NODE*)LockRef;

	/* Perform a quick validation */
	assert( LockRef );

	/* Drop the lock as required by the lock type */
	rv = mlc_UnlockIt( ltLockType, &pNode->NodeLock );

	/* Return a non-NULL value to indicate success */
	return rv;
}

/**------------------------------------------------------------------------
 ** MLC_CleanList
 **
 ** Description
 **	 Destroys the contents of the list. The list object remains usable
 **	 after being cleaned, but it is empty.
 **
 ** Parameters
 **	 pCollection (input) - List object to clean.
 **	 TearDown (input) - Function used to destroy each record in the list.
 **	   The function is responsible for deallocating all system resources,
 **	   including the memory for the record.
 **
 ** Return Value
 **	 BioAPI_OK - Cleaning succeeded
 **	 MLC_ERR_LOCKING - The list could not be locked to deallocate the
 **	   records.
 **	 MLC_ERR_POSSIBLE_RES_LEAK - When destroying records in the list, one
 **	   of the records could not be locked before deallocating it. In this
 **	   case, the lock was not deallocated because it is assumed to be
 **	   invalid. The resources associated with the lock may have been lost.
 **------------------------------------------------------------------------
 **/
BioAPI_RETURN MLC_CleanList( MAF_LIST_COLLECTION *pCollection,
						   MLC_ITEM_TEARDOWN_FUNC TearDown )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_LIST_COLLECTION_NODE *pNode = NULL, *pDoomed = NULL;

	/* Perform a quick validation */
	assert( pCollection );

	/* Get a writer lock on the list */
	if ( cssm_SWMRLockWaitToWrite( &pCollection->ListLock,
								   CSSM_INFINITE_WAIT ) != CSSM_OK )
	{
		ERR( rv = MLC_ERR_LOCKING );
	}
	else
	{
		/* Iterate down the list, get a writer lock on each record and delete
		 * it.
		 */
		pNode = pCollection->pHead;
		while ( pNode )
		{
			/* Get a writer lock on the individual record */
			if ( cssm_SWMRLockWaitToWrite( &pNode->NodeLock,
										   CSSM_INFINITE_WAIT ) == CSSM_OK )
			{
				/* Only unlock and deallocate the lock if we successfully
				 * locked it. Doing anything else with a bad lock could do
				 * something nasty. This should never happen unless the OS
				 * is having problems.
				 */
				cssm_SWMRLockDoneWriting( &pNode->NodeLock );
				cssm_SWMRLockDelete( &pNode->NodeLock );
			}
			else
			{
				ERR( rv = MLC_ERR_POSSIBLE_RES_LEAK );
			}

			/* Free the record resources for the item.
			 */
			TearDown( pNode->pValue );

			/* Hand of the old record and update the node pointer */
			pDoomed = pNode;
			pNode = pNode->pNext;

			/* Free the doomed node */
			BioAPI_free( pDoomed, NULL );
		}

		/* Clear the list structure */
		pCollection->uNumNodes = 0;
		pCollection->pHead = pCollection->pTail = NULL;

		/* Drop the writer lock on the list */
		cssm_SWMRLockDoneWriting( &pCollection->ListLock );
	}

	/* Return the list structure address to indicate success */
	return rv;
}

/**------------------------------------------------------------------------
 ** MLC_ForEach
 **
 ** Description
 **	 Walks the list and executes the Iteration function on each record.
 **	 pParam is passed to Iteration unmodified. Each record is locked
 **	 according to ltLockType before Iteration is called. The operation can
 **	 be cancelled before all entries in the list have been processed by
 **	 returning a zero value from Iteration.
 **
 ** Parameters
 **	 pCollection (input) - List that will walked.
 **	 Iteration (input) - Function to be executed for each record in the
 **	   list.
 **	 pParam (input) - opaque value that will have meaning to the Iterator
 **	   function.
 **	 ltLockType (input) - Type of lock to aquire on each record.
 **
 ** Return Value
 **	 BioAPI_OK - The entire list has been walked.
 **	 MLC_ERR_LOCKING - The list could not be locked for walking.
 **	 MLC_ERR_LOCKING_MODE - An invalid locking mode was specified.
 **	 MLC_ERR_LOCKING_PARTIAL - Attempting to lock a record in the list
 **	   failed. Walking the list stopped before calling Iterator on that
 **	   record. All records following the bad lock in the list were not
 **	   processed.
 **	 MLC_ERR_CANCELLED - The Iterator function returned a zero value to
 **	   stop walking the list before all records have been processed.
 **------------------------------------------------------------------------
 **/
BioAPI_RETURN MLC_ForEach( MAF_LIST_COLLECTION *pCollection,
						 MLC_ITEM_ITERATOR_FUNC Iteration,
						 void *pParam,
						 MLC_LOCK_TYPE ltLockType )
{
	BioAPI_RETURN rv = BioAPI_OK;
	MAF_LIST_COLLECTION_NODE *pNode = NULL;
	int nReturn;

	/* Perform a quick validation */
	assert( pCollection && Iteration );

	/* Get a reader lock on the list */
	if ( cssm_SWMRLockWaitToRead( &pCollection->ListLock,
								  CSSM_INFINITE_WAIT ) != CSSM_OK )
	{
		ERR( rv = MLC_ERR_LOCKING );
	}
	else
	{
		/* Interate down the list applying Iteration(<item>,pParam) to all
		 * items in the list.
		 */
		pNode = pCollection->pHead;
		while ( pNode )
		{
			/* Get the appropriate lock on the record */
			if ( ( rv = mlc_LockIt( ltLockType, &pNode->NodeLock ) ) == BioAPI_OK )
			{
				/* Call the manipulator */
				nReturn = Iteration( pNode->pValue, pParam );

				mlc_UnlockIt( ltLockType, &pNode->NodeLock );
			}
			else
			{
				ERR( rv = MLC_ERR_LOCKING_PARTIAL );
				break; /* Break out of the while loop */
			}

			/* Check return value */
			if ( !nReturn )
			{
				/* Drop the writer lock and return */
				ERR( rv = MLC_ERR_CANCELLED );
				break; /* Break out of the while loop */
			}

			/* Move to the next record */
			pNode = pNode->pNext;
		}

		/* Drop the reader lock on list and return */
		cssm_SWMRLockDoneReading( &pCollection->ListLock );
	}

	return rv;
}
