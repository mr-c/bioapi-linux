/*-----------------------------------------------------------------------
 * File: FF_PORT.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file declares the flat file porting functions.
 */

#ifndef FF_PORT_H
#define FF_PORT_H

#include "bioapi.h"
#include "port/bioapi_port.h"
#include "port/bioapi_lock.h"
#include "ff_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
	/* Win32 version */
	#include "winbase.h"
	typedef HANDLE PORT_FILE_HANDLE;
	typedef HANDLE PORT_MAPPING_HANDLE;
#elif defined (UNIX)
	typedef int	   PORT_FILE_HANDLE;
	typedef void * PORT_MAPPING_HANDLE;
#else
	/* Non-Win32 version */
	#error Non-Win32 implementation not written
#endif

#ifdef WIN32
typedef uint32 hLock_t;
#else
typedef uintptr_t hLock_t;
#endif

struct ff_mmf_header
{
	uint32 FileIdentifier;
	uint32 Extent;
	uint32 HeaderSize;
};

struct ffport_mmf_page
{
	PORT_FILE_HANDLE hFile;
	PORT_MAPPING_HANDLE hMapping;
	uint8 *pData;
	uint32 PageNumber;
	uint32 Mode;
};

struct ff_mm_file
{
	cssm_SWMRLock WRLock;

	struct ffport_mmf_page FirstPage, MoveablePage;

	PATH_NAME szUniqueFileId;
};

typedef uint8 FF_MM_FILE_BUFF[sizeof( struct ff_mm_file )];
#define FF_MM_FILE_SIZE (sizeof(FF_MM_FILE_BUFF))
typedef FF_MM_FILE_BUFF * FF_MM_FILE_HANDLE;

typedef uint8 FF_MMF_HEADER_BUFF[sizeof (struct ff_mmf_header)];
#define FF_MMF_HEADER_SIZE (sizeof(FF_MMF_HEADER_BUFF))

/* Macros */

/* Reteive the starting offset in a MMF where the first non-header byte is */
#define GET_DATA_START_POS(HeaderSize) (HeaderSize + FF_MMF_HEADER_SIZE)

/* Adjust byte sex so that files are binary compatible */
#if defined(_WIN32) || defined(__digital__) || defined (LINUX)
	uint32 FIX_BYTE_SEX(uint32 uint32Val);
#else /* Big Endian Systems (0xabcdefgh = 0xab, 0xcd, 0xef, 0xgh) */
        #define FIX_BYTE_SEX( x )   ( x )
/*        #define FIX_BYTE_SEX( x )   ( x )*/
#endif

#ifdef DEFINE_FF_MM_FILE
	/* define FF_MM_FILE & FF_MMF_HEADER, this is ff_port.c */
	typedef struct ff_mm_file FF_MM_FILE;
	typedef FF_MM_FILE * FF_MM_FILE_PTR;
	typedef FF_MM_FILE const * FF_CONST_MM_FILE_PTR;

	typedef struct ff_mmf_header FF_MMF_HEADER;
	typedef FF_MMF_HEADER * FF_MMF_HEADER_PTR;
	typedef FF_MMF_HEADER const * FF_CONST_MMF_HEADER_PTR;

	typedef struct ffport_mmf_page FFPORT_MMF_PAGE;
#else
	/* keep FF_MM_FILE & FF_MMF_HEADER private, this is not ff_port.c */
	#define ff_mm_file DO_NOT_USE_ME
	#define ff_mmf_header DO_NOT_USE_ME
	#define ffport_mmf_page DO_NOT_USE_ME
#endif

/* Memory mapped file routines */

CSSM_RETURN ffport_mmf_Construct(
	FF_MM_FILE_HANDLE hMMFile,
	const char *szFileName,
	uint32 FileIdentifier,
	DAL_CONST_DATA_PTR pFileHeader,
	const DAL_MODULE_PARAMETERS * Parameters);

void ffport_mmf_Deconstruct(FF_MM_FILE_HANDLE);

CSSM_RETURN ffport_mmf_CreateFile(
	const char *szFileName,
	uint32 FileIdentifier,
	DAL_CONST_DATA_PTR pFileHeader,
	const char *FileMode);

FF_ERROR ffport_mmf_eWrite(
	FF_MM_FILE_HANDLE hMMFile,
	uint32 *pOffset,
	uint32 bufferLen,
	const uint8 *buffer,
	uint32 paddingBytes,
    hLock_t *p_hLock);

FF_ERROR ffport_mmf_eWriteCssmData(
	FF_MM_FILE_HANDLE hMMFile,
	uint32 *pOffset,
	DAL_CONST_DATA_PTR pData,
    hLock_t *p_hLock);

FF_ERROR ffport_mmf_eRead(
	FF_MM_FILE_HANDLE hMMFile,
	uint32 Offset,
	uint32 bufferLen,
	uint8 *buffer,
    hLock_t *p_hLock);

FF_ERROR ffport_mmf_eReadCssmData(
	FF_MM_FILE_HANDLE hMMFile,
	uint32 Offset,
	CSSM_DATA_PTR rgData,
    hLock_t *p_hLock);

FF_ERROR ffport_mmf_eGetWriterLock(
	FF_MM_FILE_HANDLE hMMFile,
    hLock_t * p_hLock);

FF_ERROR ffport_mmf_eGetReaderLock(
	FF_MM_FILE_HANDLE hMMFile,
    hLock_t * p_hLock);

void ffport_mmf_nrReleaseLock(
	FF_MM_FILE_HANDLE hMMFile,
    hLock_t hLock);

/* File, Directory & Time Routeens */
FF_ERROR ffport_eGetPathStart(const char *DbName, 
							  char *szPathStart, 
							  char* AccessPath);
FF_ERROR ffport_eDeleteDbFiles(const char *szPathStart);
FF_ERROR ffport_eDeleteDbRelationFiles(CSSM_DB_RECORDTYPE RelationID, const char *szPathStart);
CSSM_BOOL ffport_neDoesFileExist(const char *szPathName);

void ffport_nrSetCache(CSSM_BOOL fEnableWriteBackCache);

sint32 ffport_SetupFilePermissionsStart(
					const CSSM_DATA * TemplatePassphrase,
					const char *szDbComponentFileName,
					const CSSM_NET_ADDRESS *DbLocation,
					char *FileMode,
					void **AccessCtx);

sint32 ffport_SetupFilePermissionsEnd(
					void *AccessCtx);

CSSM_BOOL ffport_IsValidFilePermissions (
					const CSSM_DATA *Passphrase,
					const char * szDbComponentFileName,
					CSSM_DB_ACCESS_TYPE RequestAccessMask,
					CSSM_DB_ACCESS_TYPE AccessFilter,
					uint32 *Error);

#ifdef __cplusplus
}
#endif

#endif
