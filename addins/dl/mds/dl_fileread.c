/*-----------------------------------------------------------------------
 * File: DL_FILEREAD.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the functions that read the data contained in the dl
 * schema file.
 */

#include "dlutil.h"
#include "ff_port.h"
#include "bioapi_util.h"


/*---------------------------------------------------------------
 *
 *Name: dl_ReadDataStruct
 *
 *Description:
 *	 Generic function to read a CSSM_DATA structure from the schema file
 *
 *Parameters:
 *	 buf_ptr (input)	- pointer to the structure in the schema file
 *	 Data (output)		- pointer to the CSSM_DATA structure
 *
 *Returns:
 *	 CSSM_OK - data is read from the file
 *-------------------------------------------------------------*/
CSSM_RETURN dl_ReadDataStruct(void *buf_ptr,
							  CSSM_DATA_PTR Data)
{
	CSSM_RETURN ret = CSSM_OK;
	sint32 i, Len;

	if ((buf_ptr == NULL) || (Data == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	Len = sizeof (Data->Length);
	if ((ret = port_fread((void *)&i,
						   Len,
						   1,
						   buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	if ((Data->Data = (unsigned char *) BioAPI_calloc (i, 1, NULL)) == NULL)
	{
		BioAPI_free (Data, NULL);
		return (CSSMERR_DL_MEMORY_ERROR);
	}

	Data->Length = i;
	if ((ret = port_fread((void *)Data->Data,
						  Data->Length,
						  1,
						  buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	return (CSSM_OK);
}

/*---------------------------------------------------------------
 *
 *Name: dl_ReadDLDBRecordAttributeInfo
 *
 *Description:
 *	   Read the DL's reocrd attribute information from
 *	   the schema file.
 *
 *Parameters:
 *	   buf_ptr (input)	- the ptr pointing to the addin
 *	   Info (output)	- pointer to data that describes reocrd attribute
 *						  information
 *
 *Returns:
 *	   CSSM_OK - data is returned in the parameter list
 *-------------------------------------------------------------*/
CSSM_RETURN dl_ReadDLDBAttributeInfo(void *buf_ptr,
									 CSSM_DB_ATTRIBUTE_INFO_PTR Info)
{
	CSSM_RETURN ret = CSSM_OK;
	sint32 Len = 0;

	if ((buf_ptr == NULL) || (Info == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	if ((ret = port_fread(&Info->AttributeNameFormat,
						  4,
						  1,
						  buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	switch(Info->AttributeNameFormat)
	{
		case CSSM_DB_ATTRIBUTE_NAME_AS_STRING:
		{
			/* get the length to read */
			if ((ret = port_fread(&Len,
								  4,
								  1,
								  buf_ptr)) != CSSM_OK)
			{
				return (ret);
			}

			if (Len == 0)
			{
				return (CSSMERR_DL_INTERNAL_ERROR);
			}

			if ((Info->Label.AttributeName = (char *) BioAPI_calloc(Len, 1, NULL)) == NULL)
			{
				return (CSSMERR_DL_MEMORY_ERROR);
			}

			/* read till the required length */
			if ((ret = port_fread(Info->Label.AttributeName,
								  Len,
								  1,
								  buf_ptr)) != CSSM_OK)
			{
				return (ret);
			}
		}
		break;

		case CSSM_DB_ATTRIBUTE_NAME_AS_OID:
		{
			if((ret = dl_ReadDataStruct(buf_ptr,
										&Info->Label.AttributeOID)) != CSSM_OK)
			{
				return (ret);
			}
		}
		break;

		case CSSM_DB_ATTRIBUTE_NAME_AS_INTEGER:
		{
			if((ret = port_fread(&Info->Label.AttributeID,
								 sizeof(uint32),
								 1,
								 buf_ptr)) != CSSM_OK)
			{
				return (ret);
			}
		}
		break;
	}

	if ((ret = port_fread(&Info->AttributeFormat,
						  4,
						  1,
						  buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	return (CSSM_OK);
}

/*---------------------------------------------------------------
 *
 *Name: dl_ReadDLDBRecordAttribute
 *
 *Description:
 *	   Read the DL's record attribute from the schema file.
 *
 *Parameters:
 *	   buf_ptr (input)	- the ptr to the location in the file that
 *						  identifies the addin
 *	   Info (output)	- pointer to data that describes reocrd attribute
 *
 *Returns:
 *	   CSSM_OK - data is return in parameter list
 *-------------------------------------------------------------*/
CSSM_RETURN dl_ReadDLDBRecordAttribute(void *buf_ptr,
									   CSSM_DB_RECORD_ATTRIBUTE_INFO_PTR Info)
{
	CSSM_RETURN ret = CSSM_OK;
	uint32 i;

	if ((buf_ptr == NULL) || (Info == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	if ((ret = port_fread(&Info->DataRecordType,
						  4,
						  1,
						  buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	if ((ret = port_fread(&Info->NumberOfAttributes,
						  4,
						  1,
						  buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	if (Info->NumberOfAttributes > 0)
	{
		Info->AttributeInfo = (CSSM_DB_ATTRIBUTE_INFO_PTR)
			BioAPI_calloc(Info->NumberOfAttributes * sizeof(CSSM_DB_ATTRIBUTE_INFO), 1, NULL);

		if (Info->AttributeInfo == NULL)
		{
			return (CSSMERR_DL_MEMORY_ERROR);
		}

		for (i = 0; i < Info->NumberOfAttributes; i++)
		{
			if ((ret = dl_ReadDLDBAttributeInfo(buf_ptr,
												&Info->AttributeInfo[i]))
												!= CSSM_OK)
			{
				BioAPI_free(Info->AttributeInfo, NULL);
				return (ret);
			}
		}
	}
	else
		Info->AttributeInfo = NULL;

	return (CSSM_OK);
}

/*---------------------------------------------------------------
 *
 *Name: dl_ReadDLDBRecordIndex
 *
 *Description:
 *	   Read the DL's reocrd index information from
 *	   the schema file.
 *
 *Parameters:
 *	   buf_ptr (input)	- the pointer to the addin in the file
 *	   Info (output)	- pointer to data that describes reocrd index
 *
 *Returns:
 *	   CSSM_OK - data is returned in the parameter list
 *-------------------------------------------------------------*/
CSSM_RETURN dl_ReadDLDBRecordIndex(void *buf_ptr,
								   CSSM_DB_RECORD_INDEX_INFO_PTR Info)
{
	CSSM_RETURN ret = CSSM_OK;
	uint32 i;

	if ((buf_ptr == NULL) || (Info == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	if ((ret = port_fread(&Info->DataRecordType,
						  4,
						  1,
						  buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	if ((ret = port_fread(&Info->NumberOfIndexes,
						  4,
						  1,
						  buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	if (Info->NumberOfIndexes > 0)
	{
		Info->IndexInfo = (CSSM_DB_INDEX_INFO_PTR)
			BioAPI_calloc(Info->NumberOfIndexes * sizeof(CSSM_DB_INDEX_INFO), 1, NULL);

		if (Info->IndexInfo == NULL)
		{
			return (CSSMERR_DL_MEMORY_ERROR);
		}

		for (i = 0; i < Info->NumberOfIndexes; i++)
		{
			if ((ret = port_fread(&Info->IndexInfo[i].IndexType,
								  4,
								  1,
								  buf_ptr)) != CSSM_OK)
			{
				BioAPI_free(Info->IndexInfo, NULL);
				return (ret);
			}

			if ((ret = port_fread(&Info->IndexInfo[i].IndexedDataLocation,
								  4,
								  1,
								  buf_ptr)) != CSSM_OK)
			{
				BioAPI_free(Info->IndexInfo, NULL);
				return (ret);
			}

			if ((ret = dl_ReadDLDBAttributeInfo(buf_ptr,
												&Info->IndexInfo[i].Info))
												!= CSSM_OK)
			{
				BioAPI_free(Info->IndexInfo, NULL);
				return (ret);
			}
		}
	}
	else
		Info->IndexInfo = NULL;

	return (CSSM_OK);
}

/*-----------------------------------------------------------------------------
 *
 *Name: dl_ReadDLDataStoreInfo
 *
 *Description:
 *		Read the DL's data store information from the schema files.
 *
 *Parameters:
 *		schemaPath (input)	- pointer to the path of the schema file
 *		DSInfo (output)		- pointer to data that describes data store
 *
 *Returns:
 *		CSSM_OK - data is returned in parameter list
 *---------------------------------------------------------------------------*/
CSSM_RETURN dl_ReadDLDataStoreInfo(char* schemaPath,
								   CSSM_DBINFO_PTR DSInfo)
{
	CSSM_RETURN ret = CSSM_OK;
	FILE* file_ptr = NULL;
	sint32 Len = 0;

	if ((DSInfo == NULL) || (schemaPath == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	if ((ret = ffport_neDoesFileExist(schemaPath)) != CSSM_TRUE)
	{
		return (CSSMERR_DL_DATABASE_CORRUPT);
	}

	if ((ret = port_fopen(schemaPath, "rb", &file_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	/* read the number of recordtypes present in the data store */
	if ((ret = port_fread(&DSInfo->NumberOfRecordTypes,
						  4,
						  1,
						  file_ptr)) != CSSM_OK)
	{
		port_fclose(file_ptr);
		return (ret);
	}

	if (DSInfo->NumberOfRecordTypes > 0)
	{
		uint32 cType;

		if ((ret = port_fread(&DSInfo->IsLocal,
							  4,
							  1,
							  file_ptr)) != CSSM_OK)
		{
			port_fclose(file_ptr);
			return (ret);
		}

		/* read the length of the AccessPath */
		if ((ret = port_fread(&Len,
							  4,
							  1,
							  file_ptr)) != CSSM_OK)
		{
			port_fclose(file_ptr);
			return (ret);
		}

		if (Len != 0)
		{
			if ((DSInfo->AccessPath = (char *) BioAPI_calloc(Len, 1, NULL)) == NULL)
			{
				port_fclose(file_ptr);
				return (CSSMERR_DL_MEMORY_ERROR);
			}

			if ((ret = port_fread(DSInfo->AccessPath,
								  Len,
								  1,
								  file_ptr)) != CSSM_OK)
			{
				port_fclose(file_ptr);
				return (ret);
			}
		}
		else
		{
			DSInfo->AccessPath = NULL;
		}

		DSInfo->DefaultParsingModules = (CSSM_DB_PARSING_MODULE_INFO_PTR)
			BioAPI_calloc(DSInfo->NumberOfRecordTypes * sizeof(CSSM_DB_PARSING_MODULE_INFO), 1, NULL);

		if (DSInfo->DefaultParsingModules == NULL)
		{
			BioAPI_free(DSInfo->AccessPath, NULL);
			DSInfo->AccessPath = NULL;
			port_fclose(file_ptr);
			return (CSSMERR_DL_MEMORY_ERROR);
		}

		DSInfo->RecordAttributeNames = (CSSM_DB_RECORD_ATTRIBUTE_INFO_PTR)
			BioAPI_calloc(DSInfo->NumberOfRecordTypes * sizeof(CSSM_DB_RECORD_ATTRIBUTE_INFO), 1, NULL);

		if (DSInfo->RecordAttributeNames == NULL)
		{
			BioAPI_free(DSInfo->DefaultParsingModules, NULL);
			BioAPI_free(DSInfo->AccessPath, NULL);
			DSInfo->DefaultParsingModules = NULL;
			DSInfo->AccessPath = NULL;
			port_fclose(file_ptr);
			return (CSSMERR_DL_MEMORY_ERROR);
		}

		DSInfo->RecordIndexes = (CSSM_DB_RECORD_INDEX_INFO_PTR)
			BioAPI_calloc(DSInfo->NumberOfRecordTypes * sizeof(CSSM_DB_RECORD_INDEX_INFO), 1, NULL);

		if (DSInfo->RecordIndexes == NULL)
		{
			BioAPI_free(DSInfo->RecordAttributeNames, NULL);
			BioAPI_free(DSInfo->DefaultParsingModules, NULL);
			BioAPI_free(DSInfo->AccessPath, NULL);
			DSInfo->AccessPath = NULL;
			DSInfo->DefaultParsingModules = NULL;
			DSInfo->RecordAttributeNames = NULL;
			port_fclose(file_ptr);
			return (CSSMERR_DL_MEMORY_ERROR);
		}

		for (cType = 0; cType < DSInfo->NumberOfRecordTypes; cType++)
		{
			if (((ret = port_fread(&DSInfo->DefaultParsingModules[cType],
								  sizeof(CSSM_DB_PARSING_MODULE_INFO),
								  1,
								  file_ptr))
								  != CSSM_OK)
				||
				((ret = dl_ReadDLDBRecordAttribute(file_ptr,
												   &DSInfo->RecordAttributeNames[cType]))
												   != CSSM_OK)
				||
				((ret = dl_ReadDLDBRecordIndex(file_ptr,
											   &DSInfo->RecordIndexes[cType]))
											   != CSSM_OK))
			{
				BioAPI_free(DSInfo->RecordAttributeNames, NULL);
				BioAPI_free(DSInfo->RecordIndexes, NULL);
				BioAPI_free(DSInfo->AccessPath, NULL);
				BioAPI_free(DSInfo->DefaultParsingModules, NULL);
				DSInfo->AccessPath = NULL;
				DSInfo->RecordAttributeNames = NULL;
				DSInfo->RecordIndexes = NULL;
				DSInfo->DefaultParsingModules = NULL;
				port_fclose(file_ptr);
				return (ret);
			}
		}
	}
	else
	{
		DSInfo->DefaultParsingModules = NULL;
		DSInfo->RecordAttributeNames = NULL;
		DSInfo->RecordIndexes = NULL;
		DSInfo->IsLocal = CSSM_FALSE;
		DSInfo->AccessPath = NULL;
	}
	/* Reserved is not being written/read */
	DSInfo->Reserved = NULL;
	port_fclose(file_ptr);

	return (CSSM_OK);
}
