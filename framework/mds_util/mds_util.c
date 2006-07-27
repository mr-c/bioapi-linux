/*-----------------------------------------------------------------------
 * File: MDS_UTIL.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


#include <assert.h>
#include <stdlib.h>		/* NULL */

#include "bioapi_type.h"
#include "bioapi_uuid.h"
#include "port/bioapi_port.h"
#include "cssmtype.h"
#include "cssmerr.h"

#include "mds.h"
#include "mds_util.h"

#include "internal.h"

#if defined (WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif


/* MDS function table used by all MDS contexts. This value may be global
 * without causing problems since the same values will be written to the
 * table for each context initialization.
 */
static MDS_FUNCS s_MDSUFuncs;

BioAPI_RETURN BioAPI MDSU_Init( MDSU_CONTEXT *pContext,
							   const BioAPI_MEMORY_FUNCS *pMemFuncs,
							   const BioAPI_UUID *pUuid,
							   const char *szDirectoryName,
							   CSSM_DB_ACCESS_TYPE AccessType,
							   uint32 CommitSize )
{
	BioAPI_RETURN rv = CSSM_OK;

	/* Parameter check */
	if ( ( pContext == NULL ) ||
		 ( pMemFuncs == NULL ) ||
		 ( szDirectoryName == NULL ) )
	{
		rv = CSSMERR_CSSM_INVALID_POINTER;
	}
	else
	{
		/* Set the default access value if one is not specified */
		if ( AccessType == 0 )
		{
			AccessType = CSSM_DB_ACCESS_READ;
		}

		/* Wipe the context clean */
		memset( pContext, 0, sizeof(MDSU_CONTEXT) );

		/* Set the memory functions */
		pContext->MemFuncs = *pMemFuncs;

		/* Attempt to initialize the MDS and get an MDS handle */
		if ( ( rv = MDS_Initialize( pUuid,
									NULL,
									&pContext->MemFuncs,
									&s_MDSUFuncs,
									&pContext->hMds ) ) == CSSM_OK )
		{
			/* Open the requested MDS directory */
			 if ( ( rv = s_MDSUFuncs.DbOpen( pContext->hMds,
											 szDirectoryName,
											 NULL,
											 AccessType,
											 NULL,
											 NULL,
											 &pContext->hDb.DBHandle ) ) == CSSM_OK )
			 {
				 pContext->hDb.DLHandle = pContext->hMds;

				/* Copy the proper values into the MDSU context */
				if ( pUuid != NULL )
				{
					BioAPI_CopyUuid (&pContext->ModuleUuid, pUuid);
				}
				else
				{
					BioAPI_ClearUuid (&pContext->ModuleUuid);
				}

				/* Allocate the commit cache */
				pContext->pCommitCache =
							MDSU_calloc( pContext,
										 sizeof(CSSM_DB_UNIQUE_RECORD_PTR),
										 CommitSize );
				pContext->CommitSize = CommitSize;
				pContext->CacheCount = 0;

				/* Close the database if there is an error condition */
				if ( rv != CSSM_OK )
				{
					s_MDSUFuncs.DbClose( pContext->hDb );
				}
			 }

			 /* Terminate the MDS in an error condition */
			 if ( rv != CSSM_OK )
			 {
				 MDS_Terminate( pContext->hMds );
				 memset( pContext, 0, sizeof(MDSU_CONTEXT) );
			 }
		} /* MDS_Initialize */
	} /* Validation */

	return rv;
}

BioAPI_RETURN BioAPI MDSU_Term( MDSU_CONTEXT *pContext )
{
	BioAPI_RETURN rv = CSSM_OK;

	/* Parameter check */
	if ( pContext == NULL )
	{
		rv = CSSMERR_CSSM_INVALID_POINTER;
	}
	else
	{
		/* Commit the records in the cache */
		MDSU_Commit( pContext );

		/* Free the commit cache */
		MDSU_free( pContext, pContext->pCommitCache );

		/* Close the database handle */
		if ( ( rv = s_MDSUFuncs.DbClose( pContext->hDb ) ) == CSSM_OK )
		{
			/* Close the MDS connection */
			if ( ( rv = MDS_Terminate( pContext->hMds ) ) == CSSM_OK )
			{
				/* Wipe the context */
				memset( pContext, 0, sizeof(MDSU_CONTEXT) );
			}
		} /* DbClose */
	} /* Validation */

	return rv;
}

