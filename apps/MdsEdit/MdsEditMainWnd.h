/****************************************************************************
** Form interface generated from reading ui file 'MdsEditMainWnd.ui'
**
** Created: Mon Jun 13 00:44:58 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef M_MAINWND_H
#define M_MAINWND_H

#include <qvariant.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QSplitter;
class QListView;
class QListViewItem;

class MdsEditMainWnd : public QMainWindow
{
    Q_OBJECT

public:
    MdsEditMainWnd( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~MdsEditMainWnd();

    QSplitter* m_splitter;
    QListView* m_treeView;
    QListView* m_listView;
    QMenuBar *menubar;
    QPopupMenu *mdsMenu;
    QPopupMenu *editMenu;
    QPopupMenu *helpMenu;
    QAction* mdsExitAction;
    QAction* findAction;
    QAction* helpAboutAction;
    QAction* importMDSFileAction;
    QAction* exportMDSFileAction;
    QAction* importBlobAction;
    QAction* exportBlobAction;
    QAction* modifyAttributeAction;
    QAction* newRecordAction;
    QAction* deleteAction;
    QAction* findNextAction;

public slots:
    virtual void helpAbout();
    virtual void slotAttributeDoubleClicked(QListViewItem *);
    virtual void slotAttrSelectionChanged(QListViewItem *);
    virtual void slotFindAction();
    virtual void slotFindNextAction();
    virtual void slotModifyAttributeAction();
    virtual void slotNewRecordAction();
    virtual void slotSelectionChanged(QListViewItem *);

protected:
    QHBoxLayout* m_mainWndLayout;

protected slots:
    virtual void languageChange();

};

#endif // M_MAINWND_H
