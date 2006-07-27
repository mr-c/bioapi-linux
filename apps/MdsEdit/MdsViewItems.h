#ifndef __MDSVIEWITEMS_H__
#define __MDSVIEWITEMS_H__

#include <qlistview.h>

class QMDS;

class QRecordTypeViewItem;
class QRecordViewItem;
class QRecType;

// QDirectoryViewItem

class QDirectoryViewItem : public QListViewItem
{
private:

  QDirectory *m_pDir;

public:

  QDirectoryViewItem(QMDS *pMds, LPCTSTR szDirName, QListViewItem *parent);

  virtual ~QDirectoryViewItem();

  QDirectory *getQDir() { return m_pDir; }

  QRecordTypeViewItem *addRecordType(QRecType *recType);

};


// QRecordTypeViewItem

class QRecordTypeViewItem : public QListViewItem
{
private:

  QRecType *m_pRecType;

public:

  QRecordTypeViewItem(QRecType *recType, QDirectoryViewItem *parent);

  virtual ~QRecordTypeViewItem();

  QRecordViewItem *addRecord(QRec *pRec);

  QRecType *recType() { return m_pRecType; }
};


// QRecordViewItem

class QRecordViewItem : public QListViewItem
{
private:

  QRec *m_pRec;

public:

  QRecordViewItem(QRec *pRec, QRecordTypeViewItem *parent);

  virtual ~QRecordViewItem();

  QRec *rec() { return m_pRec; }
};

#endif // __MDSVIEWITEMS_H__
