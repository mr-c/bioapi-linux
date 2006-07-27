/*-----------------------------------------------------------------------
 *      File:   portmemm.c
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

#if defined (AIX) ||  defined(LINUX) || defined (SOLARIS)
#include <pthread.h>            /* Must be the first include file */
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "trc_util.h"
#include "port_codefile.h"

#if defined (WIN32)
#include <direct.h>     /* getcwd */
#define MAP_SIZE 0x4000
#endif

#include "cssmport.h"
#include "cssmerr.h"
#ifdef __alpha
/*
 * Fast, simple check that catches really bad pointers, particularly,
 * the ones in the cmdtests.  It doesn't catch everything though
 * because pointers within this range may still be bad.
 *
 * XXX We could check conclusively with mvalid(), but is it worth it?
 */
#define IsBadAlphaPtr(p) \
    (((uint64)(p) > 0x000003ffffffffffUL) || \
     ((uint64)(p) < 0x0000000000010000UL))
#endif

#ifndef WIN32

/*-----------------------------------------------------------------------------
 * Name: port_memset
 *
 * Description:  Initializes all elements of a memory block to a value
 * 
 * Parameters: 
 * ptr (input)       : Pointer to memory block
 * vlaue (input)     : Value to initialize memory to
 * num_bytes (input) : Number of bytes of memory to initialize
 *
 * Returns:
 * Ptr to memory (same as pointer passed in)
 * 
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
void * CSSMAPI 
port_memset(
    void *ptr, 
    sint32 value, 
    uint32 num_bytes )
{
    TRACE(port_trace_info, FNCODE_cssm_memset);

    memset(ptr, value, (size_t)num_bytes);
    RETURN(ptr)
}



/*-----------------------------------------------------------------------------
 * Name: port_memcpy
 *
 * Description:  Copies contents of one memory block to another
 * 
 * Parameters: 
 * dest_ptr (input)       : Pointer to destination memory block
 * src_ptr (input)        : Pointer to source memory block
 * num_bytes (input)      : Number of bytes of memory to copy
 *
 * Returns:
 * Ptr to memory (same as pointer passed in)
 * 
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
void * CSSMAPI 
port_memcpy(
    void *dest_ptr, 
    const void *src_ptr, 
    uint32  num_bytes )
{
    TRACE(port_trace_info, FNCODE_cssm_memcpy);

    memcpy(dest_ptr, src_ptr, (size_t)num_bytes);
    RETURN(dest_ptr)
}



/*-----------------------------------------------------------------------------
 * Name: port_memmove
 *
 * Description:  Copies contents of one memory block to another -
 *                  handles overlapping memory regions
 * 
 * Parameters: 
 * dest_ptr (input)       : Pointer to destination memory block
 * src_ptr (input)        : Pointer to source memory block
 * num_bytes (input)      : Number of bytes of memory to copy
 *
 * Returns:
 * Ptr to memory (same as pointer passed in)
 * 
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
void * CSSMAPI 
port_memmove(
    void *dest_ptr, 
    void *src_ptr, 
    uint32 num_bytes )
{
    TRACE(port_trace_info, FNCODE_cssm_memmove);

    memmove(dest_ptr, src_ptr, (size_t)num_bytes);
    RETURN(dest_ptr)
}



/*-----------------------------------------------------------------------------
 * Name: port_memcmp
 *
 * Description:  Compares the contents of two memory blocks
 * 
 * Parameters: 
 * ptr1 (input)        : Pointer to first memory block
 * ptr2 (input)        : Pointer to second memory block
 * count (input)       : Number of bytes of memory to compare
 *
 * Returns:
 * < 0          ptr1 less than ptr2
 * 0            ptr1 identical to ptr2
 * > 0          ptr1 greater than ptr2
 * 
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
sint32 CSSMAPI 
port_memcmp(
    const void *ptr1, 
    const void *ptr2, 
    uint32  count)
{
    TRACE(port_trace_info, FNCODE_cssm_memcmp);

    RETURN(memcmp(ptr1, ptr2, (size_t)count))
}

#endif /* WIN32 */



/*-----------------------------------------------------------------------------
 * Name: port_IsBadStrPtr
 *
 * Description: On Windows platforms, port_IsBadStrPtr verifies that the 
 *              calling process has read access to a range of memory pointed 
 *              to by a string pointer.  On all platforms, this function will
 *              check that the pointer passed consists of printable character
 *              bytes.
 * 
 * Parameters: 
 * str (input)           : Pointer to a string
 * length (input)        : Length of pointer to check
 *
 * Returns:
 * CSSM_TRUE - string pointer is bad
 * CSSM_FALSE - string pointer is okay
 * 
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
CSSM_BOOL CSSMAPI port_IsBadStrPtr (const char *str, uint32 length)
{
    uint32 i;
    TRACE(port_trace_info, FNCODE_port_IsBadStrPtr);

    if ( 0 == length )
        RETURN( CSSM_FALSE )
    
    if (str == NULL)
        RETURN( CSSM_TRUE )
#if defined(WIN32)
    if  ( IsBadStringPtr( str, length ) )
        RETURN(CSSM_TRUE)
#elif defined(__alpha)
    /*
     * XXX This isn't perfect because we should stop just before
     * the string's terminating null character if it lies within the
     * specified length.
     */
    if (IsBadAlphaPtr(str) || IsBadAlphaPtr((uint8 *)str + length - 1))
        RETURN(CSSM_TRUE)
