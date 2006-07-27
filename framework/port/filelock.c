/*-----------------------------------------------------------------------
 * File: FILELOCK.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#include "internal.h"
#include <assert.h>
#ifdef WIN32
#include <io.h>
#endif

#ifdef WIN32
/* Use a platform indicator to determine how to lock and unlock files.
 * Windows NT supports reader/writer locking but Windows 95 and 
 * Windows 98 do not.
 */
static DWORD g_PlatformID = (DWORD)(~0); /* Invalid Win32 platform constant */

static CSSM_RETURN port_DeterminePlatform()
{
    CSSM_RETURN rv = CSSM_OK;

    if ( g_PlatformID == ~0 )
    {
        OSVERSIONINFO VerInfo;

        VerInfo.dwOSVersionInfoSize = sizeof(VerInfo);
        if ( !GetVersionEx( &VerInfo ) )
        {
            ERR( rv = CSSM_ERRCODE_INTERNAL_ERROR );
        }
        else
        {
            g_PlatformID = VerInfo.dwPlatformId;
        }
    }

    return rv;
}
#endif

#ifdef UNIX
static CSSM_RETURN ErrnoToErrcode(int err)
{
    CSSM_RETURN rv;

    switch (err) {
    case EBADF:
        rv = CSSM_ERRCODE_INVALID_DATA;
        break;
    case EACCES:
    case EAGAIN:
    case EDEADLK:
        rv = CSSM_ERRCODE_FUNCTION_FAILED;
        break;
    default:
        rv = CSSM_ERRCODE_INTERNAL_ERROR;
        break;
    }

    return rv;
}
#endif
/* These functions lock regions of a file for shared or exclusive access.
 * They operate on file descriptors returned from the open or creat system
 * calls.
 *
 * Parameters:
 *  FileDescriptor (input) File to lock.
 *  LockType       (input) Mask of the following values that determine the
 *                         type of locking operation.
 *    PORT_FLOCK_READ      Shared lock. Multiple processes and/or threads
 *                         can have shared locks at the same time.
 *    PORT_FLOCK_WRITE     Exclusive lock. A single process is given
 *                         exclusive access to the file region.
 *    PORT_FLOCK_NBLOCK    Causes the function to return immediately if
 *                         another thread or process has already locked the
 *                         region. If this flag is not set, the function
 *                         will block until the lock is obtained.
 *
 * Return Codes:
 *  CSSM_ERRCODE_INVALID_DATA
 *      The file descriptor is invalid.
 *  CSSM_ERRCODE_FUNCTION_FAILED
 *      Another thread or process already holds the lock
 *  CSSM_ERRCODE_INTERNAL_ERROR
 *      The lock could not be obtained for an unknown reason.
 */

CSSM_RETURN port_LockFile(
                        int FileDescriptor,
                        PORT_FLOCK_TYPE LockType,
                        const PORT_FLOCK_RANGE *LockRange )
{
    CSSM_RETURN rv = CSSM_OK;

    assert( ( LockType & PORT_FLOCK_READ ) ||
            ( LockType & PORT_FLOCK_WRITE ) );
    assert( LockRange );

#if defined(WIN32)
    { /* Win32 */
        HANDLE hFile;
        OVERLAPPED StartValue;
        DWORD Flags = 0;

        if ( g_PlatformID == (DWORD)(~0) ) /* Avoid a function call if we can */
        {
            rv = port_DeterminePlatform();
        }

        if ( rv == CSSM_OK )
        {
            hFile = (HANDLE)_get_osfhandle( FileDescriptor );
            if ( hFile == (HANDLE)-1 )
            {
                ERR( rv = CSSM_ERRCODE_INVALID_DATA );
            }
            else
            {
                if ( g_PlatformID == VER_PLATFORM_WIN32_NT )
                { /* Windows NT */
                    /* Setup the OVERLAPPED structure for locking */
                    StartValue.hEvent = 0; /* No asynchronous I/O */
                    StartValue.Offset = LockRange->Start;
                    StartValue.OffsetHigh = 0;

                    /* Determine the correct set of lock flags */
                    if ( LockType & PORT_FLOCK_WRITE )
                    {
                        Flags |= LOCKFILE_EXCLUSIVE_LOCK;
                    }
                    if ( LockType & PORT_FLOCK_NBLOCK )
                    {
                        Flags |= LOCKFILE_FAIL_IMMEDIATELY;
                    }

                    /* Attempt to lock the file region */
                    if ( !LockFileEx( hFile,
                                      Flags,
                                      0,
                                      LockRange->Length,
                                      0,
                                      &StartValue ) )
                    {   
                        DWORD Err = GetLastError();
                        if ( Err == ERROR_LOCK_FAILED )
                        {
                            ERR( rv = CSSM_ERRCODE_FUNCTION_FAILED );
                        }
                        else
                        {
                            ERR( rv = CSSM_ERRCODE_INTERNAL_ERROR );
                        }
                    } /* Lock the file region */
                } /* Windows NT */
                else if ( g_PlatformID == VER_PLATFORM_WIN32_WINDOWS )
                { /* Windows 95/98 */
                    /* Attempt to lock the file region */
                    if ( !LockFile( hFile,
                                    LockRange->Start,
                                    0,
                                    LockRange->Length,
                                    0 ) )
                    {   
                        DWORD Err = GetLastError();
                        if ( Err == ERROR_LOCK_FAILED )
                        {
                            ERR( rv = CSSM_ERRCODE_FUNCTION_FAILED );
                        }
                        else
                        {
                            ERR( rv = CSSM_ERRCODE_INTERNAL_ERROR );
                        }
                    } /* Lock the file region */
                } /* Windows 95/98 */
                else
                { /* Windows ??? (CE?, 32s?, 3.1?) */
                    ERR( rv = CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED );
                } /* Windows ??? (CE?, 32s?, 3.1?) */
            } /* Get the OS specific handle */
        } /* Determine platform type */
    } /* Win32 */
#elif defined(UNIX)
    {
        int cmd;
        struct flock lock;
        cmd = (LockType & PORT_FLOCK_NBLOCK) ? F_SETLK : F_SETLKW;
        lock.l_type   =
            (LockType & PORT_FLOCK_READ) ? F_RDLCK : F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start  = LockRange->Start;
        lock.l_len    = LockRange->Length;

        if (fcntl(FileDescriptor, cmd, &lock))
            rv = ErrnoToErrcode(errno);
    }
#else
# error "port_LockFile not ported to this platform!"
#endif

    return rv;
}

