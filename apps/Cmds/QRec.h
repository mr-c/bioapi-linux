#ifndef __QREC_H__
#define __QREC_H__

#include <qstring.h>

class QRecType;

class QRec
{
private:

    QRec() :
      m_pRecType(NULL),
      m_recordId(NULL),
      m_outputAttributeData(NULL)
    {
      memset(&m_outputAttributes, 0, sizeof(m_outputAttributes));
    }

public:

    static bool s_bViewBlobAsString;
    virtual int FindStringInRecord(const char *,
				   bool,
				   bool,
				   bool,
				   bool,
				   int  = 0) 
    { return -1; }

    QString operator [](int i) const;

    QString AttributeFormatBlob(int i) const;
    QString AttributeFormatString(int i) const;
    QString AttributeFormatUint32(int i) const;
    QString AttributeFormatSint32( int i ) const;
    QString AttributeFormatMultiUint32(int i) const;

    QRecType *m_pRecType;
    CSSM_DB_UNIQUE_RECORD_PTR m_recordId;
    CSSM_DB_ATTRIBUTE_DATA_PTR m_outputAttributeData;
    CSSM_DB_RECORD_ATTRIBUTE_DATA m_outputAttributes;
    
    QRec(QRecType * pRectType,
	 CSSM_DB_UNIQUE_RECORD_PTR recordId);

    virtual ~QRec();

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:
};

#endif
