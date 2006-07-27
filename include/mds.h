/*-----------------------------------------------------------------------
 * File: MDS.H
 *-----------------------------------------------------------------------
 */

#ifndef MDS_H
#define MDS_H

/**************************************************************************
 *** MDS Constants ********************************************************
 **************************************************************************/
#define MDS_CDSA_DIRECTORY		"BioAPIMDSDirectory"

/* MDS predefined values for a 16K name space */
#define CSSM_DB_RELATIONID_MDS_START (0x40000000)
#define CSSM_DB_RELATIONID_MDS_END (0x40004000)

#ifdef __cplusplus
extern "C" {
#endif

#include "bioapi_type.h"
#include <cssmtype.h>

/**************************************************************************
 *** MDS type definitions *************************************************
 **************************************************************************/
typedef CSSM_DL_HANDLE MDS_HANDLE;
typedef CSSM_DL_DB_HANDLE MDS_DB_HANDLE;

typedef struct mds_funcs {
	CSSM_RETURN (CSSMAPI *DbOpen)(
						MDS_HANDLE MdsHandle,
						const char *DbName,
						const CSSM_NET_ADDRESS *DbLocation,
						CSSM_DB_ACCESS_TYPE AccessRequest,
						const CSSM_ACCESS_CREDENTIALS *AccessCred,
						const void *OpenParameters,
						CSSM_DB_HANDLE *hMds );

	CSSM_RETURN (CSSMAPI *DbClose)(
						MDS_DB_HANDLE MdsDbHandle );

	CSSM_RETURN (CSSMAPI *GetDbNames)(
						MDS_HANDLE MdsHandle,
						CSSM_NAME_LIST_PTR *NameList );

	CSSM_RETURN (CSSMAPI *GetDbNameFromHandle)(
						MDS_DB_HANDLE MdsDbHandle,
						char ** DbName );

	CSSM_RETURN (CSSMAPI *FreeNameList)(
						MDS_HANDLE MdsHandle,
						CSSM_NAME_LIST_PTR NameList );

	CSSM_RETURN (CSSMAPI *DataInsert)(
						MDS_DB_HANDLE MdsDbHandle,
						CSSM_DB_RECORDTYPE RecordType,
						const CSSM_DB_RECORD_ATTRIBUTE_DATA *Attributes,
						const CSSM_DATA *Data,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueId );

	CSSM_RETURN (CSSMAPI *DataDelete)(
						MDS_DB_HANDLE MdsDbHandle,
						const CSSM_DB_UNIQUE_RECORD *UniqueRecordIdentifier );

	CSSM_RETURN (CSSMAPI *DataModify)(
						MDS_DB_HANDLE MdsDbHandle,
						CSSM_DB_RECORDTYPE RecordType,
						CSSM_DB_UNIQUE_RECORD_PTR UniqueRecordIdentifier,
						const CSSM_DB_RECORD_ATTRIBUTE_DATA *AttributesToBeModified,
						const CSSM_DATA *DataToBeModified,
						CSSM_DB_MODIFY_MODE ModifyMode );

	CSSM_RETURN (CSSMAPI *DataGetFirst)(
						MDS_DB_HANDLE MdsDbHandle,
						const CSSM_QUERY *Query,
						CSSM_HANDLE_PTR ResultsHandle,
						CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
						CSSM_DATA_PTR Data,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueId );

	CSSM_RETURN (CSSMAPI *DataGetNext)(
						MDS_DB_HANDLE MdsDbHandle,
						CSSM_HANDLE ResultsHandle,
						CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
						CSSM_DATA_PTR Data,
						CSSM_DB_UNIQUE_RECORD_PTR *UniqueId );

	CSSM_RETURN (CSSMAPI *DataAbortQuery)(
						MDS_DB_HANDLE MdsDbHandle,
						CSSM_HANDLE ResultsHandle );

	CSSM_RETURN (CSSMAPI *DataGetFromUniqueRecordId)(
						MDS_DB_HANDLE MdsDbHandle,
						CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord,
						CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
						CSSM_DATA_PTR Data );

	CSSM_RETURN (CSSMAPI *FreeUniqueRecord)(
						MDS_DB_HANDLE MdsDbHandle,
						CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord );

	CSSM_RETURN (CSSMAPI *CreateRelation)(
						CSSM_DL_DB_HANDLE DLDBHandle,
						CSSM_DB_RECORDTYPE RelationID,
						const char *RelationName,
						uint32 NumberOfAttributes,
						const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
						uint32 NumberOfIndexes,
						const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo );

	CSSM_RETURN (CSSMAPI *DestroyRelation)(
						CSSM_DL_DB_HANDLE DLDBHandle,
						CSSM_DB_RECORDTYPE RelationID );

} MDS_FUNCS, *MDS_FUNCS_PTR;



/**************************************************************************
 *** Pointers to MDS Functions ********************************************
 **************************************************************************/
	typedef CSSM_RETURN (CSSMAPI *_MDSAPI_Initialize_PTR)(
						const CSSM_GUID *pCallerGuid,
						const CSSM_DATA *pCallerManifest,
						const CSSM_MEMORY_FUNCS *pMemoryFunctions,
						MDS_FUNCS_PTR pDlFunctions,
						MDS_HANDLE *hMds );

	typedef CSSM_RETURN (CSSMAPI *_MDSAPI_Terminate_PTR)(
						MDS_HANDLE MdsHandle );

	typedef CSSM_RETURN (CSSMAPI *_MDSAPI_Install_PTR)(
						MDS_HANDLE MdsHandle );

	typedef CSSM_RETURN (CSSMAPI *_MDSAPI_Uninstall_PTR)(
						MDS_HANDLE MdsHandle );



/**************************************************************************
 *** MDS Function Declarations ********************************************
 **************************************************************************/
BioAPI_RETURN BioAPI MDS_Initialize(
						const BioAPI_UUID *pCallerUuid,
						const BioAPI_DATA *pCallerManifest,
						const BioAPI_MEMORY_FUNCS *pMemoryFunctions,
						MDS_FUNCS_PTR pDlFunctions,
						MDS_HANDLE *hMds );

CSSM_RETURN CSSMAPI MDS_Terminate(
						MDS_HANDLE MdsHandle );

CSSM_RETURN CSSMAPI MDS_Install(
						MDS_HANDLE MdsHandle );

CSSM_RETURN CSSMAPI MDS_Uninstall(
						MDS_HANDLE MdsHandle );


#ifdef __cplusplus
}
#endif

#endif
