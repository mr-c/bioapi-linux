#include "stdafx.h"
#include <QMDS.h>

#include "MdsEditView.h"
#include "MdsEditDoc.h"
#include "MdsEditMainWnd.h"
#include "MdsViewItems.h"

#include "MdsModifyStringDialog.h"
#include "MdsModifyUintDialog.h"
#include "MdsModifyMultiUintDialog.h"
#include "MdsFindDialog.h"

#include <qlistview.h>
#include <qpixmap.h>
#include <qdragobject.h>
#include <qstatusbar.h>
#include <qaction.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qdialog.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <stdarg.h>

extern QMdsEditDoc *g_doc;

#define FORMAT_BUFFER_SIZE 1024

#ifdef LINUX
static QString QStringFormat(const char *format, ...)
{
  va_list ap;
  char buf[FORMAT_BUFFER_SIZE];

  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);

  return QString(buf);
}
#else
static QString QStringFormat(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  
  char buf[FORMAT_BUFFER_SIZE];
  vsprintf(buf, format, ap);
  va_end(ap);

  return QString(buf);
}
#endif

static QPixmap uic_load_pixmap( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}

MdsEditView::~MdsEditView()
{
  if (m_searchIterator)
  {
    delete m_searchIterator;
    m_searchIterator = NULL;
  }
}

void MdsEditView::refresh()
{
  CSSM_NAME_LIST_PTR pDirNameList = NULL;

  CSSM_RETURN ret = g_doc->m_pMds->GetDbNames(&pDirNameList);

  m_treeView->clear();

  QListViewItem * item = new QListViewItem( m_treeView, 0 );
  item->setText( 0, "MDS");
  item->setPixmap( 0, uic_load_pixmap( "root.png" ) );

  if ( ret == CSSM_OK )
  {
    for ( uint32 i = 0; i < pDirNameList->NumStrings; i++ )
    {
      addDirectory( (g_doc->m_pMds), pDirNameList->String[i], item );
    }
  }
}

QListViewItem *MdsEditView::addDirectory(QMDS *pMds, LPCTSTR szDirName, QListViewItem *hTreeItemParent )
{
  return new QDirectoryViewItem(pMds, szDirName, hTreeItemParent);
}

QListViewItem *MdsEditView::addRecordType(QRecType *pRecType, QListViewItem *hTreeItemDir)
{
  QListViewItem *hTreeItemRecType = new QListViewItem(hTreeItemDir, pRecType->Name());
  hTreeItemRecType->setPixmap(0, uic_load_pixmap("folderclosed.png"));

	/*BOOL bStatus = */
  //GetTreeCtrl().SetItemData(
  //hTreeItemRecType,
  //(DWORD) pRecType);

  for (QRec * pRec = pRecType->FirstRec();
       pRec;
       pRec = pRecType->NextRec())
  {
    /*HTREEITEM hTreeItemRec = AddRecord( pRec, hTreeItemRecType );*/
  }
  
  return hTreeItemRecType;
}

QString MdsEditView::buildRecordPath(QListViewItem *item) const
{
  QString strRecPath = item->text(0);
  
  while((item = item->parent()))
  {
    strRecPath = item->text(0) + "/" + strRecPath;
  }
  
  return strRecPath;
}
 
bool MdsEditView::isMdsRoot(QListViewItem *item) const
{
  return (item == m_treeView->firstChild());
}

bool MdsEditView::isDirectory(QListViewItem *item) const
{
  QListViewItem *parent = item->parent();
  if (!parent) return false;

  return isMdsRoot(parent);
}

bool MdsEditView::isRecordType(QListViewItem *item) const
{
  QListViewItem *parent = item->parent();
  if (!parent) return false;

  return isDirectory(parent);
}

bool MdsEditView::isRecord(QListViewItem *item) const
{
  QListViewItem *parent = item->parent();
  if (!parent) return false;

  return isRecordType(parent);
}

void MdsEditView::slotSelectionChanged(QListViewItem *item)
{
  QString path = buildRecordPath(item);
  statusBar()->message(path);

  refreshListView();

  newRecordAction->setEnabled(isRecordType(item));
}

void MdsEditView::refreshListView()
{
  m_listView->clear();
  m_listView->setSorting(-1);
  slotAttrSelectionChanged(NULL);
  m_pRec = NULL;

  QListViewItem *item = m_treeView->selectedItem();

  if (isRecord(item))
  {
    populateListView((QRecordViewItem *)item);
  }
}

