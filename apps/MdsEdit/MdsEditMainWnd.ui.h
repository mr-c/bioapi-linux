/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

#include "stdafx.h"
#include <QMDS.h>
#include "MDSEditAboutDlg.h"
#include "MdsEditDoc.h"

void MdsEditMainWnd::helpAbout()
{
    MdsEditAboutDlg dlg(this);
    dlg.exec();
}



void MdsEditMainWnd::slotSelectionChanged( QListViewItem *item )
{

}


void MdsEditMainWnd::slotAttrSelectionChanged( QListViewItem * )
{

}

void MdsEditMainWnd::slotModifyAttributeAction()
{

}

void MdsEditMainWnd::slotAttributeDoubleClicked( QListViewItem * )
{

}



void MdsEditMainWnd::slotNewRecordAction()
{

}

void MdsEditMainWnd::slotFindAction()
{

}

void MdsEditMainWnd::slotFindNextAction()
{

}
