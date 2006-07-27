/*-----------------------------------------------------------------------
 * File: FF_PORT.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file implements the flat file porting functions.
 */

#if defined (UNIX)
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fnmatch.h>
#endif
#include "port/bioapi_port.h"
#if defined (__digital__)
#include "compaq/dprintf.h"
#endif
#if defined (UNIX)
#ifndef DPRINTF
#define DPRINTF
#endif
#endif
#define DEFINE_FF_MM_FILE
#include "ff_port.h"
#include "maf_collectn.h"
#include "maf_util.h"
#include "bioapi_util.h"

#if defined (WIN32)
/* INVALID_HANDLE_VALUE already defined through <windows.h>. */
#elif defined (UNIX)
#define INVALID_HANDLE_VALUE -1L
#else
#error Not implemented.
#endif

#define CAST_HANDLE(hMMFile) \
	FF_MM_FILE_PTR pThis; \
	ASSERT(hMMFile); \
	pThis = (FF_MM_FILE_PTR)hMMFile;

/* Get the header (the first page) */
#define GetHeader(pThis) ((struct ff_mmf_header *)((pThis)->FirstPage.pData))

/* Get the file identifier (the magic number) */
#define GetFileIdentifier(pThis) (FIX_BYTE_SEX(GetHeader(pThis)->FileIdentifier))

/* Get the size of the custom file header that the caller imposed on create */
#define GetHeaderSize(pThis) (FIX_BYTE_SEX(GetHeader(pThis)->HeaderSize))

/* Get/Set the maximum extent that we have writen to in the file */
#define GetExtent(pThis) (FIX_BYTE_SEX(GetHeader(pThis)->Extent))
#define SetExtent(pThis, newExtent) ( GetHeader(pThis)->Extent = FIX_BYTE_SEX(newExtent) )
#define GetExtentAddress(pThis) ( &(GetHeader(pThis)->Extent) )

/* Definition of locking macros. They all always expect success, they return
   BOOL to indicate success. or failure */

#ifdef _DEBUG /* Debug mode */

static FF_BOOL ffport_debug_CheckLock(CSSM_RETURN Ret)
{
	if (Ret != CSSM_OK)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return FF_TRUE;
	}

	return FF_FALSE;
}

/* Added to prevent compilation error if CssmTrace.h is not available */
#ifndef CSSMTRACE
#define CSSMTRACE
#endif

/* Locking/Unlocking Macros */
#define fWaitToRead() \
( \
	CSSMTRACE( "cssm_SWMRLockWaitToRead 0x%X\n", &pThis->WRLock ), \
	ffport_debug_CheckLock(cssm_SWMRLockWaitToRead(&pThis->WRLock, LOCK_TIMEOUT_PERIOD)) \
)

#define fWaitToWrite() \
( \
	CSSMTRACE( "cssm_SWMRLockWaitToWrite 0x%X\n", &pThis->WRLock ), \
	ffport_debug_CheckLock(cssm_SWMRLockWaitToWrite (&pThis->WRLock, LOCK_TIMEOUT_PERIOD)) \
)

#define DoneReading() \
( \
	CSSMTRACE( "cssm_SWMRLockDoneReading 0x%X\n", &pThis->WRLock ), \
	cssm_SWMRLockDoneReading(&pThis->WRLock) \
)

#define DoneWriting() \
( \
	CSSMTRACE( "cssm_SWMRLockDoneWriting 0x%X\n", &pThis->WRLock ), \
	cssm_SWMRLockDoneWriting(&pThis->WRLock) \
)

#else /* Release Mode */

#define fWaitToRead() \
	(CSSM_OK != cssm_SWMRLockWaitToRead(&pThis->WRLock, LOCK_TIMEOUT_PERIOD))

#define fWaitToWrite() \
	(CSSM_OK != cssm_SWMRLockWaitToWrite (&pThis->WRLock, LOCK_TIMEOUT_PERIOD))

#define DoneReading() \
	(cssm_SWMRLockDoneReading(&pThis->WRLock))

#define DoneWriting() \
	(cssm_SWMRLockDoneWriting(&pThis->WRLock))

#endif

/* Private Data structure declarations */
typedef struct ffmmf_header FFMMF_HEADER;

static CSSM_BOOL s_fUseWriteBackCache = CSSM_FALSE;
#if defined (WIN32)
static DWORD s_dwAllocationGranularity = 0;
#elif defined (UNIX)
static long s_dwAllocationGranularity = 0;
#endif

/* This is the size of every page in a file, PAGE_SIZE needs to be greator than or equal to
   s_dwAllocationGranularity. A larger value will lead to better preformance, higher memory
   usage, and larger file sizes (since the file size always rounds up to a multiple of this
   number) */
#define _PAGE_SIZE (s_dwAllocationGranularity)

#if defined (WIN32)
static uint32 MapWin32Error(void);
#elif defined (UNIX)
static uint32 MapErrno(void);
#endif

static CSSM_RETURN ffport_mmf_page_Construct(FFPORT_MMF_PAGE *pThis);

static void ffport_mmf_page_Deconstruct(FFPORT_MMF_PAGE *pThis);

static FF_ERROR ffport_mmf_page_eMapPage(
	FFPORT_MMF_PAGE *pThis,
	const char * szMappingName,
	uint32 PageNumber);

/*-------------------------------------------------------- Construction & Destruction (mmf) --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ff_mmf_Construct																			  *
 * ff_mmf_Deconstruct																		  *
 *--------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_Construct
 *
 * Description:
 * Constructs a file object, initializing any variables
 *
 * Parameters:
 * hMMFile (input) : Handle to the MMF to initialize
 * szFileName (input) : Complete filename (including path) of the file that
 *	 will be opened as a memory mapped file
 * FileIdentifer (input) : 32-bit value that identifies the file type. This
 *	 value is used to check the validity of a file.
 * pFileHeader (input) : the file header of the file. This value is used to
 *	 check the validity of a file.
 *
 * Return value:
 * CSSM_OK if no error occurred, otherwise CSSM_FAIL
 *---------------------------------------------------------------------------*/
CSSM_RETURN ffport_mmf_Construct(
	FF_MM_FILE_HANDLE hMMFile,
	const char *szFileName,
	uint32 FileIdentifier,
	DAL_CONST_DATA_PTR pFileHeader,
	const DAL_MODULE_PARAMETERS * Parameters)
{
	CSSM_RETURN retValue = CSSM_OK;
	/* Name of the lock */
	char LockName[MAX_PATH_NAME_SIZE + 2];
	uint32 i;
	uint32 dwDesiredAccess = 0;
#if defined (WIN32)
    DWORD dwFlagsAndAttributes;
	uint32 dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE; /* By default,
															 all other opens
															 are allowed to
															 succede */

	static SECURITY_DESCRIPTOR sd = {0};
	static SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), &sd,TRUE};

#elif defined (UNIX)
	mode_t dwFlagsAndAttributes;
	/* dwShareMode not needed on UNIX. */
#else
#error Not implemented.
#endif

	CAST_HANDLE(hMMFile);
	ASSERT(szFileName && *szFileName);

	memset(pThis, 0, sizeof(FF_MM_FILE));

	/* First initialize with default values */
	if ((retValue = ffport_mmf_page_Construct(&pThis->FirstPage)) != CSSM_OK)
		return retValue;

	if ((retValue = ffport_mmf_page_Construct(&pThis->MoveablePage)) != CSSM_OK)
		return retValue;

	/* Get the system info */
	if (s_dwAllocationGranularity == 0)
	{
#if defined (WIN32)
		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);
		s_dwAllocationGranularity = SystemInfo.dwAllocationGranularity;
#elif defined (UNIX)
		if ((s_dwAllocationGranularity = sysconf(_SC_PAGE_SIZE)) == -1L)
			return CSSMERR_DL_INTERNAL_ERROR;
#else
#error Not implemented.
#endif
	}

	/* Next create the lock, and make ourself a reader */

	/* replace all the slashes ('\') with a symbol ('+') which is invalid for a filename */
	for (i = 0; szFileName[i]; i++)
	{
		if (szFileName[i] == DIRECTORY_SEPARATOR)
			pThis->szUniqueFileId[i] = '+';
		else
			pThis->szUniqueFileId[i] = szFileName[i];
	}
	pThis->szUniqueFileId[i] = 0;

	sprintf(LockName, "%s.L", pThis->szUniqueFileId);

	if (CSSM_OK != cssm_SWMRLockCreate(&pThis->WRLock, LockName))
		return CSSMERR_DL_INTERNAL_ERROR;

	if (fWaitToRead())
		return CSSMERR_DL_INTERNAL_ERROR;

