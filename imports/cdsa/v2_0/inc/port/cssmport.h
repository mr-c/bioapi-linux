/*-----------------------------------------------------------------------
 * File: CSSMPORT.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 * Port on Linux by Bull 1999-2000 (afchine.madjlessi@bull.net)
 *-----------------------------------------------------------------------
 */

/* Portions:
 * (C) COPYRIGHT International Business Machines Corp. 1996, 1997
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _CSSMPORT_H
#define _CSSMPORT_H    

#if defined (POSIX)
#include <pthread.h>  /* Must always be the first include */
#endif  /* POSIX */

#if defined (WIN32)
#pragma warning (disable:4201 4514 4214 4115)
#include <windows.h>
#pragma warning (default:4201 4214 4115)
#endif  /* WIN32 */

#if defined (UNIX)
/* For UNIX C library calls */
#if defined (LINUX) || defined (SOLARIS)
#include <link.h>
#endif 
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <malloc.h>
#include <dlfcn.h>
#include <ctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <syslog.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <pwd.h>
#endif  /* UNIX */

#if defined (AIX)
#include <strings.h> 
#endif /* AIX */

/* standard and CDSA includes */
#include <stdio.h>    /* for FILE def */
#include <sys/types.h>
#include <sys/stat.h>
#include "cssmtype.h"

#if defined(UNIX)

/* _MAX_PATH includes room for 0-terminator, PATH_MAX doesn't. */
#define _MAX_PATH (PATH_MAX + 1)
#define  MAX_PATH _MAX_PATH
#define _MAX_FNAME _MAX_PATH
#define _MAX_EXT _MAX_PATH
#define _MAX_DIR _MAX_PATH
#define _MAX_DRIVE 3

#define OutputDebugString(s) (fputs(s, stderr))
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define _mkdir(path) mkdir(path, S_IWUSR | S_IRUSR | S_IXUSR)

typedef void * UNIX_HANDLE;

#endif /* UNIX */

#if defined(WIN32)
#define CSSM_MAX_PATH	_MAX_PATH + 1
#define CSSM_MAX_REG	REGSTR_MAX_VALUE_LENGTH + 1
#elif defined (UNIX)
#define CSSM_MAX_PATH	(PATH_MAX + 1)
#define CSSM_MAX_REG 256
#else
#define CSSM_MAX_PATH 256
#define CSSM_MAX_REG 256
#endif  /* WIN32 */


#if defined (POSIX)
#define CSSM_POLL_INT                   20 /* polling interval in ms */
#endif

#if defined (WIN32)
#pragma warning (disable:4201 4514 4214 4115)
#include <regstr.h>
#pragma warning (default:4201 4214 4115)
#define getpid _getpid
#endif

#if defined (WIN32)
#define DllImport       __declspec( dllimport )
#define DllExport       __declspec( dllexport )
#else
#define DllImport	extern
#define DllExport
#define __stdcall
#endif

#if defined (UNIX) || defined (AIX)
#define CDSA_SYSDIR	"/var/cdsa"
#endif

/* Linux-Port: Default location of BioAPI db/registry files */
#if defined (UNIX)
#define BioAPI_SYSDIR	MDS_PATH
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Initialization and termination routines */
CSSM_RETURN CSSMAPI port_init(void);
CSSM_RETURN CSSMAPI port_terminate(void);


/* Memory Allocation Routines */

#ifndef CSSM_MEMTRACK_ON

void * CSSMAPI
internal_malloc(uint32  mem_size,
                void* allocRef);

void * CSSMAPI
internal_calloc(uint32  num_elem,
                uint32  num_bytes,
                void* allocRef);

void * CSSMAPI
internal_realloc(void *old_ptr,
                 uint32  num_bytes,
                 void* allocRef);

void CSSMAPI
internal_free(void *mem_ptr,
              void* allocRef);


#define local_malloc(s, t) internal_malloc((s), (t))
#define local_calloc(s, t, u) internal_calloc((s), (t) ,(u))
#define local_realloc(s, t, u) internal_realloc((s), (t), (u))
#define local_free(s, t) internal_free((s), (t))

