/*-----------------------------------------------------------------------
 * File: FF_DATA.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the declaration of the interface to the flatfile data file.
 * This file holds the value for the semantic information, attributes, indexes,
 * and the data object.
 */

#ifndef FF_DATA_H
#define FF_DATA_H

#include "ff_freelist.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct ff_data
{
	FF_MM_FILE_BUFF bfPortDataFile;
	FF_FREE_LIST_BUFF bfFreeList;
	uint32 NumFields;
};

typedef uint8 FF_DATA_BUFF[sizeof( struct ff_data )];
#define FF_DATA_SIZE (sizeof(FF_DATA_BUFF))
typedef FF_DATA_BUFF * FF_DATA_HANDLE;
typedef FF_DATA_BUFF const * FF_CONST_DATA_HANDLE;

#ifdef DEFINE_FF_DATA
	/* define FF_DATA, this is ff_data.c */
	typedef struct ff_data FF_DATA;
	typedef FF_DATA * FF_DATA_PTR;
	typedef FF_DATA const * FF_CONST_DATA_PTR;
#else
	/* keep FF_DATA private, this is not ff_data.c */
	#define ff_data DO_NOT_USE_ME
#endif

CSSM_RETURN ff_data_WriteNewFile(const char *szFileName, uint32 NumFields, const char *FileMode);

/* Construction & Destruction */
CSSM_RETURN ff_data_Construct(
	FF_DATA_HANDLE hDataFile,
	const char * szDataFileName,
	const char * szFreeListFileName,
	uint32 dwNumberOfFields,
	const DAL_MODULE_PARAMETERS * Parameters);

void ff_data_Deconstruct(FF_DATA_HANDLE);

/* Operations */
FF_ERROR ff_data_eDeleteData(
	FF_DATA_HANDLE hDataFile,
	uint32 FilePosition);

FF_ERROR ff_data_eInsertData(
	FF_DATA_HANDLE hDataFile,
	const DAL_CONST_DATA_PTR rgFields,
	uint32 Semantics,
	uint32 *pFilePosition);

FF_ERROR ff_data_eGetData(
	FF_DATA_HANDLE hDataFile,
	uint32 FilePosition,
	uint32 FirstField,
	uint32 NumFields,
	CSSM_DATA_PTR rgFields,
	uint32 *pSemantics);

#ifdef __cplusplus
}
#endif

#endif