#if defined (WIN32)
	dwFlagsAndAttributes = FILE_FLAG_RANDOM_ACCESS;
	if (CSSM_FALSE == s_fUseWriteBackCache)
		dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;


	if ((Parameters->AccessRequest) & CSSM_DB_ACCESS_READ) {
		dwDesiredAccess = GENERIC_READ;
/*		dwShareMode = FILE_SHARE_READ; */
	}

	if ((Parameters->AccessRequest) & CSSM_DB_ACCESS_WRITE) {
		dwDesiredAccess |= GENERIC_WRITE;
/*		dwShareMode &= ~FILE_SHARE_WRITE; */ /* Clear the write share */
	}

	if (dwDesiredAccess & GENERIC_WRITE) {
		pThis->FirstPage.Mode = PAGE_READWRITE;
		pThis->MoveablePage.Mode = PAGE_READWRITE;
	}

	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

	/* now open the memory mapped file (header copy) */
	pThis->FirstPage.hFile = CreateFile(
		szFileName, /* lpFileName */
		dwDesiredAccess,
		dwShareMode,
		&sa, /* lpSecurityAttributes */
		OPEN_EXISTING, /* dwCreationDistribution */
		dwFlagsAndAttributes, /* dwFlagsAndAttributes */
		NULL); /* hTemplateFile */
#elif defined (UNIX)
	dwFlagsAndAttributes = 0;
	if (CSSM_FALSE == s_fUseWriteBackCache)
		dwFlagsAndAttributes |= O_SYNC;

	if (((Parameters->AccessRequest) &
		(CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE)) ==
		(CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE)) {
		dwDesiredAccess = O_RDWR;
		pThis->FirstPage.Mode = pThis->MoveablePage.Mode =
			PROT_READ | PROT_WRITE;
	} else if ((Parameters->AccessRequest) & CSSM_DB_ACCESS_READ)  {
		dwDesiredAccess = O_RDONLY;
		pThis->FirstPage.Mode = pThis->MoveablePage.Mode =
			PROT_READ;
	} else {
		/*
		 * This catches the invalid AccessRequests of 0 and
		 * CSSM_DB_ACCESS_WRITE from utmds's test_MDS_DbOpen0.
		 * The WIN32 implementation catches them indirectly
		 * later on when CreateFileMapping() fails with an
		 * error of ERROR_ACCESS_DENIED.
		 */
		return CSSMERR_DL_INVALID_ACCESS_REQUEST;
	}

	/* now open the memory mapped file (header copy) */
	pThis->FirstPage.hFile = open(szFileName, dwDesiredAccess,
					  dwFlagsAndAttributes);
	DPRINTF("ff", "(ffport_mmf_Construct) fd %d = first page of %s\n",
		pThis->FirstPage.hFile, szFileName);
#else
#error Not implemented.
#endif

	if (pThis->FirstPage.hFile == INVALID_HANDLE_VALUE) {
		DoneReading();
		return CSSMERR_DL_FUNCTION_FAILED;
	}

#if defined (WIN32)
	dwFlagsAndAttributes = FILE_FLAG_SEQUENTIAL_SCAN;
	if (CSSM_FALSE == s_fUseWriteBackCache)
		dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
#elif defined (UNIX)
	dwFlagsAndAttributes = 0;
	if (CSSM_FALSE == s_fUseWriteBackCache)
		dwFlagsAndAttributes |= O_SYNC;
#else
#error Not implemented.
#endif

	/* now open the memory mapped file (data copy) */
#if defined (WIN32)
	pThis->MoveablePage.hFile = CreateFile(
		szFileName, /* lpFileName */
		dwDesiredAccess, /* dwDesiredAccess */
		dwShareMode, /* dwShareMode */
		&sa, /* lpSecurityAttributes */
		OPEN_EXISTING, /* dwCreationDistribution */
		dwFlagsAndAttributes, /* dwFlagsAndAttributes */
		NULL); /* hTemplateFile */
#elif defined (UNIX)
	/* pThis->MoveablePage.hFile = dup(pThis->FirstPage.hFile); */
	pThis->MoveablePage.hFile = pThis->FirstPage.hFile;
	DPRINTF("ff", "(ffport_mmf_Construct) fd %d = moveable page of %s\n",
		pThis->MoveablePage.hFile, szFileName);
#else
#error Not implemented.
#endif

	if (pThis->MoveablePage.hFile == INVALID_HANDLE_VALUE) {
		DoneReading();
		return CSSMERR_DL_FUNCTION_FAILED;
	}

	CATCH_FUNCTION(ffport_mmf_page_eMapPage(&pThis->FirstPage, pThis->szUniqueFileId, 0))
	{
		DoneReading();
		RETURN_ERROR();
	}

	DoneReading();

	ASSERT(GetHeader(pThis));
	if (GetFileIdentifier(pThis) != FileIdentifier)
		return CSSMERR_DL_DATABASE_CORRUPT;

	if (GetHeaderSize(pThis) != pFileHeader->Length)
		/* The length of the header does not match the expected */
		return CSSMERR_DL_DATABASE_CORRUPT;

	if (GET_DATA_START_POS(GetHeaderSize(pThis)) > GetExtent(pThis))
		/* The length of the header disagrees with the extent */
		return CSSMERR_DL_DATABASE_CORRUPT;

	/* the header must be on the first page (since we are about to read it) */
    ASSERT(GetHeaderSize(pThis) + sizeof(struct ff_mmf_header) < _PAGE_SIZE);

	if (0 != memcmp(&pThis->FirstPage.pData[sizeof(struct ff_mmf_header)], pFileHeader->Data,
		pFileHeader->Length))
		/* The file header did not match the expected file header */
		return CSSMERR_DL_DATABASE_CORRUPT;

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_Deconstruct
 *
 * Description:
 * Destroy/free all allocated structures.
 *
 * Parameters:
 * hMMFile (input) : Handle to the memory mapped file to destroy
 *
 * Return value:
 * None.
 *---------------------------------------------------------------------------*/
