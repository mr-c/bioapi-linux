/*-----------------------------------------------------------------------
 * File: MDS_CREATE.CPP
 *-----------------------------------------------------------------------
 */

/*
 * This file contains module management functions for the DLM
 */

#if defined(_DEBUG) && defined(WIN32)
#undef new
#include <crtdbg.h>
static char s_szFileName[] = __FILE__;
#define new new(_NORMAL_BLOCK, s_szFileName, __LINE__)
#endif

#ifndef ASSERT
	#ifdef _DEBUG
		#include <assert.h>
		/* Passing this boolean expression is nice when checking
		 * pointers on 64-bit architectures. It prevents lots of
		 * compiler warnings about "conversion from pointer to
		 * smaller integer"
		 */
		#define ASSERT(cond) assert(0 != (cond))
	#else
		#define ASSERT(cond)
	#endif
#endif

#include "port/bioapi_port.h"
#include "mds.h"
#include "port/bioapi_lock.h"
#include "bioapi_err.h"
#include "maf_collectn.h"
#include "maf_util.h"
#include "mds_internal.h"
#include "bioapi_schema.h"
#include "dal_common.h"
#define CSSMDLI CSSMAPI

/* prototype internal function from dal_src.cpp */


CSSM_RETURN dal_GetDbNames(CSSM_DL_HANDLE DLHandle, CSSM_NAME_LIST_PTR *NameList);

CSSM_RETURN dal_FreeNameList(CSSM_DL_HANDLE DLHandle, CSSM_NAME_LIST_PTR NameList);

CSSM_RETURN dal_DbCreate (
	CSSM_DL_HANDLE DLHandle,
	const char *DB_NAME_PARAMATER,
	const CSSM_NET_ADDRESS *DbLocation,
	const CSSM_DBINFO *DbInfo,
	CSSM_DB_ACCESS_TYPE AccessRequest,
	const CSSM_RESOURCE_CONTROL_CONTEXT *CredAndAclEntry,
	const void *OpenParameters,
	CSSM_DB_HANDLE *DbHandle);

CSSM_RETURN dal_DbDelete (
	CSSM_DL_HANDLE DLHandle,
	const char * DB_NAME_PARAMATER,
	const CSSM_NET_ADDRESS * DbLocation,
	const CSSM_ACCESS_CREDENTIALS * AccessCred);

CSSM_RETURN dal_DbClose (CSSM_DL_DB_HANDLE DLDBHandle);


#define UINDEX(attribute) { \
	CSSM_DB_INDEX_UNIQUE, /* IndexType */ \
	CSSM_DB_INDEX_ON_ATTRIBUTE, /* IndexedDataLocation */ \
	attribute \
}

#define INDEX(attribute) { \
	CSSM_DB_INDEX_NONUNIQUE, /* IndexType */ \
	CSSM_DB_INDEX_ON_ATTRIBUTE, /* IndexedDataLocation */ \
	attribute \
}


static void mds_HLayer_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList)
{
	static CSSM_DB_ATTRIBUTE_INFO rgAttributes[] = {
		BIOAPI_ATTRINFO_MODULEID,
		BIOAPI_ATTRINFO_MODULENAME,
		BIOAPI_ATTRINFO_PRODUCTVERSION,
		BIOAPI_ATTRINFO_SPECVERSION,
		BIOAPI_ATTRINFO_VENDOR,
		BIOAPI_ATTRINFO_DESCRIPTION
	};

	ASSERT(pList->DataRecordType == BIOAPI_H_LAYER_RECORDTYPE);
	pList->NumberOfAttributes = BIOAPI_H_LAYER_NUM_ATTRIBUTES;
	pList->AttributeInfo = rgAttributes;
	ASSERT(sizeof(rgAttributes) / sizeof(CSSM_DB_ATTRIBUTE_INFO) == pList->NumberOfAttributes);
}
static void mds_HLayer_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList)
{
	static CSSM_DB_INDEX_INFO rgIndexes[BIOAPI_H_LAYER_NUM_INDEXES] = {
		UINDEX(BIOAPI_ATTRINFO_MODULEID),
		INDEX(BIOAPI_ATTRINFO_MODULENAME),
		INDEX(BIOAPI_ATTRINFO_PRODUCTVERSION),
		INDEX(BIOAPI_ATTRINFO_SPECVERSION),
		INDEX(BIOAPI_ATTRINFO_VENDOR),
		INDEX(BIOAPI_ATTRINFO_DESCRIPTION)
	};

	ASSERT(pList->DataRecordType == BIOAPI_H_LAYER_RECORDTYPE);
	pList->NumberOfIndexes = BIOAPI_H_LAYER_NUM_INDEXES;
	pList->IndexInfo = rgIndexes;
	ASSERT(sizeof(rgIndexes) / sizeof(CSSM_DB_INDEX_INFO) == pList->NumberOfIndexes);
}

