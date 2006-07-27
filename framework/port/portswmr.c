/*-----------------------------------------------------------------------
 * File: CSSM_LOCK.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

 
/*
 * This file contains the code for interfaces used for handling the 
 * single-writer-multiple-reader locking mechanism.
 */



#include "internal.h"


/* define ASSERT_NOT_MUTEX_OWNER macro that is used to verify that the current
   thread does not own the mutex in win32
*/
#if defined(_WIN32) && defined(_DEBUG)

#include <crtdbg.h>
static void ASSERT_NOT_MUTEX_OWNER(void * mutex) 
{
    DWORD Win32Error;
    DWORD ReturnValue;

    ReturnValue = ReleaseMutex(mutex);
    Win32Error = GetLastError();

    /* Check previous call on call stack to see why this failed */
    _ASSERT(((0 == ReturnValue) &&
             (ERROR_NOT_OWNER == Win32Error)));
}

#else /* non-win32 or non-debug */
#define ASSERT_NOT_MUTEX_OWNER(mutex)
#endif


/*-----------------------------------------------------------------------
 *
 *Name: cssm_SWMRLockCreate
 *
 *Description:
 * Initializes a SWMR structure. This structure must be 
 * initialized before any writer or reader threads attempt
 * to wait on it.
 * The structure must be allocated by the application and 
 * the structure's address is passed as the first parameter.
 *
 *
 *Parameters:
 * SWMRLock(input) - Single-writer-multiple-reader lock to be intialized.
 * LocakName(input/optional) - Name of the lock
 *
 *Note: If the reader/writter lock is being used to guard against multi-threading
 * issues (and not multi-process issues), then LockName should be NULL.
 *
 *Returns:
 * CSSM_OK - On succesful intialization
 * All other results are errors.
 *---------------------------------------------------------------------*/

CSSM_RETURN CSSMAPI 
cssm_SWMRLockCreate(
    cssm_SWMRLock_PTR pSWMRLock, 
    const char *LockName)
{
    CSSM_RETURN ret = CSSM_OK;

#ifdef WIN32

    char *mutexName = NULL;
    char *eventName = NULL;
    char *semaName = NULL;

    static SECURITY_DESCRIPTOR sd = {0};
    static SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), &sd, TRUE};

	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

    if(NULL != LockName)
    {
        mutexName = local_calloc(1,
                                 (strlen(LockName)+1+strlen("mutex")),
                                 NULL);
        if(NULL != mutexName)
        {
            strcpy(mutexName,
                   LockName);
            strcat(mutexName,
                   "mutex");

            eventName = local_calloc(1,
                                     (strlen(LockName)+1+strlen("event")),
                                     NULL);
            if(NULL != eventName)
            {
                strcpy(eventName,
                       LockName);
                strcat(eventName,
                       "event");

                semaName = local_calloc(1,
                                        (strlen(LockName)+1+strlen("sema")),
                                        NULL);
                if(NULL != semaName)
                {
                    strcpy(semaName,
                           LockName);
                    strcat(semaName,
                           "sema");
                }
                else
                {
                    ERR(ret = CSSMERR_CSSM_MEMORY_ERROR);
                }
            }
            else
            {
                ERR(ret = CSSMERR_CSSM_MEMORY_ERROR);
            }
        }
        else
        {
            ERR(ret = CSSMERR_CSSM_MEMORY_ERROR);
        }
    }

    if(CSSM_OK == ret)
    {
        /*
         * Initialize all data members to NULL so that we can
         * accurately check whether an error has occurred.
         */
        pSWMRLock->hMutexNoWriter = NULL;
        pSWMRLock->hNoReaders = NULL;
        pSWMRLock->hSemNumReaders = NULL;

        /*
         * This mutex guards access to the other objects
         * managed by this data structure and also indicates 
         * whether there are any writer threads writing.
         * Initially no thread owns the mutex.
         */
        pSWMRLock->hMutexNoWriter = CreateMutex(&sa,
                                                FALSE,
                                                mutexName);

        /*
         * Create the event determines that the lock is available when 
         * no reader threads are reading.
         * Initially no reader threads are reading.
         */
        pSWMRLock->hNoReaders = CreateEvent(&sa,
                                            TRUE,
                                            TRUE,
                                            eventName);

        /*
         * Initialize the variable that indicates the number of 
         * reader threads that are reading.
         * Initially no reader threads are reading, so the initial count of
         * semaphore is 0.
         */
        pSWMRLock->hSemNumReaders = CreateSemaphore(&sa,
                                                    0,
                                                    0x7FFFFFFF,
                                                    semaName);

        if((NULL == pSWMRLock->hMutexNoWriter)   || 
           (NULL == pSWMRLock->hNoReaders)  ||
           (NULL == pSWMRLock->hSemNumReaders)) 
        {
            /*
             * If a synchronization object could not be created,
             * destroy any created objects and return failure.
             */
            cssm_SWMRLockDelete(pSWMRLock);
            /*
             * note: it might happen that the last ret was ok, but
             * a previous ret was not and hence this condition is 
             * captured!!
             */
            ERR(ret = CSSMERR_CSSM_INTERNAL_ERROR);
        }
    }

    /*
     * free the locally allocated memory.
     */
    if (mutexName)
        local_free(mutexName, NULL);
    if (eventName)
        local_free(eventName, NULL);
    if (semaName)
        local_free(semaName, NULL);

