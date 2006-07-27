#ifndef __CDSARECTYPE_H__
#define __CDSARECTYPE_H__

#include "QRecType.h"
#include "MDSRecType.h"

class QDirectory;

template < class RecInfoType >
class QCdsaRecType : public QRecType
{
public:
	virtual QRec * NewRec( CSSM_DB_UNIQUE_RECORD_PTR recordId );
	virtual QRec * NextRec();
	virtual QRec * FirstRec();
	virtual QString Name() const;
	QCdsaRecType( const RecInfoType & recType, QDirectory *pDir );
	virtual ~QCdsaRecType();

	void SpecifyAttributes();

	QString m_strName;
};

#include "CdsaRec.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template < class RecInfoType >
QCdsaRecType< RecInfoType >::QCdsaRecType(const RecInfoType & recType, QDirectory *pDir) :
  QRecType( pDir, recType.m_dwRecType, recType.m_dwNumAttrs ),
  m_strName( recType.m_strName )
{
	m_outputAttributeData =
		new CSSM_DB_ATTRIBUTE_DATA[m_dwNumAttrs];

	m_cqQuery.RecordType			 = recType.m_dwRecType;
	m_cqQuery.Conjunctive			 = CSSM_DB_NONE;
	m_cqQuery.NumSelectionPredicates = 0;
	m_cqQuery.SelectionPredicate	 = NULL;
	m_cqQuery.QueryLimits.TimeLimit	 = CSSM_QUERY_TIMELIMIT_NONE;
	m_cqQuery.QueryLimits.SizeLimit	 = CSSM_QUERY_SIZELIMIT_NONE;
	m_cqQuery.QueryFlags			 = 0;

	memset( &m_outputAttributes, 0, sizeof(CSSM_DB_RECORD_ATTRIBUTE_DATA) );
	m_outputAttributes.DataRecordType	  = m_dwRecType;
	m_outputAttributes.NumberOfAttributes = m_dwNumAttrs;
	m_outputAttributes.AttributeData	  = (struct cssm_db_attribute_data *) m_outputAttributeData;

	SpecifyAttributes();
}

template < class RecInfoType >
QCdsaRecType< RecInfoType >::~QCdsaRecType()
{
	delete [] m_outputAttributeData;
	m_outputAttributeData = NULL;
}

template < class RecInfoType >
QString QCdsaRecType< RecInfoType >::Name() const
{
	return m_strName;
}

template < class RecInfoType >
QRec * QCdsaRecType< RecInfoType >::FirstRec()
{
	QRec * pRec = NULL;
	CSSM_DB_UNIQUE_RECORD_PTR recordId =
		QRecType::FirstRecordId( m_outputAttributes );
	if ( recordId )
	{
		RecInfoType recType;
		pRec =
			new QCdsaRec< RecInfoType >(
					recType,
					this,
					recordId,
					&m_outputAttributes);
	}

	return pRec;
}

template < class RecInfoType >
QRec * QCdsaRecType< RecInfoType >::NextRec()
{
	QRec * pRec = NULL;
	CSSM_DB_UNIQUE_RECORD_PTR recordId =
		QRecType::NextRecordId( m_outputAttributes );
	if ( recordId )
	{
		RecInfoType recType;
		pRec =
			new QCdsaRec< RecInfoType >(
					recType,
					this,
					recordId,
					&m_outputAttributes);
	}

	return pRec;
}

template <class RecInfoType >
QRec * QCdsaRecType< RecInfoType >::NewRec(CSSM_DB_UNIQUE_RECORD_PTR recordId)
{
	RecInfoType recInfoType;
	QRec * pRec =
		new QCdsaRec< RecInfoType >(
				recInfoType,
				this,
				recordId,
				&m_outputAttributes);

	return pRec;
}

#endif // !defined(AFX_CDSARECORDTYPE_H__27F7C8BB_6CF4_11D2_A091_00A0C90988EF__INCLUDED_)
