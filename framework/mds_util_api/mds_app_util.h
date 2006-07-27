/*-----------------------------------------------------------------------
 * File: MDS_APP_UTIL.H
 *-----------------------------------------------------------------------
 */


#ifndef __MDS_APP_UTIL_H__
#define __MDS_APP_UTIL_H__

#include "bioapi_type.h"
#include "cssmtype.h"
#include "mds.h"
#include "bioapi_uuid.h"


extern MDS_FUNCS MDSFuncs;
extern CSSM_DL_DB_HANDLE hDLDBBioAPI;
extern BioAPI_BOOL Initialised;


#ifdef __cplusplus
extern "C" {
#endif


BioAPI_RETURN BioAPI MDSUTIL_FreeUniqueRecord(CSSM_DB_UNIQUE_RECORD *pUID);

#ifdef __cplusplus
}
#endif

#endif /* ifndef __MDS_UTIL_H__ */