#define LOCAL_MALLOC_WRAPPER(s, t, u) (s)((t), (u))
#define LOCAL_CALLOC_WRAPPER(s, t, u, v) (s)((t) ,(u), (v))
#define LOCAL_REALLOC_WRAPPER(s, t, u, v) (s)((t), (u), (v))
#define LOCAL_FREE_WRAPPER(s, t, u) (s)((t), (u))

#else /* CSSM_MEMTRACK_ON */

void * CSSMAPI
internal_malloc(uint32  mem_size,
                void* allocRef,
                const char * szFilename,
                uint32 u32LineNumber);

void * CSSMAPI
internal_calloc(uint32  num_elem,
                uint32  num_bytes,
                void* allocRef,
                const char *szFilename,
                uint32 u32LineNumber);

void * CSSMAPI
internal_realloc(void *old_ptr,
                 uint32  num_bytes,
                 void* allocRef,
                 const char *szFilename,
                 uint32 u32LineNumber);

void CSSMAPI
internal_free(void *mem_ptr,
              void* allocRef,
              const char *szFilename,
              uint32 u32LineNumber);


#define local_malloc(s, t) internal_malloc((s), (t), __FILE__, __LINE__)
#define local_calloc(s, t, u) internal_calloc((s), (t) ,(u), __FILE__, __LINE__)
#define local_realloc(s, t, u) internal_realloc((s), (t), (u), __FILE__, __LINE__)
#define local_free(s, t) internal_free((s), (t), __FILE__, __LINE__)

#define LOCAL_MALLOC_WRAPPER(s, t, u) (s)((t), (u), __FILE__, __LINE__)
#define LOCAL_CALLOC_WRAPPER(s, t, u, v) (s)((t) ,(u), (v), __FILE__, __LINE__)
#define LOCAL_REALLOC_WRAPPER(s, t, u, v) (s)((t), (u), (v), __FILE__, __LINE__)
#define LOCAL_FREE_WRAPPER(s, t, u) (s)((t), (u), __FILE__, __LINE__ )

#endif  /* CSSM_MEMTRACK_ON */

#if defined (__digital__)
/*
 * These redefinitions are needed on platforms like Tru64 UNIX
 * where malloc(0) returns NULL.  On Win32 and most other platforms,
 * it returns a non-NULL pointer.
 *
 * The reason the redefinitions are needed is that much of the
 * CDSA code, especially the CL code, expects the Win32 behavior.
 * Rather than fix all that code, we just emulate the Win32
 * behavior inside internal_malloc() and then redefine malloc()
 * to be local_malloc() and thus internal_malloc().
 */
#define malloc(s)       local_malloc((s), NULL)
#define calloc(s, t)    local_calloc((s), (t), NULL)
#define realloc(s, t)   local_realloc((s), (t), NULL)
#define free(s)         local_free((s), NULL)
#endif

/* trace routines */

#ifdef _DEBUG

CSSM_RETURN port_InitMemoryTracking();
CSSM_RETURN port_DumpMemoryLeaks(const char * szModule);

#else

#define port_InitMemoryTracking() CSSM_OK
#define port_DumpMemoryLeaks(modname) CSSM_OK

#endif  /* _DEBUG */


#ifdef WIN32

#define port_memset( p, v, n) memset( (p) , (v) , (n) )
#define port_memcpy( d, s, n) memcpy( (d) , (s) , (n) )
#define port_memmove( d, s, n) memmove( (d) , (s) , (n) )
#define port_memcmp( p, q, n) memcmp( (p) , (q) , (n) )

#else
void * CSSMAPI
port_memset(void * ptr,
            sint32  value,
            uint32 num_bytes);

void * CSSMAPI
port_memcpy(void * dest_ptr,
            const void * src_ptr,
            uint32 num_bytes);

void * CSSMAPI
port_memmove(void * dest_ptr,
             void * src_ptr,
             uint32 num_bytes);

sint32 CSSMAPI
port_memcmp(const void * ptr1,
            const void * ptr2,
            uint32 count);