BioAPI_RETURN BioAPI MDSU_UpdateSchema(
						MDSU_CONTEXT *pContext,
						const IfMdsuSchemaManipulator *Manipulator,
						void *pSchemaData,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueId )
{
	BioAPI_RETURN rv = CSSM_OK;
	CSSM_DB_UNIQUE_RECORD_PTR TempId = NULL;

	assert( pContext && pSchemaData && Manipulator );
	assert( Manipulator->AttributeConstructor &&
			Manipulator->AttributeDestructor );

	/* Is this a record type we understand? */
	if ( Manipulator->AttributeConstructor == NULL )
	{
		rv = CSSMERR_CSSM_INTERNAL_ERROR;
	}
	else
	{
		/* Update the MDS */
		rv = __MDSU_UpdateSchema(
							pContext,
							pSchemaData,
							Manipulator->AttributeConstructor,
							Manipulator->AttributeDestructor,
							&TempId );
		if ( rv == CSSM_OK )
		{
			/* Either cache the record or return it depending on the value
			 * of the UniqueId parameter.
			 */
			if ( UniqueId == NULL )
			{
				/* Add the record to the cache */
				if ( pContext->CacheCount == pContext->CommitSize )
				{
					/* Cache is full, forget the record */
					MDSU_FreeUniqueRecord( pContext, TempId );
				}
				else
				{
					pContext->pCommitCache[pContext->CacheCount] = TempId;
					pContext->CacheCount++;
				}
			}
			else
			{
				*UniqueId = TempId;
			}
		}
	}

	return rv;
}

BioAPI_RETURN BioAPI MDSU_Revert( MDSU_CONTEXT *pContext )
{
	uint32 i;

	assert( pContext );

	/* Iterate through the commit cache and delete the records */
	for ( i = 0; i < pContext->CacheCount; i++ )
	{
		MDSU_DeleteRecord( pContext, pContext->pCommitCache[i] );
	}

	/* Set the cache count to zero */
	pContext->CacheCount = 0;

	return CSSM_OK;
}

BioAPI_RETURN BioAPI MDSU_Commit( MDSU_CONTEXT *pContext )
{
	uint32 i;

	assert( pContext );

	/* Iterate through the commit cache and free the record IDs */
	for ( i = 0; i < pContext->CacheCount; i++ )
	{
		MDSU_FreeUniqueRecord( pContext, pContext->pCommitCache[i] );
	}

	/* Set the cache count to zero */
	pContext->CacheCount = 0;


	return CSSM_OK;
}

BioAPI_RETURN BioAPI MDSU_FindFirst(
						MDSU_CONTEXT *pContext,
						const IfMdsuSchemaManipulator *Manipulator,
						void *pSchemaTemplate,
						MDSU_TEMPLATE_MASK ValidityMask,
						void *pSchemaData,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueId )
{
	BioAPI_RETURN rv = CSSM_OK;
	CSSM_DB_UNIQUE_RECORD_PTR TempId = NULL;

	assert( pContext && Manipulator && pSchemaData &&
			( ( ValidityMask && pSchemaTemplate ) ||
			  ( !ValidityMask && !pSchemaTemplate ) ) );
	assert( Manipulator->AttributeConstructor &&
			Manipulator->AttributeDestructor &&
			Manipulator->PredicateConstructor &&
			Manipulator->PredicateDestructor &&
			Manipulator->AttributeConverter );

	/* Is there already a search in progress? */
	if ( pContext->hResults != CSSM_INVALID_HANDLE )
	{
		rv = CSSMERR_CSSM_INTERNAL_ERROR;
	}
	/* Is this a record type we understand? */
	else if ( ( Manipulator->AttributeConstructor == NULL ) ||
			  ( Manipulator->PredicateConstructor == NULL ) )
	{
		rv = CSSMERR_CSSM_INTERNAL_ERROR;
	}
	else
	{
		/* Search the MDS */
		if ( ( rv = __MDSU_FindFirst(
							pContext,
							pSchemaTemplate,
							ValidityMask,
							pSchemaData,
							Manipulator->AttributeConstructor,
							Manipulator->AttributeDestructor,
							Manipulator->PredicateConstructor,
							Manipulator->PredicateDestructor,
							Manipulator->AttributeConverter,
							&TempId ) ) == CSSM_OK )
		{
			/* Save the record type */
			pContext->pManipulator = Manipulator;

			/* Either cache the record or return it depending on the value
			 * of the UniqueId parameter.
			 */
			if ( UniqueId == NULL )
			{
				/* Add the record to the cache */
				if ( pContext->CacheCount == pContext->CommitSize )
				{
					/* Cache is full, forget the record */
					MDSU_FreeUniqueRecord( pContext, TempId );
				}
				else
				{
					pContext->pCommitCache[pContext->CacheCount] = TempId;
					pContext->CacheCount++;
				}
			}
			else
			{
				*UniqueId = TempId;
			}
		}
	} /* Validation */

	return rv;
}