CSSM_RETURN port_UnlockFile(
                        int FileDescriptor,
                        const PORT_FLOCK_RANGE *LockRange )
{
    CSSM_RETURN rv = CSSM_OK;

    assert( LockRange );

#if defined(WIN32)
    { /* Win32 */
        HANDLE hFile;
        OVERLAPPED StartValue;

        if ( g_PlatformID == (DWORD)(~0) ) /* Avoid a function call if we can */
        {
            rv = port_DeterminePlatform();
        }

        if ( rv == CSSM_OK )
        {
            hFile = (HANDLE)_get_osfhandle( FileDescriptor );
            if ( hFile == (HANDLE)-1 )
            {
                ERR( rv = CSSM_ERRCODE_INVALID_DATA );
            }
            else
            {
                if ( g_PlatformID == VER_PLATFORM_WIN32_NT )
                { /* Windows NT */
                    /* Setup the OVERLAPPED structure for locking */
                    StartValue.hEvent = 0; /* No asynchronous I/O */
                    StartValue.Offset = LockRange->Start;
                    StartValue.OffsetHigh = 0;

                    /* Attempt to lock the file region */
                    if ( !UnlockFileEx( hFile,
                                        0,
                                        LockRange->Length,
                                        0,
                                        &StartValue ) )
                    {   
                        DWORD Err = GetLastError();
                        if ( Err == ERROR_LOCK_FAILED )
                        {
                            ERR( rv = CSSM_ERRCODE_FUNCTION_FAILED );
                        }
                        else
                        {
                            ERR( rv = CSSM_ERRCODE_INTERNAL_ERROR );
                        }
                    } /* Unlock the file region */
                } /* Windows NT */
                else if ( g_PlatformID == VER_PLATFORM_WIN32_WINDOWS )
                { /* Windows 95/98 */
                    /* Attempt to lock the file region */
                    if ( !UnlockFile( hFile,
                                      LockRange->Start,
                                      0,
                                      LockRange->Length,
                                      0 ) )
                    {   
                        DWORD Err = GetLastError();
                        if ( Err == ERROR_LOCK_FAILED )
                        {
                            ERR( rv = CSSM_ERRCODE_FUNCTION_FAILED );
                        }
                        else
                        {
                            ERR( rv = CSSM_ERRCODE_INTERNAL_ERROR );
                        }
                    } /* Unlock the file region */
                } /* Windows 95/98 */
                else
                { /* Windows ??? (CE?, 32s?, 3.1?) */
                    ERR( rv = CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED );
                } /* Windows ??? (CE?, 32s?, 3.1?) */
            } /* Get the OS specific handle */
        } /* Determine the platform ID */
    } /* Win32 */
#elif defined(UNIX)
    {
        int cmd;
        struct flock lock;
        cmd = F_SETLK;
        lock.l_type   = F_UNLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start  = LockRange->Start;
        lock.l_len    = LockRange->Length;

        if (fcntl(FileDescriptor, cmd, &lock))
            rv = ErrnoToErrcode(errno);
    }
#else
# error "port_UnlockFile not ported to this platform!"
#endif

    return rv;
}
