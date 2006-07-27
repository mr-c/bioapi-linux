/*-----------------------------------------------------------------------
 *		File:	BIOAPI_INSTALL.C
 *-----------------------------------------------------------------------
 */
#include "../precomp.h"
#include "precomp_install.h"
#include "mds_util.h"
//#define SCALAR_FIX 1
#include "bioapi_schema.h"

/*************************************************************************
 *************************************************************************
 ***** Addin Memory Functions ********************************************
 *************************************************************************
 *************************************************************************/

#if defined (WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif

/* Addin's local memory allocation functions. These functions match the
 * prototypes required to build a BioAPI_MEMORY_FUNCS structure for
 * attaching to other modules.
 */

void* BioAPI _Addin_malloc(
						uint32 size,
						void *AllocRef
#ifdef BioAPI_MEMTRACK_ON
						, const char *szFilename,
						uint32 u32LineNumber
#endif
						)
{
#if defined(_DEBUG) && defined(BioAPI_MEMTRACK_ON) && defined(WIN32)
	return _malloc_dbg( size, _NORMAL_BLOCK, szFilename, u32LineNumber );
#else
	return malloc( size );
#endif
}

void* BioAPI _Addin_realloc(
						void *Ptr,
						uint32 size,
						void *AllocRef
#ifdef BioAPI_MEMTRACK_ON
						, const char *szFilename,
						uint32 u32LineNumber
#endif
						)
{
#if defined(_DEBUG) && defined(BioAPI_MEMTRACK_ON) && defined(WIN32)
	return _realloc_dbg( Ptr, size, _NORMAL_BLOCK, szFilename, u32LineNumber );
#else
	return realloc( Ptr, size );
#endif
}

void BioAPI _Addin_free(
						void *Ptr,
						void *AllocRef
#ifdef BioAPI_MEMTRACK_ON
						, const char *szFilename,
						uint32 u32LineNumber
#endif
						)
{
#if defined(_DEBUG) && defined(BioAPI_MEMTRACK_ON) && defined(WIN32)
	_free_dbg( Ptr, _NORMAL_BLOCK );
#else
	free( Ptr );
#endif
}

void* BioAPI _Addin_calloc(
						uint32 size,
						uint32 count,
						void* AllocRef
#ifdef BioAPI_MEMTRACK_ON
						, const char *szFilename,
						uint32 u32LineNumber
#endif
						)
{
#if defined(_DEBUG) && defined(BioAPI_MEMTRACK_ON) && defined(WIN32)
	return _calloc_dbg( size, count, _NORMAL_BLOCK, szFilename, u32LineNumber );
#else
	return calloc( size, count );
#endif
}

/* "Global" table of memory functions to be used by an addin to attach to
 * other service providers.
 */
const BioAPI_MEMORY_FUNCS Addin_APIMemFuncs = { _Addin_malloc,
												  _Addin_free,
												  _Addin_realloc,
												  _Addin_calloc };

/*************************************************************************
 *************************************************************************
 ***** End Of Addin Memory Functions *************************************
 *************************************************************************
 *************************************************************************/

extern IfMdsuSchemaManipulator IfiMdsuBioAPICapabilitySchema;
 /* Number of MDS update operations that will be remembered by mds_util for
 * error clean-up during installation.
 */
#define BIOAPI_MDS_COMMIT_SIZE 20