#endif


/* "isBad" functions */

CSSM_BOOL CSSMAPI
port_IsBadStrPtr(const char *str,
                 uint32  length);

CSSM_BOOL CSSMAPI
port_IsBadCodePtr(CSSM_PROC_ADDR code_ptr);

CSSM_BOOL CSSMAPI
port_IsBadReadPtr(const void * ptr,
                  uint32 length);

CSSM_BOOL CSSMAPI
port_IsBadWritePtr(void * ptr,
                   uint32 length);



CSSM_RETURN CSSMAPI
port_BigToHostEndian(CSSM_DATA_PTR Value,
                     CSSM_DATA_PTR ConvertedValue);

char * CSSMAPI
port_strlwr (char *str);


/* File I/O Routines */

CSSM_RETURN CSSMAPI
port_fwrite(const void * buffer,
            uint32 size,
            uint32 num,
            FILE * stream);

CSSM_RETURN CSSMAPI
port_fread(void * buffer,
           uint32 size,
           uint32 num, 
           FILE * stream);

CSSM_RETURN CSSMAPI
port_GetLength(FILE * stream,
               sint32 *len) ;

CSSM_RETURN CSSMAPI
port_fopen(const char * filename,
           const char * mode,
           FILE **file_ptr);

CSSM_RETURN CSSMAPI
port_fclose(FILE * stream);

#ifdef WIN32
#define open _open
#define close _close
#define read _read
#define write _write
#define unlink  remove
#endif /* WIN32 */

/* Sync Routines */

#if defined(WIN32)
#define CSSM_INFINITE_WAIT INFINITE
#else
#define CSSM_INFINITE_WAIT	( 0xFFFFFFFF )
#endif
#define CSSM_ZERO_WAIT 0

#if defined (WIN32)
typedef HANDLE PORT_MUTEX_HANDLE;
#elif defined (POSIX)
typedef pthread_mutex_t* PORT_MUTEX_HANDLE;
#else
typedef void* PORT_MUTEX_HANDLE;
#endif
 
/* lock related functions */

CSSM_RETURN CSSMAPI
port_CreateMutex(const char *name, 
                 PORT_MUTEX_HANDLE *mutex);

CSSM_RETURN CSSMAPI
port_LockMutex(PORT_MUTEX_HANDLE mutex, 
               uint32 timeout);
  
CSSM_RETURN CSSMAPI
port_TrylockMutex(PORT_MUTEX_HANDLE mutex);
    
CSSM_RETURN CSSMAPI
port_UnlockMutex(PORT_MUTEX_HANDLE mutex); 
     
CSSM_RETURN CSSMAPI
port_CloseMutex(PORT_MUTEX_HANDLE mutex);

/* Static mutex initializer. On Win32 systems it simply places
 * a handle into a known state. A call to port_CreateMutex is
 * usually placed in the DllMain function for the module in these
 * cases. On POSIX systems, a static mutext initialized with
 * this value allows it to be used immediately, eliminating the
 * need for a DllMain-like function.
 */
#if defined(WIN32)
#define PORT_STATIC_MUTEX_INITIALIZER NULL
#elif defined(POSIX)
#define PORT_STATIC_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#endif

#if defined (UNIX)
#define GetCurrentDirectory(size, dir) \
    (port_getcwd(dir, size) == CSSM_OK ? strlen(dir) : 0)
#define DeleteFile unlink
#define SetCurrentDirectory(pstrDirName) (chdir(pstrDirName) == 0)
#define _chmod chmod
#define _close close
#define _read read
#define _stat stat
#define strcmpi strcasecmp
#define stricmp strcasecmp
#define Sleep(ms) (sleep((ms)/1000))
#define IsBadStringPtr  port_IsBadStrPtr
#define IsBadCodePtr    port_IsBadCodePtr
#define IsBadReadPtr    port_IsBadReadPtr
#define IsBadWritePtr   port_IsBadWritePtr
#endif

/* Dynamic Loading of Libraries */