void MdsEditView::populateListView(QRecordViewItem *recordItem)
{
  QRec *pRec = recordItem->rec();
  m_pRec = pRec;

  if ( !pRec || !(pRec->m_pRecType) )
  {
    return;
  }

  for ( int i = (int)pRec->m_pRecType->m_dwNumAttrs - 1; i >= 0; i-- )
  {
    QString strImage;

    switch ( pRec->m_outputAttributeData[i].Info.AttributeFormat )
    {
    case CSSM_DB_ATTRIBUTE_FORMAT_STRING:

      strImage = "string.png";
      break;

    case CSSM_DB_ATTRIBUTE_FORMAT_UINT32:
    case CSSM_DB_ATTRIBUTE_FORMAT_SINT32:
      strImage = "uint.png";
      break;

    case CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32:
      strImage = "multiuint.png";
      break;

    case CSSM_DB_ATTRIBUTE_FORMAT_BLOB:
      strImage = "blob.png";
      break;

    default:
      strImage = "spacer.png";
      break;
    } //switch

    QString strLength = "%1";
    strLength = strLength.arg(pRec->m_outputAttributeData[i].Value->Length);

    QListViewItem *item = new QListViewItem(m_listView, 
					    pRec->m_outputAttributeData[i].Info.Label.AttributeName,
					    (*pRec)[i],
					    strLength);
    item->setPixmap(0, uic_load_pixmap(strImage));
  }
}

int MdsEditView::getSelectedIndex(QListView *lv)
{
  int idx = -1;
  int i = 0;

  QListViewItemIterator it(lv);

  for(i = 0; it.current() != 0; ++i,++it ) 
  {
    if (it.current()->isSelected())
    {
      idx = i;
      break;
    }
  }
    
  return idx;
}

void MdsEditView::slotAttrSelectionChanged(QListViewItem *)
{
  int idx = getSelectedIndex(m_listView);

  if (idx < 0)
  {
    modifyAttributeAction->setEnabled(false);
  }
  else
  {
    modifyAttributeAction->setEnabled(
	       m_pRec->m_outputAttributeData[idx].Info.AttributeFormat !=
	       CSSM_DB_ATTRIBUTE_FORMAT_BLOB);
  }
}

void MdsEditView::slotAttributeDoubleClicked(QListViewItem *)
{
  int i = getSelectedIndex(m_listView);
  if ( i < 0 )
  {
    fprintf(stderr, "No items were selected!\n");
    return;
  }

	//CDir *pDir = m_pRec->m_pRecType->m_pDir;
	//CMDS *pMds = pDir->m_pMds;
	//CSSM_DB_RECORDTYPE dwRecType = m_pRec->m_pRecType->m_dwRecType;

  if (m_pRec->m_outputAttributeData[i].Info.AttributeFormat ==
      CSSM_DB_ATTRIBUTE_FORMAT_BLOB)
  {
    if ( m_pRec->m_outputAttributeData[i].Value->Data )
    {
      //TODO:OnEditExportBlob();
    }
    else
    {
      //TODO:OnEditImportBlob();
    }
  }
  else
  {
    slotModifyAttributeAction();
  }  
}

