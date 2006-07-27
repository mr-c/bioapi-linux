/*-----------------------------------------------------------------------
 * File: DL_REG.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains code to read and add to the dl's registry.
 *
 * NOTE: If the ability to update the registry is requested, then most
 * of this code would need to be relooked at because of concurency issues
 */

#include <assert.h>
#include "dlutil.h"
#include "dlutil_reg.h"
#include "port/bioapi_port.h"
#include "bioapi_util.h"

#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
#define DL_SECTION "Software/BioAPI/DL/"
#else
#define DL_SECTION "Software\\BioAPI\\DL\\"
#endif

/* Declaration of member functions and macros */
void dlnr_static_dlregGenerateKeyName(char* szBuffer,
									  const char* szModuleGuid,
									  const char* DbName);

/*-----------------------------------------------------------------------------
 * Name: dl_dlregDoesDbExist
 *
 * Description:
 * Determines if a Db is in the registry.
 *
 * Parameters:
 * szModuleGuid (input) - the GUID for the DL calling this function
 * DbName (input)		- Name of the database being searched for
 *
 * Return value:
 * CSSM_FALSE if db does not exists, else CSSM_TRUE
 *
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED
 *---------------------------------------------------------------------------*/
CSSM_BOOL dl_dlregDoesDbExist(const char* szModuleGuid,
							  const char* DbName)
{
	char Label[CSSM_MAX_REG];
	char szSection[CSSM_MAX_REG];
	sint32 Count;
	uint32 i;
	uint32 IsOkay = 0;
	CSSM_BOOL FoundDb = CSSM_FALSE;
	CSSM_NAME_LIST_PTR pDbNamesList;
	CSSM_RETURN ret = CSSM_OK;

	assert(szModuleGuid);
	assert(DbName);

	/* Zero length DbNames are illegal in the registry, so they can't exist */
	if (!DbName[0])
	{
		return (CSSM_FALSE);
	}

	dlnr_static_dlregGenerateKeyName(Label, szModuleGuid, DbName);

	if (((ret = dlutil_port_GetValueCount(Label,
										  &Count,
										  REGISTRY_PREFERENCE_GLOBAL))
										  != CSSM_OK)
		|| Count <= 0)
	{
		return (CSSM_FALSE);
	}

	sprintf(szSection, "%s%s", DL_SECTION, szModuleGuid);

	pDbNamesList = (CSSM_NAME_LIST_PTR) local_calloc(sizeof(CSSM_NAME_LIST), 1, NULL);

	/* Fill in pDbNamesList->String */
	if ((ret = dlutil_port_GetSectionList(szSection,
										  &pDbNamesList->String,
										  REGISTRY_PREFERENCE_GLOBAL))
										  != CSSM_OK)
	{
		pDbNamesList->NumStrings = 0;
		pDbNamesList->String = NULL;
		return (CSSM_FALSE);
	}

	for (i = 0; pDbNamesList->String[i]; i++)
	{
		if (strncmp(pDbNamesList->String[i],
					DbName,
					sizeof(DbName)) == 0)
		{
			FoundDb = CSSM_TRUE;
		}
	}

	pDbNamesList->NumStrings = i;

	/* Free the pDbNamesList->String Structure */
	if (pDbNamesList->NumStrings)
	{
		IsOkay = pDbNamesList->String &&
				 !IsBadReadPtr(pDbNamesList->String,
							   sizeof(char *) * pDbNamesList->NumStrings);

		if (IsOkay)
		{
			for (i = 0; i < pDbNamesList->NumStrings; i++)
			{
				if (!pDbNamesList->String[i] ||
					IsBadReadPtr(pDbNamesList->String[i], 1))
				{
					IsOkay = 0;
				}
				else
				{
					local_free(pDbNamesList->String[i], NULL);
					pDbNamesList->String[i] = NULL;
				}
			}

			local_free(pDbNamesList->String, NULL);
			pDbNamesList->String = NULL;
		}
	}
	else
	{
		local_free(pDbNamesList->String, NULL);
		pDbNamesList->String = NULL;
	}

	local_free(pDbNamesList, NULL);
	pDbNamesList = NULL;

	if ((IsOkay != 0) && (FoundDb != CSSM_FALSE))
		return(CSSM_TRUE);
	else
		return(CSSM_FALSE);
}