#if defined(WIN32)
typedef HINSTANCE
#else
typedef void *
#endif
CSSM_LIB_HANDLE;
typedef CSSM_LIB_HANDLE *CSSM_LIB_HANDLE_PTR;


CSSM_RETURN CSSMAPI
port_FreeLibrary(CSSM_LIB_HANDLE handle);

CSSM_RETURN CSSMAPI
port_LoadLibrary(const char *pLibraryPathName,
                 CSSM_LIB_HANDLE *libHandle);

CSSM_RETURN CSSMAPI
port_GetProcAddress(CSSM_LIB_HANDLE handle,
                    const char *func_name,
                    CSSM_PROC_ADDR *procAddr);

CSSM_RETURN CSSMAPI
port_ComputeAddressInModule(CSSM_LIB_HANDLE LibHandle,
                            void* *StartAddress,
                            void* *EndAddress);

CSSM_RETURN CSSMAPI
port_GetModuleHandle(const char *module_name,
                     CSSM_LIB_HANDLE *module);

CSSM_RETURN CSSMAPI
port_GetModuleFileName(CSSM_LIB_HANDLE module,
                       char *filename,
                       sint32 buf_length);


#ifdef POSIX
typedef pthread_t PORT_THREAD_HANDLE;
#elif defined ( WIN32 )
typedef HANDLE PORT_THREAD_HANDLE;
#else
# error "PORT_THREAD_HANDLE must be defined!"
#endif

typedef CSSM_RETURN (CSSMAPI *port_ThreadProc)( void *Param );


CSSM_RETURN CSSMAPI
port_CreateThread(port_ThreadProc Proc,
                  void *ProcParam,
                  CSSM_BOOL Detached,
                  PORT_THREAD_HANDLE *NewThread);


CSSM_RETURN CSSMAPI
port_JoinThread(PORT_THREAD_HANDLE Thread,
                CSSM_RETURN *ThreadReturn);



/* Thread local storage management */
#if defined (WIN32)
typedef DWORD PORT_TLS_INDEX;
#elif defined (POSIX)
typedef pthread_key_t PORT_TLS_INDEX;
#else
typedef uint32 PORT_TLS_INDEX;
#endif
#define PORT_INVALID_TLS_INDEX	( 0xFFFFFFFF )

typedef void (CSSMAPI *PORT_TLS_DESTRUCTOR)( void *pValue );
#if defined(WIN32)
/* This macro must be placed in the DllMain of any DLL using TLS. The
 * destructor function must be different for each TLS index unless the
 * data type is the same for each index.
 */
#define PORT_CALL_TLS_DESTRUCTOR( _Destructor_, _Index_ ) \
{ \
	void *_pTempValue_ = NULL; \
	if ( port_GetTlsValue( _Index_, &_pTempValue_ ) == CSSM_OK ) \
	{ \
		_Destructor_( _pTempValue_ ); \
	} \
}
#else
/* The pthreads library includes an automatic TLS destructor call. */
#define PORT_CALL_TLS_DESTRUCTOR( _Destructor_, _Index_ )
#endif

CSSM_RETURN CSSMAPI port_AllocateTlsIndex( PORT_TLS_INDEX *pIndex,
										   PORT_TLS_DESTRUCTOR pDestructor );

CSSM_RETURN CSSMAPI port_DeleteTlsIndex( PORT_TLS_INDEX Index );

CSSM_RETURN CSSMAPI port_SetTlsValue( PORT_TLS_INDEX Index,
									  const void *Value );

CSSM_RETURN CSSMAPI port_GetTlsValue( PORT_TLS_INDEX Index,
									  void **pValue );

/* File locking functions */
typedef int PORT_FLOCK_TYPE;
#define PORT_FLOCK_READ		( 0x1 )
#define PORT_FLOCK_WRITE	( 0x2 )
#define PORT_FLOCK_NBLOCK	( 0x4 )

#if defined(WIN32)
typedef DWORD PORT_FLOCK_OFFSET;
#elif defined(UNIX)
typedef off_t PORT_FLOCK_OFFSET;
#else
typedef uint32 PORT_FLOCK_OFFSET;
#endif