void ffport_mmf_Deconstruct(FF_MM_FILE_HANDLE hMMFile)
{
	CAST_HANDLE(hMMFile);

	/* write the moveable page first, we only want to update the extent after
	   all the data has been flushed (or as close to that as possible) */
	ffport_mmf_page_Deconstruct(&pThis->MoveablePage);
	ffport_mmf_page_Deconstruct(&pThis->FirstPage);

	cssm_SWMRLockDelete(&pThis->WRLock);
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_CreateFile
 *
 * Description:
 * Creates a new file. This file contains the header that is common to all
 * memory mapped files, and also contains a File Header that is specific
 * to the type of file
 *
 * Parameters:
 * szFileName (input) : Complete path to the file to create (if it already exits,
 * it will be overwritten)
 * FileIdentifer (input) : 32-bit value that identifies the file type
 * pFileHeader (input) : a data structure that contains any additional header
 * information that needs to be specified.
 *
 * Return value:
 * CSSM_OK if no error occurred, else CSSM_FAIL
 *---------------------------------------------------------------------------*/
CSSM_RETURN ffport_mmf_CreateFile(
	const char *szFileName,
	uint32 FileIdentifier,
	DAL_CONST_DATA_PTR pFileHeader,
	const char *FileMode)
{
	FILE * pFile = NULL;
	FF_MMF_HEADER mmfFileHeader;

	/* Open the file */
	pFile = fopen(szFileName, FileMode);
	if (!pFile)
		/* TODO */
		return CSSMERR_DL_FUNCTION_FAILED;

	/* Initialize the MMF Header */
	mmfFileHeader.FileIdentifier = FIX_BYTE_SEX(FileIdentifier);

	if (pFileHeader)
		mmfFileHeader.HeaderSize = FIX_BYTE_SEX(pFileHeader->Length);
	else
		mmfFileHeader.HeaderSize = 0;

	mmfFileHeader.Extent = FIX_BYTE_SEX(GET_DATA_START_POS(FIX_BYTE_SEX(mmfFileHeader.HeaderSize)));

	if (1 != fwrite((void *)&mmfFileHeader, sizeof(FF_MMF_HEADER), 1, pFile))
	{
		/* TODO */
		fclose(pFile);
		return CSSMERR_DL_FUNCTION_FAILED;
	}

	/* Write the portion of the header that is optional and different for every file
	   type */
	if (pFileHeader)
	{
		if (1 != fwrite((void *)pFileHeader->Data, pFileHeader->Length, 1, pFile))
		{
			/* TODO */
			fclose(pFile);
			return CSSMERR_DL_FUNCTION_FAILED;
		}
	}

	fclose(pFile);

	return CSSM_OK;
}

/*---------------------------------------------------------------- Lock Status Declarations --*
 *																							  *
 *--------------------------------------------------------------------------------------------*/

#ifdef WIN32
typedef uint32 LOCK_STATUS;
#else
typedef uintptr_t LOCK_STATUS;
#endif

/* Flags used to indicate the current condition to ffp_lockStatus_eRelease */
#define COND_FAILURE			0x80000000
#define COND_SUCCESS			0x40000000
typedef uint32 CURRENT_CONDITION;

static void ffp_lockStatus_nrInitialize(
	const FF_MM_FILE *pThis,
    const hLock_t *p_hLock,
	LOCK_STATUS *pLockStatus);

static FF_ERROR ffp_lockStatus_eGetReader(
	FF_MM_FILE *pThis,
	LOCK_STATUS *pLockStatus);

static FF_ERROR ffp_lockStatus_eGetWriter(
	FF_MM_FILE *pThis,
	LOCK_STATUS *pLockStatus);

static void ffp_lockStatus_nrRelease(
	FF_MM_FILE *pThis,
	LOCK_STATUS *pLockStatus,
	CURRENT_CONDITION Condition,
    hLock_t *p_hLock);


/*------------------------------------------------------ Other Static Function Declarations --*
 *																							  *
 *--------------------------------------------------------------------------------------------*/

typedef FF_ERROR (* FFP_COPY_FUNC)(uint8 *pFileData, void *pMemory, uint32 Size);

static FF_ERROR ffp_eCopyData(
	FF_MM_FILE *pThis,
	uint32 StartOffset,
	uint32 Size,
	void *Buffer,
	FFP_COPY_FUNC CopyFunc);

static FF_ERROR ffp_eCopyToFile(uint8 *pFileData, void *pMemory, uint32 Size);
static FF_ERROR ffp_eCopyFromFile(uint8 *pFileData, void *pMemory, uint32 Size);

/*------------------------------------------------------------------------ Operations (mmf) --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ffport_mmf_CreateFile																	  *
 * ffport_mmf_eWrite																		  *
 * ffport_mmf_eRead																			  *
 *--------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_eWrite
 *
 * Description:
 * Writes a block of memory to a mmf file.
 *
 * Parameters:
 * hMMFile (input) : Handle to a mmf file object
 * pOffset (input/ouput): Pointer to a buffer that holds the offset to write.
 *	 If *pOffset is DAL_OOB, then the write will be treated as an append. In this
 *	 case, on exit, *pOffset will equal the address that the data was written to.
 * bufferLen (input) : Size of buffer in bytes
 * buffer (input) : Data that is to be written to the file
 * paddingBytes (input) : Number of bytes of padding to write at the end of
 * the inserted block
 *
 * p_hLock (input/output/optional) : This is an optional method of preserving a
 * lock after the function exits, or using an already existing lock. If p_hLock
 * is Null, the paramater is ignored. If *p_hLock is DAL_OOB, then it will be
 * filled in with a handle to the lock. This handle must be released. If the
 * function fails and *p_hLock was DAL_OOB, it will remain DAL_OOB.
 *
 * Return value:
 * FF_OK if no error occurred, CSSMERR_DL_INTERNAL_ERROR if an attempt was made
 * to write to an area that is after EOF else the error that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ffport_mmf_eWrite(
	FF_MM_FILE_HANDLE hMMFile,
	uint32 *pOffset,
	uint32 bufferLen,
	const uint8 *buffer,
	uint32 paddingBytes,
    hLock_t *p_hLock)
{
	FF_BOOL fAppend;
    LOCK_STATUS LockStatus;
	CAST_HANDLE(hMMFile);

	ASSERT(GetHeader(pThis));
	ASSERT(pOffset);
	ASSERT(buffer);
	ASSERT(bufferLen);

	ffp_lockStatus_nrInitialize(pThis, p_hLock, &LockStatus);
	THROW_FAILURE(ffp_lockStatus_eGetWriter(pThis, &LockStatus));

	/* We should be reading after the beginning of the file */
	if (*pOffset < GET_DATA_START_POS(GetHeaderSize(pThis)))
	{
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
	}

	if (*pOffset == DAL_OOB)
	{
		fAppend = FF_TRUE;
		*pOffset = GetExtent(pThis);
	}
	else
	{
		if (*pOffset > GetExtent(pThis))
		{
			/* The start of the write block must be before or at the end extent */
			ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
			THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
		}

		fAppend = (*pOffset + paddingBytes + bufferLen > GetExtent(pThis));
	}

	/* Retieve the data block to write to */
	CATCH_FUNCTION(ffp_eCopyData(pThis, *pOffset, bufferLen, (void *)buffer, ffp_eCopyToFile))
	{
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_ERROR();
	}

	if (fAppend)
	{
		/* Update extent */
		SetExtent(pThis, *pOffset + paddingBytes + bufferLen);

		if (CSSM_FALSE == s_fUseWriteBackCache)
		{
#if defined (WIN32)
			if (!FlushViewOfFile(GetExtentAddress(pThis), sizeof(GetExtent(pThis))))
			{
				return MapWin32Error();
			}
#elif defined (UNIX)
			uintptr_t pExtent = (uintptr_t) GetExtentAddress(pThis);
			uintptr_t pPage = (pExtent / _PAGE_SIZE) * _PAGE_SIZE;
			DPRINTF("ff", "(ffport_mmf_eWrite) syncing page at 0x%p\n", pPage);
			if (msync((void *) pPage, _PAGE_SIZE, MS_SYNC))
			{
				return MapErrno();
			}
#else
#error Not implemented.
#endif
		}
	}

	ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_SUCCESS, p_hLock);
	return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_eWriteCssmData
 *
 * Description:
 * Write a CSSM_DATA structure to a mmf file.
 *
 * Parameters:
 * hMMFile (input) : Handle to a mmf file object
 * pOffset (input/ouput): Pointer to a buffer that holds the offset to write.
 *	 If *pOffset is DAL_OOB, then the write will be treated as an append. In this
 *	 case, on exit, *pOffset will equal the address that the data was written to.
 * pData (input) : Data to write out
 *
 * p_hLock (input/output/optional) : This is an optional method of preserving a
 * lock after the function exits, or using an already existing lock. If p_hLock
 * is Null, the paramater is ignored. If *p_hLock is DAL_OOB, then it will be
 * filled in with a handle to the lock. This handle must be released. If the
 * function fails and *p_hLock was DAL_OOB, it will remain DAL_OOB.
 *
 * Return value:
 * FF_OK if no error occurred, CSSMERR_DL_INTERNAL_ERROR if an attempt was made
 * to write to an area that is after EOF else the error that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ffport_mmf_eWriteCssmData(
	FF_MM_FILE_HANDLE hMMFile,
	uint32 *pOffset,
	DAL_CONST_DATA_PTR pData,
    hLock_t *p_hLock)
{
	FF_BOOL fAppend;
    LOCK_STATUS LockStatus;
	uint32 bufferLen;
	uint32 byteSexFixedLength;
	CAST_HANDLE(hMMFile);

	ASSERT(pOffset);
	ASSERT(GetHeader(pThis));
	ASSERT(pData);

	ffp_lockStatus_nrInitialize(pThis, p_hLock, &LockStatus);

	bufferLen = sizeof(pData->Length) + pData->Length;

	THROW_FAILURE(ffp_lockStatus_eGetWriter(pThis, &LockStatus));

	/* We should be reading after the beginning of the file */
	if (*pOffset < GET_DATA_START_POS(GetHeaderSize(pThis)))
	{
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
	}

	if (*pOffset == DAL_OOB)
	{
		fAppend = FF_TRUE;
		*pOffset = GetExtent(pThis);
	}
	else
	{
		if (*pOffset > GetExtent(pThis))
		{
			/* The start of the write block must be before or at the end extent */
			ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
			THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
		}

		fAppend = (*pOffset + bufferLen > GetExtent(pThis));
	}

	/* Retieve the data block to write to */
	byteSexFixedLength = FIX_BYTE_SEX(pData->Length);
	CATCH_FUNCTION(ffp_eCopyData(pThis, *pOffset, sizeof(byteSexFixedLength), &byteSexFixedLength,
		ffp_eCopyToFile))
	{
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_ERROR();
	}

	if (0 != pData->Length)
	{
		CATCH_FUNCTION(ffp_eCopyData(pThis, *pOffset + sizeof(byteSexFixedLength), pData->Length,
			pData->Data, ffp_eCopyToFile))
		{
			ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
			THROW_ERROR();
		}
	}

	/* Update extent if we need to */
	if (fAppend)
	{
		SetExtent(pThis, *pOffset + bufferLen);

		if (CSSM_FALSE == s_fUseWriteBackCache)
#if defined (WIN32)
		{
			if (!FlushViewOfFile(GetExtentAddress(pThis), sizeof(GetExtent(pThis))))
			{
				return MapWin32Error();
			}
		}
#elif defined (UNIX)
		{
			uintptr_t pExtent = (uintptr_t) GetExtentAddress(pThis);
			uintptr_t pPage = (pExtent / _PAGE_SIZE) * _PAGE_SIZE;
			DPRINTF("ff", "(ffport_mmf_eWriteCssmData) syncing page at 0x%p\n", pPage);
			if (msync((void *) pPage, _PAGE_SIZE, MS_SYNC))
			{
				return MapErrno();
			}
		}
#else
		{
#error Not implemented.
		}
#endif
	}

	ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_SUCCESS, p_hLock);
	return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_eRead
 *
 * Description:
 * Read data from a mmf file.
 *
 * Parameters:
 * hMMFile (input) : Handle to a mmf file object
 * Offset (input) : Position in the file to read from
 * bufferLen (input) : Size (in bytes) to read from the file to buffer
 * buffer (output) : output buffer to hold the contense of the file
 * p_hLock (input/output/optional) : This is an optional method of preserving a
 * lock after the function exits, or using an already existing lock. If p_hLock
 * is Null, the paramater is ignored. If *p_hLock is DAL_OOB, then it will be
 * filled in with a handle to the lock. This handle must be released. If the
 * function fails and *p_hLock was DAL_OOB, it will remain DAL_OOB.
 *
 * Return value:
 * FF_OK if no error occurred, CSSMERR_DL_NO_DATA_FOUND if an attempt to read past
 * the end of file is made, else the error that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ffport_mmf_eRead(
	FF_MM_FILE_HANDLE hMMFile,
	uint32 Offset,
	uint32 bufferLen,
	uint8 *buffer,
    hLock_t *p_hLock)
{
    LOCK_STATUS LockStatus;
	CAST_HANDLE(hMMFile);

	ASSERT(buffer);
	ASSERT(bufferLen);

	ffp_lockStatus_nrInitialize(pThis, p_hLock, &LockStatus);

	THROW_FAILURE(ffp_lockStatus_eGetReader(pThis, &LockStatus));

	/* We should be reading after the beginning of the file */
	if (Offset < GET_DATA_START_POS(GetHeaderSize(pThis)))
	{
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
	}

	/* We should be reading before the end of file */
	if (Offset + bufferLen > GetExtent(pThis))
	{
		/* we are past the end of the file */
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_NEW_ERROR (CSSMERR_DL_ENDOFDATA);
	}

	/* Retieve the data block */
	CATCH_FUNCTION(ffp_eCopyData(pThis, Offset, bufferLen, buffer, ffp_eCopyFromFile))
	{
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_ERROR();
	}

	ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_SUCCESS, p_hLock);
	return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_eReadCssmData
 *
 * Description:
 * Reads a CSSM_DATA structure from a mmf file.
 *
 * Parameters:
 * hMMFile (input) : Handle to a mmf file object
 * Offset (input) : Position in the file to read from
 * pData (Ouput) : The contense of the CSSM data structure at Offset
 * p_hLock (input/output/optional) : This is an optional method of preserving a
 * lock after the function exits, or using an already existing lock. If p_hLock
 * is Null, the paramater is ignored. If *p_hLock is DAL_OOB, then it will be
 * filled in with a handle to the lock. This handle must be released. If the
 * function fails and *p_hLock was DAL_OOB, it will remain DAL_OOB.
 *
 * Note:
 * Memory is allocated with the dl's malloc function
 *
 * Return value:
 * FF_OK if no error occurred, CSSMERR_DL_NO_DATA_FOUND if an attempt to read past
 * the end of file is made, CSSMERR_DL_DATABASE_CORRUPT if the length of the
 * CSSM_DATA is not valid, else the error that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ffport_mmf_eReadCssmData(
	FF_MM_FILE_HANDLE hMMFile,
	uint32 Offset,
	CSSM_DATA_PTR pData,
    hLock_t *p_hLock)
{
    LOCK_STATUS LockStatus;
	CAST_HANDLE(hMMFile);

	ASSERT(pData);

	pData->Length = 0;
	pData->Data = NULL;

	ffp_lockStatus_nrInitialize(pThis, p_hLock, &LockStatus);

	/* Retieve the data block to read from (we are just reading the length */
	THROW_FAILURE(ffp_lockStatus_eGetReader(pThis, &LockStatus));

	/* We should be reading after the beginning of the file */
	if (Offset < GET_DATA_START_POS(GetHeaderSize(pThis)))
	{
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);
	}

	/* We should be reading before the end of file */
	if (Offset + sizeof(uint32) > GetExtent(pThis))
	{
		/* we are past the end of the file */
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_NEW_ERROR (CSSMERR_DL_ENDOFDATA);
	}

	/* Retieve the data block to read from, and get the data's length */
	CATCH_FUNCTION(ffp_eCopyData(pThis, Offset, sizeof(uint32), (uint8 *)&pData->Length,
		ffp_eCopyFromFile))
	{
		ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
		THROW_ERROR();
	}

	pData->Length = FIX_BYTE_SEX(pData->Length);

	if (pData->Length)
	{
		/* We should be reading before the end of file */
		if (Offset + sizeof(pData->Length) + pData->Length > GetExtent(pThis))
		{
			/* we are past the end of the file, this should not occur here */
			ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
			pData->Length = 0;
			THROW_NEW_ERROR (CSSMERR_DL_DATABASE_CORRUPT);
		}

		pData->Data = (uint8 *)BioAPI_calloc(pData->Length, 1, NULL);
		if (!pData->Data)
		{
			pData->Length = 0;
			ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
			THROW_NEW_ERROR (CSSMERR_DL_MEMORY_ERROR);
		}

		CATCH_FUNCTION(ffp_eCopyData(pThis, Offset + sizeof(pData->Length), pData->Length, pData->Data,
			ffp_eCopyFromFile))
		{
			ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_FAILURE, p_hLock);
			BioAPI_free(pData->Data, NULL);
			pData->Data = NULL;
			pData->Length = 0;
			THROW_ERROR();
		}
	}

	ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_SUCCESS, p_hLock);
	return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_eGetWriterLock
 *
 * Description:
 * Fills p_hLock with the writer lock. This function is useful in cases when
 * it is necesary to read from a file, then write to a file without another
 * writer first getting access. This is needed because it is not possible to
 * create a ReaderToWriter functon.
 *
 * Paramaters:
 * hMMFile (input) : Handle to a mmf file object
 *
 * p_hLock (input/output/optional) : This is an optional method of preserving a
 * lock after the function exits, or using an already existing lock. If p_hLock
 * is Null, the paramater is ignored. If *p_hLock is DAL_OOB, then it will be
 * filled in with a handle to the lock. This handle must be released. If the
 * function fails and *p_hLock was DAL_OOB, it will remain DAL_OOB.
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ffport_mmf_eGetWriterLock(
	FF_MM_FILE_HANDLE hMMFile,
    hLock_t * p_hLock)
{
    LOCK_STATUS LockStatus;
	CAST_HANDLE(hMMFile);

	ffp_lockStatus_nrInitialize(pThis, p_hLock, &LockStatus);

	THROW_FAILURE(ffp_lockStatus_eGetWriter(pThis, &LockStatus));

	ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_SUCCESS, p_hLock);
	return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_eGetReaderLock
 *
 * Description:
 * Fills p_hLock with the Reader lock.
 *
 * Paramaters:
 * hMMFile (input) : Handle to a mmf file object
 *
 * p_hLock (input/output/optional) : This is an optional method of preserving a
 * lock after the function exits, or using an already existing lock. If p_hLock
 * is Null, the paramater is ignored. If *p_hLock is DAL_OOB, then it will be
 * filled in with a handle to the lock. This handle must be released. If the
 * function fails and *p_hLock was DAL_OOB, it will remain DAL_OOB.
 *
 * Return value:
 * FF_OK on success, otherwise the error code that occurred
 *---------------------------------------------------------------------------*/