void MdsEditView::slotModifyAttributeAction()
{

  int i = getSelectedIndex(m_listView);
  if ( i < 0 )
  {
    fprintf(stderr,"No items were selected!\n");
    return;
  }

  fprintf(stderr, "idx=%d\n", i);

  QDirectory *pDir = m_pRec->m_pRecType->m_pDir;
  QMDS *pMds = pDir->m_pMds;
  CSSM_DB_RECORDTYPE dwRecType = m_pRec->m_pRecType->m_dwRecType;
  QString strAttrValue;
  
  QRegExp rx("\\s+"); // A regular expression to remove excess whitespace

  switch ( m_pRec->m_outputAttributeData[i].Info.AttributeFormat )
  {
  case CSSM_DB_ATTRIBUTE_FORMAT_UINT32:
    {
      MdsModifyUintDialog modifyUint32Dlg(this);
      modifyUint32Dlg.m_attrName->setText(m_pRec->m_outputAttributeData[i].Info.Label.AttributeName);
      
      uint32 u32Value;
      if ( m_pRec->m_outputAttributeData[i].Value->Data )
      {
	u32Value = *((uint32 *) m_pRec->m_outputAttributeData[i].Value->Data);
	
	strAttrValue = QStringFormat( "%.8X", u32Value );
      }

      modifyUint32Dlg.m_attrValue->setText(strAttrValue);

      if ( modifyUint32Dlg.exec() != QDialog::Accepted )
      {
	return;
      }

      if ( modifyUint32Dlg.m_attrValue->text() == strAttrValue )
      {
	return;
      }

      //modifyUint32Dlg.m_strAttrValue.Remove( ' ' ); VC6.0
      //modifyUint32Dlg.m_strAttrValue.TrimLeft(); 
      // We can use QT regexps for this...
      strAttrValue = modifyUint32Dlg.m_attrValue->text();
      strAttrValue.replace(rx, "");

      if ( strAttrValue.length() == 0 )
      {
	CSSM_FREE_WRAPPER(pMds->m_memoryFunctions.free_func,
			  m_pRec->m_outputAttributeData[i].Value->Data,
			  pMds->m_memoryFunctions.AllocRef);

	m_pRec->m_outputAttributeData[i].Value->Data = NULL;
	m_pRec->m_outputAttributeData[i].Value->Length = 0;
      }
      else
      {
	if ( m_pRec->m_outputAttributeData[i].Value->Data == NULL )
	{
	  m_pRec->m_outputAttributeData[i].Value->Length = 4;

	  m_pRec->m_outputAttributeData[i].Value->Data =
	    (uint8 *) CSSM_MALLOC_WRAPPER(pMds->m_memoryFunctions.malloc_func,
					  m_pRec->m_outputAttributeData[i].Value->Length,
					  pMds->m_memoryFunctions.AllocRef);
	}

	if ( m_pRec->m_outputAttributeData[i].Value->Data == NULL )
	{
	  //	  AfxThrowMemoryException();
	}

	sscanf((const char *)strAttrValue, "%X", (unsigned int *)m_pRec->m_outputAttributeData[i].Value->Data );
      }
    }
    break;

  case CSSM_DB_ATTRIBUTE_FORMAT_STRING:
    {
      MdsModifyStringDialog modifyStringDlg(this);

      modifyStringDlg.m_attrName->setText(m_pRec->m_outputAttributeData[i].Info.Label.AttributeName);

      if ( m_pRec->m_outputAttributeData[i].Value->Data == NULL )
      {
	modifyStringDlg.m_isNull->setChecked(TRUE);
      }
      else
      {
	modifyStringDlg.m_isNull->setChecked(FALSE);
	modifyStringDlg.m_attrValue->setText((const char *)m_pRec->m_outputAttributeData[i].Value->Data);
      }

	
      if ( modifyStringDlg.exec() != QDialog::Accepted )
      {
	return;
      }

      //exit if there is really no change necessary to MDS
      if ( modifyStringDlg.m_isNull->isChecked() )
      {
	if ( m_pRec->m_outputAttributeData[i].Value->Data == NULL )
        {
	  return;
	}
      }
      else
      {
	if (m_pRec->m_outputAttributeData[i].Value->Data &&
	    (modifyStringDlg.m_attrValue->text() == (const char *)m_pRec->m_outputAttributeData[i].Value->Data))
	{
	  return;
	}
      }
      
      //fix up m_pRec->m_outputAttributeData[i].Value
      if ( m_pRec->m_outputAttributeData[i].Value->Data )
      {
	CSSM_FREE_WRAPPER(pMds->m_memoryFunctions.free_func,
			  m_pRec->m_outputAttributeData[i].Value->Data,
			  pMds->m_memoryFunctions.AllocRef);
	
	m_pRec->m_outputAttributeData[i].Value->Data = NULL;
	m_pRec->m_outputAttributeData[i].Value->Length = 0;
      }

      ASSERT( m_pRec->m_outputAttributeData[i].Value->Data == NULL );
      
      if ( modifyStringDlg.m_isNull->isChecked() == FALSE )
      {
	m_pRec->m_outputAttributeData[i].Value->Length = modifyStringDlg.m_attrValue->text().length() + 1;
	
	m_pRec->m_outputAttributeData[i].Value->Data = (uint8 *) CSSM_MALLOC_WRAPPER(pMds->m_memoryFunctions.malloc_func,
										     m_pRec->m_outputAttributeData[i].Value->Length,
										     pMds->m_memoryFunctions.AllocRef);
	  
	memcpy(m_pRec->m_outputAttributeData[i].Value->Data,
	       (const char *)modifyStringDlg.m_attrValue->text(),
	       m_pRec->m_outputAttributeData[i].Value->Length);
      }
    } //case CSSM_DB_ATTRIBUTE_FORMAT_STRING
    break;

  case CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32:
    {
      const uint32 iMaxNumUint32 = 10;

      MdsModifyMultiUintDialog modifyMultiUint32Dlg(this);
      modifyMultiUint32Dlg.m_attrName->setText(m_pRec->m_outputAttributeData[i].Info.Label.AttributeName);
	
      QString dlgStrAttrValue[iMaxNumUint32];

      if ( m_pRec->m_outputAttributeData[i].Value->Length >iMaxNumUint32*4 )
      {
	QMessageBox::warning(this, "Error", "The set of uint32 is too large for MDS Edit",
			     QMessageBox::Ok, QMessageBox::NoButton);
      }
	
      if ( m_pRec->m_outputAttributeData[i].Value->Data )
      {
	for (uint32 j = 0; j < m_pRec->m_outputAttributeData[i].Value->Length; j+=4 )
	{
	  uint32 u32Value =  *((uint32 *) (m_pRec->m_outputAttributeData[i].Value->Data+j));
		
	  dlgStrAttrValue[j/4] = QStringFormat( "%.8X", u32Value );
	}
      }
	
      // Ugly: upload the values to the dialog box
      modifyMultiUint32Dlg.m_attrValue0->setText(dlgStrAttrValue[0]);
      modifyMultiUint32Dlg.m_attrValue1->setText(dlgStrAttrValue[1]);
      modifyMultiUint32Dlg.m_attrValue2->setText(dlgStrAttrValue[2]);
      modifyMultiUint32Dlg.m_attrValue3->setText(dlgStrAttrValue[3]);
      modifyMultiUint32Dlg.m_attrValue4->setText(dlgStrAttrValue[4]);
      modifyMultiUint32Dlg.m_attrValue5->setText(dlgStrAttrValue[5]);
      modifyMultiUint32Dlg.m_attrValue6->setText(dlgStrAttrValue[6]);
      modifyMultiUint32Dlg.m_attrValue7->setText(dlgStrAttrValue[7]);
      modifyMultiUint32Dlg.m_attrValue8->setText(dlgStrAttrValue[8]);
      modifyMultiUint32Dlg.m_attrValue8->setText(dlgStrAttrValue[9]);

      if ( modifyMultiUint32Dlg.exec() != QDialog::Accepted )
      {
	return;
      }
	
      // Ugly: retrieve the values from the dialog box
      dlgStrAttrValue[0] = modifyMultiUint32Dlg.m_attrValue0->text();
      dlgStrAttrValue[1] = modifyMultiUint32Dlg.m_attrValue1->text();
      dlgStrAttrValue[2] = modifyMultiUint32Dlg.m_attrValue2->text();
      dlgStrAttrValue[3] = modifyMultiUint32Dlg.m_attrValue3->text();
      dlgStrAttrValue[4] = modifyMultiUint32Dlg.m_attrValue4->text();
      dlgStrAttrValue[5] = modifyMultiUint32Dlg.m_attrValue5->text();
      dlgStrAttrValue[6] = modifyMultiUint32Dlg.m_attrValue6->text();
      dlgStrAttrValue[7] = modifyMultiUint32Dlg.m_attrValue7->text();
      dlgStrAttrValue[8] = modifyMultiUint32Dlg.m_attrValue8->text();
      dlgStrAttrValue[9] = modifyMultiUint32Dlg.m_attrValue9->text();

      uint32 u32DlgValue[iMaxNumUint32];
      memset( u32DlgValue, 0, sizeof(u32DlgValue) );
      uint32 k = 0;
      //construct the new CSSM_DATA
      for ( uint32 j = 0; j < iMaxNumUint32; j++ )
      {
	dlgStrAttrValue[j].replace(rx, "");
	    
	if ( dlgStrAttrValue[j].length() > 0 )
	{
	  sscanf(dlgStrAttrValue[j], "%X", &u32DlgValue[k]);
	  k++;
	}
      }
	
      CSSM_DATA cdData;
      cdData.Data = (uint8 *) u32DlgValue;
      cdData.Length = k * 4;
      if ((cdData.Length == m_pRec->m_outputAttributeData[i].Value->Length) &&
	  (memcmp(cdData.Data,
		  m_pRec->m_outputAttributeData[i].Value->Data,
		  cdData.Length) == 0))
      {
	return;
      }
	
      //modify m_pRec->m_outputAttributeData[i].Value.Data
      if ( m_pRec->m_outputAttributeData[i].Value->Data )
      {
	CSSM_FREE_WRAPPER(pMds->m_memoryFunctions.free_func,
			  m_pRec->m_outputAttributeData[i].Value->Data,
			  pMds->m_memoryFunctions.AllocRef);
	m_pRec->m_outputAttributeData[i].Value->Data = NULL;
	m_pRec->m_outputAttributeData[i].Value->Length = 0;
      }
	
      if ( cdData.Length > 0 )
      {
	m_pRec->m_outputAttributeData[i].Value->Length = cdData.Length;
	    
	m_pRec->m_outputAttributeData[i].Value->Data =
	  (uint8 *) CSSM_MALLOC_WRAPPER(pMds->m_memoryFunctions.malloc_func,
					m_pRec->m_outputAttributeData[i].Value->Length,
					pMds->m_memoryFunctions.AllocRef);
	    
	memcpy(m_pRec->m_outputAttributeData[i].Value->Data,
	       cdData.Data,
	       m_pRec->m_outputAttributeData[i].Value->Length);
      }
    }
    break;
      
  default:
    fprintf(stderr,"Don't Know How To Edit This RecordType\n");
    ASSERT(FALSE);
    return;
    break;
  } //switch
    
  modifyMds(dwRecType, i, m_listView->selectedItem(), pMds, pDir);
}