#elif defined (POSIX)

    ret = MapPthreadStatus(pthread_rwlock_init( &(pSWMRLock->Lock), NULL ));

#endif

    return(ret);
}


/*-----------------------------------------------------------------------
 *
 *Name: cssm_SWMRLockDelete
 *
 *Description:
 * Deletes the system resources associated with a SWMRG 
 * structure. The structure must be deleted only when
 * no writer or reader threads in the calling process
 * will wait on it.
 *
 *Parameters:
 * SWMRLock(input) - Single-writer-multiple-reader lock to be deleted.
 *
 *Returns:
 * void
 *---------------------------------------------------------------------*/

CSSM_RETURN CSSMAPI cssm_SWMRLockDelete(cssm_SWMRLock_PTR pSWMRLock)
{
    CSSM_RETURN ret = CSSM_OK;

#ifdef WIN32

    /*
     * Destroy any synchronization objects that were 
     * successfully created.
     */

    if (NULL != pSWMRLock->hMutexNoWriter)
        CloseHandle(pSWMRLock->hMutexNoWriter);

    if (NULL != pSWMRLock->hNoReaders)
        CloseHandle(pSWMRLock->hNoReaders);

    if (NULL != pSWMRLock->hSemNumReaders)
        CloseHandle(pSWMRLock->hSemNumReaders);

#elif defined (POSIX)

    ret = MapPthreadStatus( pthread_rwlock_destroy( &(pSWMRLock->Lock) ) );

#endif

    return (ret);   
}


/*-----------------------------------------------------------------------
 *
 *Name: cssm_SWMRLockWaitToWrite
 *
 *Description:
 *  A writer thread calls this function to know when it can successfully 
 *  write to the shared data.
 *
 *Parameters:
 * SWMRLock(input) - Single-writer-multiple-reader lock.
 * timeout(input) - timeout (in milliseconds) to wait for the writer lock.
 *
 *Returns:
 * CSSM_OBJECT_LOCKED - pSWMRLock is successfully locked
 * CSSM_OBJECT_BUSY - pSWMRLock is busy, cannot lock
 * CSSM_OBJECT_ERROR - See error codes
 *
 * Error Codes:
 * CSSM_LOCK_OBJECT_FAILED - Unable to lock the mutex
 * CSSM_INVALID_OBJECT_PTR - mutex pointer is not valid
 *---------------------------------------------------------------------*/

CSSM_RETURN CSSMAPI 
cssm_SWMRLockWaitToWrite(
    cssm_SWMRLock_PTR pSWMRLock, 
    uint32 timeout)
{
    CSSM_RETURN ret = CSSM_OK;

#ifdef WIN32
    HANDLE aHandles[2];

    /*
     * We can write if the following are true:
     * 1. The mutex guard is available and
     *    no other threads are writing.
     * 2. No threads are reading.
     */
    
    /* This assertion will fail if the mutex is not valid (it was not created,
       or has already been closed), or if you would deadlock your self by
       attempting to get the writer lock when you already have the writer lock
       for the current thread
    */
    ASSERT_NOT_MUTEX_OWNER(pSWMRLock->hMutexNoWriter);

    aHandles[0] = pSWMRLock->hMutexNoWriter;
    aHandles[1] = pSWMRLock->hNoReaders;

    ret = (DWORD)WaitForMultipleObjects(2,
                                        aHandles,
                                        TRUE,
                                        timeout);

    if(WAIT_TIMEOUT == (DWORD)ret)
    {
        ERR(ret = CSSMERR_CSSM_INTERNAL_ERROR);
    }
    else
    {
        if(WAIT_OBJECT_0 == (DWORD)ret)
        {
            ret = CSSM_OK;
        }
        else
        {
            ERR(ret = CSSMERR_CSSM_INTERNAL_ERROR);
        }
    }

#elif defined (POSIX)

    ret = MapPthreadStatus(pthread_rwlock_wrlock( &(pSWMRLock->Lock) ) );

#endif

    return(ret);
}