FF_ERROR ffport_mmf_eGetReaderLock(
	FF_MM_FILE_HANDLE hMMFile,
    hLock_t * p_hLock)
{
    LOCK_STATUS LockStatus;
	CAST_HANDLE(hMMFile);

	ffp_lockStatus_nrInitialize(pThis, p_hLock, &LockStatus);

	THROW_FAILURE(ffp_lockStatus_eGetReader(pThis, &LockStatus));

	ffp_lockStatus_nrRelease(pThis, &LockStatus, COND_SUCCESS, p_hLock);
	return FF_OK;
}

/*-------------------------------------------------------- File, Directory & Time Functions --*
 *																							  *
 * Declares the following functions in this section:										  *
 * ffport_eGetPathStart																		  *
 * ffport_eDeleteDbFiles																	  *
 * ffport_neDoesFileExist																	  *
 * ffport_GetSystemTime																		  *
 *--------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name: ffport_eGetPathStart
 *
 * Description:
 * Creates a new directory based on DbName. The new directory name is returned
 * in szNewDirName. If a directory of the desired name already exists, the error
 * should be ignored.
 *
 * Parameters:
 * DbName (input) : Name of the database being created
 * szPathStart (output) : The start of the path to the files associated with
 * a database. This is normally "CURRENT_DIR\DB_NAME." but it will be shrunk
 * if either the current directory or the DB Name is too long.
 *
 * Return value:
 * CSSM_OK if no error occurred, else CSSM_FAIL
 *---------------------------------------------------------------------------*/
