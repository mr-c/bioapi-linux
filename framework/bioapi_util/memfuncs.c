/*
 * memfuncs.c
 *
 * General-purpose memory allocation/deallocation functions.
 *
 * These are NOT a part of the BioAPI specification.
 *
 * All applications and BSPs must provide functions for performing memory
 * allocation and deallocation.  By placing these in a public, static library,
 * anyone who wants to make use of this default functionality may do so.  Of
 * course, anyone who needs specialized versions is still free to roll their own.
 */

#include "bioapi_util.h"

/* Linux-Port: added <malloc.h> include file */
#if defined (UNIX)
#include <malloc.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef BioAPI_MEMTRACK_ON

/*
 * Standard memory functions
 */
void* BioAPI _BioAPI_malloc(uint32 size, void* ref)
{
	return malloc( size );
}

void BioAPI _BioAPI_free( void* ptr, void* ref)
{
	if(NULL != ptr)
	{
		free( ptr );
	}
}

void* BioAPI _BioAPI_realloc(void* ptr, uint32 size, void* ref)
{
	return realloc( ptr, size);
}

void* BioAPI _BioAPI_calloc(uint32 num, uint32 size, void* ref)
{
	return calloc( num, size );
}

/* Linux-Port: added WIN32 branch */
#if defined (WIN32)
/*
 * Windows-style (GlobalAlloc, etc.) memory functions
 */
void* BioAPI _BioAPI_Win_malloc(uint32 size, void* ref)
{
	return(GlobalAlloc(GPTR, size));
}

void BioAPI _BioAPI_Win_free( void* ptr, void* ref)
{
	GlobalFree(ptr);
}

void* BioAPI _BioAPI_Win_realloc(void* ptr, uint32 size, void* ref)
{
	return(GlobalReAlloc(ptr, size, GMEM_MOVEABLE));
}

void* BioAPI _BioAPI_Win_calloc(uint32 num, uint32 size, void* ref)
{
	return(GlobalAlloc(GPTR, num*size));
}
#endif /* WIN32 */


#else	/* BioAPI_MEMTRACK_ON */


#if defined (WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif

/*
 * Standard memory functions
 */
void* BioAPI _BioAPI_malloc(uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber)
{
#ifdef _DEBUG
	return _malloc_dbg( size, _NORMAL_BLOCK, szFilename, u32LineNumber );
#else
	return malloc( size );
#endif
}

void BioAPI _BioAPI_free(void* ptr, void* ref,
							const char * szFilename, uint32 u32LineNumber )
{
	if (ptr)
	{
#ifdef _DEBUG
	_free_dbg( ptr, _NORMAL_BLOCK );
#else
	free( ptr );
#endif
	}
}

void* BioAPI _BioAPI_realloc(void* ptr, uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber)
{
#ifdef _DEBUG
	return _realloc_dbg( ptr, size, _NORMAL_BLOCK, szFilename, u32LineNumber );
#else
	return realloc( ptr, size );
#endif
}

void* BioAPI _BioAPI_calloc(uint32 num, uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber)
{
#ifdef _DEBUG
	return _calloc_dbg( num, size, _NORMAL_BLOCK, szFilename, u32LineNumber );
#else
	return calloc( num, size );
#endif
}


/*
 * Windows-style (GlobalAlloc, etc.) memory functions.
 * These currently do nothing with the filename and line number, but if you're
 * programming under Windows, then you probably have a good IDE with integrated
 * memory leak checking, so you don't need this functionality.
 */
/* Linux-Port: added WIN32 branch */
#if defined (WIN32)
void* BioAPI _BioAPI_Win_malloc(uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber)
{
	return(GlobalAlloc(GPTR, size));
}

void BioAPI _BioAPI_Win_free(void* ptr, void* ref,
							const char * szFilename, uint32 u32LineNumber )
{
	GlobalFree(ptr);
}

void* BioAPI _BioAPI_Win_realloc(void* ptr, uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber)
{
	return(GlobalReAlloc(ptr, size, GMEM_MOVEABLE));
}

void* BioAPI _BioAPI_Win_calloc(uint32 num, uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber)
{
	return(GlobalAlloc(GPTR, num*size));
}

#endif  /* WIN32 */
#endif	/* BioAPI_MEMTRACK_ON */


/* Memory function tables */
const BioAPI_MEMORY_FUNCS BioAPIMemoryFuncs =
{
	_BioAPI_malloc,
	_BioAPI_free,
	_BioAPI_realloc,
	_BioAPI_calloc,
	NULL
};

#if defined (WIN32)
const BioAPI_MEMORY_FUNCS BioAPIWinMemoryFuncs =
{
	_BioAPI_Win_malloc,
	_BioAPI_Win_free,
	_BioAPI_Win_realloc,
	_BioAPI_Win_calloc,
	NULL
};
#endif