BioAPI_RETURN BioAPI MDSU_FindNext(
						MDSU_CONTEXT *pContext,
						void *pSchemaData,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueId )
{
	BioAPI_RETURN rv = CSSM_OK;
	CSSM_DB_UNIQUE_RECORD_PTR TempId;

	assert( pContext && pSchemaData );

	/* Is there a search in progress? */
	if ( pContext->hResults == CSSM_INVALID_HANDLE )
	{
		rv = CSSMERR_CSSM_INTERNAL_ERROR;
	}
	else
	{
		/* Search the MDS */
		if ( ( rv = __MDSU_FindNext(
							pContext,
							pSchemaData,
							pContext->pManipulator->AttributeConstructor,
							pContext->pManipulator->AttributeDestructor,
							pContext->pManipulator->AttributeConverter,
							&TempId ) ) == CSSM_OK )
		{
			/* Either cache the record or return it depending on the value
			 * of the UniqueId parameter.
			 */
			if ( UniqueId == NULL )
			{
				/* Add the record to the cache */
				if ( pContext->CacheCount == pContext->CommitSize )
				{
					/* Cache is full, forget the record */
					MDSU_FreeUniqueRecord( pContext, TempId );
				}
				else
				{
					pContext->pCommitCache[pContext->CacheCount] = TempId;
					pContext->CacheCount++;
				}
			}
			else
			{
				*UniqueId = TempId;
			}
		}
		else
		{
			/* The search is done. Clear the manipulator. */
			pContext->pManipulator = NULL;
		} /* __MDSU_FindNext */
	} /* Validation */

	return rv;
}

BioAPI_RETURN BioAPI MDSU_FreeUniqueRecord( MDSU_CONTEXT *pContext,
										   CSSM_DB_UNIQUE_RECORD_PTR pUID )
{
	assert( pContext && pUID );

	return s_MDSUFuncs.FreeUniqueRecord( pContext->hDb, pUID );
}

BioAPI_RETURN BioAPI MDSU_DeleteRecord(
						MDSU_CONTEXT *pContext,
						CSSM_DB_UNIQUE_RECORD *pUID )
{
	BioAPI_RETURN rv;

	assert( pContext && pUID );

	if ( ( rv = s_MDSUFuncs.DataDelete( pContext->hDb, pUID ) ) == CSSM_OK )
	{
		MDSU_FreeUniqueRecord( pContext, pUID );
	}

	return rv;
}

BioAPI_RETURN BioAPI MDSU_FindAbort( MDSU_CONTEXT *pContext )
{
	BioAPI_RETURN rv;

	assert( pContext );

	/* Abort the query operation */
	rv = s_MDSUFuncs.DataAbortQuery( pContext->hDb,
									 pContext->hResults );

	/* Invalidate the search handles */
	pContext->hResults = CSSM_INVALID_HANDLE;
	pContext->pManipulator = NULL;

	return rv;
}

BioAPI_RETURN BioAPI __MDSU_UpdateSchema(
							MDSU_CONTEXT *pContext,
							const void *pSchemaData,
							_MDSU_AttributeConstructor ConstructAttributes,
							_MDSU_AttributeDestructor DestructAttributes,
							CSSM_DB_UNIQUE_RECORD_PTR *UniqueId )
{
	BioAPI_RETURN rv;
	CSSM_DB_RECORD_ATTRIBUTE_DATA DbRecord;
	void *pState = NULL;

	assert( pContext && pSchemaData &&
			ConstructAttributes && DestructAttributes );

	/* Build the attribute list */
	if ( ( rv = ConstructAttributes( pContext,
									 pSchemaData,
									 &DbRecord,
									 CSSM_TRUE,
									 &pState ) ) == CSSM_OK )
	{
		/* Add the record to the MDS */
		rv = s_MDSUFuncs.DataInsert( pContext->hDb,
									 DbRecord.DataRecordType,
									 &DbRecord,
									 NULL,
									 UniqueId );

		/* Get rid of attribute data */
		DestructAttributes( pContext, &DbRecord, CSSM_FALSE, pState );
	}

	return rv;
}

