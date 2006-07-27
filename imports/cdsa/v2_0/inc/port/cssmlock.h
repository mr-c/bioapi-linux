/*-----------------------------------------------------------------------
 * File: CSSMLOCK.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#ifndef _CSSMLOCK_H
#define _CSSMLOCK_H

#ifdef POSIX
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The single-writer/multiple-reader guard 
 * compound synchronization object
 */
typedef struct SingleWriterMultiReaderLock
{

#if defined (WIN32)
   /*
    * This mutex guards access to the other objects
    * managed by this data structure and also indicates 
    * whether any writer threads are writing.
	*/
   HANDLE hMutexNoWriter;

   /*
    * This manual-reset event is signaled when
    * no reader threads are reading.
	*/
   HANDLE hNoReaders;
	
   /*
    * This semaphore is used simply as a counter that is
    * accessible between multiple processes. It is NOT
    * used for thread synchronization.
    * The count is the number of reader threads reading.
	*/
   HANDLE hSemNumReaders;
#elif defined (POSIX)
   pthread_rwlock_t Lock;
#else
#error Lock not implemented yet
/* void *not_implemented_yet; */
#endif

} cssm_SWMRLock, *cssm_SWMRLock_PTR;

/* Initialization value for an SWMR lock. Using this
 * initializer does not make the object ready for use.
 * cssm_SWMRLockCreate must still be called before
 * attempting to lock.
 */
#if defined(WIN32)
#define PORT_SWMR_LOCK_INITIALIZER { 0, 0, 0 }
#elif defined (POSIX)
#define PORT_SWMR_LOCK_INITIALIZER { PTHREAD_RWLOCK_INITIALIZER }
#endif /* WIN32 */

/*
 * Initializes a SWMRG structure. This structure must be 
 * initialized before any writer or reader threads attempt
 * to wait on it.
 * The structure must be allocated by the application and 
 * the structure's address is passed as the first parameter.
 * The lpszName parameter is the name of the object. Pass
 * NULL if you do not want to share the object.
 */
CSSM_RETURN CSSMAPI 
cssm_SWMRLockCreate(cssm_SWMRLock_PTR pSWMRLock,
                    const char *LockName);


                    
 /*
  * Deletes the system resources associated with a SWMRG 
  * structure. The structure must be deleted only when
  * no writer or reader threads in the calling process
  * will wait on it.
  */
CSSM_RETURN CSSMAPI
cssm_SWMRLockDelete(cssm_SWMRLock_PTR pSWMRLock);



 /*
  * A writer thread calls this function to know when 
  * it can successfully write to the shared data.
  */
CSSM_RETURN CSSMAPI
cssm_SWMRLockWaitToWrite(cssm_SWMRLock_PTR pSWMRLock,
                         uint32 timeout);



 /*
  * A writer thread calls this function to let other threads
  * know that it no longer needs to write to the shared data.
  */
CSSM_RETURN CSSMAPI
cssm_SWMRLockDoneWriting(cssm_SWMRLock_PTR pSWMRLock);



 /*
  * A reader thread calls this function to know when 
  * it can successfully read the shared data.
  */
CSSM_RETURN CSSMAPI
cssm_SWMRLockWaitToRead(cssm_SWMRLock_PTR pSWMRLock,
                        uint32 timeout);



 /*
  * A reader thread calls this function to let other threads
  * know when it no longer needs to read the shared data.
  */
CSSM_RETURN CSSMAPI
cssm_SWMRLockDoneReading(cssm_SWMRLock_PTR pSWMRLock);



 /*
  *	A writer thread calls this function to convert to reader thread, such that
  *	no other writer threads can get in before getting the reader lock.
  */
CSSM_RETURN CSSMAPI
cssm_SWMRLockWriteToRead(cssm_SWMRLock_PTR pSWMRLock);

#ifdef POSIX
/* A function to translate POSIX errors into CSSM errors */
CSSM_RETURN 
MapPthreadStatus(int status);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CSSMLOCK_H */
