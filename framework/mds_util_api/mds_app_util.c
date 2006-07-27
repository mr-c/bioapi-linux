/*-----------------------------------------------------------------------
 * File: MDS_APP_UTIL.C
 *-----------------------------------------------------------------------
 */

#include <assert.h>

#ifdef BioAPI_MEMTRACK_ON
#define CSSM_MEMTRACK_ON
#endif

#include "mds_util_intrn.h"	 /* IsBadCodePtr(), IsBadReadPtr() */
#include "bioapi_type.h"
#include "bioapi_uuid.h"
#include "bioapi_util.h"

MDS_FUNCS MDSFuncs;
CSSM_DL_DB_HANDLE hDLDBBioAPI;
CSSM_BOOL Initialised = CSSM_FALSE;


CSSM_RETURN CSSMAPI MDSUTIL_InitEx(CSSM_BOOL ReadWrite)
{
	CSSM_DL_HANDLE Handle = 0;
	CSSM_DB_HANDLE dbHandle = 0;
	CSSM_DB_ACCESS_TYPE AccessType = CSSM_DB_ACCESS_READ;
	CSSM_RETURN retValue = CSSM_OK;
 	/* Linux-Port: added "const" keyword for Linux version */
#if defined (UNIX)
 	const BioAPI_UUID MDSUTIL_Uuid = ref_h_layer_uuid_init;
#elif
	BioAPI_UUID MDSUTIL_Uuid = ref_h_layer_uuid_init;
#endif


	/* Attempt to initialize the MDS and get an MDS handle */
	if ((retValue = MDS_Initialize(&MDSUTIL_Uuid,
				   NULL,
				   &BioAPIMemoryFuncs,
				   &MDSFuncs,
				   &Handle)) != CSSM_OK)
		return retValue;

	if ( ReadWrite == CSSM_TRUE )
		AccessType |= CSSM_DB_ACCESS_WRITE;

	if ((retValue = MDSFuncs.DbOpen (Handle, MDS_CDSA_DIRECTORY, NULL, AccessType, NULL, NULL, &dbHandle)) != CSSM_OK)
	{
		MDS_Terminate (Handle);
		return (retValue);
	}

	hDLDBBioAPI.DLHandle = Handle;
	hDLDBBioAPI.DBHandle = dbHandle;
	Initialised = CSSM_TRUE;
	return (CSSM_OK);
}

void CSSMAPI MDSUTIL_Term()
{
	if (!IsBadCodePtr((CSSM_PROC_ADDR)MDSFuncs.DbClose))
	{
		MDSFuncs.DbClose (hDLDBBioAPI);
		MDS_Terminate (hDLDBBioAPI.DLHandle);
		memset (&MDSFuncs, 0, sizeof (MDS_FUNCS));
	}
	Initialised = CSSM_FALSE;
	return;
}