typedef struct port_flock_range
{
	PORT_FLOCK_OFFSET Start;
	PORT_FLOCK_OFFSET Length;
}
PORT_FLOCK_RANGE;

CSSM_RETURN port_LockFile(
						int FileDescriptor,
						PORT_FLOCK_TYPE LockType,
						const PORT_FLOCK_RANGE *LockRange );

CSSM_RETURN port_UnlockFile(
						int FileDescriptor,
						const PORT_FLOCK_RANGE *LockRange );

/* Shared Memory Functions */

#if defined(WIN32)
#define CSSM_INVALID_FILE_HANDLE NULL 
typedef HANDLE                      
#else
#define CSSM_INVALID_FILE_HANDLE -1 
typedef int 
#endif
CSSM_SHARED_FILE;

#if defined(WIN32)

#define CSSM_CREATE_READWRITE       PAGE_READWRITE
#define CSSM_CREATE_READ            PAGE_READONLY
#define CSSM_MAP_READWRITE          FILE_MAP_ALL_ACCESS
#define CSSM_MAP_READ               FILE_MAP_READ

#elif defined (UNIX)

#include <fcntl.h>
#include <sys/mman.h>
#define CSSM_CREATE_READWRITE       O_RDWR
#define CSSM_CREATE_READ            O_RDONLY
#define CSSM_MAP_READWRITE          (PROT_READ | PROT_WRITE)
#define CSSM_MAP_READ               (PROT_READ)
#define _O_RDONLY		    O_RDONLY
#define _O_RDWR                     O_RDWR
#define _O_BINARY                   0
#define _S_IREAD                    S_IREAD
#define _S_IWRITE                   S_IWRITE

#endif

CSSM_RETURN CSSMAPI 
port_CreateMemoryFile(const char * name,
                      sint32 perms,
                      CSSM_SHARED_FILE *file_handle);

CSSM_RETURN CSSMAPI 
port_OpenMemoryFile(const char * name,
                    sint32 perms,
                    CSSM_SHARED_FILE *file_handle);

CSSM_RETURN CSSMAPI 
port_MapMemoryFile(CSSM_SHARED_FILE handle,
                   sint32 perms,
                   void **map_ptr);

CSSM_RETURN CSSMAPI
port_UnmapMemoryFile(const void *file_addr);

CSSM_RETURN CSSMAPI
port_FlushMemoryFile(const void *file_addr);

CSSM_RETURN CSSMAPI
port_CloseMemoryFile(CSSM_SHARED_FILE handle);


/* Misc. functions */
#if defined (UNIX)
#define _timezone timezone
#define itoa(value,string,radix) sprintf ((string),"%d",(value))
#define _itoa(value,string,radix) sprintf ((string),"%d",(value))
#define MessageBox(hWnd,lpText,lpCaption,uType) printf ("\n%s: %s\n",lpCaption,lpText)

uint32
GetEnvironmentVariable(const char *lpName,
                       char *lpBuffer,
                       uint32 nSize);

#endif

/* Misc. functions */
CSSM_RETURN CSSMAPI 
port_getcwd(char *dir, 
			uint32 size);

#ifdef UNIX
#include <stdarg.h> 

void port_syslog(int Facility, int Priority, char* format, ...);

CSSM_RETURN CreateSystemDirectory(void);

CSSM_RETURN DeleteSystemDirectory(void);

CSSM_RETURN GetSystemDirectory(char *lpBuffer, size_t uSize);

CSSM_RETURN SearchPath(char* path, char* name, char* ext, 
		 size_t buflen, char* buffer, char** lastpart);

CSSM_BOOL CopyFile(char* lpExistingFileName, char* lpNewFileName, CSSM_BOOL bFailIfExists);

char *_fullpath( char *absPath, const char *relPath, size_t maxLength );

void _splitpath(char* path, char* drive, char* directory, 
		    char* filename, char* extension );

void _makepath( char* path,  char* drive,
		    char* directory,  char* filename,
		    char* extension );
#endif

