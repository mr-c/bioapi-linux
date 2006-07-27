#include "stdafx.h"
#include "MdsViewItems.h"
#include <QMDS.h>
#include <CdsaRecType.h>
#include <qpixmap.h>
#include <qdragobject.h>

static QPixmap uic_load_pixmap( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}

QDirectoryViewItem::QDirectoryViewItem(QMDS *pMds, LPCTSTR szDirName, QListViewItem *parent) :
  QListViewItem(parent, szDirName),
  m_pDir(NULL)
{
  QString strDirName = szDirName;
  setPixmap(0, uic_load_pixmap("folderclosed.png"));

  if ( strDirName == MDS_CDSA_DIRECTORY )
  {
    m_pDir = new QDirectory( pMds, szDirName );

    /*h_layer*/
    CCssmRecInfoType cssmRecInfoType;
    QRecType *pRecType = new QCdsaRecType< CCssmRecInfoType >( cssmRecInfoType, m_pDir );
    addRecordType( pRecType );

    /*bsp*/
    CEmmRecInfoType emmRecInfoType;
    pRecType = new QCdsaRecType< CEmmRecInfoType >( emmRecInfoType, m_pDir );
    addRecordType( pRecType );

    /*device*/
    CBioAPI_DeviceInfoType bioDeviceRecInfoType;
    pRecType = new QCdsaRecType< CBioAPI_DeviceInfoType >( bioDeviceRecInfoType, m_pDir );
    addRecordType( pRecType );

  }
}

QDirectoryViewItem::~QDirectoryViewItem()
{
  if (m_pDir) delete m_pDir;
}

QRecordTypeViewItem *QDirectoryViewItem::addRecordType(QRecType *recType)
{
  return new QRecordTypeViewItem(recType, this);
}

// QRecordTypeViewItem

QRecordTypeViewItem::QRecordTypeViewItem(QRecType *pRecType, QDirectoryViewItem *parent) :
  QListViewItem(parent, pRecType->Name()),
     m_pRecType(pRecType)
{
  setPixmap(0, uic_load_pixmap("folderclosed.png"));

  for (QRec * pRec = pRecType->FirstRec(); pRec; pRec = pRecType->NextRec())
  {
    addRecord(pRec);
  }
}

QRecordTypeViewItem::~QRecordTypeViewItem()
{
  if (m_pRecType) delete m_pRecType;
}

QRecordViewItem *QRecordTypeViewItem::addRecord(QRec *pRec)
{
  return new QRecordViewItem(pRec, this);
}


// QRecordViewItem

QRecordViewItem::QRecordViewItem(QRec *pRec, QRecordTypeViewItem *parent) :
  QListViewItem(parent, pRec->AttributeFormatString(0)),
  m_pRec(pRec)
{
  setPixmap(0, uic_load_pixmap("folderclosed.png"));
}

QRecordViewItem::~QRecordViewItem()
{
  if (m_pRec) delete m_pRec;
}
