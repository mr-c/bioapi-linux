/*-----------------------------------------------------------------------
 * File: DL_MEMUTIL.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the free memory routines for dbinfo.
 */

#include "dlutil.h"
#include "bioapi_util.h"


/*---------------------------------------------------------------
 *
 *Name: cssm_FreeDBAttributeInfo
 *
 *Description:
 *	 Free memory associated with the DB_ATTRIBUTE structure
 *
 *Parameters:
 *	 Info (input) - pointer to DB_ATTRIBUTE Info
 *
 *Returns:
 *	 CSSM_FAIL - unable to free
 *	 CSSM_OK - free memory
 *
 *-------------------------------------------------------------*/
void cssm_FreeDbAttributeInfo(CSSM_DB_ATTRIBUTE_INFO_PTR Info)
{
	if( Info != NULL)
	{
		if (Info->AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_STRING)
		{
			if (Info->Label.AttributeName != NULL)
			{
				BioAPI_free(Info->Label.AttributeName, NULL);
				Info->Label.AttributeName = NULL;
			}
		}
		else if (Info->AttributeNameFormat == CSSM_DB_ATTRIBUTE_NAME_AS_OID)
		{
			if (Info->Label.AttributeOID.Data != NULL)
			{
				BioAPI_free(Info->Label.AttributeOID.Data, NULL);
				Info->Label.AttributeOID.Data = NULL;
			}
		}
	}
}

/*---------------------------------------------------------------
 *
 *Name: cssm_FreeDBInfo
 *
 *Description:
 *	 Free memory associated with the DBINFO structure
 *
 *Parameters:
 *	 Info (input) - pointer to DB Info
 *
 *Returns:
 *	 CSSM_FAIL - unable to free
 *	 CSSM_OK - free memory
 *
 *-------------------------------------------------------------*/
CSSM_RETURN cssm_FreeDbInfo (CSSM_DBINFO_PTR Info)
{
	uint32 i;

	if( Info != NULL)
	{
		if (port_IsBadReadPtr (Info->RecordAttributeNames, sizeof (CSSM_DB_RECORD_ATTRIBUTE_INFO)
				* Info->NumberOfRecordTypes))
		{
			return(CSSMERR_CSSM_INVALID_POINTER);
		}

		if (port_IsBadReadPtr (Info->RecordIndexes, sizeof (CSSM_DB_RECORD_INDEX_INFO)
				* Info->NumberOfRecordTypes))
		{
			return(CSSMERR_CSSM_INVALID_POINTER);
		}

		/* free all record types */
		for (i = 0; i < Info->NumberOfRecordTypes; i++)
		{
			uint32 j;
			if (Info->RecordAttributeNames[i].AttributeInfo)
			{
				if (port_IsBadReadPtr (Info->RecordAttributeNames[i].AttributeInfo, sizeof
						(CSSM_DB_ATTRIBUTE_INFO) * Info->RecordAttributeNames[i].NumberOfAttributes))
				{
					return(CSSMERR_CSSM_INVALID_POINTER);
				}

				for (j = 0; j < Info->RecordAttributeNames[i].NumberOfAttributes; j++)
				{
					cssm_FreeDbAttributeInfo(&Info->RecordAttributeNames[i].AttributeInfo[j]);
				}
				BioAPI_free (Info->RecordAttributeNames[i].AttributeInfo, NULL);
				Info->RecordAttributeNames[i].AttributeInfo = NULL;
			}

			if (Info->RecordIndexes[i].IndexInfo)
			{
				if (port_IsBadReadPtr (Info->RecordIndexes[i].IndexInfo, sizeof
						(CSSM_DB_INDEX_INFO) * Info->RecordIndexes[i].NumberOfIndexes))
				{
					return(CSSMERR_CSSM_INVALID_POINTER);
				}

				for (j = 0; j < Info->RecordIndexes[i].NumberOfIndexes; j++)
				{
					cssm_FreeDbAttributeInfo(&Info->RecordIndexes[i].IndexInfo[j].Info);
				}

				BioAPI_free (Info->RecordIndexes[i].IndexInfo, NULL);
				Info->RecordIndexes[i].IndexInfo = NULL;
			}
		}

		if (Info->DefaultParsingModules)
		{
			BioAPI_free (Info->DefaultParsingModules, NULL);
			Info->DefaultParsingModules = NULL;
		}
		if (Info->RecordAttributeNames)
		{
			BioAPI_free (Info->RecordAttributeNames, NULL);
			Info->RecordAttributeNames = NULL;
		}
		if (Info->RecordIndexes)
		{
			BioAPI_free (Info->RecordIndexes, NULL);
			Info->RecordIndexes = NULL;
		}
		if (Info->AccessPath)
		{
			BioAPI_free (Info->AccessPath, NULL);
			Info->AccessPath = NULL;

		}
		if (Info->Reserved && ((CSSM_DATA_PTR)Info->Reserved)->Data)
		{
			BioAPI_free (((CSSM_DATA_PTR)Info->Reserved)->Data, NULL);
		}
	}
	return(CSSM_OK);
}


CSSM_RETURN freeDbInfoRecord (CSSM_DBINFO_PTR Info, uint32 index)
{
	uint32 j;

	if (Info->RecordAttributeNames[index].AttributeInfo)
	{
		if (port_IsBadReadPtr (Info->RecordAttributeNames[index].AttributeInfo, sizeof
				(CSSM_DB_ATTRIBUTE_INFO) * Info->RecordAttributeNames[index].NumberOfAttributes))
		{
			return (CSSMERR_CSSM_INVALID_POINTER);
		}

		for (j = 0; j < Info->RecordAttributeNames[index].NumberOfAttributes; j++)
		{
			cssm_FreeDbAttributeInfo(&Info->RecordAttributeNames[index].AttributeInfo[j]);
		}
		BioAPI_free (Info->RecordAttributeNames[index].AttributeInfo, NULL);
	}

	if (Info->RecordIndexes[index].IndexInfo)
	{
		if (port_IsBadReadPtr (Info->RecordIndexes[index].IndexInfo, sizeof
				(CSSM_DB_INDEX_INFO) * Info->RecordIndexes[index].NumberOfIndexes))
		{
			return (CSSMERR_CSSM_INVALID_POINTER);
		}

		for (j = 0; j < Info->RecordIndexes[index].NumberOfIndexes; j++)
		{
			cssm_FreeDbAttributeInfo(&Info->RecordIndexes[index].IndexInfo[j].Info);
		}

		BioAPI_free (Info->RecordIndexes[index].IndexInfo, NULL);

	}

	return CSSM_OK;
}


