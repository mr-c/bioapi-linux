/*-----------------------------------------------------------------------
 * File: MDS_UTIL_QUERY.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the code used to Query MDS.
 */

#ifdef BioAPI_MEMTRACK_ON
#define CSSM_MEMTRACK_ON
#endif

#include <sys/stat.h>
#include "cssmerr.h"
#include "mds_util_intrn.h"
#include "mds.h"
#include "bioapi_schema.h"
#include "bioapi_util.h"
#include "mds_app_util.h"

void BioAPI
mdsutil_FreeRecord (uint32 NumAttributes,
	CSSM_DB_ATTRIBUTE_DATA *OutputAttributeData)
{
	uint32 i;

	for (i = 0; i < NumAttributes; i++)
	{
		if (OutputAttributeData[i].Value)
		{
			if (OutputAttributeData[i].Value->Data)
			{
				BioAPI_free (OutputAttributeData[i].Value->Data, NULL);
			}

			BioAPI_free (OutputAttributeData[i].Value, NULL);
		}
	}
}


BioAPI_RETURN BioAPI mdsutil_GetRecord(CSSM_DL_DB_HANDLE hDLDB,
									  CSSM_QUERY Query,
									  CSSM_DB_RECORDTYPE RecordType,
									  uint32 NumAttributes,
									  CSSM_HANDLE_PTR ResultsHandle,
									  CSSM_DB_ATTRIBUTE_DATA *OutputAttributeData)
{
	CSSM_DB_UNIQUE_RECORD_PTR RecordId = NULL;
	CSSM_DB_RECORD_ATTRIBUTE_DATA OutputAttributes;
	BioAPI_RETURN retValue = CSSM_OK;

	memset (&OutputAttributes, 0, sizeof (CSSM_DB_RECORD_ATTRIBUTE_DATA));

	/* We want to get NumAttributes back */
	OutputAttributes.DataRecordType = RecordType;
	OutputAttributes.NumberOfAttributes = NumAttributes;
	OutputAttributes.AttributeData = OutputAttributeData;

	if (0 == *ResultsHandle)
	{
		if( !IsBadCodePtr((CSSM_PROC_ADDR)MDSFuncs.DataGetFirst))
		{
			retValue = MDSFuncs.DataGetFirst (hDLDB, /* DLDBHandle */
											  &Query,
											  ResultsHandle,
											  &OutputAttributes,
											  NULL,
											  &RecordId);
		}
	} else {
		if( !IsBadCodePtr((CSSM_PROC_ADDR)MDSFuncs.DataGetNext))
		{
			retValue = MDSFuncs.DataGetNext (hDLDB, /* DLDBHandle */
											 *ResultsHandle,
											 &OutputAttributes,
											 NULL,
											 &RecordId);
		}
	}

	if (retValue != CSSM_OK)
		return retValue;

	if (!IsBadCodePtr((CSSM_PROC_ADDR)MDSFuncs.FreeUniqueRecord))
		MDSFuncs.FreeUniqueRecord(hDLDB, RecordId);

	return (retValue);
}

BioAPI_RETURN BioAPI mdsutil_unconditionalGetRecord(
	CSSM_DL_DB_HANDLE hDLDB,
	CSSM_DB_RECORDTYPE RecordType,
	uint32 NumAttributes,
	CSSM_HANDLE_PTR ResultsHandle,
	CSSM_DB_ATTRIBUTE_DATA *OutputAttributeData)
{
	CSSM_QUERY Query;

	memset (&Query, 0, sizeof (CSSM_QUERY));
	Query.RecordType = RecordType;
	Query.Conjunctive = CSSM_DB_NONE;
	Query.NumSelectionPredicates = 0;
	Query.SelectionPredicate = NULL;
	Query.QueryLimits.TimeLimit = CSSM_QUERY_TIMELIMIT_NONE;
	Query.QueryLimits.SizeLimit = CSSM_QUERY_SIZELIMIT_NONE;
	Query.QueryFlags = 0;

	return(mdsutil_GetRecord(hDLDB,
							 Query,
							 RecordType,
							 NumAttributes,
							 ResultsHandle,
							 OutputAttributeData));
}

