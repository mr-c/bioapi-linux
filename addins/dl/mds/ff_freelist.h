/*-----------------------------------------------------------------------
 * File: FF_FREELIST.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the declaration of the interface to the flatfile free
 * list file. This file is a list to manage the free space in the data file
 * created by file deletes.
 */

#ifndef FF_FREELIST_H
#define FF_FREELIST_H

#include "ff_port.h"

#ifdef __cplusplus
extern "C"
{
#endif

CSSM_RETURN ff_freeList_WriteNewFile(const char *szFileName, const char *FileMode);

struct ff_free_list
{
	FF_MM_FILE_BUFF bfPortFreeListFile;
};

typedef uint8 FF_FREE_LIST_BUFF[sizeof( struct ff_free_list )];
#define FF_FREE_LIST_SIZE (sizeof(FF_FREE_LIST_BUFF))
typedef FF_FREE_LIST_BUFF * FF_FREE_LIST_HANDLE;
typedef FF_FREE_LIST_BUFF const * FF_CONST_FREE_LIST_HANDLE;

#ifdef DEFINE_FF_FREE_LIST
	/* define FF_FREE_LIST, this is ff_freelist.c */
	typedef struct ff_free_list FF_FREE_LIST;
	typedef FF_FREE_LIST * FF_FREE_LIST_PTR;
	typedef FF_FREE_LIST const * FF_CONST_FREE_LIST_PTR;
#else
	/* keep FF_FREE_LIST private, this is not ff_freelist.c */
	#define ff_free_list DO_NOT_USE_ME
#endif


/* Construction & Destruction */
CSSM_RETURN ff_freeList_Construct(
	FF_FREE_LIST_HANDLE hFreeListFile,
	const char * szFileName,
	const DAL_MODULE_PARAMETERS * Parameters);

void ff_freeList_Deconstruct(FF_FREE_LIST_HANDLE);

struct ff_block_info
{
	uint32 Offset;
	uint32 Size;
};

typedef struct ff_block_info FF_BLOCK_INFO;

/* Operations */
FF_ERROR ff_freeList_eFreeBlock(
	FF_FREE_LIST_HANDLE hFreeList,
	const FF_BLOCK_INFO * pInsertedBlock);

FF_ERROR ff_freeList_eGetBlock(
	FF_FREE_LIST_HANDLE hFreeList,
	uint32 NumBlocks,
	uint32 * pFirstBlockOffset);

#ifdef __cplusplus
}
#endif

#endif
