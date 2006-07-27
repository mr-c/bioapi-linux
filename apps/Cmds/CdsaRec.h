#ifndef __CDSAREC_H__
#define __CDSAREC_H__

#include "QRec.h"
#include "AppMemTrack.h"
#include "cssmMemTrack.h"

template < class RecInfoType >
class QCdsaRec : public QRec
{
public:
  virtual int NumAttributes() const;

  virtual int FindStringInRecord(const char *szString,
				 bool bMatchWholeWordOnly,
				 bool bMatchCase,
				 bool bExamineAttrName,
				 bool bExamineAttrValue,
				 int iStartSearchAt = 0);

  QCdsaRec(const RecInfoType & recType,
	   QRecType *pRecType,
	   CSSM_DB_UNIQUE_RECORD_PTR recordId = NULL,
	   const CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR outputAttributes = NULL);
  virtual ~QCdsaRec();

};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class RecInfoType >
QCdsaRec< RecInfoType >::QCdsaRec(const RecInfoType & ,
				  QRecType *pRecType,
				  CSSM_DB_UNIQUE_RECORD_PTR recordId,
				  const CSSM_DB_RECORD_ATTRIBUTE_DATA_PTR outputAttributes) :
  QRec( pRecType, recordId )
{
  m_outputAttributeData = new CSSM_DB_ATTRIBUTE_DATA[m_pRecType->m_dwNumAttrs];

  if ( outputAttributes )
  {
    memcpy(m_outputAttributeData,
	   outputAttributes->AttributeData,
	   sizeof(CSSM_DB_ATTRIBUTE_DATA)*(m_pRecType->m_dwNumAttrs));

    memcpy(&m_outputAttributes,
	   outputAttributes,
	   sizeof(m_outputAttributes));
  }
  m_outputAttributes.AttributeData = m_outputAttributeData;
}

template <class RecInfoType >
QCdsaRec< RecInfoType >::~QCdsaRec()
{
  for ( unsigned int i = 0; i < m_pRecType->m_dwNumAttrs; i++ )
  {
    CSSM_FREE_WRAPPER(QMdsUtil_free, m_outputAttributeData[i].Value->Data, &QMdsUtil_AllocRef );
    memset( &m_outputAttributeData[i].Value, 0, sizeof(m_outputAttributeData[i].Value) );
  }

  delete [] m_outputAttributeData;
  m_outputAttributeData = NULL;
}

template <class RecInfoType >
int QCdsaRec< RecInfoType >::NumAttributes() const
{
  return m_pRecType->m_dwNumAttrs;
}

template <class RecInfoType >
int QCdsaRec< RecInfoType >::FindStringInRecord(const char * szString,
						bool bMatchWholeWordOnly,
						bool bMatchCase,
						bool bExamineAttrName,
						bool bExamineAttrValue,
						int iStartSearchAt)
{
  ASSERT( bExamineAttrName || bExamineAttrValue );
  QString strString = szString;
  if ( !bMatchCase )
  {
    strString = strString.lower();
  }

  ASSERT( strString != "" );
  for ( unsigned int i = iStartSearchAt; i < m_pRecType->m_dwNumAttrs; i++ )
  {
    QString strAttrName	 = m_outputAttributeData[i].Info.Label.AttributeName;
    QString strAttrValue = (const char *)m_outputAttributeData[i].Value->Data;

    if ( !bMatchCase )
    {
      strAttrName = strAttrName.lower();
      strAttrValue = strAttrValue.lower();
    }

    if ( bExamineAttrName )
    {
      if ((bMatchWholeWordOnly  && (strString == strAttrName))	||
	  (!bMatchWholeWordOnly && (strAttrName.find(strString) != -1)))
      {
	return i;
      }
    }

    if (bExamineAttrValue &&
	(m_outputAttributeData[i].Info.AttributeFormat == CSSM_DB_ATTRIBUTE_FORMAT_STRING) &&
	m_outputAttributeData[i].Value->Data)
    {
      if ((bMatchWholeWordOnly  && (strString == strAttrValue))	||
	  (!bMatchWholeWordOnly && (strAttrValue.find(strString) != -1)))
      {
	return i;
      }
    }
  }

  return -1;
}

#endif // __CDSAREC_H__