BioAPI_RETURN BioAPI mdsutil_GetRecordByUuid(CSSM_DL_DB_HANDLE hDLDB,
											char* UuidStr,
											CSSM_DB_RECORDTYPE RecordType,
											uint32 NumAttributes,
											CSSM_HANDLE_PTR ResultsHandle,
											CSSM_DB_ATTRIBUTE_DATA *OutputAttributeData)
{
	BioAPI_RETURN retValue = CSSM_OK;
	CSSM_QUERY Query;
	CSSM_SELECTION_PREDICATE Predicate;

	memset (&Query, 0, sizeof (CSSM_QUERY));
	memset (&Predicate, 0, sizeof (CSSM_SELECTION_PREDICATE));
	Query.RecordType = RecordType;
	Query.Conjunctive = CSSM_DB_NONE;
	Query.NumSelectionPredicates = 1;
	Query.SelectionPredicate = &Predicate;
	Query.QueryLimits.TimeLimit = CSSM_QUERY_TIMELIMIT_NONE;
	Query.QueryLimits.SizeLimit = CSSM_QUERY_SIZELIMIT_NONE;
	Query.QueryFlags = 0;

	Predicate.DbOperator = CSSM_DB_EQUAL;

	/* NOTE: The first attribute *NEEDS* to be an index */
	Predicate.Attribute.Info = s_BioApiAttrInfo_ModuleId;

	Predicate.Attribute.Value = (CSSM_DATA *)BioAPI_malloc(sizeof (CSSM_DATA), NULL);
	if (Predicate.Attribute.Value == NULL)
		return CSSMERR_CSSM_MEMORY_ERROR;

	Predicate.Attribute.NumberOfValues = 1;
	Predicate.Attribute.Value->Data = (unsigned char *)UuidStr;
	Predicate.Attribute.Value->Length = (uint32)strlen (UuidStr) + 1;

	retValue = mdsutil_GetRecord(hDLDB,
								 Query,
								 RecordType,
								 NumAttributes,
								 ResultsHandle,
								 OutputAttributeData);

	BioAPI_free(Predicate.Attribute.Value, NULL);

	return retValue;
}

BioAPI_RETURN BioAPI mdsutil_GetRecordByUuidAndDeviceId(CSSM_DL_DB_HANDLE hDLDB,
												   char* UuidStr,
												   uint32 DeviceId,
												   CSSM_DB_RECORDTYPE RecordType,
												   uint32 NumAttributes,
												   CSSM_HANDLE_PTR ResultsHandle,
												   CSSM_DB_ATTRIBUTE_DATA *OutputAttributeData)
{
	BioAPI_RETURN retValue = CSSM_OK;
	CSSM_QUERY Query;
	CSSM_SELECTION_PREDICATE Predicate[2];

	memset (&Query, 0, sizeof (CSSM_QUERY));
	memset (&Predicate[0], 0, sizeof (CSSM_SELECTION_PREDICATE));
	memset (&Predicate[1], 0, sizeof (CSSM_SELECTION_PREDICATE));

	Query.RecordType = RecordType;
	Query.Conjunctive = CSSM_DB_AND;
	Query.NumSelectionPredicates = 2;

	Query.SelectionPredicate = Predicate;
	Query.QueryLimits.TimeLimit = CSSM_QUERY_TIMELIMIT_NONE;
	Query.QueryLimits.SizeLimit = CSSM_QUERY_SIZELIMIT_NONE;
	Query.QueryFlags = 0;

	Predicate[0].DbOperator = CSSM_DB_EQUAL;
	Predicate[0].Attribute.Info = s_BioApiAttrInfo_ModuleId;
	Predicate[0].Attribute.Value = (CSSM_DATA *) BioAPI_malloc(sizeof (CSSM_DATA), NULL);
	if (Predicate[0].Attribute.Value == NULL)
		return CSSM_ERRCODE_MEMORY_ERROR;
	Predicate[0].Attribute.NumberOfValues = 1;
	Predicate[0].Attribute.Value->Data = (unsigned char *)UuidStr;
	Predicate[0].Attribute.Value->Length = (uint32)strlen (UuidStr) + 1;

	Predicate[1].DbOperator = CSSM_DB_EQUAL;
	Predicate[1].Attribute.Info = s_BioApiAttrInfo_DeviceId;
	Predicate[1].Attribute.Value = (CSSM_DATA *) BioAPI_malloc(sizeof (CSSM_DATA), NULL);
	if (Predicate[1].Attribute.Value == NULL)
	{
		BioAPI_free(Predicate[0].Attribute.Value, NULL);
		return CSSM_ERRCODE_MEMORY_ERROR;
	}
	Predicate[1].Attribute.NumberOfValues = 1;
	Predicate[1].Attribute.Value->Data = (unsigned char *)&DeviceId;
	Predicate[1].Attribute.Value->Length = (uint32)sizeof(uint32);

	retValue = mdsutil_GetRecord(hDLDB,
								 Query,
								 RecordType,
								 NumAttributes,
								 ResultsHandle,
								 OutputAttributeData);

	BioAPI_free(Predicate[0].Attribute.Value, NULL);
	BioAPI_free(Predicate[1].Attribute.Value, NULL);

	return retValue;
}