#endif    /* check that all characters are printable */
    for(i=0; i < strlen(str); i++)
    {
        if (!(isprint(str[i])) && 
             str[i] != 0x09    && /* Horizontal tab */
             str[i] != 0x0A    && /* Line feed */
             str[i] != 0x0D)      /* Carriage Return */
            RETURN(CSSM_TRUE)
    }

    RETURN( CSSM_FALSE )
}



/*-----------------------------------------------------------------------------
 * Name: port_IsBadCodePtr
 *
 * Description:  On Windows platforms, port_IsBadCodePtr determines whether 
 *               the calling process has read access to the memory at the 
 *               specified address.  On other platforms, port_IsBadCodePtr 
 *               may simply ensure that the pointer is not NULL
 * 
 * Parameters: 
 * code_ptr (input)           : Pointer to a piece of code
 *
 * Returns:
 * CSSM_TRUE - code pointer is bad
 * CSSM_FALSE - code pointer is okay
 * 
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
CSSM_BOOL CSSMAPI port_IsBadCodePtr(CSSM_PROC_ADDR code_ptr)
{
    TRACE(port_trace_info, FNCODE_cssm_IsBadCodePtr);

    if (code_ptr == NULL)
        RETURN(CSSM_TRUE)

#if defined(WIN32)

    if (IsBadCodePtr(code_ptr))
        RETURN(CSSM_TRUE)
#elif defined(__alpha)
    if (IsBadAlphaPtr(code_ptr))
        RETURN(CSSM_TRUE)
#endif

    RETURN(CSSM_FALSE)
}



/*-----------------------------------------------------------------------------
 * Name: port_IsBadReadPtr
 *
 * Description:  On Windows platforms, port_IsBadReadPtr verifies that the 
 *               calling process has read access to the specified range
 *               of memory.  On other platforms, port_IsBadStrPtr 
 *               may simply ensure that the pointer is not NULL
 * 
 * Parameters: 
 * str (input)           : Pointer 
 * length (input)        : Length of pointer to check
 *
 * Returns:
 * CSSM_TRUE - pointer is bad
 * CSSM_FALSE - pointer is okay
 * 
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
CSSM_BOOL CSSMAPI port_IsBadReadPtr (const void *ptr, uint32 length)
{
    TRACE(port_trace_info, FNCODE_port_IsBadReadPtr);

    if ( 0 == length )
        RETURN( CSSM_FALSE )
    
    if (ptr == NULL)
        RETURN( CSSM_TRUE )
#if defined(WIN32)
    if  ( IsBadReadPtr( ptr, length ) )
        RETURN(CSSM_TRUE)
#elif defined(__alpha)
    if (IsBadAlphaPtr(ptr) || IsBadAlphaPtr((uint8 *)ptr + length - 1))
        RETURN(CSSM_TRUE)
#endif
    RETURN( CSSM_FALSE )

}



/*-----------------------------------------------------------------------------
 * Name: port_IsBadWritePtr
 *
 * Description:  On Windows platforms, port_IsBadWritePtr verifies that the 
 *               calling process has write access to the specified range
 *               of memory.  On other platforms, port_IsBadStrPtr 
 *               may simply ensure that the pointer is not NULL
 * 
 * Parameters: 
 * str (input)           : Pointer 
 * length (input)        : Length of pointer to check
 *
 * Returns:
 * CSSM_TRUE - pointer is bad
 * CSSM_FALSE - pointer is okay
 * 
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
CSSM_BOOL CSSMAPI port_IsBadWritePtr (void *ptr, uint32 length)
{
    TRACE(port_trace_info, FNCODE_port_IsBadWritePtr);

    if ( 0 == length )
        RETURN( CSSM_FALSE )
    
    if (ptr == NULL)
        RETURN( CSSM_TRUE )
#if defined(WIN32)
    if  ( IsBadWritePtr( ptr, length ) )
        RETURN(CSSM_TRUE)
#elif defined(__alpha)
    if (IsBadAlphaPtr(ptr) || IsBadAlphaPtr((uint8 *)ptr + length - 1))
        RETURN(CSSM_TRUE)
#endif
    RETURN( CSSM_FALSE )
}
