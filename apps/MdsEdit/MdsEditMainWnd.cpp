/****************************************************************************
** Form implementation generated from reading ui file 'MdsEditMainWnd.ui'
**
** Created: Mon Jun 13 00:52:56 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "MdsEditMainWnd.h"

#include <qvariant.h>
#include <qsplitter.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "MdsEditMainWnd.ui.h"
/*
 *  Constructs a MdsEditMainWnd as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
MdsEditMainWnd::MdsEditMainWnd( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "m_mainWnd" );
    setIcon( QPixmap::fromMimeSource( "mdsedit.png" ) );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    m_mainWndLayout = new QHBoxLayout( centralWidget(), 1, 1, "m_mainWndLayout"); 

    m_splitter = new QSplitter( centralWidget(), "m_splitter" );
    m_splitter->setFrameShape( QSplitter::Box );
    m_splitter->setOrientation( QSplitter::Horizontal );

    m_treeView = new QListView( m_splitter, "m_treeView" );
    m_treeView->addColumn( QString::null );
    m_treeView->header()->setClickEnabled( FALSE, m_treeView->header()->count() - 1 );
    m_treeView->header()->setResizeEnabled( FALSE, m_treeView->header()->count() - 1 );
    m_treeView->setFrameShape( QListView::Panel );
    m_treeView->setFrameShadow( QListView::Sunken );
    m_treeView->setResizePolicy( QListView::AutoOneFit );
    m_treeView->setRootIsDecorated( TRUE );
    m_treeView->setResizeMode( QListView::AllColumns );

    m_listView = new QListView( m_splitter, "m_listView" );
    m_listView->addColumn( tr( "Attribute Name" ) );
    m_listView->addColumn( tr( "Data" ) );
    m_listView->addColumn( tr( "Length" ) );
    m_listView->setResizePolicy( QListView::AutoOneFit );
    m_listView->setAllColumnsShowFocus( TRUE );
    m_listView->setResizeMode( QListView::AllColumns );
    m_mainWndLayout->addWidget( m_splitter );

    // actions
    mdsExitAction = new QAction( this, "mdsExitAction" );
    mdsExitAction->setAccel( 0 );
    findAction = new QAction( this, "findAction" );
    findAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "searchfind" ) ) );
    findAction->setAccel( 4194374 );
    helpAboutAction = new QAction( this, "helpAboutAction" );
    helpAboutAction->setAccel( 0 );
    importMDSFileAction = new QAction( this, "importMDSFileAction" );
    importMDSFileAction->setEnabled( FALSE );
    exportMDSFileAction = new QAction( this, "exportMDSFileAction" );
    exportMDSFileAction->setEnabled( FALSE );
    importBlobAction = new QAction( this, "importBlobAction" );
    importBlobAction->setEnabled( FALSE );
    exportBlobAction = new QAction( this, "exportBlobAction" );
    exportBlobAction->setEnabled( FALSE );
    modifyAttributeAction = new QAction( this, "modifyAttributeAction" );
    modifyAttributeAction->setEnabled( FALSE );
    newRecordAction = new QAction( this, "newRecordAction" );
    newRecordAction->setEnabled( FALSE );
    deleteAction = new QAction( this, "deleteAction" );
    deleteAction->setEnabled( FALSE );
    deleteAction->setAccel( 0 );
    findNextAction = new QAction( this, "findNextAction" );
    findNextAction->setEnabled( TRUE );
    findNextAction->setAccel( 4146 );


    // toolbars


    // menubar
    menubar = new QMenuBar( this, "menubar" );


    mdsMenu = new QPopupMenu( this );
    importMDSFileAction->addTo( mdsMenu );
    exportMDSFileAction->addTo( mdsMenu );
    mdsMenu->insertSeparator();
    mdsExitAction->addTo( mdsMenu );
    menubar->insertItem( QString(""), mdsMenu, 1 );

    editMenu = new QPopupMenu( this );
    importBlobAction->addTo( editMenu );
    exportBlobAction->addTo( editMenu );
    editMenu->insertSeparator();
    modifyAttributeAction->addTo( editMenu );
    newRecordAction->addTo( editMenu );
    deleteAction->addTo( editMenu );
    editMenu->insertSeparator();
    findAction->addTo( editMenu );
    findNextAction->addTo( editMenu );
    menubar->insertItem( QString(""), editMenu, 2 );

    helpMenu = new QPopupMenu( this );
    helpAboutAction->addTo( helpMenu );
    menubar->insertItem( QString(""), helpMenu, 3 );

    languageChange();
    resize( QSize(743, 533).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );
    connect( mdsExitAction, SIGNAL( activated() ), this, SLOT( close() ) );
    connect( m_treeView, SIGNAL( selectionChanged(QListViewItem*) ), this, SLOT( slotSelectionChanged(QListViewItem*) ) );
    connect( modifyAttributeAction, SIGNAL( activated() ), this, SLOT( slotModifyAttributeAction() ) );
    connect( m_listView, SIGNAL( doubleClicked(QListViewItem*) ), this, SLOT( slotAttributeDoubleClicked(QListViewItem*) ) );
    connect( m_listView, SIGNAL( selectionChanged(QListViewItem*) ), this, SLOT( slotAttrSelectionChanged(QListViewItem*) ) );
    connect( newRecordAction, SIGNAL( activated() ), this, SLOT( slotNewRecordAction() ) );
    connect( findAction, SIGNAL( activated() ), this, SLOT( slotFindAction() ) );
    connect( findNextAction, SIGNAL( activated() ), this, SLOT( slotFindNextAction() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MdsEditMainWnd::~MdsEditMainWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MdsEditMainWnd::languageChange()
{
    setCaption( tr( "MDS Editor" ) );
    m_treeView->header()->setLabel( 0, QString::null );
    m_treeView->clear();
    QListViewItem * item = new QListViewItem( m_treeView, 0 );
    item->setPixmap( 0, QPixmap::fromMimeSource( "root.png" ) );

    m_listView->header()->setLabel( 0, tr( "Attribute Name" ) );
    m_listView->header()->setLabel( 1, tr( "Data" ) );
    m_listView->header()->setLabel( 2, tr( "Length" ) );
    mdsExitAction->setText( tr( "Exit" ) );
    mdsExitAction->setMenuText( tr( "E&xit" ) );
    findAction->setText( tr( "Find" ) );
    findAction->setMenuText( tr( "&Find..." ) );
    helpAboutAction->setText( tr( "About" ) );
    helpAboutAction->setMenuText( tr( "&About..." ) );
    importMDSFileAction->setText( tr( "&Import MDS File..." ) );
    importMDSFileAction->setMenuText(tr( "&Import MDS File..." ));
    exportMDSFileAction->setText( tr( "&Export MDS File..." ) );
    exportMDSFileAction->setMenuText(tr( "&Export MDS File..." ));
    importBlobAction->setText( tr( "&Import Blob Attribute..." ) );
    importBlobAction->setMenuText(tr( "&Import Blob Attribute..." ));
    exportBlobAction->setText( tr( "&Export Blob Attribute..." ) );
    exportBlobAction->setMenuText(tr( "&Export Blob Attribute..." ));
    modifyAttributeAction->setText( tr( "&Modify Attribute..." ) );
    modifyAttributeAction->setMenuText(tr( "&Modify Attribute..." ));
    newRecordAction->setText( tr( "&New Record" ) );
    newRecordAction->setMenuText(tr( "&New Record" ));
    deleteAction->setText( tr( "Delete" ) );
    deleteAction->setMenuText(tr( "Delete" ));
    findNextAction->setText( tr( "Find Next" ) );
    findNextAction->setMenuText(tr( "Find Next" ));
    if (menubar->findItem(1))
        menubar->findItem(1)->setText( tr( "&MDS" ) );
    if (menubar->findItem(2))
        menubar->findItem(2)->setText( tr( "&Edit" ) );
    if (menubar->findItem(3))
        menubar->findItem(3)->setText( tr( "&Help" ) );
}