FF_ERROR ffport_eGetPathStart(const char *DbName, 
							  char *szPathStart, 
							  char* AccessPath)
{
    size_t DirLen, DbLen;
	CSSM_RETURN ret;
	char tmpPath[MAX_PATH_START_LENGTH+1];

	ASSERT(DbName);
	ASSERT(szPathStart);

	DbLen = strlen(DbName);

	if (AccessPath != NULL)
	{
#if defined (UNIX)
		/* Check if we have an absolute path */
		if (AccessPath[0] != '/')
		{
			return (CSSMERR_DL_INVALID_POINTER);
		}
#endif
		strncpy(tmpPath, AccessPath, MAX_PATH_START_LENGTH);
	}
	else
	{
		/* Get the current directory name (absolute path) */
		if ((ret = port_getcwd(tmpPath, MAX_PATH_START_LENGTH)) != CSSM_OK)
		{
			return ret;
		}
	}

	tmpPath[MAX_PATH_START_LENGTH] = '\0';
	DirLen = strlen(tmpPath);

	if (DirLen == 0)
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}
	else if ((DirLen + 1 + DbLen) <= MAX_PATH_START_LENGTH)
	{
		/* Copy the AccessPath to AdditionalInformation */
	    memcpy(szPathStart, tmpPath, DirLen+1);
	}
	else
	{
		/* the current directory is too long, use a default dir */
#if defined (WIN32)
		strcpy(szPathStart, "c:");
#elif defined (UNIX)
		strcpy(szPathStart, "/tmp");
#endif
		DirLen = strlen(szPathStart);
	}

	if ((DirLen + 1 + DbLen) > MAX_PATH_START_LENGTH)
	{
		return (CSSMERR_DL_INVALID_DB_NAME);
	}

    szPathStart[DirLen] = DIRECTORY_SEPARATOR;
    memcpy(&szPathStart[DirLen+1], DbName, DbLen);
    szPathStart[DirLen+1+DbLen] = '\0';

    return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_eDeleteDbFiles
 *
 * Description:
 * Deletes a directory and all the files in that directory
 *
 * Parameters:
 * szPathStart (input) : File prefix used for files of a particular database, includes
 * path (i.e. c:\winnt\system32\databases\dbname)
 *
 * Return value:
 * CSSM_OK if no error occurred, else CSSM_FAIL
 *---------------------------------------------------------------------------*/
FF_ERROR ffport_eDeleteDbFiles(const char *szPathStart)
{
#if defined (WIN32)
	HANDLE FindHandle;
	PATH_NAME Search;
	WIN32_FIND_DATA FileData;
	PATH_START szDirName;
	uint32 i;

	ASSERT(szPathStart);

	/* First get the dir path, we will need this to delete the files */
	i = strlen(szPathStart)-1;
	for (; szPathStart[i] != DIRECTORY_SEPARATOR; i--)
	{
		if (i == 0)
		{
			/* there should always be atleast one slash in the path */
			DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
			return CSSMERR_DL_INTERNAL_ERROR;
		}
	}
	memcpy(szDirName, szPathStart, i);
	szDirName[i] = 0;

	/* Now preform the search for files */
	sprintf(Search, "%s.*.ffdb", szPathStart);
	FindHandle = FindFirstFile(Search, &FileData);

	/* Remove all the files in the directory */
	while (FindHandle != INVALID_HANDLE_VALUE)
	{
		char FilePath[MAX_PATH];

		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY ||
			FileData.dwFileAttributes == FILE_ATTRIBUTE_READONLY)
		{
			return CSSMERR_DL_FUNCTION_FAILED;
		}

		sprintf(FilePath, "%s\\%s", szDirName, FileData.cFileName);
		if (!DeleteFile(FilePath))
		{
			return MapWin32Error();
		}

		if (!FindNextFile(FindHandle, &FileData))
		{
			if (!FindClose(FindHandle))
			{
				return MapWin32Error();
			}
			FindHandle = INVALID_HANDLE_VALUE;
		}
	}

	return FF_OK;
#elif defined (UNIX)
	DIR *dir;
	PATH_NAME Search;
	struct dirent *ent;
	PATH_START szDirName;
	uint32 i;

	ASSERT(szPathStart);

	DPRINTF("ff", "(ffport_eDeleteDbFiles) deleting files in %s\n",
		szPathStart);

	/* First get the dir path, we will need this to delete the files */
	i = strlen(szPathStart)-1;
	for (; szPathStart[i] != '/'; i--)
	{
		if (i == 0)
		{
			/* there should always be atleast one slash in the path */
			DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
			return CSSMERR_DL_INTERNAL_ERROR;
		}
	}
	memcpy(szDirName, szPathStart, i);
	szDirName[i] = 0;

	/* Now perform the search for files */
	sprintf(Search, "%s.*.ffdb", &szPathStart[i] + 1);
	if (!(dir = opendir(szDirName)))
		return MapErrno();

	/* Remove all the files in the directory */
	for (ent = readdir(dir); ent; ent = readdir(dir)) {
		if (fnmatch(Search, ent->d_name, 0) == 0) {
			char FilePath[MAX_PATH_NAME_LENGTH];
			sprintf(FilePath, "%s/%s", szDirName, ent->d_name);
			if (unlink(FilePath)) {
				closedir(dir);
				return MapErrno();
			}
		}
	}

	closedir(dir);

	return FF_OK;
#else
#error Not implemented.
	return FF_FAIL;
#endif
}

FF_ERROR ffport_eDeleteDbRelationFiles(CSSM_DB_RECORDTYPE RelationID, const char *szPathStart)
{
#if defined (WIN32)
	HANDLE FindHandle;
	PATH_NAME Search;
	WIN32_FIND_DATA FileData;
	PATH_START szDirName;
	uint32 i;

	ASSERT(szPathStart);

	/* First get the dir path, we will need this to delete the files */
	i = strlen(szPathStart)-1;
	for (; szPathStart[i] != DIRECTORY_SEPARATOR; i--)
	{
		if (i == 0)
		{
			/* there should always be atleast one slash in the path */
			DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
			return CSSMERR_DL_INTERNAL_ERROR;
		}
	}
	memcpy(szDirName, szPathStart, i);
	szDirName[i] = 0;

	/* Now preform the search for files */
	sprintf(Search, "%s.%08x.*.ffdb", szPathStart, (uint32)RelationID);
	FindHandle = FindFirstFile(Search, &FileData);

	/* Remove all the files in the directory */
	while (FindHandle != INVALID_HANDLE_VALUE)
	{
		char FilePath[MAX_PATH];

		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY ||
			FileData.dwFileAttributes == FILE_ATTRIBUTE_READONLY)
		{
			return CSSMERR_DL_FUNCTION_FAILED;
		}

		sprintf(FilePath, "%s\\%s", szDirName, FileData.cFileName);
		if (!DeleteFile(FilePath))
		{
			return MapWin32Error();
		}

		if (!FindNextFile(FindHandle, &FileData))
		{
			if (!FindClose(FindHandle))
			{
				return MapWin32Error();
			}
			FindHandle = INVALID_HANDLE_VALUE;
		}
	}

	return FF_OK;
#elif defined (UNIX)
	DIR *dir;
	PATH_NAME Search;
	struct dirent *ent;
	PATH_START szDirName;
	uint32 i;

	ASSERT(szPathStart);

	DPRINTF("ff", "(ffport_eDeleteDbRelationFiles) deleting files in %s\n",
		szPathStart);

	/* First get the dir path, we will need this to delete the files */
	i = strlen(szPathStart)-1;
	for (; szPathStart[i] != '/'; i--)
	{
		if (i == 0)
		{
			/* there should always be atleast one slash in the path */
			DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
			return CSSMERR_DL_INTERNAL_ERROR;
		}
	}
	memcpy(szDirName, szPathStart, i);
	szDirName[i] = 0;

	/* Now perform the search for files */
	sprintf(Search, "%s.%08x.*.ffdb", &szPathStart[i] + 1, (uint32)RelationID);
	if (!(dir = opendir(szDirName)))
		return MapErrno();

	/* Remove all the files in the directory */
	for (ent = readdir(dir); ent; ent = readdir(dir)) {
		if (fnmatch(Search, ent->d_name, 0) == 0) {
			char FilePath[MAX_PATH_NAME_LENGTH];
			sprintf(FilePath, "%s/%s", szDirName, ent->d_name);
			if (unlink(FilePath)) {
				closedir(dir);
				return MapErrno();
			}
		}
	}

	closedir(dir);

	return FF_OK;
#else
#error Not implemented.
	return FF_FAIL;
#endif
}

/*-----------------------------------------------------------------------------
 * Name: ffport_neDoesFileExist
 *
 * Description:
 * Determine if a file exists
 *
 * Parameters:
 *
 * Return value:
 * CSSM_OK if no error occurred, else CSSM_FAIL
 *---------------------------------------------------------------------------*/