CSSM_RETURN MdsEditView::modifyMds(CSSM_DB_RECORDTYPE  dwRecType,
				   int                 i,
				   QListViewItem      *item,
				   QMDS               *pMds,
				   QDirectory         *pDir)
{
  // fixme, is this the right mode?
  CSSM_DB_MODIFY_MODE mode = CSSM_DB_MODIFY_ATTRIBUTE_NONE;

  CSSM_RETURN crStatus = pMds->DataModify(pDir->m_hDb,
					  dwRecType,
					  m_pRec->m_recordId,
					  &(m_pRec->m_outputAttributes),
					  NULL,
					  mode);
  if ( crStatus != CSSM_OK )
  {
    QMessageBox::warning(this, "MDS Error", "Modification Failed",
			 QMessageBox::Ok, QMessageBox::NoButton);
    refresh();
    return crStatus;
  }

  item->setText(1, (*m_pRec)[i]);

  QString strLength = QStringFormat("%d",
				    m_pRec->m_outputAttributeData[i].Value->Length);

  item->setText(2, strLength);

  if ( getSelectedIndex(m_listView) == 0 )
  {
    m_treeView->selectedItem()->setText(0, (*m_pRec)[i]);
  }

  return crStatus;
}

void MdsEditView::slotNewRecordAction()
{
  QRecordTypeViewItem *selected = (QRecordTypeViewItem *)m_treeView->selectedItem();
  QRecType *pRecType = selected->recType();

  QDirectory *pDir = pRecType->m_pDir;
  QMDS *pMds = pDir->m_pMds;
  
  CSSM_DB_UNIQUE_RECORD_PTR recordId = NULL;
  
  CSSM_RETURN ret = pMds->DataInsert(pDir->m_hDb,
				     pRecType->m_dwRecType,
				     &(pRecType->m_outputAttributes),
				     NULL,
				     &recordId);

  if ( ret != CSSM_OK )
  {
    QMessageBox::warning(this, "MDS Error", "DataInsert Failed",
			 QMessageBox::Ok, QMessageBox::NoButton);
    refresh();
    return;
  }

  QRec *pRec = pRecType->NewRec( recordId );
  if ( pRec == NULL )
  {
    //AfxThrowMemoryException();
    return;
  }

  QListViewItem *newItem = addRecord(pRec, selected);

  m_treeView->setSelected(newItem, TRUE);
  //TODO:InvalidateFind();
}

