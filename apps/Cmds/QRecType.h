#ifndef __QRECTYPE_H__
#define __QRECTYPE_H__

#include "stdafx.h"

class QMDS;
class QDirectory;
class QRec;

class QRecType
{
public:

  virtual QRec * NewRec( CSSM_DB_UNIQUE_RECORD_PTR recordId ) = 0;
  virtual QRec * NextRec() = 0;
  virtual QRec * FirstRec() = 0;
  QDirectory * m_pDir;
  CSSM_DB_RECORDTYPE m_dwRecType;
  DWORD m_dwNumAttrs;
  CSSM_DB_ATTRIBUTE_DATA_PTR m_outputAttributeData;
  CSSM_DB_RECORD_ATTRIBUTE_DATA m_outputAttributes;
  
  CSSM_DB_UNIQUE_RECORD_PTR FirstRecordId( CSSM_DB_RECORD_ATTRIBUTE_DATA & outputAttributes );
  CSSM_DB_UNIQUE_RECORD_PTR NextRecordId( CSSM_DB_RECORD_ATTRIBUTE_DATA & outputAttributes );
  virtual QString Name() const = 0;
  QRecType( QDirectory *pDir, CSSM_DB_RECORDTYPE dwRecType, DWORD dwNumAttrs );
  virtual ~QRecType();

#ifdef _DEBUG
  virtual void AssertValid() const;
#endif

protected:
  CSSM_QUERY m_cqQuery;
  CSSM_BOOL m_cbEndOfDataStore;
  CSSM_HANDLE m_resultsHandle;
};

#endif // __QRECTYPE_H__