CSSM_BOOL ffport_neDoesFileExist(const char *szPathName)
{
#if defined (WIN32)
	HANDLE FindHandle;
	WIN32_FIND_DATA FileData;

	FindHandle = FindFirstFile(szPathName, &FileData);
	if (FindHandle == INVALID_HANDLE_VALUE)
	{
		return CSSM_FALSE;
	}

	FindClose(FindHandle);
	return CSSM_TRUE;
#elif defined (UNIX)
	struct stat statbuf;
	if (stat(szPathName, &statbuf))
		return CSSM_FALSE;

	return CSSM_TRUE;
#else
#error Not implemented.
	return CSSM_FALSE;
#endif
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_page_Construct
 *
 * Description:
 * Construct a new ffport_mmf_page structure
 *
 * Parameters:
 * pThis (input) : Pointer to the object to construct
 *
 * Return value:
 * CSSM_OK if no error occurred, else CSSM_FAIL
 *---------------------------------------------------------------------------*/
static CSSM_RETURN ffport_mmf_page_Construct(FFPORT_MMF_PAGE *pThis)
{
	ASSERT(pThis);

	pThis->hFile = INVALID_HANDLE_VALUE;
	pThis->hMapping = NULL;
	pThis->pData = NULL;
	pThis->PageNumber = DAL_OOB;
#if defined (WIN32)
	pThis->Mode = PAGE_READONLY;
#elif defined (UNIX)
	pThis->Mode = PROT_READ;
#else
#error Not implemented.
#endif

	return CSSM_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_page_Deconstruct
 *
 * Description:
 * Deconstruct a ffport_mmf_page structure
 *
 * Parameters:
 * pThis (input) : Pointer to the object to deconstruct
 *
 * Return value:
 * None.
 *---------------------------------------------------------------------------*/
#ifndef _DEBUG
#pragma warning(disable: 4189)	/* Turn off warnings about Result initialized but not referenced */
#endif
static void ffport_mmf_page_Deconstruct(FFPORT_MMF_PAGE *pThis)
{
#if defined (WIN32)
	ASSERT(pThis);

	if (pThis->pData != NULL)
	{
		FF_BOOL Result = UnmapViewOfFile(pThis->pData);
		ASSERT(Result != FF_FALSE);
	}

	if (pThis->hMapping != NULL)
	{
		FF_BOOL Result = CloseHandle(pThis->hMapping);
		ASSERT(Result != FF_FALSE);
	}

	if (pThis->hFile && pThis->hFile != INVALID_HANDLE_VALUE)
	{
		FF_BOOL Result = CloseHandle(pThis->hFile);
		ASSERT(Result != FF_FALSE);
	}
#elif defined (UNIX)
	ASSERT(pThis);

	DPRINTF("ff", "(ffport_mmf_page_Deconstruct) unmapping page at 0x%p, closing fd %d\n", pThis->pData, pThis->hFile);

	if (pThis->pData != NULL)
	{
		int Result = munmap(pThis->pData, _PAGE_SIZE);
		ASSERT(Result == 0);
	}

	if (pThis->hFile != INVALID_HANDLE_VALUE)
	{
		int Result = close(pThis->hFile);
		ASSERT(Result == 0);
	}
#else
#error Not implemented.
#endif
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_page_eCreateMap
 *
 * Description:
 * Maps a portion of the current processes memory to an open file
 *
 * Parameters:
 * pThis (input/output) :
 *
 * Return value:
 * FF_OK if no error occurred, else the error that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR ffport_mmf_page_eMapPage(
	FFPORT_MMF_PAGE *pThis,
	const char *szUniqueFileId,
	uint32 PageNumber)
{
#if defined (WIN32)
	uint32 dwDesiredAccess;

	ASSERT(pThis);
	ASSERT(pThis->hFile && pThis->hFile != INVALID_HANDLE_VALUE);
	ASSERT(s_dwAllocationGranularity != 0);
	ASSERT(szUniqueFileId && *szUniqueFileId);
	ASSERT(PageNumber != DAL_OOB);

	if (PageNumber != pThis->PageNumber)
	{
		char szMappingName[MAX_PATH_NAME_LENGTH+7];
		static SECURITY_DESCRIPTOR sd = {0};
		static SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), &sd,TRUE};

		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

		pThis->PageNumber = DAL_OOB;

		sprintf(szMappingName, "%06x:%d:%s", PageNumber, pThis->Mode, szUniqueFileId);

		if (pThis->pData != NULL)
		{
			FF_BOOL Result = UnmapViewOfFile(pThis->pData);
			ASSERT(Result != FF_FALSE);
			pThis->pData = NULL;
		}

		if (pThis->hMapping != NULL)
		{
			FF_BOOL Result = CloseHandle(pThis->hMapping);
			ASSERT(Result != FF_FALSE);
			pThis->hMapping = NULL;
		}

        ASSERT(PageNumber < (0xffffffff / _PAGE_SIZE));
		pThis->hMapping = CreateFileMapping(
			pThis->hFile, /* hFile */
			&sa, /* pFileMappingAttributes */
			pThis->Mode, /* flProtect */
			0, /* dwMaximumSizeHigh */
            _PAGE_SIZE * (PageNumber+1), /* dwMaximumSizeLow */
			szMappingName); /* lpName */

		if (pThis->hMapping == NULL)
		{
			return MapWin32Error();
		}
		else
		{
			/*uint32 Error = GetLastError();*/
			GetLastError();
		}

		if (PAGE_READWRITE == pThis->Mode)
			dwDesiredAccess = FILE_MAP_WRITE;
		else
			dwDesiredAccess = FILE_MAP_READ;

		pThis->pData = (uint8 *)MapViewOfFile(
			pThis->hMapping, /* hFileMappingObject */
			dwDesiredAccess, /* dwDesiredAccess */
			0, /* dwFileOffsetHigh */
            PageNumber * _PAGE_SIZE, /* dwFileOffsetLow */
            _PAGE_SIZE); /* dwNumberOfBytesToMap */

		if (pThis->pData == NULL)
		{
			return MapWin32Error();
		}

		pThis->PageNumber = PageNumber;
	}

	ASSERT(pThis->pData);
	ASSERT(pThis->hMapping);
	ASSERT(pThis->PageNumber == PageNumber);
	return FF_OK;
#elif defined (UNIX)
	ASSERT(pThis);
	ASSERT(pThis->hFile && pThis->hFile != INVALID_HANDLE_VALUE);
	ASSERT(s_dwAllocationGranularity != 0);
	ASSERT(szUniqueFileId && *szUniqueFileId);
	ASSERT(PageNumber != DAL_OOB);

	if (PageNumber != pThis->PageNumber)
	{
		off_t PageStart = PageNumber * _PAGE_SIZE;
		off_t PageEnd   = PageStart  + _PAGE_SIZE;
		struct stat statbuf;
		int flags;

		if (pThis->pData != NULL)
		{
			int Result = munmap(pThis->pData, _PAGE_SIZE);
			ASSERT(Result == 0);
			DPRINTF("ff", "(ffport_mmf_page_eMapPage) unmapping page at 0x%p for fd %d\n", pThis->pData, pThis->hFile);
			pThis->pData = NULL;
		}

		if (fstat(pThis->hFile, &statbuf))
			return MapErrno();

		if (PageEnd > statbuf.st_size)
			if (ftruncate(pThis->hFile, PageEnd))
				return MapErrno();

		/*
		 * The only one of these that's actually in the UNIX
		 * standard is MAP_SHARED.
		 */
		flags  = MAP_SHARED;
#ifdef MAP_FILE
		flags |= MAP_FILE;
#endif
#ifdef MAP_VARIABLE
		flags |= MAP_VARIABLE;
#endif
		pThis->pData = mmap(NULL,
			_PAGE_SIZE,
			pThis->Mode,
			flags,
			pThis->hFile,
			PageStart);

		if (pThis->pData == NULL)
			return MapErrno();

		DPRINTF("ff", "(ffport_mmf_page_eMapPage) mapping page %d at 0x%p for fd %d\n", PageNumber, pThis->pData, pThis->hFile);

		pThis->PageNumber = PageNumber;
	}

	ASSERT(pThis->pData);
	ASSERT(pThis->PageNumber == PageNumber);
	return FF_OK;
#else
#error Not implemented.
	return FF_FAIL;
#endif
}
#ifndef _DEBUG
#pragma warning(default: 4189)
#endif

/*-----------------------------------------------------------------------------
 * Name: ffp_eCopyData
 *
 * Description:
 * Copys data between memory and a file
 *
 * Parameters:
 * pThis (input/output) : Pointer to the MMF object
 * Start (input) : Byte offset in the file where we should copy from or to
 * Size (input) : Number of bytes to Copy
 * Buffer (input) : Memory to copy from or two copy
 * eCopyFunc (input) : Function to preform the copy,
 *
 * Return value:
 * FF_OK if no error occurred, else the error that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR ffp_eCopyData(
	FF_MM_FILE *pThis,
	uint32 StartOffset,
	uint32 Size,
	void *Buffer,
	FFP_COPY_FUNC eCopyFunc)
{
	uint32 StartPage, EndPage, cPage, EndOffset;

	ASSERT(pThis);
	ASSERT(StartOffset && StartOffset != DAL_OOB);
	ASSERT(Size);
	ASSERT(Buffer);
	ASSERT(eCopyFunc);
    ASSERT(_PAGE_SIZE && _PAGE_SIZE >= s_dwAllocationGranularity);

	/* compute the last page we need an how much to write on that page, do not end by writing zero
	   bytes */
    EndPage = (StartOffset + Size - 1) / _PAGE_SIZE;
    EndOffset = ((StartOffset + Size - 1) % _PAGE_SIZE) + 1;

    StartPage = StartOffset / _PAGE_SIZE;
    StartOffset %= _PAGE_SIZE;

	/* Copy the data to the file 1 page at a time */
	for (cPage = StartPage; cPage <= EndPage; cPage++, StartOffset = 0)
	{
		uint8 *pFileData; /* Location to copy from/to in the file */
		uint32 pageBytesToCopy; /* number of bytes to copy to the current page */

		/* computer the number of bytes to copy */
		if (cPage == EndPage)
			pageBytesToCopy = EndOffset - StartOffset;
		else
            pageBytesToCopy = _PAGE_SIZE - StartOffset;

		/* get a pointer to where in the file we copy the data */
		if (cPage == 0)
		{
			ASSERT(pThis->FirstPage.PageNumber == 0);
			pFileData = &pThis->FirstPage.pData[StartOffset];
		}
		else
		{
			THROW_FAILURE(
				ffport_mmf_page_eMapPage(&pThis->MoveablePage, pThis->szUniqueFileId, cPage));
			pFileData = &pThis->MoveablePage.pData[StartOffset];
		}

		THROW_FAILURE(eCopyFunc(pFileData, Buffer, pageBytesToCopy));
		Buffer = ((uint8 *)Buffer) + pageBytesToCopy;
	}

	return FF_OK;
}