BioAPI_RETURN BioAPI mdsutil_GetBioAPIRecord(CSSM_DL_DB_HANDLE hDLDB,
										  char* UuidStr,
										  bioapi_MDS_H_LAYER_PTR Record,
										  CSSM_DB_ATTRIBUTE_DATA_PTR HLayerAttributes)
{
	CSSM_DB_RECORD_ATTRIBUTE_DATA Attributes;
	CSSM_HANDLE ResultsHandle = 0;
	BioAPI_RETURN retValue = CSSM_OK;

	memset(&Attributes, 0, sizeof(CSSM_DB_RECORD_ATTRIBUTE_DATA));

	Attributes.AttributeData = HLayerAttributes;
	if ((retValue = h_layer_schema_GetAttributes (Record, &Attributes)) != CSSM_OK)
		return (retValue);

	if ((retValue = mdsutil_GetRecordByUuid(hDLDB,
											UuidStr,
											BIOAPI_H_LAYER_RECORDTYPE,
											BIOAPI_H_LAYER_NUM_ATTRIBUTES,
											&ResultsHandle,
											HLayerAttributes)) != CSSM_OK)
	{
		return (retValue);
	}

	MDSFuncs.DataAbortQuery( hDLDB, ResultsHandle );

	Record->moduleid = *HLayerAttributes[0].Value;
	Record->modulename = *HLayerAttributes[1].Value;
	Record->productversion = *HLayerAttributes[2].Value;
	Record->specversion = *HLayerAttributes[3].Value;
	Record->vendor = *HLayerAttributes[4].Value;
	Record->description = *HLayerAttributes[5].Value;

	return (retValue);
}


