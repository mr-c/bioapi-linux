/*-----------------------------------------------------------------------
 * File: MAF_COLLECTN.H
 *-----------------------------------------------------------------------
 */


/*
 * This file defines the collection interface -- a set of list primitives that
 * are independant of type.
 */

#ifndef __COLLECTN_H__
#define __COLLECTN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Check to see that all required headers have been included beforehand */
#ifndef _BIOAPITYPE_H_
#error bioapi_type.h must be included before this file
#endif
#ifndef _BIOAPIERR_H_
#error bioapi_err.h must be included before this file
#endif
#ifndef _BIOAPIPORT_H
#error port/bioapi_port.h must be included before this file
#endif
#ifndef _BIOAPILOCK_H
#error port/bioapi_lock.h must be included before this file
#endif

/********************************************/
/* MAF List Collection Specific Error codes */
/********************************************/

/* Record is not found in the list */
#define MLC_ERR_NOT_FOUND			((BioAPI_RETURN)( MAF_INTERNAL_ERROR_BASE + 1 ))

/* The list, or the requested record could not be locked */
#define MLC_ERR_LOCKING				((BioAPI_RETURN)( MAF_INTERNAL_ERROR_BASE + 2 ))

/* When destroying a record, it could not be locked. The was not explicitly
 * deallocated to prevent destabilizing the system. The resource may have leaked
 */
#define MLC_ERR_POSSIBLE_RES_LEAK	((BioAPI_RETURN)( MAF_INTERNAL_ERROR_BASE + 3 ))

/* An invalid locking mode was specified */
#define MLC_ERR_LOCKING_MODE		((BioAPI_RETURN)( MAF_INTERNAL_ERROR_BASE + 4 ))

/* A system resource could not be allocated */
#define MLC_ERR_RESOURCE			((BioAPI_RETURN)( MAF_INTERNAL_ERROR_BASE + 5 ))

/* When iterating through the list, locking one of the records failed and
 * iteration stopped. The list was partially processed.
 */
#define MLC_ERR_LOCKING_PARTIAL		((BioAPI_RETURN)( MAF_INTERNAL_ERROR_BASE + 6 ))

/* The callers iterator function signalled that the iteration should be
 * stopped.
 */
#define MLC_ERR_CANCELLED			((BioAPI_RETURN)( MAF_INTERNAL_ERROR_BASE + 7 ))

/*****************/
/* Locking types */
/*****************/
/* All record indexes are referenced by their "lock reference". This type
 * must be used transparently to avoid incompatibilities with future changes.
 */
typedef void* MLC_LOCK_REF;
#define MLC_NULL_LOCK_REF NULL

/* Locking modes */
typedef enum
{
	MLC_WRITE_LOCK,
	MLC_READ_LOCK,
	MLC_NO_LOCK
}
MLC_LOCK_TYPE;

/*******************************/
/* Manipulation callback types */
/*******************************/

/* Define a calling convention abstraction for the callback functions.
 * Win32 compilers provide the ability to use a calling convention called
 * "fastcall" that passes up to two parameters in registers. This could
 * significantly increase the efficiency of the callback functions.
 */
#ifdef WIN32
#define MLC_FASTCALL __fastcall
#else
#define MLC_FASTCALL
#endif

/* This function type is used to compare a user specified "key" against a
 * list item. The function is responsible for casting the key and list
 * record to the appropriate types. The function should return zero to
 * indicate a match.
 */
typedef int (MLC_FASTCALL *MLC_FIND_FUNC) (
						void *pItem,
						void *pKey );

/* This function type is used to destroy a record in the list. All
 * resources, nested structures, and the pItem structure must be deallocated
 * by the function.
 */
typedef void (MLC_FASTCALL *MLC_ITEM_TEARDOWN_FUNC) (
						void *pItem );

/* This function is executed on each record in the list. The function is
 * responsible for casting the key and list record to the appropriate types.
 * If the function returns zero, iteration of the list will stop.
 */
typedef int (MLC_FASTCALL *MLC_ITEM_ITERATOR_FUNC) (
						void *pItem,
						void *pParam );

/*******************/
/* List structures */
/*******************/

/* A node is allocated for each record in the list. It contains the record
 * lock and pointers to adjacent nodes.
 */
typedef struct _maf_collection_node
{
	struct _maf_collection_node *pNext;
	struct _maf_collection_node *pPrev;
	void *pValue;
	cssm_SWMRLock NodeLock;
}
MAF_LIST_COLLECTION_NODE;

/* Main list structure. Callers should only use this data structure. They
 * will never be given node structures.
 */
typedef struct _maf_list_collection
{
	MAF_LIST_COLLECTION_NODE *pHead;
	MAF_LIST_COLLECTION_NODE *pTail;
	uint32 uNumNodes;
	MLC_ITEM_TEARDOWN_FUNC TearDown;
	cssm_SWMRLock ListLock;
}
MAF_LIST_COLLECTION;

/************************/
/* "Quick" query macros */
/************************/

/* Wipe a list structure to nothing */
#define MLC_Clear( collection ) \
	memset( &(collection), 0, sizeof(collection) )

/* Easy indication of whether or not the list is empty. Use this macro
 * instead of accessing the structure directly for compatibility if
 * the implementation changes.
 */
#define MLC_IsEmpty( collection ) \
	( (collection).uNumNodes == 0 )

#define MLC_Size( collection ) \
	( (collection).uNumNodes )

/*****************************/
/* List manipulation methods */
/*****************************/

BioAPI_RETURN MLC_Init(	  MAF_LIST_COLLECTION *pCollection,
						MLC_ITEM_TEARDOWN_FUNC TearDown );

BioAPI_RETURN MLC_Term(	  MAF_LIST_COLLECTION *pCollection );

BioAPI_RETURN MLC_AddItem(
						MAF_LIST_COLLECTION *pCollection,
						void *pNewItem,
						MLC_LOCK_TYPE ltLockType,
						MLC_LOCK_REF *pLockRef );

BioAPI_RETURN MLC_DeleteItem(
						MAF_LIST_COLLECTION *pCollection,
						MLC_FIND_FUNC ItemFind,
						void *pKey,
						void **ppValue );

BioAPI_RETURN MLC_FindItem(
						MAF_LIST_COLLECTION *pCollection,
						MLC_FIND_FUNC ItemFind,
						void *pKey,
						MLC_LOCK_TYPE ltLockType,
						MLC_LOCK_REF *pLockRef,
						void **ppValue );

BioAPI_RETURN MLC_ForEach(
						MAF_LIST_COLLECTION *pCollection,
						MLC_ITEM_ITERATOR_FUNC Iteration,
						void *pParam,
						MLC_LOCK_TYPE ltLockType );

BioAPI_RETURN MLC_RelockItem(
						MLC_LOCK_TYPE ltLockType,
						MLC_LOCK_REF pLockRef,
						void **ppValue );

BioAPI_RETURN MLC_ReleaseItem(
						MLC_LOCK_TYPE ltLockType,
						MLC_LOCK_REF LockRef );

BioAPI_RETURN MLC_CleanList(
						MAF_LIST_COLLECTION *pCollection,
						MLC_ITEM_TEARDOWN_FUNC TearDown );

#ifdef __cplusplus
}
#endif

#endif /* ifndef __COLLECTN_H__ */