static void mds_BSP_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList)
{
	static CSSM_DB_ATTRIBUTE_INFO rgAttributes[] = {
		BIOAPI_ATTRINFO_MODULEID,
		BIOAPI_ATTRINFO_DEVICEID,
		BIOAPI_ATTRINFO_BSPNAME,
		BIOAPI_ATTRINFO_PRODUCTVERSION,
		BIOAPI_ATTRINFO_SPECVERSION,
		BIOAPI_ATTRINFO_SUPPORTEDFORMATS,
		BIOAPI_ATTRINFO_VENDOR,
		BIOAPI_ATTRINFO_FACTORSMASK,
		BIOAPI_ATTRINFO_OPERATIONS,
		BIOAPI_ATTRINFO_OPTIONS,
		BIOAPI_ATTRINFO_PAYLOADPOLICY,
		BIOAPI_ATTRINFO_MAXPAYLOADSIZE,
		BIOAPI_ATTRINFO_DEFAULTVERIFYTIMEOUT,
		BIOAPI_ATTRINFO_DEFAULTIDENTIFYTIMEOUT,
		BIOAPI_ATTRINFO_DEFAULTCAPTURETIMEOUT,
		BIOAPI_ATTRINFO_DEFAULTENROLLTIMEOUT,
		BIOAPI_ATTRINFO_MAXBSPSDBSIZE,
		BIOAPI_ATTRINFO_MAXIDENTIFY,
		BIOAPI_ATTRINFO_DESCRIPTION,
		BIOAPI_ATTRINFO_PATH
	};

	ASSERT(pList->DataRecordType == BIOAPI_BSP_RECORDTYPE);
	pList->NumberOfAttributes = BIOAPI_BSP_NUM_ATTRIBUTES;
	pList->AttributeInfo = rgAttributes;
	ASSERT(sizeof(rgAttributes) / sizeof(CSSM_DB_ATTRIBUTE_INFO) == pList->NumberOfAttributes);
}
static void mds_BSP_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList)
{
	static CSSM_DB_INDEX_INFO rgIndexes[] = {
		UINDEX(BIOAPI_ATTRINFO_MODULEID),
		INDEX(BIOAPI_ATTRINFO_DEVICEID),
		INDEX(BIOAPI_ATTRINFO_BSPNAME),
		INDEX(BIOAPI_ATTRINFO_PRODUCTVERSION),
		INDEX(BIOAPI_ATTRINFO_SPECVERSION),
		INDEX(BIOAPI_ATTRINFO_SUPPORTEDFORMATS),
		INDEX(BIOAPI_ATTRINFO_VENDOR),
		INDEX(BIOAPI_ATTRINFO_FACTORSMASK),
		INDEX(BIOAPI_ATTRINFO_OPERATIONS),
		INDEX(BIOAPI_ATTRINFO_OPTIONS),
		INDEX(BIOAPI_ATTRINFO_PAYLOADPOLICY),
		INDEX(BIOAPI_ATTRINFO_MAXPAYLOADSIZE),
		INDEX(BIOAPI_ATTRINFO_DEFAULTVERIFYTIMEOUT),
		INDEX(BIOAPI_ATTRINFO_DEFAULTIDENTIFYTIMEOUT),
		INDEX(BIOAPI_ATTRINFO_DEFAULTCAPTURETIMEOUT),
		INDEX(BIOAPI_ATTRINFO_DEFAULTENROLLTIMEOUT),
		INDEX(BIOAPI_ATTRINFO_MAXBSPSDBSIZE),
		INDEX(BIOAPI_ATTRINFO_MAXIDENTIFY),
		INDEX(BIOAPI_ATTRINFO_DESCRIPTION),
		INDEX(BIOAPI_ATTRINFO_PATH)
	};

	ASSERT(pList->DataRecordType == BIOAPI_BSP_RECORDTYPE);
	pList->NumberOfIndexes = BIOAPI_BSP_NUM_INDEXES;
	pList->IndexInfo = rgIndexes;
	ASSERT(sizeof(rgIndexes) / sizeof(CSSM_DB_INDEX_INFO) == pList->NumberOfIndexes);
}