/*-----------------------------------------------------------------------
 *
 *Name: cssm_SWMRLockDoneWriting
 *
 *Description:
 *  A writer thread calls this function to let other threads
 *  know that it no longer needs to write to the shared data.
 *
 *Parameters:
 * SWMRLock(input) - Single-writer-multiple-reader lock.
 *
 *Returns:
 * void
 *---------------------------------------------------------------------*/

CSSM_RETURN CSSMAPI 
cssm_SWMRLockDoneWriting(
    cssm_SWMRLock_PTR pSWMRLock)
{
    /*
     * Presumably, a writer thread calling this function has
     * successfully called WaitToWrite. This means that we
     * do not have to wait on any synchronization objects 
     * here because the writer already owns the mutex.
     *
     * Allow other writer/reader threads to use
     * the SWMRG synchronization object.
     */
    CSSM_RETURN ret = CSSM_OK;

#ifdef WIN32

    ReleaseMutex(pSWMRLock->hMutexNoWriter);

#elif defined (POSIX)

    ret = MapPthreadStatus( pthread_rwlock_unlock( &(pSWMRLock->Lock) ) );

#endif

    return ret;
}

/*-----------------------------------------------------------------------
 *
 *Name: cssm_SWMRLockWaitToRead
 *
 *Description:
 *  A reader thread calls this function to know when it can successfully read
 *  the shared data.
 *
 *Parameters:
 * SWMRLock(input) - Single-writer-multiple-reader lock.
 * timeout(input) - timeout (in milliseconds) to wait for the reader lock.
 *
 *Returns:
 * CSSM_OBJECT_LOCKED - pSWMRLock is successfully locked
 * CSSM_OBJECT_BUSY - pSWMRLock is busy, cannot lock
 * CSSM_OBJECT_ERROR - See error codes
 *
 * Error Codes:
 * CSSM_LOCK_OBJECT_FAILED - Unable to lock the mutex
 * CSSM_INVALID_OBJECT_PTR - mutex pointer is not valid
 *---------------------------------------------------------------------*/

CSSM_RETURN CSSMAPI 
cssm_SWMRLockWaitToRead(
    cssm_SWMRLock_PTR pSWMRLock, 
    uint32 timeout)
{
    CSSM_RETURN ret = CSSM_OK;
#ifdef WIN32
    sint32 prevCount;

    /*
     * We can read if the mutex guard is available
     * and no threads are writing.
     */

    /* This assertion will fail if the mutex is not valid (it was not created,
       or has already been closed), or if you would deadlock your self by
       getting the reader lock when your current thread has the writer lock
    */
    ASSERT_NOT_MUTEX_OWNER(pSWMRLock->hMutexNoWriter);


    ret = (DWORD)WaitForSingleObject(pSWMRLock->hMutexNoWriter,
                                     timeout);

    if(WAIT_TIMEOUT != (DWORD)ret) 
    {
        if(WAIT_OBJECT_0 == ret)
        {
            /*
             * This thread can read from the shared data.
             *
             * Increment the number of reader threads.
             */
            ReleaseSemaphore(pSWMRLock->hSemNumReaders,
                             1,
                             (long*)&prevCount);
            if (prevCount == 0) 
            {
                /*
                 * If this is the first reader thread, 
                 * set our event to reflect this.
                 */
                ResetEvent(pSWMRLock->hNoReaders);
            }

            /*
             * Allow other writer/reader threads to use
             * the SWMRG synchronization object.
             */
            ReleaseMutex(pSWMRLock->hMutexNoWriter);
        }
        else
        {
            ERR(ret = CSSMERR_CSSM_INTERNAL_ERROR);
        }
    }
    else
    {
        ERR(ret = CSSMERR_CSSM_INTERNAL_ERROR);
    }

#elif defined (POSIX)

    ret = MapPthreadStatus( pthread_rwlock_rdlock( &(pSWMRLock->Lock) ) );

#endif

    return(ret);
}