#if defined (LINUX) || defined (SOLARIS)
int dlnam(void *handle, char *filename, int len);

void* dlladdr(void *handle);

#ifdef ELF_FORMAT
#include <elf.h>

#ifdef __ia64__
#define __arch64__
#endif /* __ia64__ */

#ifdef __arch64__
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Word Elf_Word;
typedef Elf64_Sword Elf_Sword;
typedef Elf64_Rel Elf_Rel;
typedef Elf64_Rela Elf_Rela;
typedef Elf64_Half Elf_Half;
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
#define ELF_R_TYPE ELF64_R_TYPE
#else
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Word Elf_Word;
typedef Elf32_Sword Elf_Sword;
typedef Elf32_Rel Elf_Rel;
typedef Elf32_Rela Elf_Rela;
typedef Elf32_Half Elf_Half;
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Shdr Elf_Shdr;
#define ELF_R_TYPE ELF32_R_TYPE
#endif /* __arch64__ */

#endif /* ELF_FORMAT */

#endif /* LINUX */

/* debugging macro */
#ifdef _DEBUG

#ifdef WIN32

CSSM_RETURN
DebugError(const char *File,
           uint32 Line,
           CSSM_RETURN error);

#define ERR(error) DebugError (__FILE__, __LINE__, (error))

#else /* WIN32 */

#undef ERR
#define ERR(error) (error)

#endif /* WIN32 */

#else   /* _DEBUG */

#ifndef WIN32
#undef ERR
#endif
#define ERR(error) (error)

#endif  /* _DEBUG */

#ifdef WIN32
#define DIRECTORY_SEPARATOR '\\'
#define DIRECTORY_SEPARATOR_STRING "\\"
#define PATH_SEPARATOR ';'
#define PATH_SEPARATOR_STRING ";"
#define DLL_EXTENSION ".dll"
#define DLL_STR "dll"
#else
#define DIRECTORY_SEPARATOR '/'
#define DIRECTORY_SEPARATOR_STRING "/"
#define PATH_SEPARATOR ':'
#define PATH_SEPARATOR_STRING ":"
#define DLL_EXTENSION ".so"
#define DLL_STR "so"
#endif

#if defined (UNIX)
#define CSSM_CDSA_SYSLOG_IDENT		"CDSA"

/* Priorities (ordered) */
#define CSSM_LOG_EMERG		LOG_EMERG
#define CSSM_LOG_ALERT		LOG_ALERT
#define CSSM_LOG_CRIT		LOG_CRIT
#define CSSM_LOG_ERR		LOG_ERR
#define CSSM_LOG_WARNING	LOG_WARNING
#define CSSM_LOG_NOTICE		LOG_NOTICE
#define CSSM_LOG_INFO		LOG_INFO
#define CSSM_LOG_DEBUG		LOG_DEBUG

/* facility codes */
#define CSSM_LOG_KERN		LOG_KERN	/* kernel messages */
#define CSSM_LOG_USER		LOG_USER	/* random user-level messages */
#define CSSM_LOG_DAEMON		LOG_DAEMON	/* system daemons */
#define CSSM_LOG_AUTHPRIV	LOG_AUTHPRIV	/* security/authorization messages (private) */
#define CSSM_LOG_EISL		LOG_LOCAL0	/* reserved for local use */
#define CSSM_LOG_PORT		LOG_LOCAL1	/* reserved for local use */
#define CSSM_LOG_SSL		LOG_LOCAL2	/* reserved for local use */
#define CSSM_LOG_CSSM		LOG_LOCAL3	/* reserved for local use */
#define CSSM_LOG_LOCAL4		LOG_LOCAL4	/* reserved for local use */
#define CSSM_LOG_LOCAL5		LOG_LOCAL5	/* reserved for local use */
#define CSSM_LOG_LOCAL6		LOG_LOCAL6	/* reserved for local use */
#define CSSM_LOG_LOCAL7		LOG_LOCAL7	/* reserved for local use */
#endif

#define GUID_FORMAT_STRING "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}"

#ifdef __cplusplus
}
#endif

#endif
