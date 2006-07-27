#ifndef __QMDSUTIL_H__
#define __QMDSUTIL_H__

#include <cssmtype.h>

#ifndef CSSM_MEMTRACK_ON

void* CSSMAPI QMdsUtil_malloc(uint32 size,
			      void* ref);

void CSSMAPI QMdsUtil_free( void* ptr, void* ref);

void* CSSMAPI QMdsUtil_realloc(void* ptr,
			       uint32 size,
			       void* ref);

void* CSSMAPI QMdsUtil_calloc(uint32 num,
			      uint32 size,
			      void* ref);


#else

void* CSSMAPI QMdsUtil_malloc(uint32 size,
			      void* ref,
			      const char * szFilename,
			      uint32 u32LineNumber);

void CSSMAPI QMdsUtil_free( void* ptr, void* ref,
			    const char * szFilename,
			    uint32 u32LineNumber );

void* CSSMAPI QMdsUtil_realloc(void* ptr,
			       uint32 size,
			       void* ref,
			       const char * szFilename,
			       uint32 u32LineNumber);

void* CSSMAPI QMdsUtil_calloc(uint32 num,
			      uint32 size,
			      void* ref,
			      const char * szFilename,
			      uint32 u32LineNumber);
#endif

extern unsigned int QMdsUtil_AllocRef;


#endif // __QMDSUTIL_H__
