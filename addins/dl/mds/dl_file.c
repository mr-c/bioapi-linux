/*-----------------------------------------------------------------------
 * File: DL_FILE.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains code to read and add schema to the dl files.
 *
 * NOTE: If the ability to update the file is requested, then most
 * of this code would need to be relooked at because of concurency issues
 */

#include <assert.h>
#if defined (UNIX)
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <io.h>
#endif
#include "dlutil.h"
#include "dlutil_reg.h"
#include "string.h"
#include "ff_port.h"
#include "bioapi_util.h"


/*-----------------------------------------------------------------------------
 * Name: dl_schemaFileAddDb
 *
 * Description:
 * Adds a database to the DL's schema file
 *
 * Parameters:
 * szModuleGuid (input)				- the GUID for the DL calling this function
 * DbName (input)					- Name of the database being CREATED
 *									  (not updated)
 * pDbInfo (input)					- The DB INFO strucuture of the Db
 * pAdditionalInformation (input)	- DL specific information that the
 *									  DL wishes to associate with this DB
 *
 * Return value:
 * CSSM_OK if no error occurred
 *
 * Error Codes:
 * CSSMERR_DL_INVALID_DB_NAME
 * CSSMERR_DL_MEMORY_ERROR
 *
 * NOTE: The caller must take steps to ensure that this call is not
 * concurrently called (either in the same or another process) for the same
 * database. In otherwords, the caller must ensure that the same database is
 * not concurently created.
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_schemaFileAddDb(const char* szModuleGuid,
							   const char* DbName,
							   const CSSM_DBINFO* pDbInfo,
							   const CSSM_DATA* pAdditionalInformation)
{
	CSSM_RETURN ret = CSSM_OK;
	char Label[CSSM_MAX_REG];
	char* schemaPath = NULL;

	assert(szModuleGuid);
	assert(DbName);
	assert(pDbInfo);

	/* Zero length DbNames are illegal, so they can't exist */
	if (!DbName[0])
	{
		return (CSSMERR_DL_INVALID_DB_NAME);
	}

	dlnr_static_dlregGenerateKeyName(Label, szModuleGuid, DbName);

	if (pAdditionalInformation && pAdditionalInformation->Length)
	{
		/* Write the additional information to the registry */
		assert(pAdditionalInformation->Data);
		if ((ret = dlutil_port_SetValue(Label,
										"AdditionalInformation",
										(void *)pAdditionalInformation->Data,
										pAdditionalInformation->Length,
										DLUTIL_VALUE_BINARY,
										REGISTRY_PREFERENCE_GLOBAL))
										!= CSSM_OK)
		{
			return (ret);
		}

		schemaPath = BioAPI_calloc(pAdditionalInformation->Length + 13, 1, NULL);
		strncpy(schemaPath,
				(const char*)pAdditionalInformation->Data,
				pAdditionalInformation->Length);

		/* create the schema path leading to the .ffdb file */
		strncat (schemaPath, ".schema.ffdb", 12);
	}
	else
	{
		char* pathStart = NULL;
		uint32 DirLen, DbLen,PathLen;

		/* Since no path is specified, we will try to put the Db 
		 * in the current directory 
		 */
		DirLen = GetCurrentDirectory(0, NULL);
		DbLen = strlen(DbName);
    
		if (0 == DirLen)
		{
			return (CSSMERR_DL_INTERNAL_ERROR);
		}
		else if (DirLen + DirLen < MAX_PATH_START_LENGTH)
		{
			/* Get the current directory name */
			GetCurrentDirectory(MAX_PATH_START_SIZE, pathStart);
			ASSERT(strlen(pathStart) + 1 == DirLen);
		}
		else
		{
			/* the current directory is too long, put it in c:\ */
			strcpy(pathStart, "c:");
			DirLen = 3;
		}

		pathStart[DirLen-1] = DIRECTORY_SEPARATOR;
		memcpy(&pathStart[DirLen], DbName, DbLen);
		pathStart[DirLen+DbLen] = 0;
		PathLen = DirLen+DbLen;

		ASSERT(DirLen+DbLen <= MAX_PATH_START_LENGTH);

		if ((ret = dlutil_port_SetValue(Label,
										"AdditionalInformation",
                                        (void *)pathStart,
                                        PathLen,
										DLUTIL_VALUE_BINARY,
										REGISTRY_PREFERENCE_GLOBAL))
										!= CSSM_OK)
		{
			return (ret);
		}

        schemaPath = BioAPI_calloc(PathLen + 13, 1, NULL);
        strncpy(schemaPath, 
                pathStart,
                PathLen);

		/* create the schema path leading to the .ffdb file */
		strncat (schemaPath, ".schema.ffdb", 12);
	}

	if ((ret = dl_WriteDLDataStoreInfo(schemaPath,
									   (CSSM_DBINFO_PTR) pDbInfo)) != CSSM_OK)
	{
		/* delete anything that was put there */
		dl_schemaFileDeleteDb(szModuleGuid, DbName);
		BioAPI_free(schemaPath, NULL);
		return (ret);
	}

	BioAPI_free(schemaPath, NULL);
	return (ret);
}