static void mds_BIODevice_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList)
{
	static CSSM_DB_ATTRIBUTE_INFO rgAttributes[] = {
		BIOAPI_ATTRINFO_MODULEID,
		BIOAPI_ATTRINFO_DEVICEID,
		BIOAPI_ATTRINFO_DEVICESUPPORTEDFORMATS,
		BIOAPI_ATTRINFO_SUPPORTEDEVENTS,
		BIOAPI_ATTRINFO_DEVICEVENDOR,
		BIOAPI_ATTRINFO_DEVICEDESCRIPTION,
		BIOAPI_ATTRINFO_DEVICESERIALNUMBER,
		BIOAPI_ATTRINFO_DEVICEHARDWAREVERSION,
		BIOAPI_ATTRINFO_DEVICEFIRMWAREVERSION,
		BIOAPI_ATTRINFO_AUTHENTICATEDDEVICE
	};

	ASSERT(pList->DataRecordType == BIOAPI_BIO_DEVICE_RECORDTYPE);
	pList->NumberOfAttributes = BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES;
	pList->AttributeInfo = rgAttributes;
	ASSERT(sizeof(rgAttributes) / sizeof(CSSM_DB_ATTRIBUTE_INFO) == pList->NumberOfAttributes);
}
static void mds_BIODevice_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList)
{
	static CSSM_DB_INDEX_INFO rgIndexes[] = {
		UINDEX(BIOAPI_ATTRINFO_MODULEID),
		UINDEX(BIOAPI_ATTRINFO_DEVICEID),
		INDEX(BIOAPI_ATTRINFO_DEVICESUPPORTEDFORMATS),
		INDEX(BIOAPI_ATTRINFO_SUPPORTEDEVENTS),
		INDEX(BIOAPI_ATTRINFO_DEVICEVENDOR),
		INDEX(BIOAPI_ATTRINFO_DEVICEDESCRIPTION),
		INDEX(BIOAPI_ATTRINFO_DEVICESERIALNUMBER),
		INDEX(BIOAPI_ATTRINFO_DEVICEHARDWAREVERSION),
		INDEX(BIOAPI_ATTRINFO_DEVICEFIRMWAREVERSION),
		INDEX(BIOAPI_ATTRINFO_AUTHENTICATEDDEVICE)
	};

	ASSERT(pList->DataRecordType == BIOAPI_BIO_DEVICE_RECORDTYPE);
	pList->NumberOfIndexes = BIOAPI_BIO_DEVICE_NUM_INDEXES;
	pList->IndexInfo = rgIndexes;
	ASSERT(sizeof(rgIndexes) / sizeof(CSSM_DB_INDEX_INFO) == pList->NumberOfIndexes);
}


/*********************************************************************************************************/
/* CDSA Directory Schema
 */