BioAPI_RETURN BioAPI mdsutil_GetBspRecord(CSSM_DL_DB_HANDLE hDLDB,
											 char* UuidStr,
											 bioapi_MDS_BSP_RECORD_PTR Record,
											 CSSM_DB_ATTRIBUTE_DATA_PTR BspAttributes)
{
	CSSM_DB_RECORD_ATTRIBUTE_DATA Attributes;
	CSSM_HANDLE ResultsHandle = 0;
	BioAPI_RETURN retValue = CSSM_OK;

	memset( &Attributes, 0, sizeof(CSSM_DB_RECORD_ATTRIBUTE_DATA));

	Attributes.AttributeData = BspAttributes;
	if ((retValue = bsp_schema_GetAttributes (Record, &Attributes)) != CSSM_OK)
		return (retValue);

	if ((retValue = mdsutil_GetRecordByUuid(hDLDB,
											UuidStr,
											BIOAPI_BSP_RECORDTYPE,
											BIOAPI_BSP_NUM_ATTRIBUTES,
											&ResultsHandle,
											BspAttributes)) != CSSM_OK)
	{
		return (retValue);
	}

	MDSFuncs.DataAbortQuery( hDLDB, ResultsHandle );

	Record->moduleid = *BspAttributes[0].Value;
	Record->deviceid = *BspAttributes[1].Value;
	Record->bspname = *BspAttributes[2].Value;
	Record->productversion = *BspAttributes[3].Value;
	Record->specversion = *BspAttributes[4].Value;
	Record->supportedformats = *BspAttributes[5].Value;
	Record->vendor = *BspAttributes[6].Value;
	Record->factorsmask = *BspAttributes[7].Value;
	Record->operations = *BspAttributes[8].Value;
	Record->options = *BspAttributes[9].Value;
	Record->payloadpolicy = *BspAttributes[10].Value;
	Record->maxpayloadsize = *BspAttributes[11].Value;
	Record->defaultverifytimeout = *BspAttributes[12].Value;
	Record->defaultidentifytimeout = *BspAttributes[13].Value;
	Record->defaultcapturetimeout = *BspAttributes[14].Value;
	Record->defaultenrolltimeout = *BspAttributes[15].Value;
	Record->maxbspdbsize = *BspAttributes[16].Value;
	Record->maxidentify = *BspAttributes[17].Value;
	Record->description = *BspAttributes[18].Value;
	Record->path = *BspAttributes[19].Value;

	return (retValue);
}


BioAPI_RETURN BioAPI mdsutil_DeleteRecordByUuid(CSSM_DL_DB_HANDLE hDLDB,
											   const BioAPI_UUID *uuid,
											   CSSM_DB_RECORDTYPE RecordType)
{
	CSSM_QUERY Query;
	CSSM_SELECTION_PREDICATE Predicate;
	CSSM_DB_UNIQUE_RECORD_PTR RecordId = NULL;
	BioAPI_RETURN retValue = CSSM_OK;
	CSSM_HANDLE ResultsHandle = 0;
	char UuidStr[BioAPI_PRINTABLE_UUID_LENGTH];

	memset (&Query, 0, sizeof (CSSM_QUERY));
	memset (&Predicate, 0, sizeof (CSSM_SELECTION_PREDICATE));
	Query.RecordType = RecordType;
	Query.Conjunctive = CSSM_DB_NONE;
	Query.NumSelectionPredicates = 1;
	Query.SelectionPredicate = &Predicate;
	Query.QueryLimits.TimeLimit = CSSM_QUERY_TIMELIMIT_NONE;
	Query.QueryLimits.SizeLimit = CSSM_QUERY_SIZELIMIT_NONE;
	Query.QueryFlags = 0;

	Predicate.DbOperator = CSSM_DB_EQUAL;

	/*
	 * Convert the Uuid to a string.
	 */
	BioAPI_GetPrintableUUID (uuid, UuidStr);

	Predicate.Attribute.Info = s_BioApiAttrInfo_ModuleId;
	Predicate.Attribute.Value = (CSSM_DATA *) BioAPI_malloc(sizeof (CSSM_DATA), 0);
	if (Predicate.Attribute.Value == NULL)
		return (CSSM_ERRCODE_MEMORY_ERROR);
	Predicate.Attribute.NumberOfValues = 1;
	Predicate.Attribute.Value->Data = (unsigned char *)UuidStr;
	Predicate.Attribute.Value->Length = (uint32)strlen (UuidStr) + 1;

	if( !IsBadCodePtr((CSSM_PROC_ADDR)MDSFuncs.DataGetFirst))
		retValue = MDSFuncs.DataGetFirst(hDLDB, /* DLDBHandle */
											 &Query,
											 &ResultsHandle,
											 NULL,
											 NULL,
											 &RecordId);

	while (CSSM_OK == retValue)
	{
		if( !IsBadCodePtr((CSSM_PROC_ADDR)MDSFuncs.DataDelete))
			retValue = MDSFuncs.DataDelete(hDLDB, RecordId);

		if( !IsBadCodePtr((CSSM_PROC_ADDR)MDSFuncs.FreeUniqueRecord))
			MDSFuncs.FreeUniqueRecord(hDLDB, RecordId);

		if( !IsBadCodePtr((CSSM_PROC_ADDR)MDSFuncs.DataGetNext))
			retValue = MDSFuncs.DataGetNext(hDLDB, /* DLDBHandle */
											ResultsHandle,
											NULL,
											NULL,
											&RecordId);
	}

	if (CSSMERR_DL_ENDOFDATA == retValue)
	{
		retValue = CSSM_OK;
	}

	MDSFuncs.DataAbortQuery( hDLDB, ResultsHandle );
	BioAPI_free(Predicate.Attribute.Value, NULL);
	return (retValue);
}