/*-----------------------------------------------------------------------------
 * Name: dl_schemaFileReadDb
 *
 * Description:
 * Reads information about a database from the DL's schema file
 *
 * Parameters:
 * szModuleGuid (input):							The GUID for the DL calling
 *													this function
 * DbName (input):									Name of the database being
 *													CREATED (not updated)
 * pDbInfo (output/optional):						Pointer to a buffer to be
 *													filled with the DbInfo
 *													structure
 * pAdditionalInformationLength (output/optional):	On input, the length of the
 *													pAdditionalInformationData
 *													buffer. On output, the
 *													amount of the buffer used
 * pAdditionalInformationData (output/optional):	Pointer to the buffer to be
 *													filled with additional
 *													information
 *
 * Return value:
 * CSSM_OK if no error occurred
 *
 * Error Codes:
 * CSSMERR_DL_
 *
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_schemaFileReadDb(const char* szModuleGuid,
								const char* DbName,
								CSSM_DBINFO* pDbInfo,
								uint32 * pAdditionalInformationLength,
								uint8* pAdditionalInformationData)
{
	char Label[CSSM_MAX_REG];
	CSSM_RETURN ret = CSSM_OK;
	char* schemaPath = NULL;
	sint32 ValueType = 0;
	sint32 schemaLength = 0;

	assert(szModuleGuid);
	assert(DbName);

	dlnr_static_dlregGenerateKeyName(Label, szModuleGuid, DbName);

	if (pAdditionalInformationLength && *pAdditionalInformationLength &&
		pAdditionalInformationData)
	{
		/* Read the additional information from the registry */
		if ((ret = dlutil_port_GetValue(Label,
										"AdditionalInformation",
										(char *)pAdditionalInformationData,
										(sint32 *)pAdditionalInformationLength,
										&ValueType,
										DLUTIL_CONFIG_GLOBAL)) != CSSM_OK)
		{
			return (ret);
		}

		if (ValueType != DLUTIL_VALUE_BINARY || (*(sint32 *)pAdditionalInformationLength) < 0)
		{
			*pAdditionalInformationLength = 0;
			return (CSSMERR_DL_INTERNAL_ERROR);
		}

		schemaPath = BioAPI_calloc(*pAdditionalInformationLength + 13, 1, NULL);
		strncpy(schemaPath, (const char*)pAdditionalInformationData, *pAdditionalInformationLength);

		/* create the schemaPath for reading the schema file */
		strcat(schemaPath, ".schema.ffdb");
	}
	/* else ignore the pAdditionalInformation block */
	else
	{
		if ((ret = dlutil_port_GetValue(Label,
										"AdditionalInformation",
										schemaPath,
										&schemaLength,
										&ValueType,
										DLUTIL_CONFIG_GLOBAL)) != CSSM_OK)
		{
			return (ret);
		}

		/* create the schemaPath for reading the schema file */
		strcat(schemaPath, ".schema.ffdb");
	}

	if (pDbInfo && ((ret = dl_ReadDLDataStoreInfo(schemaPath,
												  pDbInfo)) != CSSM_OK))
	{
		BioAPI_free(schemaPath, NULL);
		return (ret);
	}

	BioAPI_free(schemaPath, NULL);
	return (ret);
}

/*-----------------------------------------------------------------------------
 * Name: dl_schemaFileDeleteDb
 *
 * Description:
 * Deletes the database entries from the schema file
 *
 * Parameters:
 * schemaPath	- the path to the schema file
 *
 * Return value:
 * CSSM_OK if no error occurred
 *
 * Error Codes:
 * CSSMERR_DL_REGISTRY_ERROR
 * CSSMERR_DL_MEMORY_ERROR
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_schemaFileDeleteDb(const char * szModuleGuid,
								  const char * DbName)
{
	CSSM_RETURN ret = CSSM_OK;
	char Label[CSSM_MAX_REG];
	char* schemaPath = NULL;
	sint32 schemaLength = 0, ValueType = 0;

	assert(szModuleGuid);
	assert(DbName);

	dlnr_static_dlregGenerateKeyName(Label, szModuleGuid, DbName);

	if ((ret = dlutil_port_GetValue(Label,
									"AdditionalInformation",
									schemaPath,
									&schemaLength,
									&ValueType,
									DLUTIL_CONFIG_GLOBAL)) != CSSM_OK)
	{
		return (ret);
	}

	strcat(schemaPath, ".schema.ffdb");

	/* delete the schema file */
	_chmod(schemaPath, _S_IREAD | _S_IWRITE);
	if (DeleteFile(schemaPath) == 0)
	{
		return (CSSMERR_DL_OS_ACCESS_DENIED);
	}

	return (ret);
}