/*-----------------------------------------------------------------------------
 * Name: ffp_eCopyToFile
 *
 * Description:
 * static function to copy data to a file from memory
 *
 * Parameters:
 * pFileData (input/output) : Destination
 * pMemory (input) : Source
 * Size (input) : Number of bytes to Copy
 *
 * Return value:
 * FF_OK if no error occurred, else the error that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR ffp_eCopyToFile(uint8 *pFileData, void *pMemory, uint32 Size)
{
#if defined (WIN32)
	ASSERT(pFileData);
	ASSERT(pMemory);
	ASSERT(Size);

	memcpy(pFileData, pMemory, Size);

	if (CSSM_FALSE == s_fUseWriteBackCache)
	{
		if (!FlushViewOfFile(pFileData, Size))
		{
			return MapWin32Error();
		}
	}

	return FF_OK;
#elif defined (UNIX)
	uintptr_t pPage = ((uintptr_t) pFileData / _PAGE_SIZE) * _PAGE_SIZE;

	ASSERT(pFileData);
	ASSERT(pMemory);
	ASSERT(Size);

	DPRINTF("ff", "(ffp_eCopyToFile) syncing page at 0x%p\n", pPage);

	memcpy(pFileData, pMemory, Size);
	if (msync((void *) pPage, _PAGE_SIZE, MS_SYNC))
	{
		return MapErrno();
	}

	return FF_OK;
#else
#error Not implemented.
	return FF_FAIL;
#endif
}

/*-----------------------------------------------------------------------------
 * Name: ffp_eCopyToFile
 *
 * Description:
 * static function to copy data from a file to memory
 *
 * Parameters:
 * pFileData (input) : Source
 * pMemory (input/output) : Destination
 * Size (input) : Number of bytes to Copy
 *
 * Return value:
 * FF_OK if no error occurred, else the error that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR ffp_eCopyFromFile(uint8 *pFileData, void *pMemory, uint32 Size)
{
	ASSERT(pFileData);
	ASSERT(pMemory);
	ASSERT(Size);

	memcpy(pMemory, pFileData, Size);

	return FF_OK;
}


/*-------------------------------------------------------------- Lock Status Implementation --*
 *																							  *
 *--------------------------------------------------------------------------------------------*/

/* flags for LOCK_STATUS */
#define NOT_LOCKED 0x0
#define READER_LOCK 0x1
#define WRITER_LOCK (0x2 | READER_LOCK)
#define RELEASE_ON_FAILURE COND_FAILURE
#define RELEASE_ON_SUCCESS COND_SUCCESS

/*-----------------------------------------------------------------------------
 * Name: ffp_lockStatus_nrInitialize
 *
 * Description:
 * Initialize a LockStatus variable bassed on the input p_hLock
 *
 * Parameters:
 *
 * Return value:
 * None.
 *---------------------------------------------------------------------------*/
static void ffp_lockStatus_nrInitialize(
	const FF_MM_FILE *pThis,
    const hLock_t *p_hLock,
	LOCK_STATUS *pLockStatus)
{
	ASSERT(pLockStatus);
	ASSERT(pThis);

	if (p_hLock)
	{
		if (*p_hLock == DAL_OOB)
			*pLockStatus = RELEASE_ON_FAILURE | NOT_LOCKED;
		else
		{
			/* Look at the value of hLock passed in, it should have been LockStatus + pThis */
            LOCK_STATUS LockStatus = *p_hLock - (hLock_t)pThis;
			if (LockStatus == NOT_LOCKED || LockStatus == WRITER_LOCK || LockStatus == READER_LOCK)
				*pLockStatus = LockStatus;
			else
			{
				DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
				*pLockStatus = RELEASE_ON_FAILURE | NOT_LOCKED;
			}
		}
	}
	else
		*pLockStatus = RELEASE_ON_FAILURE | RELEASE_ON_SUCCESS | NOT_LOCKED;
}

/*-----------------------------------------------------------------------------
 * Name: ffp_lockStatus_eGetReader
 *
 * Description:
 * Get the reader lock to the current mmf and updated the status appropriately
 *
 * Parameters:
 *
 * Return value:
 * FF_OK if no error occurred, else the error that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR ffp_lockStatus_eGetReader(
	FF_MM_FILE *pThis,
	LOCK_STATUS *pLockStatus)
{
	if (READER_LOCK == (*pLockStatus & READER_LOCK))
		return FF_OK;

	if (fWaitToRead())
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		THROW_NEW_ERROR (CSSMERR_DL_INTERNAL_ERROR);
	}
	else
	{
		*pLockStatus |= READER_LOCK;
		return FF_OK;
	}
}

/*-----------------------------------------------------------------------------
 * Name: ffp_lockStatus_eGetWriter
 *
 * Description:
 * Get the writer lock to the current mmf and updated the status appropriately
 *
 * Parameters:
 *
 * Return value:
 * FF_OK if no error occurred, else the error that occurred
 *---------------------------------------------------------------------------*/
static FF_ERROR ffp_lockStatus_eGetWriter(
	FF_MM_FILE *pThis,
	LOCK_STATUS *pLockStatus)
{
	if (WRITER_LOCK == (*pLockStatus & WRITER_LOCK))
		return FF_OK;

	if (READER_LOCK == (*pLockStatus & READER_LOCK))
	{
		/* We have to give up the reader lock before we can obtain the writer lock,
		   theirfore, if you want the writer lock, you should explicitily get it
		   before the read */
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		DoneReading();
	}

	if (fWaitToWrite())
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		THROW_NEW_ERROR (CSSMERR_DL_INTERNAL_ERROR);
	}
	else
	{
		*pLockStatus |= WRITER_LOCK;
		return FF_OK;
	}
}

/*-----------------------------------------------------------------------------
 * Name: ffp_lockStatus_nrRelease
 *
 * Description:
 * Release the lock if it appropriate to do so based on the rules set up in
 * ffp_lockStatus_nrInitialize.
 *
 * Parameters:
 *
 * Return value:
 * none
 *---------------------------------------------------------------------------*/
