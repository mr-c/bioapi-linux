/*-----------------------------------------------------------------------
 *      File:   PORTMUTX.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */
 
/* Portions:
 * (C) COPYRIGHT International Business Machines Corp. 1996, 1997
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include "internal.h"

/* Define ASSERT_NOT_MUTEX_OWNER macro that is used to verify that the 
 * current thread does not own the mutex in win32 */
#if defined(_WIN32) && defined(_DEBUG)
#define ASSERT_NOT_MUTEX_OWNER(mutex) \
    _ASSERTE(ReleaseMutex(mutex) == 0 && GetLastError() == ERROR_NOT_OWNER);

#else /* non-win32 or non-debug */
#define ASSERT_NOT_MUTEX_OWNER(mutex)
#endif

#if defined (POSIX)
CSSM_RETURN MapPthreadStatus(int status)
{
    switch (status) {
    case 0:
        return CSSM_OK;
    case EBUSY:
        return CSSMERR_CSSM_LOCK_TIMEOUT;
    case EINVAL:
        return CSSMERR_CSSM_INVALID_POINTER;
    case ENOMEM:
        return CSSMERR_CSSM_MEMORY_ERROR;
    }

    return CSSMERR_CSSM_OS_ACCESS_DENIED;
}
#endif

#if defined (LINUX)
extern int pthread_mutexattr_settype __P ((pthread_mutexattr_t *__attr,
					   int __kind));
#endif	
/* Synchronization / Mutex Routines */

/*-----------------------------------------------------------------------------
 * Name: port_CreateMutex
 *
 * Description:  creates a mutex object
 * 
 * Parameters: 
 * name (input/optional) :      Name of mutex 
 *
 * Note: if 'name' is NULL, the mutex should be a new locking object that is 
 * unique. This should be used to to guard against multi-threading
 * issues (and not multi-process issues).
 *
 * Returns:
 * Pointer to the mutex object
 * NULL if an error occurent
 * 
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED - Unable to create the mutex
 * CSSMERR_CSSM_INVALID_POINTER - name pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_CreateMutex(
    const char *name,
    PORT_MUTEX_HANDLE *mhandle)
{
#if defined (WIN32)
    static SECURITY_DESCRIPTOR sd = {0};
    static SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), &sd,TRUE};
#elif defined (POSIX)
    pthread_mutexattr_t attr;
    int status;
#endif

    TRACE(port_trace_info, FNCODE_cssm_CreateMutex);
    
    if (mhandle == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

#if defined (WIN32)
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
    
    if (NULL != name) 
    { 
        if (port_IsBadReadPtr(name, 1))
        {
            RETURN(CSSMERR_CSSM_INVALID_POINTER);
        }
        if (port_IsBadReadPtr(name, strlen(name)))
        {
            RETURN(CSSMERR_CSSM_INVALID_POINTER);
        }
    }

    *mhandle = NULL;
    *mhandle = CreateMutex(&sa, FALSE, name);
    if (*mhandle == NULL)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }

#elif defined (POSIX)
    if ((status = pthread_mutexattr_init(&attr)))
    {
        goto cleanup;
    }

    /*
     * WIN32 mutexes are recursive, so match that behavior.
     */

    if ((status = pthread_mutexattr_settype(&attr, 
#if defined (LINUX) 
        PTHREAD_MUTEX_RECURSIVE_NP)))
#else
        PTHREAD_MUTEX_RECURSIVE)))
#endif
    {
        (void) pthread_mutexattr_destroy(&attr);
        goto cleanup;
    }

    if ((status = pthread_mutex_init(*mhandle, &attr)))
    {
        (void) pthread_mutexattr_destroy(&attr);
        goto cleanup;
    }

cleanup:
    RETURN(MapPthreadStatus(status));
#else
    *mhandle = (void *) 1; /* Any non-zero value will do. */
#endif

    RETURN(CSSM_OK);
}