QListViewItem *MdsEditView::addRecord(QRec *pRec, QRecordTypeViewItem *parent)
{
  //this is wrong, but it will have to do
  QString strItemSelected = pRec->AttributeFormatString(0); 

  QRecordViewItem *newItem = new QRecordViewItem(pRec, parent);
  newItem->setText(0, strItemSelected);
  newItem->setPixmap(0, uic_load_pixmap("folderclosed.png"));

  return newItem;
}

QListViewItem *MdsEditView::findFirst()
{
  MdsFindDialog findDlg(this);

  findDlg.m_searchString->setText(m_find.m_strFind);
  findDlg.m_useAttrName->setChecked(m_find.m_bExamineAttrName);
  findDlg.m_useAttrValue->setChecked(m_find.m_bExamineAttrValue);
  findDlg.m_matchCase->setChecked(m_find.m_bMatchCase);
  findDlg.m_matchWholeString->setChecked(m_find.m_bMatchWholeStringOnly);
  
  if ( findDlg.exec() != QDialog::Accepted )
  {
    return NULL;
  }

  m_find.m_strFind		 = findDlg.m_searchString->text();
  m_find.m_bExamineAttrName	 = findDlg.m_useAttrName->isChecked();
  m_find.m_bExamineAttrValue	 = findDlg.m_useAttrValue->isChecked();
  m_find.m_bMatchCase	         = findDlg.m_matchCase->isChecked();
  m_find.m_bMatchWholeStringOnly = findDlg.m_matchWholeString->isChecked();

  // First determine the root for the search
  invalidateFind();
  m_searchRoot = m_treeView->selectedItem();
  if (!m_searchRoot)
  {
    m_searchRoot = m_treeView->firstChild();
  }

  // Next create an iterator starting at the search root

  m_searchIterator = new QListViewItemIterator(m_searchRoot);

  if ( isRecord(m_searchRoot) )
  {
    QRec * pRec =	((QRecordViewItem *)m_searchRoot)->rec();
    ASSERT( pRec );
    
    m_iMatch = pRec->FindStringInRecord(m_find.m_strFind,
					m_find.m_bMatchWholeStringOnly,
					m_find.m_bMatchCase,
					m_find.m_bExamineAttrName,
					m_find.m_bExamineAttrValue);
    if ( m_iMatch != -1 )
    {
      m_treeView->setSelected(m_searchRoot, TRUE);
      expandTreeToShowItem(m_searchRoot);
      
      shiftFocusToListView(m_iMatch);
      return m_searchRoot;
    }
  }

  return findNext();
}