const CSSM_DBINFO *mds_bioapi_schema_GetDBInfo()
{
	const CSSM_DB_RECORDTYPE rgRecordTypes[BIOAPI_MODULE_NUM_RELATIONS] = {
		BIOAPI_H_LAYER_RECORDTYPE, BIOAPI_BSP_RECORDTYPE, BIOAPI_BIO_DEVICE_RECORDTYPE
	};

	static CSSM_DB_PARSING_MODULE_INFO rgDefaultParsingModule[BIOAPI_MODULE_NUM_RELATIONS];
	static CSSM_DB_RECORD_ATTRIBUTE_INFO rgDefaultRecordAttributeNames[BIOAPI_MODULE_NUM_RELATIONS];
	static CSSM_DB_RECORD_INDEX_INFO rgDefaultRecordIndexes[BIOAPI_MODULE_NUM_RELATIONS];

	static const CSSM_DBINFO DefaultDbInfo = {
		BIOAPI_MODULE_NUM_RELATIONS, /* NumberOfRecordTypes */
		rgDefaultParsingModule, /* DefaultParsingModules */
		rgDefaultRecordAttributeNames, /* RecordAttributeNames */
		rgDefaultRecordIndexes, /* RecordIndexes */
		CSSM_TRUE, /* IsLocal */
		NULL, /* AccessPath */
		NULL /* Reserved */
	};
	uint32 i;

	memset(rgDefaultParsingModule, 0, sizeof(rgDefaultParsingModule));
	memset(rgDefaultRecordAttributeNames, 0, sizeof(rgDefaultRecordAttributeNames));
	memset(rgDefaultRecordIndexes, 0, sizeof(rgDefaultRecordIndexes));

	for (i = 0; i < BIOAPI_MODULE_NUM_RELATIONS; i++)
	{
		rgDefaultParsingModule[i].RecordType = rgRecordTypes[i];
		rgDefaultRecordAttributeNames[i].DataRecordType = rgRecordTypes[i];
		rgDefaultRecordIndexes[i].DataRecordType = rgRecordTypes[i];
	}

	CSSM_DB_RECORD_ATTRIBUTE_INFO * pAttributeList = rgDefaultRecordAttributeNames;
	CSSM_DB_RECORD_INDEX_INFO * pIndexList = rgDefaultRecordIndexes;

	mds_HLayer_GetAttributes(pAttributeList++);
	mds_HLayer_GetIndexes(pIndexList++);
	mds_BSP_GetAttributes(pAttributeList++);
	mds_BSP_GetIndexes(pIndexList++);
	mds_BIODevice_GetAttributes(pAttributeList++);
	mds_BIODevice_GetIndexes(pIndexList++);

	return &DefaultDbInfo;
}


/*******************************************************************/
static CSSM_RETURN mds_CreateDb(
	MDS_HANDLE MdsHandle,
	const char *szDbName,
	const CSSM_DBINFO *pDbInfo)
{
	uint32 i;
	CSSM_RETURN retValue;
	CSSM_DB_HANDLE DBHandle;
	CSSM_DB_ACCESS_TYPE access = CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE;
	CSSM_DL_DB_HANDLE DLDBHandle;
	CSSM_NAME_LIST_PTR dblist = NULL;

	ASSERT(pDbInfo);
	ASSERT(szDbName);
	ASSERT(MdsHandle);

	if (!MdsHandle)
	{
		return (CSSMERR_DL_INVALID_DL_HANDLE);
	}

	if ((retValue = dal_GetDbNames(MdsHandle, &dblist)) != CSSM_OK)
		return retValue;

	if (dblist) {
		for (i = 0; i < dblist->NumStrings; i++) {
			if (!stricmp (dblist->String[i], szDbName)) {
				dal_FreeNameList (MdsHandle, dblist);
				return CSSM_OK;
			}
		}

		dal_FreeNameList (MdsHandle, dblist);
	}

	if ((retValue = dal_DbCreate (MdsHandle, szDbName, NULL, (CSSM_DBINFO_PTR)pDbInfo,
									access, NULL, NULL, &DBHandle)) != CSSM_OK)
		return retValue;

	if (!DBHandle)
		return CSSMERR_DL_INTERNAL_ERROR;

	DLDBHandle.DLHandle = MdsHandle;
	DLDBHandle.DBHandle = DBHandle;
	dal_DbClose (DLDBHandle);

	return CSSM_OK;
}

CSSM_RETURN CSSMAPI MDS_Install(MDS_HANDLE MdsHandle)
{
	void *LockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, MdsHandle, LockRef);
	if (CSSM_OK != retValue)
	{
		return retValue;
	}

	if ((retValue = mds_CreateDb(MdsHandle, MDS_CDSA_DIRECTORY, mds_bioapi_schema_GetDBInfo()))
															!= CSSM_OK)
	{
		Addin_SPIEnd(LockRef);
		return retValue;
	}

	Addin_SPIEnd(LockRef);

	return CSSM_OK;
}

CSSM_RETURN CSSMAPI MDS_Uninstall(MDS_HANDLE MdsHandle)
{
	void *LockRef;
	CSSM_RETURN retValue = CSSM_OK;

	ADDIN_SPI_BEGIN(retValue, MdsHandle, LockRef);
	if (CSSM_OK != retValue)
	{
		return retValue;
	}

	if ((retValue = dal_DbDelete(MdsHandle, MDS_CDSA_DIRECTORY, NULL, NULL)) != CSSM_OK)
	{
		Addin_SPIEnd(LockRef);
		return retValue;
	}

	Addin_SPIEnd(LockRef);

	return CSSM_OK;
}
