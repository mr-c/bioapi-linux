/*-----------------------------------------------------------------------
 * File: FF_INDEX.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the declaration of the interface to the flatfile index
 * file. This file holds the hash of all indexes and a pointer to the position
 * in the data file where the data for a particular record can be found.
 */

#ifndef FF_INDEX_H
#define FF_INDEX_H

#include "ff_data.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint32 FF_UID;

struct ff_index
{
	FF_MM_FILE_BUFF bfPortIndexFile;
	FF_DATA_BUFF bfData;
	uint32 NumIndexes;
	uint32 NumAttributes;
	uint32 RowSize;
};

typedef uint8 FF_INDEX_BUFF[sizeof( struct ff_index )];
#define FF_INDEX_SIZE (sizeof(FF_INDEX_BUFF))
typedef FF_INDEX_BUFF * FF_INDEX_HANDLE;
typedef FF_INDEX_BUFF const * FF_CONST_INDEX_HANDLE;

#ifdef DEFINE_FF_INDEX
	/* define FF_INDEX, this is ff_index.c */
	typedef struct ff_index FF_INDEX;
	typedef FF_INDEX * FF_INDEX_PTR;
	typedef FF_INDEX const * FF_CONST_INDEX_PTR;
#else
	/* keep FF_INDEX private, this is not ff_index.c */
	#define private_ff_index DO_NOT_USE_ME
#endif

CSSM_RETURN ff_index_WriteNewFile(
	const char *szFileName,
	uint32 NumIndexes,
	uint32 NumPureAttributes,
	const char *FileMode);

/* Construction & Destruction */

CSSM_RETURN ff_index_Construct(
	FF_INDEX_HANDLE hIndex,
	const char * szIndexFileName,
	const char * szDataFileName,
	const char * szFreeListFileName,
	uint32 dwNumberOfIndexes,
	uint32 dwNumberOfAttributes,
	const DAL_MODULE_PARAMETERS * Parameters);

void ff_index_Deconstruct(FF_INDEX_HANDLE hIndex);

/* Searching */
FF_ERROR ff_index_eReadHashArray(
	FF_INDEX_HANDLE hIndex,
	FF_UID *pUid,
	uint32 *rgIndexHashVals,
    hLock_t *p_hLock);

/* Insert/Read/Delete Records */
CSSM_RETURN ff_index_InsertRecord(
	FF_INDEX_HANDLE hIndexFile,
	const DAL_CONST_DATA_PTR rgFields,
	uint32 Semantics,
	FF_UID *retUid);

FF_ERROR ff_index_eReplaceRecord(
	FF_INDEX_HANDLE hIndexFile,
	FF_UID Uid,
	const DAL_CONST_DATA_PTR rgFields,
	uint32 Semantics);

FF_ERROR ff_index_eGetRecord(
	FF_INDEX_HANDLE hIndexFile,
	FF_UID Record,
	uint32 FirstField,
	uint32 NumFields,
	CSSM_DATA_PTR rgFields,
	uint32 *pSemantics,
    hLock_t *p_hLock);

FF_ERROR ff_index_eDeleteRecord(
	FF_INDEX_HANDLE hIndexFile,
	FF_UID Record);

FF_ERROR ff_index_eGetReaderLock (
	FF_INDEX_HANDLE hIndex,
    hLock_t *p_hLock);

void ff_index_eReleaseReaderLock (
	FF_INDEX_HANDLE hIndex,
    hLock_t hLock);

#ifdef __cplusplus
}
#endif

#endif