/*-----------------------------------------------------------------------------
 * Name: port_LockMutex
 *
 * Description:  locks a mutex object
 * 
 * Parameters: 
 * mutex (input)            :       Pointer to mutex object
 * timeout (input)          :       Number of milliseconds to wait on mutex 
 *
 * Returns:
 * CSSM_OBJECT_LOCKED - Mutex successfully locked
 * CSSM_OBJECT_BUSY - Mutex is busy, cannot lock
 * CSSM_OBJECT_ERROR - See error codes
 * 
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED - Unable to lock the mutex
 * CSSMERR_CSSM_INVALID_POINTER - mutex pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_LockMutex(
    PORT_MUTEX_HANDLE mutex, 
    uint32 timeout)
{
#if defined(WIN32)
    uint32 ret_val;
#elif defined (POSIX)
    int status, cnt;
#endif

/*    printf("portmutx.c: port_LockMutex() -  Entered function!!!\n");*/
    TRACE(port_trace_info, FNCODE_cssm_LockMutex);

    if (mutex == NULL)
    {
/*        printf("portmutx.c: port_LockMutex() -  Error 1.... mutex = %d\n",mutex);*/
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

#if defined(WIN32)
    ASSERT_NOT_MUTEX_OWNER(mutex);

    ret_val = WaitForSingleObject(mutex, timeout);
    
    if (ret_val == WAIT_FAILED)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
    else
    {
        if (ret_val == WAIT_OBJECT_0 || ret_val == WAIT_ABANDONED)
        {
            RETURN(CSSM_OK);
        }
        else if (ret_val == WAIT_TIMEOUT)
        {
            RETURN(CSSMERR_CSSM_LOCK_TIMEOUT);
        }
    }
    
    RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
#elif defined (POSIX)
    if (timeout == 0)
    {
/*        printf("portmutx.c: port_LockMutex() -  Cond 2....\n");*/
        RETURN(MapPthreadStatus(pthread_mutex_trylock(mutex)));
    }
    else if (timeout == CSSM_INFINITE_WAIT)
    {
/*        printf("portmutx.c: port_LockMutex() -  Cond  3....\n");*/
        RETURN(MapPthreadStatus(pthread_mutex_lock(mutex)));
    }
    else if (timeout < CSSM_POLL_INT)
    {
/*        printf("portmutx.c: port_LockMutex() -  Cond 4....\n");*/
        timeout = CSSM_POLL_INT;
    }

    cnt = (timeout + CSSM_POLL_INT - 1) / CSSM_POLL_INT;
    while (cnt-- > 0)
    {
        if ((status = pthread_mutex_trylock(mutex)) != EBUSY)
        {
            RETURN(MapPthreadStatus(status));
        }

        usleep(CSSM_POLL_INT * 1000);
    }

    RETURN(CSSMERR_CSSM_LOCK_TIMEOUT);
#else
    RETURN(CSSM_OK);
#endif
}



/*-----------------------------------------------------------------------------
 * Name: port_TrylockMutex
 *
 * Description:  Try to lock a mutex object
 * 
 * Parameters: 
 * mutex (input)            :       Pointer to mutex object
 *
 * Returns:
 * CSSM_OBJECT_LOCKED - Mutex successfully locked
 * CSSM_OBJECT_BUSY - Mutex is busy, cannot lock
 * CSSM_OBJECT_ERROR - See error codes
 * 
 * Error Codes:
 * CSSMERR_CSSM_LOCK_TIMEOUT - Unable to lock the mutex
 * CSSMERR_CSSM_INVALID_POINTER - mutex pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_TrylockMutex(
    PORT_MUTEX_HANDLE mutex)
{
#if defined (WIN32)
    uint32 ret_val;
#endif

    TRACE(port_trace_info, FNCODE_cssm_TrylockMutex);

    if (mutex == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

#if defined (WIN32)
    ret_val = WaitForSingleObject(mutex, CSSM_ZERO_WAIT);
    
    if (ret_val == WAIT_FAILED)
    {
        RETURN(CSSMERR_CSSM_LOCK_TIMEOUT);
    }
    else
    {
        if (ret_val == WAIT_OBJECT_0 || ret_val == WAIT_ABANDONED)
        {
            RETURN(CSSM_OK);
        }
        else if (ret_val == WAIT_TIMEOUT)
        {
            RETURN(CSSMERR_CSSM_LOCK_TIMEOUT);
        }
    }
    RETURN(CSSMERR_CSSM_LOCK_TIMEOUT);
#elif defined (POSIX)
    RETURN(MapPthreadStatus(pthread_mutex_trylock(mutex)));
#else
    RETURN(CSSM_OK);
#endif
}



/*-----------------------------------------------------------------------------
 * Name: port_UnlockMutex
 *
 * Description: unlocks a mutex object
 * 
 * Parameters: 
 * mutex (input)            :       Pointer to mutex object
 *
 * Returns:
 * CSSM_OBJECT_UNLOCKED - Mutex successfully unlocked
 * CSSM_OBJECT_ERROR - See error codes
 * 
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED - Unable to lock the mutex
 * CSSMERR_CSSM_INVALID_POINTER - mutex pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_UnlockMutex(
    PORT_MUTEX_HANDLE mutex)
{
    TRACE(port_trace_info, FNCODE_cssm_UnlockMutex);

    if (mutex == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

#if defined (WIN32)
    if (ReleaseMutex(mutex))
#elif defined (POSIX)
    RETURN(MapPthreadStatus(pthread_mutex_unlock(mutex)));
#endif
    {
        RETURN(CSSM_OK);
    }

    RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
}



/*-----------------------------------------------------------------------------
 * Name: port_CloseMutex
 *
 * Description:  Closes and destorys a mutex object
 * 
 * Parameters: 
 * mutex (input)            :       Pointer to mutex object
 *
 * Returns:
 * CSSM_OK if successful
 * CSSM_FAIL if an error occurred
 * 
 * Error Codes:
 * CSSM_CANT_CLOSE_OBJECT - Unable to lock the mutex
 * CSSMERR_CSSM_INVALID_POINTER - mutex pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI port_CloseMutex (PORT_MUTEX_HANDLE mutex)
{
    TRACE(port_trace_info, FNCODE_cssm_CloseMutex);

    if (mutex == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

#if defined (WIN32)
    if (CloseHandle(mutex))
#elif defined (POSIX)
    RETURN(MapPthreadStatus(pthread_mutex_destroy(mutex)));
#endif
    {
        RETURN(CSSM_OK);
    }

    RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
}
