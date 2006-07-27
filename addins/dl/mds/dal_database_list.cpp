/*-----------------------------------------------------------------------
 * File: DAL_DATABASE_LIST.CPP
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains implementation of the DAL_DATABASE_INFO_LIST class
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

#define DAL_SOURCE_FILE
#include "dal_classes.h"
#include "dal_internal.h"

/* Usage Summary for the mutex to control access to each database info, and the SWMR lock
   to control access to the database list:

functions that attempt to obtain the writer lock
	DAL_DATABASE_INFO_LIST::AddDatabase (does not have a mutex) (called by DL_DbCreate/Open)
	DAL_DATABASE_INFO_LIST::RemoveDatabase (does not have a mutex) (called by DL_DbClose)
	DAL_DATABASE_INFO_LIST::~DAL_DATABASE_INFO_LIST (called by CSSM_SPI_Detach)

functions that attempt to obtain the reader lock
	DAL_DATABASE_INFO_LIST::FindDatabaseByHandle (does not have a mutex)

functions that lock the mutex
	DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::~DAL_DATABASE_INFO_NODE()
	  called by:   DAL_DATABASE_INFO_LIST::~DAL_DATABASE_INFO_LIST (assumes exclusive access) (called on program unload)
				   DAL_DATABASE_INFO_LIST::AllocateNewDatabase (no lock) (called by DL_DbCreate/Open)
				   DAL_DATABASE_INFO_LIST::nrDeleteDatabase (no lock) (called by DL_DbCreate/Open)
				   DAL_DATABASE_INFO_LIST::RemoveDatabase (no lock) (called by DL_DbClose)

    DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::GetDatabase(PORT_MUTEX_HANDLE &hMutex)
	  called by:   DAL_DATABASE_INFO_LIST::FindDatabaseByHandle (has reader)
*/

