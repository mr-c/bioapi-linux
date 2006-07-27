#ifndef __QMDS_H__
#define __QMDS_H__

#include <iostream>
#include <ostream>
#include "QDirectory.h"
#include "QMDSUtil.h"

class QMDS
{
public:

  QMDS(const CSSM_GUID *pCallerGuid);
  CSSM_MEMORY_FUNCS m_memoryFunctions;
  MDS_HANDLE m_hMds;
  //MDS_ERROR_FUNCS m_errorFuncs;
  
  CSSM_RETURN MDS_Initialize(const CSSM_GUID *pCallerGuid);
  CSSM_RETURN MDS_Terminate( /*CSSM_ERROR_PTR pCeError*/ );

  //////////////////////////////////////////////////////////////////////
  //Wrappers Around MDS_FUNCS Function Pointers
  //////////////////////////////////////////////////////////////////////

  CSSM_RETURN DbOpen(const char *DbName,
		     const CSSM_NET_ADDRESS *DbLocation,
		     CSSM_DB_ACCESS_TYPE AccessRequest,
		     const CSSM_ACCESS_CREDENTIALS *AccessCred,
		     const void *OpenParameters,
		     CSSM_DB_HANDLE *hMds);

  CSSM_RETURN DbClose(MDS_DB_HANDLE MdsDbHandle);
  
  CSSM_RETURN GetDbNames(CSSM_NAME_LIST_PTR *pNameList);
	
  CSSM_RETURN GetDbNameFromHandle(MDS_DB_HANDLE MdsHandle,char** dbName);

  CSSM_RETURN FreeNameList(CSSM_NAME_LIST_PTR NameList);

  CSSM_RETURN DataInsert(MDS_DB_HANDLE MdsDbHandle,
			 CSSM_DB_RECORDTYPE RecordType,
			 const CSSM_DB_RECORD_ATTRIBUTE_DATA *Attributes,
			 const CSSM_DATA *Data,
			 CSSM_DB_UNIQUE_RECORD_PTR *UniqueId);
  
  CSSM_RETURN DataDelete(MDS_DB_HANDLE MdsDbHandle,
			 const CSSM_DB_UNIQUE_RECORD *UniqueRecordIdentifier);

  CSSM_RETURN DataModify(MDS_DB_HANDLE MdsDbHandle,
			 CSSM_DB_RECORDTYPE RecordType,
			 CSSM_DB_UNIQUE_RECORD_PTR  UniqueRecordIdentifier,
			 const CSSM_DB_RECORD_ATTRIBUTE_DATA *AttributesToBeModified,
			 const CSSM_DATA *DataToBeModified,
			 const CSSM_DB_MODIFY_MODE ModifyMode);

  CSSM_RETURN DataGetFirst(MDS_DB_HANDLE MdsDbHandle,
			   const CSSM_QUERY *Query,
			   CSSM_HANDLE_PTR	ResultsHandle,
			   CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
			   CSSM_DATA_PTR  Data,
			   CSSM_DB_UNIQUE_RECORD_PTR *UniqueID);

  CSSM_RETURN DataGetNext(MDS_DB_HANDLE MdsDbHandle,
			  CSSM_HANDLE ResultsHandle,
			  CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
			  CSSM_DATA_PTR Data,
			  CSSM_DB_UNIQUE_RECORD_PTR *UniqueID);

  CSSM_RETURN DataAbortQuery(MDS_DB_HANDLE MdsDbHandle,
			     CSSM_HANDLE ResultsHandle);

  CSSM_RETURN DataGetFromUniqueRecordId(MDS_DB_HANDLE MdsDbHandle,
					CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord,
					CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR Attributes,
					CSSM_DATA_PTR	 Data);

  CSSM_RETURN FreeUniqueRecord(MDS_DB_HANDLE MdsDbHandle,
			       CSSM_DB_UNIQUE_RECORD_PTR UniqueRecord);
  
  QMDS();
  virtual ~QMDS();

#ifdef _DEBUG
  virtual void AssertValid() const;
#endif

protected:
  char * m_szDirName;
  MDS_FUNCS m_dlFunctions;
 };

#include "QMDSUtil.h"
#include "CdsaRecType.h"
#include "MDSRecType.h"

#endif // __QMDS_H__