/*-----------------------------------------------------------------------
 *
 *Name: cssm_SWMRLockDoneReading
 *
 *Description:
 *  A reader thread calls this function to let other threads know when it 
 *  no longer needs to read the shared data.
 *
 *Parameters:
 * SWMRLock(input) - Single-writer-multiple-reader lock.
 *
 *Returns:
 * void
 *---------------------------------------------------------------------*/

CSSM_RETURN  CSSMAPI cssm_SWMRLockDoneReading (
                                        cssm_SWMRLock_PTR pSWMRLock)
{
    CSSM_RETURN ret = CSSM_OK;

#ifdef WIN32

    HANDLE aHandles[2];
    sint32 NumReaders;

    /*
     * We can stop reading if the mutex guard is available,
     * but when we stop reading we must also decrement the
     * number of reader threads.
     */

    /* This assertion will fail if the mutex is not valid (it was not created,
       or has already been closed), or if you would deadlock your self by
       releasing the reader lock when your current thread has the writer lock
       (which means that you are calling DoneReading instead of DoneWriting)
    */
    ASSERT_NOT_MUTEX_OWNER(pSWMRLock->hMutexNoWriter);
    
    aHandles[0] = pSWMRLock->hMutexNoWriter;
    aHandles[1] = pSWMRLock->hSemNumReaders;

    WaitForMultipleObjects(2,
                           aHandles,
                           TRUE,
                           INFINITE);

    /*
     * Get the remaining number of readers by releasing the
     * semaphore and then restoring the count by immediately
     * performing a wait.
     */
    ReleaseSemaphore(pSWMRLock->hSemNumReaders,
                     1,
                     (long*)&NumReaders);
    WaitForSingleObject(pSWMRLock->hSemNumReaders,
                        INFINITE);

    /*
     * If there are no remaining readers, 
     * set the event to relect this.
     */
    if(0 == NumReaders) 
    {
        /*
         * If there are no reader threads, 
         * set our event to reflect this.
         */
        SetEvent(pSWMRLock->hNoReaders);
    }

    /*
     * Allow other writer/reader threads to use
     * the SWMRG synchronization object.
     */
    ReleaseMutex(pSWMRLock->hMutexNoWriter);

#elif defined (POSIX)

    ret = MapPthreadStatus( pthread_rwlock_unlock( &(pSWMRLock->Lock) ) );

#endif

    return ret;
}


/*-----------------------------------------------------------------------
 *
 *Name: cssm_SWMRLockWriteToRead
 *
 *Description:
 *  A writer thread calls this function to convert to reader thread, such that
 *  no other writer threads can get in before getting the reader lock.
 *
 *Parameters:
 * SWMRLock(input) - Single-writer-multiple-reader lock.
 *
 *Returns:
 * CSSM_OK - pSWMRLock is successfully locked
 * CSSM_OBJECT_BUSY - pSWMRLock is busy, cannot lock
 * CSSM_OBJECT_ERROR - See error codes
 *
 * Error Codes:
 * CSSM_LOCK_OBJECT_FAILED - Unable to lock the mutex
 * CSSM_INVALID_OBJECT_PTR - mutex pointer is not valid
 *---------------------------------------------------------------------*/

CSSM_RETURN CSSMAPI 
cssm_SWMRLockWriteToRead(
    cssm_SWMRLock_PTR pSWMRLock)
{
    CSSM_RETURN ret = CSSM_OK;

#ifdef WIN32

    sint32 prevCount;

    /*
     *
     * Increment the number of reader threads.
     */
    ReleaseSemaphore(pSWMRLock->hSemNumReaders,
                     1,
                     (long*)&prevCount);
    if (prevCount == 0) 
    {
        /*
         * If this is the first reader thread, 
         * set our event to reflect this.
         */
        ResetEvent(pSWMRLock->hNoReaders);
    }

    /*
     * Allow other writer/reader threads to use
     * the SWMRG synchronization object.
     */
    ReleaseMutex(pSWMRLock->hMutexNoWriter);

#elif defined (POSIX)

    /* with pthreads, there is no easy way to convert a writer
       lock to a reader, so we'll just unlock and re-lock as a
       reader thread */
    ret = MapPthreadStatus(pthread_rwlock_unlock( &(pSWMRLock->Lock) ) );
    if( ret == CSSM_OK )
        ret = MapPthreadStatus( pthread_rwlock_rdlock( &(pSWMRLock->Lock) ) );

#endif

    return ret;
}
