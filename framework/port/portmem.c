/*-----------------------------------------------------------------------
 *      File:   PORTMEM.C
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

#if defined (__digital__)
/*
 * This section is needed on platforms like Tru64 UNIX where
 * malloc(0) returns NULL.  On Win32 and most other platforms, it
 * returns a non-NULL pointer.
 *
 * Since so much of the CDSA code expects the Win32 behavior,
 * we emulate it here in internal_malloc() and then in cssmport.h,
 * redefine malloc() to be internal_malloc().
 *
 * The emulation just changes sizes of 0 to 1 before calling
 * the real malloc().  I chose that approach instead of the AIX
 * approach of returning a pointer to some static variable because
 * I wanted to handle the case where memory from internal_malloc()
 * was freed with free() instead of internal_free().  Because of
 * the redefinitions in cssmport.h, that should never happen,
 * but unfortunately, not everyone includes cssmport.h.
 */

#define MALLOC_RETURNS_NULL

#ifdef malloc
#undef malloc
#endif
#ifdef calloc
#undef calloc
#endif
#ifdef realloc
#undef realloc
#endif
#ifdef free
#undef free
#endif
#endif

#ifdef _DEBUG


/**********************************************************************/

CSSM_RETURN
port_InitMemoryTracking()
{
#ifdef WIN32
    _HFILE hFile = INVALID_HANDLE_VALUE;
    char szHandle[16];  /* DWORD in string form */

    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE| _CRTDBG_MODE_DEBUG );

    if (GetEnvironmentVariable(
            "CSSMTRACE_HANDLE",
            szHandle,
            sizeof(szHandle)) > 0)
    {
        hFile = (_HFILE) atol( szHandle );
    }

    if ( hFile != INVALID_HANDLE_VALUE )
    {
        hFile = _CrtSetReportFile( _CRT_WARN, hFile );
    }
    else
    {
        hFile = _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    }

    return (hFile == _CRTDBG_HFILE_ERROR) ? CSSMERR_CSSM_INTERNAL_ERROR : CSSM_OK;
#endif
    return CSSM_OK;
}

CSSM_RETURN
port_DumpMemoryLeaks(const char * szModule)
{
#ifdef WIN32
    int iStatus = 0;

    _RPTF1(_CRT_WARN, "%s Memory Leak Dump Begin\n", szModule);
    iStatus = _CrtDumpMemoryLeaks();
    _RPTF1(_CRT_WARN, "%s Memory Leak Dump End\n", szModule);

    return iStatus ? CSSM_OK : CSSMERR_CSSM_INTERNAL_ERROR;
#endif
    return CSSM_OK;
}

#endif

#ifndef CSSM_MEMTRACK_ON

void * CSSMAPI
internal_malloc(uint32  mem_size,
                void* allocRef)
{
    void *ptr;

#ifdef MALLOC_RETURNS_NULL
    if (mem_size == 0)
        mem_size = 1;
#endif

    /* Assume all allocations from one heap - ignore allocRef */
    ptr = malloc(mem_size);

    return ptr;
}

void * CSSMAPI
internal_calloc(uint32  num_elem,
                uint32  num_bytes,
                void* allocRef)
{
  /* Assume all allocations from one heap - ignore allocRef */
    void *ptr;

#ifdef MALLOC_RETURNS_NULL
    if (num_elem == 0 || num_bytes == 0)
    {
        num_elem  = 1;
        num_bytes = 1;
    }
#endif

    ptr = calloc(num_elem,
                 num_bytes);

    return ptr;
}

void * CSSMAPI
internal_realloc(void *old_ptr,
                 uint32  num_bytes,
                 void* allocRef)
{
    /* Assume all alocations are from the same heap - ignore allocRef */
    void *new_ptr;

#ifdef MALLOC_RETURNS_NULL
    if (num_bytes == 0 && old_ptr == NULL)
        num_bytes = 1;
#endif

    new_ptr = realloc(old_ptr,
                      num_bytes);

    return new_ptr;
}

void CSSMAPI
internal_free(void *mem_ptr,
              void* allocRef)
{
    /* Assume all allocations are from the same heap - ignore allocRef */
    free(mem_ptr);
}

#else   /* CSSM_MEMTRACK_ON */

void * CSSMAPI
internal_malloc(uint32  mem_size,
                void* allocRef,
                const char * szFilename,
                uint32 u32LineNumber)
{
    /* Assume all allocations from one heap - ignore allocRef */
    void *mem_ptr;

#ifdef MALLOC_RETURNS_NULL
    if (mem_size == 0)
        mem_size = 1;
#endif

#if defined(_WIN32) && defined(_DEBUG)
    mem_ptr = _malloc_dbg(mem_size,
                          _NORMAL_BLOCK,
                          szFilename,
                          u32LineNumber);
#else
    mem_ptr = malloc(mem_size);
#endif

    return mem_ptr;
}

void * CSSMAPI
internal_calloc(uint32  num_elem,
                uint32  num_bytes,
                void* allocRef,
                const char *szFilename,
                uint32 u32LineNumber)
{
  /* Assume all allocations from one heap - ignore allocRef */
    void *ptr;

#ifdef MALLOC_RETURNS_NULL
    if (num_elem == 0 || num_bytes == 0)
    {
        num_elem  = 1;
        num_bytes = 1;
    }
#endif

#if defined(_WIN32) && defined(_DEBUG)
    ptr = _calloc_dbg(num_elem,
                      num_bytes,
                      _NORMAL_BLOCK,
                      szFilename,
                      u32LineNumber);
#else
    ptr = calloc(num_elem,
                 num_bytes);
#endif

    return ptr;
}

void * CSSMAPI
internal_realloc(void *old_ptr,
                 uint32  num_bytes,
                 void* allocRef,
                 const char *szFilename,
                 uint32 u32LineNumber)
{
    /* Assume all alocations are from the same heap - ignore allocRef */
    void *new_ptr;

#ifdef MALLOC_RETURNS_NULL
    if (num_bytes == 0 && old_ptr == NULL)
        num_bytes = 1;
#endif

#if defined(_WIN32) && defined(_DEBUG)
    new_ptr = _realloc_dbg(old_ptr,
                           num_bytes,
                           _NORMAL_BLOCK,
                           szFilename,
                           u32LineNumber);
#else
    new_ptr = realloc(old_ptr,
                      num_bytes);
#endif

    return new_ptr;
}

void CSSMAPI
internal_free(void *mem_ptr,
              void* allocRef,
              const char *szFilename,
              uint32 u32LineNumber)
{
    /* Assume all allocations are from the same heap - ignore allocRef */

#if defined(_WIN32) && defined(_DEBUG)
    _free_dbg(mem_ptr,
              _NORMAL_BLOCK);
#else
    free(mem_ptr);
#endif
}

#endif  /* CSSM_MEMTRACK_ON */
