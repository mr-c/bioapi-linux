/*-----------------------------------------------------------------------
 * File: THREADLOCAL.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/* Implements the Thread Local Storage (TLS) interfaces to the porting
 * library.
 */

#include "internal.h"

/*-------------------------------------------------------------------------
 * port_AllocateTlsIndex
 *
 * Description
 *  Allocates a TLS storage slot capable of storing a data pointer value.
 *  The initial value of the slot will be NULL.
 *
 * Parameters
 *  pIndex (output)
 *    The new TLS index.
 *  pDestructor (input/optional)
 *    A TLS value destructor function. In a POSIX environment, this value
 *    is registered as the destructor function for the TLS slot. In a Win32
 *    environment, this value is ignored and the PORT_CALL_TLS_DESTRUCTOR
 *    macro must be used in the DllMain function when the event type is
 *    THREAD_DETACH.
 *
 * Return Values
 *  CSSM_OK - The TLS slot was sucessfully created.
 *  CSSMERR_CSSM_MEMORY_ERROR - Memory could not be allocated for the index.
 *  CSSM_FAIL - General error.
 *-------------------------------------------------------------------------
 */

CSSM_RETURN CSSMAPI port_AllocateTlsIndex( PORT_TLS_INDEX *pIndex,
                                           PORT_TLS_DESTRUCTOR pDestructor )
{
    CSSM_RETURN ReturnVal = CSSM_OK;

#if defined(WIN32)
    /* Win32 implementation */
    DWORD TempIndex;
    
    /* "Use" the destructor parameter */
    pDestructor;

    TempIndex = TlsAlloc();
    if ( TempIndex == PORT_INVALID_TLS_INDEX )
    {
        if ( GetLastError() == ERROR_NOT_ENOUGH_MEMORY )
        {
            ReturnVal = CSSMERR_CSSM_MEMORY_ERROR;
        }
        else
        {
            ReturnVal = CSSMERR_CSSM_OS_ACCESS_DENIED;
        }
    }
    *pIndex = (PORT_TLS_INDEX)TempIndex;
#else
    /* pthreads implementation */
    int ptReturnVal;

    ptReturnVal = pthread_key_create( pIndex, pDestructor );
    if ( ptReturnVal != 0 )
    {
        if ( ptReturnVal == ENOMEM )
        {
            ReturnVal = CSSMERR_CSSM_MEMORY_ERROR;
        }
        else
        {
            ReturnVal = CSSMERR_CSSM_OS_ACCESS_DENIED;
        }
    }
#endif

    return ReturnVal;
}

CSSM_RETURN CSSMAPI port_DeleteTlsIndex( PORT_TLS_INDEX Index )
{
    CSSM_RETURN ReturnVal = CSSM_OK;

#if defined(WIN32)
    /* Win32 implementation */
    DWORD Error;

    if ( !TlsFree( Index ) )
    {
        Error = GetLastError();
        
        if ( Error == ERROR_NOT_ENOUGH_MEMORY )
        {
            ReturnVal = CSSMERR_CSSM_MEMORY_ERROR;
        }
        else
        {
            ReturnVal = CSSMERR_CSSM_INTERNAL_ERROR;
        }
    }
#else
    /* pthreads implementation */
    int ptReturnVal;

    ptReturnVal = pthread_key_delete( Index );
    if ( ptReturnVal != 0 )
    {
        if ( ptReturnVal == EINVAL )
        {
            ReturnVal = CSSMERR_CSSM_INTERNAL_ERROR;
        }
    }
#endif

    return ReturnVal;
}

CSSM_RETURN CSSMAPI port_SetTlsValue( PORT_TLS_INDEX Index,
                                      const void *Value )
{
    CSSM_RETURN ReturnVal = CSSM_OK;

#if defined(WIN32)
    /* Win32 implementation */
    DWORD Error;

    if ( !TlsSetValue( Index, (void *)Value ) )
    {
        Error = GetLastError();
        
        if ( Error == ERROR_NOT_ENOUGH_MEMORY )
        {
            ReturnVal = CSSMERR_CSSM_MEMORY_ERROR;
        }
        else
        {
            ReturnVal = CSSMERR_CSSM_INTERNAL_ERROR;
        }
    }
#else
    /* pthreads implementation */
    int ptReturnVal;

    ptReturnVal = pthread_setspecific( Index, Value );
    if ( ptReturnVal != 0 )
    {
        ReturnVal = CSSMERR_CSSM_INTERNAL_ERROR;
    }
#endif

    return ReturnVal;
}

CSSM_RETURN CSSMAPI port_GetTlsValue( PORT_TLS_INDEX Index,
                                      void **pValue )
{
    CSSM_RETURN ReturnVal = CSSM_OK;
    void *pTempValue = NULL;

#if defined(WIN32)
    /* Win32 implementation */
    DWORD Error;

    pTempValue = TlsGetValue( Index );
    if ( pTempValue == NULL )
    {
        Error = GetLastError();
        
        if ( Error == ERROR_NOT_ENOUGH_MEMORY )
        {
            ReturnVal = CSSMERR_CSSM_MEMORY_ERROR;
        }
        else if ( Error == ERROR_INVALID_HANDLE )
        {
            ReturnVal = CSSMERR_CSSM_INTERNAL_ERROR;
        }
        else if ( Error != NO_ERROR )
        {
            ReturnVal = CSSMERR_CSSM_INTERNAL_ERROR;
        }
    }
#else
    /* pthreads implementation */
    pTempValue = pthread_getspecific( Index );
    if ( pTempValue == 0 )
    {
        ReturnVal = CSSMERR_CSSM_INTERNAL_ERROR;
    }
#endif

	if ( ReturnVal == CSSM_OK )
    {
        *pValue = pTempValue;
    }

    return ReturnVal;
}