/*-----------------------------------------------------------------------------
 * Name: dl_dlregDeleteDb
 *
 * Description:
 * Deletes a database from the registry
 *
 * Parameters:
 * szModuleGuid (input) - the GUID for the DL calling this function
 * DbName (input)		- Name of the database being CREATED (not updated)
 *
 * Return value:
 * CSSM_OK if no error occurred
 *
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_dlregDeleteDb(const char* szModuleGuid,
							 const char* DbName)
{
	char Label[CSSM_MAX_REG];
	CSSM_RETURN ret = CSSM_OK;

	assert(szModuleGuid);
	assert(DbName);

	dlnr_static_dlregGenerateKeyName(Label, szModuleGuid, DbName);

	if ((ret = dlutil_port_DeleteSection(Label,
										 REGISTRY_PREFERENCE_GLOBAL))
										 != CSSM_OK)
	{
		return (ret);
	}

	return (ret);
}

/*-----------------------------------------------------------------------------
 * Name: dl_dlregReadDbNames
 *
 * Description:
 * Reads the list of DbNames from the DL registry and returns them
 *
 * Parameters:
 * szModuleGuid (input)		- The GUID for the DL calling this function
 * pNameList (Input/Output) - Address of NameList structure to be filled with
 *							  the DbNames installed
 *
 * Return value:
 * CSSM_OK if no error occurred
 *
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED
 * CSSMERR_CSSM_MEMORY_ERROR
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_dlregReadDbNames(const char* szModuleGuid,
								CSSM_NAME_LIST_PTR pNameList)
{
	char szSection[CSSM_MAX_REG];
	uint32 i;
	CSSM_RETURN ret = CSSM_OK;

	assert(szModuleGuid);
	assert(pNameList);

	sprintf(szSection, "%s%s", DL_SECTION, szModuleGuid);

	/* Fill in pNameList->String */
	if ((ret = dlutil_port_GetSectionList(szSection,
										  &pNameList->String,
										  REGISTRY_PREFERENCE_GLOBAL))
										  != CSSM_OK)
	{
		pNameList->NumStrings = 0;
		pNameList->String = NULL;

		return (CSSM_OK);
	}

	/* Count the number of strings (the last string has a value of NULL) */
	for (i = 0; pNameList->String[i]; i++);

	/* Fill in pNameList->NumStrings */
	pNameList->NumStrings = i;

	if (pNameList->NumStrings == 0)
	{
		BioAPI_free(pNameList->String, NULL);
		pNameList->String = NULL;
	}

	return (ret);
}

/*-----------------------------------------------------------------------------
 * Name: dlnr_static_dlregGenerateKeyName
 *
 * Description:
 * Creates the section name where DbName is stored in the Dl registry
 *
 * Parameters:
 * szBuffer (input/output)	- Buffer to be filled with the derived section name
 * szModuleGuid (input)		- GUID of the module calling this function
 * DbName (input)			- Name of the Db who's path we are creating
 *
 *
 * Return value:
 * None.
 *
 * Error Codes:
 * None.
 *---------------------------------------------------------------------------*/
void dlnr_static_dlregGenerateKeyName(char* szBuffer,
									  const char* szModuleGuid,
									  const char* DbName)
{
	assert(szBuffer);
	assert(szModuleGuid);
	assert(DbName);

    sprintf(szBuffer, "%s%s%c%s", DL_SECTION, szModuleGuid, DIRECTORY_SEPARATOR, DbName);
}

/*-----------------------------------------------------------------------------
 * Name: dlnr_FreeDbInfo
 *
 * Description:
 * Free the DbInfo that was returned from dl_dlregReadDb, dl_cssmregReadDb
 *
 * Parameters:
 * pDbInfo (input)	- DbInfo to free
 *
 * Return value:
 * CSSM_OK if no error occurred
 *
 * Error Codes:
 * CSSMERR_CSSM_INVALID_POINTER
 *---------------------------------------------------------------------------*/
void dlnr_FreeDbInfo(CSSM_DBINFO *pDbInfo)
{
#ifdef _DEBUG
	if (pDbInfo)
	{
		assert(CSSM_OK == cssm_FreeDbInfo(pDbInfo));
	}
#else
	if (pDbInfo)
	{
		cssm_FreeDbInfo(pDbInfo);
	}
#endif
}