static CSSM_RETURN
__CleanMDS(MDSU_CONTEXT *pContext)
{
	CSSM_RETURN err = CSSM_OK;
	BioAPI_H_LEVEL_FRAMEWORK_SCHEMA BioAPICapTemplate;
	BioAPI_H_LEVEL_FRAMEWORK_SCHEMA BioAPICapData;

	/* Execute a search on every record for this module to get a handle of all
	 * records and then delete them.
	 */

	/*	Remove the BioAPI Capability */
	BioAPI_CopyUuid (BioAPICapTemplate.ModuleId, bioapi_UUID);
	ERR(err = MDSU_FindFirst(pContext,
							 &IfiMdsuBioAPICapabilitySchema,
							 &BioAPICapTemplate,
							 MDSU_BIOAPI_CAP_MODULE_ID,
							 &BioAPICapData,
							 NULL));
	while(CSSM_OK == err)
	{
		ERR(err = MDSU_FindNext(pContext,
								&BioAPICapData,
								NULL));
	}

	if(CSSMERR_DL_ENDOFDATA == err)
	{
		/* Flush everything from the MDS */
		ERR(err = MDSU_Revert(pContext));

	} /* BioAPICapability removed */
	/* Make sure nothing gets removed if there is an error */
	if((CSSMERR_DL_ENDOFDATA != err) &&
	   (CSSM_OK != err))
	{
		MDSU_Commit( pContext );
	}
	return err;
}


static void bioapi_Get_H_Layer_Schema_Attributes(CSSM_DB_SCHEMA_ATTRIBUTE_INFO **pList)
{
	static CSSM_DB_SCHEMA_ATTRIBUTE_INFO rgAttributes[BIOAPI_H_LAYER_NUM_ATTRIBUTES] = {
		BIOAPI_ATTRINFO_MODULEID,
		BIOAPI_ATTRINFO_MODULENAME,
		BIOAPI_ATTRINFO_PRODUCTVERSION,
		BIOAPI_ATTRINFO_SPECVERSION,
		BIOAPI_ATTRINFO_VENDOR,
		BIOAPI_ATTRINFO_DESCRIPTION
	};

	*pList = rgAttributes;
}

static void bioapi_Get_H_Layer_Indexes(CSSM_DB_SCHEMA_INDEX_INFO ** pList)
{
	static CSSM_DB_SCHEMA_INDEX_INFO rgIndexes[BIOAPI_H_LAYER_NUM_ATTRIBUTES] = {
		BIOAPI_INDEX_SCHEMA_FRAMEWORK_MODULEID,
		BIOAPI_INDEX_SCHEMA_FRAMEWORK_MODULENAME,
		BIOAPI_INDEX_SCHEMA_FRAMEWORK_PRODUCTVERSION,
		BIOAPI_INDEX_SCHEMA_FRAMEWORK_SPECVERSION,
		BIOAPI_INDEX_SCHEMA_FRAMEWORK_VENDOR,
		BIOAPI_INDEX_SCHEMA_FRAMEWORK_DESCRIPTION
	};

	*pList = rgIndexes;
}


/* Creates the new relation*/
CSSM_RETURN CSSMAPI bioapi_install_CreateMDSRelation (const char *szDirectoryName,
										   CSSM_DB_RECORDTYPE RelationID,
										   const char *RelationName,
										   uint32 NumberOfAttributes,
										   const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
										   uint32 NumberOfIndexes,
										   const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo)
{

	static MDS_FUNCS s_bioapiMDSFuncs;
	MDS_DB_HANDLE hMds;
	CSSM_RETURN ret;

	if ( ( ret = MDS_Initialize(&bioapi_UUID,
								NULL,
								&Addin_APIMemFuncs,
								&s_bioapiMDSFuncs,
								&(hMds.DLHandle) ) ) == CSSM_OK )
	{
		/* Open the requested MDS directory */
		if ( ( ret = s_bioapiMDSFuncs.DbOpen( hMds.DLHandle,
										 szDirectoryName,
										 NULL,
										 CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE | CSSM_DB_ACCESS_PRIVILEGED,
										 NULL,
										 NULL,
										 &hMds.DBHandle ) ) == CSSM_OK )
		{
			ret = s_bioapiMDSFuncs.CreateRelation(
											hMds,
											RelationID,
											RelationName,
											NumberOfAttributes,
											pAttributeInfo,
											NumberOfIndexes,
											pIndexInfo);

		}

		s_bioapiMDSFuncs.DbClose ( hMds );

		MDS_Terminate( hMds.DLHandle );

	} /* MDS_Initialize */
	return (ret);
}

