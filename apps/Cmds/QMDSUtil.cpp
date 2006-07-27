#include "QMDSUtil.h"
#include <malloc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef CSSM_MEMTRACK_ON
void* CSSMAPI QMdsUtil_malloc(uint32 size,
							  void* ref)
{
	return malloc( size );
}

void CSSMAPI QMdsUtil_free( void* ptr, void* ref)
{
	free( ptr );
}

void* CSSMAPI QMdsUtil_realloc(void* ptr,
							   uint32 size,
							   void* ref)
{
	return realloc( ptr, size);
}

void* CSSMAPI QMdsUtil_calloc(uint32 num,
							  uint32 size,
							  void* ref)
{
	return calloc( num, size );
}


#else

void* CSSMAPI QMdsUtil_malloc(uint32 size,
							  void* ref,
							  const char * szFilename,
							  uint32 u32LineNumber)
{
	return _malloc_dbg( size, _NORMAL_BLOCK, szFilename, u32LineNumber );
}

void CSSMAPI QMdsUtil_free( void* ptr, void* ref,
							  const char * szFilename,
							  uint32 u32LineNumber )
{
	_free_dbg( ptr, _NORMAL_BLOCK );
}

void* CSSMAPI QMdsUtil_realloc(void* ptr,
							   uint32 size,
							   void* ref,
							   const char * szFilename,
							   uint32 u32LineNumber)
{
	return _realloc_dbg( ptr, size, _NORMAL_BLOCK, szFilename, u32LineNumber );
}

void* CSSMAPI QMdsUtil_calloc(uint32 num,
							  uint32 size,
							  void* ref,
							  const char * szFilename,
							  uint32 u32LineNumber)
{
	return _calloc_dbg( num, size, _NORMAL_BLOCK, szFilename, u32LineNumber );
}

#endif

#include "AppMemTrack.h"

unsigned int QMdsUtil_AllocRef = 0;