BioAPI_RETURN BioAPI __MDSU_FindFirst(
							MDSU_CONTEXT *pContext,
							void *pSchemaTemplate,
							MDSU_TEMPLATE_MASK ValidityMask,
							void *pSchemaData,
							_MDSU_AttributeConstructor ConstructAttributes,
							_MDSU_AttributeDestructor DestructAttributes,
							_MDSU_PredicateConstructor ConstructPredicate,
							_MDSU_PredicateDestructor DestructPredicate,
							_MDSU_AttributeConverter ConvertAttributes,
							CSSM_DB_UNIQUE_RECORD_PTR *UniqueId )
{
	BioAPI_RETURN rv;
	CSSM_QUERY DbQuery;
	CSSM_DB_RECORD_ATTRIBUTE_DATA DbRecord;
	CSSM_HANDLE hResults;
	void *pPredState = NULL;
	void *pAttrState = NULL;

	assert( pContext && pSchemaData &&
			( ( ValidityMask && pSchemaTemplate ) ||
			  ( !ValidityMask && !pSchemaTemplate ) ) &&
			ConstructAttributes && DestructAttributes &&
			ConstructPredicate && DestructPredicate &&
			ConvertAttributes );

	/* Build the query Predicate */
	if ( ( rv = ConstructPredicate( pContext,
									pSchemaTemplate,
									ValidityMask,
									&DbQuery,
									&pPredState ) ) == CSSM_OK )
	{
		/* Build the output attributes structure */
		if ( ( rv = ConstructAttributes( pContext,
										 NULL,
										 &DbRecord,
										 CSSM_FALSE,
										 &pAttrState ) ) == CSSM_OK )
		{
			/* Start the query */
			if ( ( rv = s_MDSUFuncs.DataGetFirst( pContext->hDb,
												  &DbQuery,
												  &hResults,
												  &DbRecord,
												  NULL,
												  UniqueId ) ) == CSSM_OK )
			{
				if ( ( rv = ConvertAttributes( pContext,
											   &DbRecord,
											   pSchemaData ) ) == CSSM_OK )
				{
					/* Save the search handle */
					pContext->hResults = hResults;
				}
				else
				{
					/* Cancel the query because something went wrong */
					s_MDSUFuncs.DataAbortQuery( pContext->hDb, hResults );
				}
			} /* DataGetFirst */

			/* Free the attributes */
			DestructAttributes( pContext, &DbRecord, CSSM_TRUE, pAttrState );
		} /* ConstructAttributes */

		/* Free the predicate */
		DestructPredicate( pContext, &DbQuery, pPredState );
	} /* ConstructPredicate */

	return rv;
}

BioAPI_RETURN BioAPI __MDSU_FindNext(
						MDSU_CONTEXT *pContext,
						void *pSchemaData,
						_MDSU_AttributeConstructor ConstructAttributes,
						_MDSU_AttributeDestructor DestructAttributes,
						_MDSU_AttributeConverter ConvertAttributes,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueId )
{
	BioAPI_RETURN rv;
	CSSM_DB_RECORD_ATTRIBUTE_DATA DbRecord;
	void *pAttrState = NULL;

	assert( pContext && pSchemaData && UniqueId );

	/* Build the output attributes structure */
	if ( ( rv = ConstructAttributes( pContext,
									 NULL,
									 &DbRecord,
									 CSSM_FALSE,
									 &pAttrState ) ) == CSSM_OK )
	{
		/* Continue the query */
		if ( ( rv = s_MDSUFuncs.DataGetNext( pContext->hDb,
											 pContext->hResults,
											 &DbRecord,
											 NULL,
											 UniqueId ) ) == CSSM_OK )
		{
			rv = ConvertAttributes( pContext,
									&DbRecord,
									pSchemaData );
		} /* DataGetFirst */

		/* Clean up if the search or conversion went wrong */
		if ( rv != CSSM_OK )
		{
			/* Cancel the query because something went wrong */
			if ( rv != CSSMERR_DL_ENDOFDATA )
			{
				s_MDSUFuncs.DataAbortQuery( pContext->hDb, pContext->hResults );
			}
			pContext->hResults = CSSM_INVALID_HANDLE;
		}

		/* Free the attributes */
		DestructAttributes( pContext, &DbRecord, CSSM_TRUE, pAttrState );
	} /* ConstructAttributes */

	return rv;
}