static void ffp_lockStatus_nrRelease(
	FF_MM_FILE *pThis,
	LOCK_STATUS *pLockStatus,
	CURRENT_CONDITION Condition,
    hLock_t *p_hLock)
{
    hLock_t LockType;
	ASSERT(pThis);
	ASSERT(pLockStatus);

	LockType = *pLockStatus & (NOT_LOCKED | READER_LOCK | WRITER_LOCK);

	if (*pLockStatus & Condition)
	{
		switch (LockType)
		{
			case NOT_LOCKED: return;
			case READER_LOCK: DoneReading(); break;
			case WRITER_LOCK: DoneWriting(); break;

			default:
				DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
				return;
		}

		*pLockStatus &= ~(READER_LOCK | WRITER_LOCK);
		*pLockStatus |= NOT_LOCKED;
	}
	else
	{
		ASSERT(p_hLock);
        *p_hLock = LockType + (hLock_t)pThis;
	}

	return;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_mmf_nrReleaseLock
 *
 * Description:
 * Releases a lock that was created in another ffport_mmf call
 *
 * Parameters:
 * hMMFile (input) : Handle to a mmf object
 * hLock (input) : Lock descripter that was returned from a ffport_mmf call
 *
 * Return value:
 * None.
 *---------------------------------------------------------------------------*/
void ffport_mmf_nrReleaseLock(
	FF_MM_FILE_HANDLE hMMFile,
    hLock_t hLock)
{
    LOCK_STATUS LockStatus;
    LOCK_STATUS LockType;
	CAST_HANDLE(hMMFile);

	if (hLock == 0 || hLock == DAL_OOB) return;

    LockStatus = hLock - (hLock_t)pThis;
	LockType = LockStatus & (NOT_LOCKED | READER_LOCK | WRITER_LOCK);
	if (LockStatus != NOT_LOCKED && LockStatus != WRITER_LOCK && LockStatus != READER_LOCK)
	{
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return;
	}
	else
	{
		switch (LockType)
		{
			case NOT_LOCKED: return;
			case READER_LOCK: DoneReading(); break;
			case WRITER_LOCK: DoneWriting(); break;

			default:
				DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
				return;
		}
		return;
	}
}

#if defined (WIN32)
static uint32 MapWin32Error(void)
{
	DWORD Error = GetLastError();

	switch (Error)
	{
	case ERROR_ACCESS_DENIED:
	case ERROR_DRIVE_LOCKED:
	case ERROR_INVALID_ACCESS:
	case ERROR_LOCK_VIOLATION:
	case ERROR_SHARING_VIOLATION:
	case ERROR_WRITE_PROTECT:
		THROW_NEW_ERROR(CSSMERR_DL_OS_ACCESS_DENIED);

	case ERROR_DEV_NOT_EXIST:
	case ERROR_DISK_CORRUPT:
	case ERROR_DISK_OPERATION_FAILED:
	case ERROR_DISK_RECALIBRATE_FAILED:
	case ERROR_DISK_RESET_FAILED:
	case ERROR_FLOPPY_BAD_REGISTERS:
	case ERROR_FLOPPY_UNKNOWN_ERROR:
	case ERROR_FLOPPY_WRONG_CYLINDER:
	case ERROR_IO_DEVICE:
	case ERROR_NO_MEDIA_IN_DRIVE:
	case ERROR_NOT_DOS_DISK:
	case ERROR_READ_FAULT:
	case ERROR_WRITE_FAULT:
	case ERROR_SEEK:
	case ERROR_SEEK_ON_DEVICE:
	case ERROR_DISK_FULL:
	case ERROR_HANDLE_DISK_FULL:

		THROW_NEW_ERROR(CSSMERR_DL_INTERNAL_ERROR);

	case ERROR_BAD_PATHNAME:
	case ERROR_FILE_CORRUPT:
	case ERROR_FILE_NOT_FOUND:
	case ERROR_OPEN_FAILED:
	case ERROR_PATH_NOT_FOUND:
		THROW_NEW_ERROR(CSSMERR_DL_DATABASE_CORRUPT);

	case ERROR_NOT_ENOUGH_MEMORY:
	case ERROR_OUTOFMEMORY:
	case ERROR_PAGEFILE_QUOTA:
		THROW_NEW_ERROR(CSSMERR_DL_MEMORY_ERROR);

	default:
		THROW_NEW_ERROR(CSSMERR_DL_FUNCTION_FAILED);
	}
}
#elif defined (UNIX)
static uint32 MapErrno(void)
{
	switch (errno)
	{
	case EACCES:
		THROW_NEW_ERROR(CSSMERR_DL_OS_ACCESS_DENIED);
	case ENOMEM:
		THROW_NEW_ERROR(CSSMERR_DL_MEMORY_ERROR);
	case EIO:
	case EMFILE:
	case ENFILE:
	case ENOSPC:
		THROW_NEW_ERROR(CSSMERR_DL_INTERNAL_ERROR);
	default:
		THROW_NEW_ERROR(CSSMERR_DL_FUNCTION_FAILED);
	}
#ifdef _DEBUG
	perror(__FILE__);
#endif
}
#endif

void ffport_nrSetCache(CSSM_BOOL fUseWriteBackCache)
{
	s_fUseWriteBackCache = fUseWriteBackCache;
}

#ifndef AIX
#ifndef FIX_BYTE_SEX
uint32 FIX_BYTE_SEX(uint32 uint32Val)
{
	uint8 buff[4];
	buff[0] = (uint8)(uint32Val >> 24);
	buff[1] = (uint8)(uint32Val >> 16);
	buff[2] = (uint8)(uint32Val >> 8);
	buff[3] = (uint8)(uint32Val >> 0);
	return *((uint32 *)buff);
}
#endif
#endif


#if MODULE_PARAMETERS_USE_FUNCTION_PARAMETERS

/*-----------------------------------------------------------------------------
 * Name: ffport_IsValidFilePermissions
 *
 * Description:
 * This function is used to enforce access control policies when a database is
 * opened and when database component files are opened for internal use
 *
 * Parameters:
 * Passphrase (input) - This is a passphrase supplied at database open time. It
 *						is compared with the template passphrase. If acceptable then
 *						access is not denied. The location of the template passphrase
 *						is implicit in the implementation of this function.
 * szDbComponentFileName (input) - This is the path and file name of a component file
 *						of the database. Each component file is tested agenst the file
 *						permissions and owner to determine if the open operation will succeed.
 * RequestAccessMask (input) - This is a representation of the caller's requested mode
 *						of accessing the database. This representation must be translated
 *						to the file system representation and a determination made if this
 *						file should be opened and with what access privilege.
 * AccessFilter (input) - This is a mask that is applied to the RequestAccessMask prior
 *						to translation to file system representation. The logical AND of
 *						RequestAccessMask and AccessFilter is applied. The AccessFilter value
 *						is obtained from a manifest or other source.
 * Error (output) -		If an error occurred as part of the processing the error code is
 *						supplied. If no error occurred then the value is set to zero.
 *						The exact value of error codes is implementation specific.
 *
 * Return value:
 * This function returns CSSM_TRUE if the requested access mode is acceptable and
 * CSSM_FALSE otherwise.
 *---------------------------------------------------------------------------*/
CSSM_BOOL ffport_IsValidFilePermissions (
					const CSSM_DATA *Passphrase,
					const char * szDbComponentFileName,
					CSSM_DB_ACCESS_TYPE RequestAccessMask,
					CSSM_DB_ACCESS_TYPE AccessFilter,
					uint32 *Error)
{
	struct _stat st;
	Passphrase;
	RequestAccessMask;
	AccessFilter;
	Error;

	/*
	 * stat the file to check the file permissions.
	 */
	if ((_stat(szDbComponentFileName, &st)) != 0)
		return CSSM_FALSE;

	/*
	 * Check the permissions RequestAccessMask masking with
	 * access filter with the permissions in st.
	 */

	return CSSM_TRUE;
}

/*-----------------------------------------------------------------------------
 * Name: ffport_SetupFilePermissionsStart
 *
 * Description:
 * This function is called prior to database creation. It registers information
 * that will be used as the baseline information when making access control decisions.
 * This function implements the access control policy most appropriate for the target
 * platform. Following the execution of	 this function the FileMode is used as part file
 * creation operations. Subsequent to file creation the SetupFilePermissionEnd() function
 * is called.
 *
 * Parameters:
 * TemplatePassphrase (input) - This is a cleartext passphrase supplied by the database
 *						creator. If an embedded passphrase is used this parameter is ignored.
 *						The passphrase must be persistent and hence should be protected before
 *						writing it to persistent storage. It is suggested that a cryptographic
 *						'wrapping' operation be used to protect the passphrase. The location
 *						and storage methods of the persistent passphrase is implicit in the implementation.
 * szDbComponentFileName (input) - This is the path and file name of a component file of the
 *						database. Each component file should be created with the appropriate
 *						file permissions and owner. It is assumed this information is embedded
 *						in the implementation of this function or is obtainable via environment
 *						varialbles or by some other means.
 * DbLocation (input) - This is the database name and location as specified by the database creator.
 *						Access policy that is dictated by knowledge of the database name can be
 *						factored into database creation.
 * FileMode (output) - This is a string containing the file mode appropriate for the specified file.
 *						It is used by the file creation routines.
 * AccessCtx (output) - This is a context pointer used to maintain state between
 *						ffport_SetupFilePermissionStart and ffport_SetupFilePermissionEnd.
 *						It contains references to memory and other state that should be cleaned
 *						up after database creation takes place.
 *
 * Return value:
 * This function returns SINT32 to indicate error conditions. A non-zero value indicates
 * errors. The precise error codes that may be returned is implementation specific.
 *---------------------------------------------------------------------------*/
sint32 ffport_SetupFilePermissionsStart(
					const CSSM_DATA * TemplatePassphrase,
					const char *szDbComponentFileName,
					const CSSM_NET_ADDRESS *DbLocation,
					char *FileMode,
					void **AccessCtx)
{
	TemplatePassphrase;
	szDbComponentFileName;
	DbLocation;
	AccessCtx;

	/*
	 * Setup the owner for the database file to be created.
	 */

	/*
	 * Create the appropriate File permissions for fopen in FileMode.
	 */
	strcpy (FileMode, "wb");

	/*
	 * Allocate a buffer for *AccessCtx and return the context information
	 * in it.
	 */

	return CSSM_OK;
}
#endif
/*-----------------------------------------------------------------------------
 * Name: ffport_SetupFilePermissionsEnd
 *
 * Description:
 * This function is called after ffport_SetupFilePermissionsStart and after the database
 * has been created. It is used to cleanup any residual state resulting from
 * ffport_SetupFilePermissionsStart. The AccessCtx parameter contains a reference to
 * a context structure. It is opaque to everything but the setup functions.
 *
 * Parameters:
 * AccessCtx (output) - This is a context pointer used to maintain state between
 *						ffport_SetupFilePermissionStart and ffport_SetupFilePermissionEnd.
 *						It contains references to memory and other state that should be cleaned
 *						up after database creation takes place.
 *
 * Return value:
 * This function returns SINT32 to indicate error conditions. A non-zero value
 * indicates errors. The precise error codes that may be returned is implementation specific.
 *---------------------------------------------------------------------------*/
sint32 ffport_SetupFilePermissionsEnd (void *AccessCtx)
{
	AccessCtx;

	/*
	 * Set the owner back to the original owner.
	 */

	return CSSM_OK;
}
