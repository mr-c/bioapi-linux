/*-----------------------------------------------------------------------
 * File: DL_FILEWRITE.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * This file contains the functions that are used to write the schema
 * information to the dl schema file
 */

#include "dlutil.h"
#include "ff_port.h"


/*---------------------------------------------------------------
 *
 *Name: dl_WriteDataStruct
 *
 *Description:
 *	 Generic function for writing a CSSM_DATA structure to the
 *	 schema file
 *
 *Parameters:
 *	 buf_ptr (input)	- pointer to the location to write in the schema file
 *						  schema file
 *	 Data (input)		- pointer to the CSSM_DATA structure to be written
 *
 *Returns:
 *	 CSSM_OK - schema file is updated
 *-------------------------------------------------------------*/
CSSM_RETURN dl_WriteDataStruct (void *buf_ptr,
								const CSSM_DATA *Data)
{
	CSSM_RETURN ret = CSSM_OK;

	if ((buf_ptr == NULL) || (Data == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	if (!IsBadReadPtr (Data, sizeof (CSSM_DATA)))
	{
		if (!IsBadReadPtr (Data->Data, Data->Length))
		{
			if ((ret = port_fwrite((void *)&Data->Length,
								   sizeof (Data->Length),
								   1,
								   buf_ptr)) != CSSM_OK)
			{
				return (ret);
			}

			if ((ret = port_fwrite((void *)Data->Data,
								   Data->Length,
								   1,
								   buf_ptr)) != CSSM_OK)
			{
				return (ret);
			}

			return (CSSM_OK);
		}
	}

	return (CSSMERR_DL_OS_ACCESS_DENIED);
}

/*---------------------------------------------------------------
 *
 *Name: dl_WriteDLDBRecordAttributeInfo
 *
 *Description:
 *	   Write the DL's record attribute information to
 *	   the schema file.
 *
 *Parameters:
 *	   buf_ptr (input)	- the pointer to the place in the schema
 *						  file, where the information is to be inserted
 *	   Info (input)		- pointer to data that describes record attribute
 *						  information
 *
 *Returns:
 *	   CSSM_OK - data is return in parameter list
 *-------------------------------------------------------------*/
CSSM_RETURN dl_WriteDLDBAttributeInfo(void *buf_ptr,
									  CSSM_DB_ATTRIBUTE_INFO_PTR Info)
{
	CSSM_RETURN ret = CSSM_OK;
	sint32 Len = 0;

	if ((buf_ptr == NULL) || (Info == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	if ((ret = port_fwrite(&Info->AttributeNameFormat,
						   sizeof (Info->AttributeNameFormat),
						   1,
						   buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	switch(Info->AttributeNameFormat)
	{
		case CSSM_DB_ATTRIBUTE_NAME_AS_STRING:
		{
			/* write the length of the string to be written */
			Len = strlen(Info->Label.AttributeName) + 1;
			if ((ret = port_fwrite(&Len,
								   sizeof(Len),
								   1,
								   buf_ptr)) != CSSM_OK)
			{
				return (ret);
			}

			/* now write the string itself */
			if ((ret = port_fwrite(Info->Label.AttributeName,
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
			if ((ret = dl_WriteDataStruct(buf_ptr,
										  &Info->Label.AttributeOID))
										  != CSSM_OK)
			{
				return (ret);
			}
		}
		break;

		case CSSM_DB_ATTRIBUTE_NAME_AS_INTEGER:
		{
			if ((ret = port_fwrite(&Info->Label.AttributeID,
								   sizeof (Info->Label.AttributeID),
								   1,
								   buf_ptr)) != CSSM_OK)
			{
				return (ret);
			}
		}
		break;
	}

	if ((ret = port_fwrite(&Info->AttributeFormat,
						   sizeof (Info->AttributeFormat),
						   1,
						   buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	return (ret);
}

/*---------------------------------------------------------------
 *
 *Name: dl_WriteDLDBRecordAttribute
 *
 *Description:
 *	   Write the DL's record attribute to the schema file.
 *
 *Parameters:
 *	   buf_ptr (input)	- the pointer to the place where the
 *						  record attribute will be written.
 *	   Info (input)		- pointer to data that describes the
 *						  record attribute
 *
 *Returns:
 *	   CSSM_OK - data is return in parameter list
 *-------------------------------------------------------------*/
CSSM_RETURN dl_WriteDLDBRecordAttribute(void *buf_ptr,
										CSSM_DB_RECORD_ATTRIBUTE_INFO_PTR Info)
{
	CSSM_RETURN ret = CSSM_OK;
	uint32 i;

	if ((buf_ptr == NULL) || (Info == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	if ((ret = port_fwrite(&Info->DataRecordType,
						   sizeof (Info->DataRecordType),
						   1,
						   buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	if ((ret = port_fwrite(&Info->NumberOfAttributes,
						   sizeof (Info->NumberOfAttributes),
						   1,
						   buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	for (i = 0; i < Info->NumberOfAttributes; i++)
	{
		if ((ret = dl_WriteDLDBAttributeInfo(buf_ptr,
											 &Info->AttributeInfo[i]))
											 != CSSM_OK)
		{
			return (ret);
		}
	}

	return (CSSM_OK);
}

/*---------------------------------------------------------------
 *
 *Name: dl_WriteDLDBRecordIndex
 *
 *Description:
 *	   Write the DL's record index information to
 *	   the schema file.
 *
 *Parameters:
 *	   buf_ptr (input)	- the pointer to the place in file where the
 *						  information will be written
 *	   Info (input)		- pointer to data that describes the reocrd index
 *
 *Returns:
 *	   CSSM_OK - data is returned in the parameter list
 *-------------------------------------------------------------*/
CSSM_RETURN dl_WriteDLDBRecordIndex(void *buf_ptr,
									CSSM_DB_RECORD_INDEX_INFO_PTR Info)
{
	CSSM_RETURN ret = CSSM_OK;
	uint32 i;

	if ((buf_ptr == NULL) || (Info == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	if ((ret = port_fwrite(&Info->DataRecordType,
						   sizeof (Info->DataRecordType),
						   1,
						   buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	if ((ret = port_fwrite(&Info->NumberOfIndexes,
						   sizeof (Info->NumberOfIndexes),
						   1,
						   buf_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	for (i = 0; i < Info->NumberOfIndexes; i++)
	{
		if ((ret = port_fwrite(&Info->IndexInfo[i].IndexType,
							   sizeof (Info->IndexInfo[i].IndexType),
							   1,
							   buf_ptr)) != CSSM_OK)
		{
			return (ret);
		}

		if ((ret = port_fwrite(&Info->IndexInfo[i].IndexedDataLocation,
							   sizeof (Info->IndexInfo[i].IndexedDataLocation),
							   1,
							   buf_ptr)) != CSSM_OK)
		{
			return (ret);
		}

		if ((ret = dl_WriteDLDBAttributeInfo(buf_ptr,
											 &Info->IndexInfo[i].Info))
											 != CSSM_OK)
		{
			return (ret);
		}
	}

	return (CSSM_OK);
}

/*---------------------------------------------------------------
 *
 *Name: dl_WriteDLDataStoreInfo
 *
 *Description:
 *		Write the DL's data store information to the schema file.
 *
 *Parameters:
 *		schemaPath (input)	- pointer to the path of the schema file
 *		DSInfo (output)		- pointer to data that describes data store
 *	   regKey (input) - the key that identifies the addin
 *	   DSInfo (input) - pointer to data that describes data store
 *
 *Returns:
 *	   CSSM_OK - data is returned in the parameter list
 *-------------------------------------------------------------*/
CSSM_RETURN dl_WriteDLDataStoreInfo(char *schemaPath,
									CSSM_DBINFO_PTR DSInfo)
{
	CSSM_RETURN ret = CSSM_OK;
	uint32 cType;
	FILE* file_ptr = NULL;
	sint32 Len = 0;

	if ((schemaPath == NULL) || (DSInfo == NULL))
	{
		return (CSSMERR_DL_INVALID_POINTER);
	}

	if ((ret = port_fopen(schemaPath, "w+b", &file_ptr)) != CSSM_OK)
	{
		return (ret);
	}

	if ((ret = port_fwrite(&DSInfo->NumberOfRecordTypes,
						   sizeof (DSInfo->NumberOfRecordTypes),
						   1,
						   file_ptr)) != CSSM_OK)
	{
		port_fclose(file_ptr);
		return (ret);
	}

	if ((ret = port_fwrite(&DSInfo->IsLocal,
						   sizeof(DSInfo->IsLocal),
						   1,
						   file_ptr)) != CSSM_OK)
	{
		port_fclose(file_ptr);
		return (ret);
	}

	/* If AccessPath is null, then write 0 in the schema file
	 * to indicate null AccessPath, else write the length and then the AccessPath.
	 */
	if (DSInfo->AccessPath == NULL)
	{
		if ((ret = port_fwrite(&Len,
							   sizeof(Len),
							   1,
							   file_ptr)) != CSSM_OK)
		{
			port_fclose(file_ptr);
			return (ret);
		}
	}
	else
	{
		Len = strlen(DSInfo->AccessPath) + 1;
		/* write the length of the AccessPath */
		if ((ret = port_fwrite(&Len,
							   sizeof(Len),
							   1,
							   file_ptr)) != CSSM_OK)
		{
			port_fclose(file_ptr);
			return (ret);
		}

		/* now write the AccessPath */
		if ((ret = port_fwrite(DSInfo->AccessPath,
							   Len,
							   1,
							   file_ptr)) != CSSM_OK)
		{
			port_fclose(file_ptr);
			return (ret);
		}
	}

	for (cType = 0; cType < DSInfo->NumberOfRecordTypes; cType++)
	{
		if ((ret = port_fwrite((void *)&DSInfo->DefaultParsingModules[cType],
							   sizeof(CSSM_DB_PARSING_MODULE_INFO),
							   1,
							   file_ptr)) != CSSM_OK)
		{
			port_fclose(file_ptr);
			return (ret);
		}

		if (((ret = dl_WriteDLDBRecordAttribute(file_ptr,
												&DSInfo->RecordAttributeNames[cType]))
												!= CSSM_OK)
			||
			((ret = dl_WriteDLDBRecordIndex(file_ptr,
											&DSInfo->RecordIndexes[cType]))
											!= CSSM_OK))
		{
			port_fclose(file_ptr);
			return (ret);
		}
	}
	/* Reserved is not being written/read */

	port_fclose(file_ptr);

	return (CSSM_OK);
}