QListViewItem *MdsEditView::findNext()
{
  if (!m_searchRoot || !m_searchIterator) return findFirst();

  QListViewItem *item = m_searchIterator->current();

  if (m_iMatch == -1)
  {
    // Wind the iterator on...
    (*m_searchIterator)++;
  }

  while(item)
  {
    if (isRecord(item))
    {
      QRec * pRec =	((QRecordViewItem *)item)->rec();
      ASSERT( pRec );
    

      m_iMatch = pRec->FindStringInRecord(m_find.m_strFind,
					  m_find.m_bMatchWholeStringOnly,
					  m_find.m_bMatchCase,
					  m_find.m_bExamineAttrName,
					  m_find.m_bExamineAttrValue,
					  m_iMatch + 1);
      if (m_iMatch != -1)
      {
	m_treeView->setSelected(item, TRUE);
	expandTreeToShowItem(item);

	shiftFocusToListView(m_iMatch);
	return item;
      }
    }

    (*m_searchIterator)++;
    item = m_searchIterator->current();
  }

  QString strMsg = "No more records with \"";
  strMsg += m_find.m_strFind;
  strMsg += "\" found";

  QMessageBox::information(this, "No More Matches", strMsg, 
			   QMessageBox::Ok, QMessageBox::NoButton);
  invalidateFind();
  return 0;
}

void MdsEditView::shiftFocusToListView(int index)
{
  QListViewItemIterator it(m_listView);
  for(;it.current() != NULL & index > 0; index--,it++);
  
  QListViewItem *item = it.current();
  if (!item) return;

  m_listView->setSelected(item, TRUE);
  m_listView->setFocus();
}

void MdsEditView::expandTreeToShowItem(QListViewItem *it)
{
  it = it->parent();
  while(it)
  {
    it->setOpen(TRUE);
    it = it->parent();
  }
}

void MdsEditView::invalidateFind()
{
  if (m_searchIterator)
  {
    delete m_searchIterator;
    m_searchIterator = NULL;
  }
  m_searchRoot = NULL;
  m_iMatch = -1;
}

void MdsEditView::slotFindAction()
{
  findFirst();
}

void MdsEditView::slotFindNextAction()
{
  findNext();
}