BioAPI_RETURN BioAPI
INSTALL_ENTRY_NAME(const char *szModuleName,
				   const char *szSearchPath,
				   BioAPI_DATA *pCredential,
				   INSTALL_ACTION Action,
				   BioAPI_INSTALL_ERROR *pError)
{
	BioAPI_RETURN err = BioAPI_OK;
	MDSU_CONTEXT MDSUContext;
	CSSM_DB_SCHEMA_ATTRIBUTE_INFO_PTR CapAttrInfo = NULL;
	CSSM_DB_SCHEMA_INDEX_INFO_PTR CapIndexInfo = NULL;
	BioAPI_H_LEVEL_FRAMEWORK_SCHEMA BioAPICapability;

	if (INSTALL_ACTION_UNINSTALL != Action)
	{
		bioapi_Get_H_Layer_Schema_Attributes(&CapAttrInfo);
		bioapi_Get_H_Layer_Indexes(&CapIndexInfo);
		err = bioapi_install_CreateMDSRelation (MDS_CDSA_DIRECTORY,
							   BIOAPI_H_LAYER_RECORDTYPE,
							   BIOAPI_H_LAYER_CAPABILITIES,
							   BIOAPI_H_LAYER_NUM_ATTRIBUTES,
							   CapAttrInfo,
							   BIOAPI_H_LAYER_NUM_ATTRIBUTES,
							   CapIndexInfo);
	}
	if( err == CSSM_OK || err == CSSMERR_DL_INVALID_RECORDTYPE )
	{
		/* Initialize MDS */
		if (ERR(err = MDSU_Init(&MDSUContext,
								&Addin_APIMemFuncs,
								&bioapi_UUID,
								MDS_CDSA_DIRECTORY,
								(CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE),
								BIOAPI_MDS_COMMIT_SIZE)) == CSSM_OK)
		{
			err = __CleanMDS(&MDSUContext);

			if((CSSM_OK == err) &&
			   (INSTALL_ACTION_UNINSTALL != Action))
			{
				/* reset error to CSSM_OK */
				err = BioAPI_OK;

				/* This is a sample entry in the capabilities record. This needs to be changed by the BSP
				vendor, filling in real values*/
				BioAPI_CopyUuid ( &BioAPICapability.ModuleId, &bioapi_UUID );
				strcpy( BioAPICapability.ModuleName, szModuleName);
				BioAPICapability.SpecVersion.Major = BIOAPI_SPEC_VERSION_MAJOR;
				BioAPICapability.SpecVersion.Minor = BIOAPI_SPEC_VERSION_MINOR;
				BioAPICapability.ProdVersion.Major = BIOAPI_HLAYER_VERSION_MAJOR;
				BioAPICapability.ProdVersion.Minor = BIOAPI_HLAYER_VERSION_MINOR;
				strcpy(BioAPICapability.Vendor, "BioAPI Consortium");
				strcpy(BioAPICapability.Description, "BioAPI H Layer");

				/* Write the data to MDS */
				ERR(err = MDSU_UpdateSchema(&MDSUContext,
											&IfiMdsuBioAPICapabilitySchema,
											&BioAPICapability,
											NULL));

				if(CSSM_OK == err)
				{
					/* commit the records */
					MDSU_Commit(&MDSUContext);

					err = CSSM_OK;
				}
				else
				{
					/* there's been an error, so we back out all changes */
					MDSU_Revert( &MDSUContext );
					sprintf(pError->ErrorString,
							"MDS Error (DL Primary): %X\n",
							err);
					pError->ErrorCode = err;
				}
			}

			/* Shut down the MDS connection */
			MDSU_Term( &MDSUContext );
		}
		else
		{
			sprintf(pError->ErrorString,
					"MDS Error (Init): %X\n",
					err);
			pError->ErrorCode = err;
		}
	}
	return err;
}