BioAPI_RETURN BioAPI mdsutil_GetHLayerCredentialInfo(char* UuidStr,
												 CSSM_DATA_PTR Credential,
												 CSSM_DATA_PTR ModuleName,
												 CSSM_DATA_PTR ModulePath)
{
	bioapi_MDS_H_LAYER Record;
	CSSM_DB_ATTRIBUTE_DATA HLayerAttributes[BIOAPI_H_LAYER_NUM_ATTRIBUTES];
	BioAPI_RETURN retValue = CSSM_OK;

	memset( &Record, 0, sizeof( bioapi_MDS_H_LAYER ));
	memset( &HLayerAttributes, 0, BIOAPI_H_LAYER_NUM_ATTRIBUTES * sizeof( CSSM_DB_ATTRIBUTE_DATA));

	if ((retValue = mdsutil_GetBioAPIRecord(hDLDBBioAPI,
											 UuidStr,
											 &Record,
											 HLayerAttributes)) != CSSM_OK)
	{
		return (retValue);
	}

	if (ModuleName)
	{
		if ((ModuleName->Data = (uint8 *)BioAPI_calloc (1, Record.modulename.Length, NULL)) == NULL)
		{
			mdsutil_FreeRecord (BIOAPI_H_LAYER_NUM_ATTRIBUTES, HLayerAttributes);
			return (CSSM_ERRCODE_MEMORY_ERROR);
		}

		memcpy (ModuleName->Data, Record.modulename.Data, Record.modulename.Length);
		ModuleName->Length = Record.modulename.Length;
	}

	if (ModulePath)
	{
		ModulePath->Data = NULL;
		ModulePath->Length = 0;
	}

	mdsutil_FreeRecord (BIOAPI_H_LAYER_NUM_ATTRIBUTES, HLayerAttributes);

	return (retValue);
}


BioAPI_RETURN BioAPI mdsutil_GetBspCredentialInfo(char* UuidStr,
												 CSSM_DATA_PTR Credential,
												 CSSM_DATA_PTR BspName,
												 CSSM_DATA_PTR BspPath)
{
	bioapi_MDS_BSP_RECORD Record;
	CSSM_DB_ATTRIBUTE_DATA BspAttributes[BIOAPI_BSP_NUM_ATTRIBUTES];
	BioAPI_RETURN retValue = CSSM_OK;

	memset( &Record, 0, sizeof( bioapi_MDS_BSP_RECORD ));
	memset( &BspAttributes, 0, BIOAPI_BSP_NUM_ATTRIBUTES * sizeof( CSSM_DB_ATTRIBUTE_DATA));

	if ((retValue = mdsutil_GetBspRecord(hDLDBBioAPI,
											 UuidStr,
											 &Record,
											 BspAttributes)) != CSSM_OK)
	{
		return (retValue);
	}

	if (BspName)
	{
		if ((BspName->Data = (uint8 *)BioAPI_calloc (1, Record.bspname.Length, NULL)) == NULL)
		{
			mdsutil_FreeRecord (BIOAPI_BSP_NUM_ATTRIBUTES, BspAttributes);
			return (CSSM_ERRCODE_MEMORY_ERROR);
		}

		memcpy (BspName->Data, Record.bspname.Data, Record.bspname.Length);
		BspName->Length = Record.bspname.Length;
	}

	if (BspPath)
	{
		if ((BspPath->Data = (uint8 *)BioAPI_calloc (1, Record.path.Length, NULL)) == NULL)
		{
			mdsutil_FreeRecord (BIOAPI_BSP_NUM_ATTRIBUTES, BspAttributes);
			return (CSSM_ERRCODE_MEMORY_ERROR);
		}

		memcpy (BspPath->Data, Record.path.Data, Record.path.Length);
		BspPath->Length = Record.path.Length;
	}

	mdsutil_FreeRecord (BIOAPI_BSP_NUM_ATTRIBUTES, BspAttributes);

	return (retValue);
}