/**************************************************************************************************/
/* Member functions for DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE							  */
/**************************************************************************************************/

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE
 *
 * Description:
 * Initializes a DAL_DATABASE_INFO_NODE by filling in all the values, m_pNext = this
 *---------------------------------------------------------------------------*/
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::DAL_DATABASE_INFO_NODE()
{
	m_hMutex = NULL;
	m_pNext = this;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::~DAL_DATABASE_INFO_NODE
 *
 * Description:
 * Destroy a DAL_DATABASE_INFO_NODE, make sure that the Reference Count is
 * zero
 *---------------------------------------------------------------------------*/
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::~DAL_DATABASE_INFO_NODE()
{
	if (m_hMutex)
	{
		if(CSSM_OK != port_LockMutex(m_hMutex, LOCK_TIMEOUT_PERIOD))
		{
			DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		}
        else
        {
            /*
             * You can't close a POSIX mutex if it's locked.
             * You can with a WIN32 mutex, but unlocking it here
             * doesn't hurt anything.
             */
            port_UnlockMutex(m_hMutex);
        }

		port_CloseMutex(m_hMutex);
#ifdef POSIX
	local_free(m_hMutex, NULL);
#endif
	}
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_NODE::Initialize
 *
 * Description:
 * Initialize a DAL_DATABASE_INFO_NODE by creating the mutex that the node uses
 * to control access to a DAL_DATABASE_INFO_NODE among the threads
 *
 * Paramaters:
 * none.
 *---------------------------------------------------------------------------*/
CSSM_RETURN
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::Initialize()
{
    CSSM_RETURN ret;

#ifdef POSIX
    m_hMutex = (PORT_MUTEX_HANDLE) local_malloc(sizeof(pthread_mutex_t), NULL);
#endif
    ret = port_CreateMutex(NULL, &m_hMutex);
	if (CSSM_OK != ret)
	{
#ifdef POSIX
	local_free(m_hMutex, NULL);
#endif
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_NODE::neGetDatabaseReference
 *
 * Description:
 * Returns a refrenece to the database. A database reference can only be returned
 * when the database is not yet entered into the database list
 *
 * Paramaters:
 * none.
 *---------------------------------------------------------------------------*/
DAL_DATABASE_INFO *
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::GetDatabaseReference()
{
	ASSERT(this && m_hMutex);

	/* Make sure that the reference is not the database list */
	ASSERT(m_pNext == this);

	return this;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_NODE::GetDatabase
 *
 * Description:
 * Returns the database from a database node. First, the mutex is locked so that
 * only one thread can access a database at a time. Concurrent access to a physical
 * database is permitted if two handles to the database are opened.
 *
 * Paramaters:
 * hMutex (output) : Returns a handle to the mutex that was used to lock the database,
 *	 this mutex must be released.
 *---------------------------------------------------------------------------*/

CSSM_RETURN DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::GetDatabase(PORT_MUTEX_HANDLE &hMutex,
																		DAL_DATABASE_INFO **dalDbInfo)
{
	CSSM_RETURN retValue = CSSM_OK;
	ASSERT(this && m_hMutex);

	if((retValue = port_LockMutex(m_hMutex, LOCK_TIMEOUT_PERIOD)) != CSSM_OK)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
        hMutex = NULL;
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	hMutex = m_hMutex;
	*dalDbInfo = this;
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_NODE::neGetNext
 *
 * Description:
 * returns the next element in the linked list of databases.
 *
 * Paramaters:
 * none
 *---------------------------------------------------------------------------*/
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE *
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::neGetNext(void) const
{
	return m_pNext;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_NODE::nrSetNext
 *
 * Description:
 * Sets the next element in the linked list of databases.
 *
 * Paramaters:
 * pNext (input) : new value for the next element in the list
 *---------------------------------------------------------------------------*/
void
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::nrSetNext(DAL_DATABASE_INFO_NODE *pNext)
{
	m_pNext = pNext;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_NODE::neGetDBHandle
 *
 * Description:
 * Retieve the DB Handle that is associated with a database
 *
 * Paramaters:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_DL_HANDLE
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::neGetDBHandle(void) const
{
	return m_DBHandle;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_NODE::nrCreateDone
 *
 * Description:
 * Indicates that the node's creation is complete and it is about to be inserted
 * into the list.
 *
 * Paramaters:
 * None.
 *
 * Note:
 * This function can only be executed while the writer lock is held on the
 * database list.
 *---------------------------------------------------------------------------*/
void
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_NODE::nrCreateDone(CSSM_DB_HANDLE InHandle)
{
	static CSSM_DB_HANDLE DBHandle = 0;
	ASSERT(this == m_pNext); /* this should be true of any created node until insert */
	if (InHandle)
		m_DBHandle = InHandle;
	else
		m_DBHandle = ++DBHandle;
}

/**************************************************************************************************/
/* Member functions for DAL_DATABASE_INFO_LIST													  */
/**************************************************************************************************/

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_LIST
 *
 * Description:
 * Initializes the DAL_DATABASE_INFO_LIST by seting the start of the Db List
 *	 and the ParseTable list to NULL
 *---------------------------------------------------------------------------*/
DAL_DATABASE_INFO_LIST::DAL_DATABASE_INFO_LIST()
{
	START_CONSTRUCTOR();

	m_pDbRoot = NULL;
	memset(&m_SWMRLock, 0, sizeof(m_SWMRLock));

	END_CONSTRUCTOR();
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::~DAL_DATABASE_INFO_LIST
 *
 * Description:
 * Destroys the Database List by freeing all the databases that have yet to
 * be freed, and frees the Parsing function list
 *
 * NOTE: This function makes the assumption that it has exclusive access to
 * the database list. This is because the function should only be called
 * on process detach
 *---------------------------------------------------------------------------*/
DAL_DATABASE_INFO_LIST::~DAL_DATABASE_INFO_LIST()
{
	AssertValid();

	/* get the writer lock, but ignore failures, it should be okay to get this because
	   CSSM should not call ModuleDetach unless that hande is not being used */
	if (CSSM_OK != cssm_SWMRLockWaitToWrite(&m_SWMRLock, LOCK_TIMEOUT_PERIOD))
	{
		DEBUG_BREAK(); /* Should not fail */
	}

	DAL_DATABASE_INFO_NODE * temp1 = m_pDbRoot;
	while (temp1 != NULL)
	{
		m_pDbRoot = m_pDbRoot->neGetNext();
		delete temp1;
		temp1 = m_pDbRoot;
	}

	cssm_SWMRLockDoneWriting(&m_SWMRLock);

	cssm_SWMRLockDelete(&m_SWMRLock);
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::neAllocateNewDatabase
 *
 * Description:
 * Allocate a database node structure. A database node inherits off of DAL_DATABASE_INFO,
 * and this will be returned.
 *
 * Parameters:
 * none
 *
 * Return value:
 * A pointer to a DAL_DATABASE_INFO_NODE, and to it's parent class DAL_DATABASE_INFO
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO_LIST::AllocateNewDatabase(DAL_DATABASE_INFO_REF_PTR *ret) const
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();

	DAL_DATABASE_INFO_NODE * NewNode = new DAL_DATABASE_INFO_NODE;
	if (!NewNode)
	{
		*ret = NULL;
		return CSSMERR_DL_MEMORY_ERROR;
	}

	if ((retValue = NewNode->Initialize()) != CSSM_OK)
	{
		delete NewNode;
		*ret = NULL;
		return retValue;
	}

	*ret = NewNode->GetDatabaseReference();

	if (!(*ret))
		delete NewNode;

	return retValue;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::nrDeleteDatabase
 *
 * Description:
 * Deletes a database node structure. This function is only used if the node has
 * not yet been added to the list
 *
 * Parameters:
 * none
 *
 * Return value:
 * A pointer to a DAL_DATABASE_INFO_NODE, and to it's parent class DAL_DATABASE_INFO
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
void DAL_DATABASE_INFO_LIST::nrDeleteDatabase(DAL_DATABASE_INFO_REF_PTR pDb) const
{
	AssertValid();

	DAL_DATABASE_INFO_NODE * pNode = (DAL_DATABASE_INFO_NODE *)pDb;
	if (pNode->neGetNext() != pNode)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return;
	}

	delete pNode;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::Initialize
 *
 * Description:
 * Initialize the the DAL_DATABASE_INFO_LIST, this occurs durring DL_Initialize
 *
 * Parameters:
 * None.
 *
 * Return value:
 * CSSM_OK on success, otherwise Error
 *
 * Error Codes:
 * CSSMERR_INTERNAL_ERROR
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO_LIST::Initialize()
{
	AssertValid();
	START_INITIALIZE();

	if (CSSM_OK != cssm_SWMRLockCreate(&m_SWMRLock, NULL))
	{
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	END_INITIALIZE();
	return CSSM_OK;
}


/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::AddDatabase
 *
 * Description:
 * Adds a Database to the list of databases
 *
 * Parameters:
 * pDb (input) : The database to add, this database WAS allocated with
 * neAllocateNewDatabase, so it is also a pointer to a database node. This can be
 * verified because pDb->m_pNext should point to pDb.
 *
 * Return value:
 * 0 if an error occurred, otherwise the DB handle of the new database
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO_LIST::AddDatabase(DAL_DATABASE_INFO_REF_PTR pDb,
												CSSM_DB_HANDLE **tmpDbHandle)
{
	AssertValid();

	if (pDb == NULL)
		return CSSMERR_DL_INTERNAL_ERROR;

	DAL_DATABASE_INFO_NODE * pNode = (DAL_DATABASE_INFO_NODE *)pDb;
	if (pNode->neGetNext() != pNode)
		return CSSMERR_DL_INTERNAL_ERROR;

	if (CSSM_OK != cssm_SWMRLockWaitToWrite(&m_SWMRLock, LOCK_TIMEOUT_PERIOD))
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	/* Insert at the front of the list */
	pNode->nrCreateDone(**tmpDbHandle);
	pNode->nrSetNext(m_pDbRoot);
	m_pDbRoot = pNode;

	cssm_SWMRLockDoneWriting(&m_SWMRLock);
	**tmpDbHandle = pNode->neGetDBHandle();
	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::RemoveDatabase
 *
 * Description:
 * Removes a database from the list of databases
 *
 * Parameters:
 * DbHandle (input) : Handle to the database to be removed from the list
 *
 * Return value:
 * CSSM_OK on success, otherwise Error
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_DB_HANDLE
 *---------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO_LIST::RemoveDatabase(CSSM_DB_HANDLE DbHandle)
{
	AssertValid();

    if (!DbHandle)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	if (m_pDbRoot)
	{
		if (CSSM_OK != cssm_SWMRLockWaitToWrite(&m_SWMRLock, LOCK_TIMEOUT_PERIOD))
		{
			DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
			return CSSMERR_DL_INTERNAL_ERROR;
		}

		DAL_DATABASE_INFO_NODE *pTemp = m_pDbRoot, *pPrev = NULL;
		while (pTemp != NULL && pTemp->neGetDBHandle() != DbHandle)
		{
			pPrev = pTemp;
			pTemp = pTemp->neGetNext();
		}

		if (pTemp != NULL)
		{
			/* Remove node from the list */
			if (pPrev != NULL)
				pPrev->nrSetNext(pTemp->neGetNext());
			else
				m_pDbRoot = m_pDbRoot->neGetNext();
		}
		else
		{
			cssm_SWMRLockDoneWriting(&m_SWMRLock);
			return CSSMERR_DL_INVALID_DB_HANDLE;
		}

		cssm_SWMRLockDoneWriting(&m_SWMRLock);

		if (pTemp != NULL) delete pTemp;
	}
	else
	{
		return CSSMERR_DL_INVALID_DB_HANDLE;
	}

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::FindDatabaseByHandle
 *
 * Description:
 * If the database is open, this function returns a pointer to the DAL_DATABASE_INFO
 * instance
 *
 * Parameters:
 * hDatabase (input) : Handle to the database to find
 * refDatabaseInfo (output) : A reference (do not free this pointer) to a DAL_DATABASE_INFO.
 *	 if the datbase is found, otherwise NULL
 * hMutex (output) : Mutex that was locked in order to return the handle
 *
 * RETURN:
 * Error if an error occurred, else CSSM_OK (handle not found is not an error).
 *
 * ERROR CODES
 * CSSMERR_DL_INTERNAL_ERROR
 *-----------------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO_LIST::FindDatabaseByHandle(
	CSSM_DB_HANDLE DbHandle,
	DAL_DATABASE_INFO_REF_PTR &refDatabaseInfo,
    PORT_MUTEX_HANDLE &hMutex)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	ASSERT(DbHandle);

	/* Initialization */
	CSSM_BOOL DbFound = CSSM_FALSE;
	DAL_DATABASE_INFO_NODE *pDatabaseInfo = m_pDbRoot;
    hMutex = NULL;
	refDatabaseInfo = NULL;

	if (CSSM_OK != cssm_SWMRLockWaitToRead(&m_SWMRLock, LOCK_TIMEOUT_PERIOD))
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	/* Loop through DatabaseInfo list looking for a matching handle */
	while(pDatabaseInfo != NULL)
	{
		if(pDatabaseInfo->neGetDBHandle() == DbHandle)
		{
			DbFound = CSSM_TRUE;
			break;
		}
		else
			pDatabaseInfo = pDatabaseInfo->neGetNext();
	}

	if(CSSM_FALSE != DbFound)
	{
		retValue = pDatabaseInfo->GetDatabase(hMutex,&refDatabaseInfo);
		cssm_SWMRLockDoneReading(&m_SWMRLock);
		return retValue;
	}
	else
	{
		cssm_SWMRLockDoneReading(&m_SWMRLock);
        hMutex = NULL;
		refDatabaseInfo = NULL;
		return CSSMERR_DL_INVALID_DB_HANDLE;
	}
}

/*-----------------------------------------------------------------------------------
 * Name: DAL_DATABASE_INFO_LIST::neFindDatabaseNode
 *
 * Description:
 * If the database is open, this function returns a pointer to the DAL_DATABASE_INFO
 * instance. This function is private.
 *
 * Parameters:
 * LastFind (input) : the last found database, NULL if this is a new find.
 * hDatabase (input) : Handle to the database
 *
 * Note: Should have the reader or writter lock before calling
 *
 * RETURN:
 * DAL_DATABASE_INFO_PTR or NULL
 *
 * ERROR CODES
 * None.
 *-----------------------------------------------------------------------------------*/
CSSM_RETURN DAL_DATABASE_INFO_LIST::neFindDatabaseNode(DAL_DATABASE_INFO_NODE const * LastFind,
													   const char * DbName,
													   DAL_DATABASE_INFO_NODE **pDatabaseInfo) const
{
	AssertValid();
	ASSERT(DbName);

	/* Initialization */
	CSSM_BOOL bSuccess = CSSM_FALSE;
	if (LastFind)
		*pDatabaseInfo = LastFind->neGetNext();
	else
		*pDatabaseInfo = m_pDbRoot;

	/* Loop through DatabaseInfo list looking for a matching handle */
	while(*pDatabaseInfo != NULL)
	{
		if (strncmp((*pDatabaseInfo)->neGetDbName(), DbName, MAX_DB_NAME_LENGTH) == 0)
		{
			bSuccess = CSSM_TRUE;
			break;
		}
		else
			*pDatabaseInfo = (*pDatabaseInfo)->neGetNext();
	}
	if (bSuccess != CSSM_FALSE)
		return CSSM_OK;
	else
		return CSSMERR_DL_INTERNAL_ERROR;
}


#ifdef _DEBUG

void DAL_DATABASE_INFO_LIST::AssertValidNotInitialized(void) const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_DATABASE_INFO_LIST)));
	ASSERT(m_pDbRoot == NULL);
}

void DAL_DATABASE_INFO_LIST::AssertValid(void) const
{
	ASSERT_DAL_VALID();
	ASSERT(!port_IsBadWritePtr((void *)this, sizeof(DAL_DATABASE_INFO_LIST)));

	if (m_debug_ObjectState == Constructing)
		return;
	if (m_debug_ObjectState == NotInitialized)
	{
		AssertValidNotInitialized();
		return;
	}

	DAL_DATABASE_INFO_NODE * pTempDb = m_pDbRoot;

	while (pTempDb)
	{
		ASSERT_VALID_HEAP_POINTER(pTempDb, sizeof(DAL_DATABASE_INFO_PTR));
		pTempDb->AssertValid();

		pTempDb = pTempDb->neGetNext();
	}
}

#endif /* End debug code */


CSSM_RETURN DAL_DATABASE_INFO_LIST::GetDBNamesAndParameters(
				CSSM_DL_DB_HANDLE DLDBHandle,
				DAL_DB_OPEN_PARAM_PTR pParam)
{
	CSSM_RETURN retValue = CSSM_OK;
	AssertValid();
	ASSERT(DLDBHandle.DLHandle);

	/* Initialization */
	/* CSSM_BOOL DbFound = CSSM_FALSE; */
	DAL_DATABASE_INFO_NODE *pDatabaseInfoNode = m_pDbRoot;

	if (CSSM_OK != cssm_SWMRLockWaitToRead(&m_SWMRLock, LOCK_TIMEOUT_PERIOD))
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return CSSMERR_DL_INTERNAL_ERROR;
	}

	/* Loop through DatabaseInfo list looking for a matching handle */
	while(pDatabaseInfoNode != NULL) {
		if (pDatabaseInfoNode->neGetDBHandle() == DLDBHandle.DBHandle) {
            PORT_MUTEX_HANDLE hMutex;
			DAL_DATABASE_INFO_PTR pDatabaseInfo;

			/*
			 * Get a database object, this will lock the mutex,
			 * which needs to be unlocked before we exit
			 */
			if ((retValue = pDatabaseInfoNode->GetDatabase(hMutex, &pDatabaseInfo)) != CSSM_OK) {
				cssm_SWMRLockDoneReading(&m_SWMRLock);
				return retValue;
			}
			char const * const retName = pDatabaseInfo->neGetDbName();
			if (!retName)
			{
				port_UnlockMutex(hMutex);
				cssm_SWMRLockDoneReading(&m_SWMRLock);
				return CSSMERR_DL_INTERNAL_ERROR;
			}

			const DAL_MODULE_PARAMETERS *pParameters = pDatabaseInfo->neGetModuleParameters();

			if (!pParameters)
			{
				port_UnlockMutex(hMutex);
				cssm_SWMRLockDoneReading(&m_SWMRLock);
				return CSSMERR_DL_INTERNAL_ERROR;
			}

			strcpy (pParam->dbName, retName);
			pParam->dbParameters = (DAL_MODULE_PARAMETERS *)pParameters;

			port_UnlockMutex(hMutex);
			cssm_SWMRLockDoneReading(&m_SWMRLock);
			return CSSM_OK;

		} else {
			pDatabaseInfoNode = pDatabaseInfoNode->neGetNext();
		}
	}

	cssm_SWMRLockDoneReading(&m_SWMRLock);
	return CSSMERR_DL_INVALID_DB_HANDLE;
}
