#ifndef __MDSEDITVIEW_H__
#define __MDSEDITVIEW_H__

#include <MdsEditMainWnd.h>

class QMDS;
class QListViewItem;
class QListViewItemIterator;
class QRecType;
class QRecordViewItem;
class QRecordTypeViewItem;
class QRec;

class MdsEditView : public MdsEditMainWnd
{
private:

  class FindParams
  {
  public:
    FindParams() :
      m_bExamineAttrValue( TRUE ),
      m_bExamineAttrName( TRUE ),
      m_bMatchCase( TRUE ),
      m_bMatchWholeStringOnly( TRUE )
    {
    }

    QString m_strFind;
    bool    m_bExamineAttrValue;
    bool    m_bExamineAttrName;
    bool    m_bMatchCase;
    bool    m_bMatchWholeStringOnly;
  };

  FindParams m_find;

  bool isMdsRoot(QListViewItem *item) const;
  bool isDirectory(QListViewItem *item) const;
  bool isRecordType(QListViewItem *item) const;
  bool isRecord(QListViewItem *item) const;

  void refreshListView();
  void populateListView(QRecordViewItem *pRec);
  void shiftFocusToListView(int index);
  void expandTreeToShowItem(QListViewItem *it);

  QListViewItemIterator *m_searchIterator;
  int                    m_iMatch;
  QListViewItem         *m_searchRoot;

  QListViewItem *findFirst();
  QListViewItem *findNext();

  void invalidateFind();

  QRec *m_pRec;

  int getSelectedIndex(QListView *lv);
 
  CSSM_RETURN modifyMds(CSSM_DB_RECORDTYPE  dwRecType,
			int                 i,
			QListViewItem      *item,
			QMDS               *pMds,
			QDirectory         *pDir);
  
public:

  MdsEditView() :
    MdsEditMainWnd(),
    m_searchIterator(NULL),
    m_iMatch(-1),
    m_searchRoot(NULL),
    m_pRec(NULL)
  {
  }

  virtual ~MdsEditView();

  void refresh();
  QListViewItem *addDirectory(QMDS *pMds, LPCTSTR title, QListViewItem *parent);
  QListViewItem *addRecordType(QRecType *pRecType, QListViewItem *hTreeItemDir);
  QListViewItem *addRecord(QRec *pRec, QRecordTypeViewItem *parent);

  QString buildRecordPath(QListViewItem *item) const;

  virtual void slotSelectionChanged(QListViewItem *item);
  virtual void slotAttrSelectionChanged(QListViewItem *item);
  virtual void slotAttributeDoubleClicked(QListViewItem *item);

  // Actions
  virtual void slotModifyAttributeAction();
  virtual void slotNewRecordAction();
  virtual void slotFindAction();
  virtual void slotFindNextAction();
};

#endif