BioAPI_RETURN BioAPI mdsutil_GetModuleCredentialInfo(const BioAPI_UUID *uuid,
													CSSM_DATA_PTR Credential,
													CSSM_DATA_PTR ModuleName,
													CSSM_DATA_PTR ModulePath)
{
	char UuidStr[BioAPI_PRINTABLE_UUID_LENGTH];

	/*
	 * Convert the Uuid to a string.
	 */
	BioAPI_GetPrintableUUID(uuid, UuidStr);

	if (mdsutil_GetHLayerCredentialInfo(UuidStr,
										 Credential,
										 ModuleName,
										 ModulePath) == BioAPI_OK)
	{
		return (BioAPI_OK);
	}

	return (mdsutil_GetBspCredentialInfo(UuidStr,
										 Credential,
										 ModuleName,
										 ModulePath));
}


BioAPI_RETURN BioAPI
mdsutil_GetModulePath (CSSM_DATA ModuleName, CSSM_DATA ModuleSearchPath, CSSM_DATA_PTR ModulePath)
{
	uint8 *SearchPtr;
	uint8 *ptr;
	uint32 len;
	CSSM_DATA tmpPath;
	struct _stat st;

	// Check for blank search path; just use the system search path
	if (ModuleSearchPath.Length == 0)
	{
		if ((ModulePath->Data = (uint8*)BioAPI_malloc (ModuleName.Length, NULL)) == NULL)
			return (CSSM_ERRCODE_MEMORY_ERROR);
		strcpy ((char*) ModulePath->Data, (char*) ModuleName.Data);
		ModulePath->Length = ModuleName.Length;
		return BioAPI_OK;
	} // if

	len = ModuleSearchPath.Length + ModuleName.Length + 2;
	if ((ptr = (uint8*)BioAPI_malloc (len, NULL)) == NULL)
		return (CSSM_ERRCODE_MEMORY_ERROR);

	SearchPtr = ModuleSearchPath.Data;
	tmpPath.Data = ModuleSearchPath.Data;
	tmpPath.Length = ModuleSearchPath.Length;

	while (SearchPtr) {
		SearchPtr = memchr (tmpPath.Data, PATH_SEPARATOR, tmpPath.Length);
		if (SearchPtr != NULL) {
			tmpPath.Length = (uint32)(SearchPtr - tmpPath.Data);
			SearchPtr++;			/* Skip the PATH_SEPARATOR */
		} else {
			/*
			 * get rid of the null character at the end of search path, if any.
			 */
			if (tmpPath.Data[tmpPath.Length-1] == '\0')
				tmpPath.Length--;
		}

		ModulePath->Data = ptr;
		memcpy(ptr, tmpPath.Data, tmpPath.Length);
		ptr = ptr + tmpPath.Length;
		*ptr++ = DIRECTORY_SEPARATOR;
		memcpy(ptr, ModuleName.Data, ModuleName.Length);		/* copy relative path */
		ptr = ptr + ModuleName.Length;					/* move to end of string */
		*ptr = '\0';									/* null terminate */
		ModulePath->Length = (uint32)(ptr - ModulePath->Data);
		/*
		 * stat the file and find out if we have found it.
		 * If yes, then return else continue searching the
		 * search path.
		 */
		if ((_stat((const char *)ModulePath->Data, &st)) == 0)
			return (CSSM_OK);

		tmpPath.Data = SearchPtr;
		if(SearchPtr != NULL)
			tmpPath.Length = (uint32)(ModuleSearchPath.Length - (uint32)(SearchPtr - ModuleSearchPath.Data));
	}

	return (CSSM_ERRCODE_MDS_ERROR);
}
