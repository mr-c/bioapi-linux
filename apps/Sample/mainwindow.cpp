/****************************************************************************
** Form implementation generated from reading ui file 'mainwindow.ui'
**
** Created: Mon Jun 13 00:17:34 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "mainwindow.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "mainwindow.ui.h"
/*
 *  Constructs a QSampleMainWnd as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
QSampleMainWnd::QSampleMainWnd( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "mainWindow" );
    setIcon( QPixmap::fromMimeSource( "icon.png" ) );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );

    GroupBox2 = new QGroupBox( centralWidget(), "GroupBox2" );
    GroupBox2->setGeometry( QRect( 10, 10, 271, 111 ) );

    TextLabel1 = new QLabel( GroupBox2, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 20, 130, 20 ) );

    TextLabel2 = new QLabel( GroupBox2, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 10, 80, 100, 20 ) );

    m_handleLabel = new QLabel( GroupBox2, "m_handleLabel" );
    m_handleLabel->setGeometry( QRect( 140, 79, 80, 20 ) );

    m_bspCombo = new QComboBox( FALSE, GroupBox2, "m_bspCombo" );
    m_bspCombo->setGeometry( QRect( 10, 50, 250, 20 ) );

    m_okButton = new QPushButton( centralWidget(), "m_okButton" );
    m_okButton->setGeometry( QRect( 300, 20, 120, 22 ) );
    m_okButton->setDefault( FALSE );

    GroupBox3 = new QGroupBox( centralWidget(), "GroupBox3" );
    GroupBox3->setGeometry( QRect( 10, 130, 271, 130 ) );

    TextLabel4 = new QLabel( GroupBox3, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 10, 20, 65, 20 ) );

    m_userID = new QLineEdit( GroupBox3, "m_userID" );
    m_userID->setGeometry( QRect( 10, 40, 250, 22 ) );

    m_enrollButton = new QPushButton( GroupBox3, "m_enrollButton" );
    m_enrollButton->setGeometry( QRect( 10, 70, 250, 22 ) );

    m_verifyButton = new QPushButton( GroupBox3, "m_verifyButton" );
    m_verifyButton->setGeometry( QRect( 10, 100, 250, 22 ) );

    // toolbars

    languageChange();
    resize( QSize(435, 281).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( m_okButton, SIGNAL( clicked() ), this, SLOT( close() ) );
    connect( m_enrollButton, SIGNAL( clicked() ), this, SLOT( slotEnrollButtonPressed() ) );
    connect( m_verifyButton, SIGNAL( clicked() ), this, SLOT( slotVerifyButtonPressed() ) );
    connect( m_bspCombo, SIGNAL( activated(int) ), this, SLOT( slotSelectedBSPChanged(int) ) );

    // tab order
    setTabOrder( m_bspCombo, m_userID );
    setTabOrder( m_userID, m_enrollButton );
    setTabOrder( m_enrollButton, m_verifyButton );
    setTabOrder( m_verifyButton, m_okButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
QSampleMainWnd::~QSampleMainWnd()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QSampleMainWnd::languageChange()
{
    setCaption( tr( "BioAPI Sample Application" ) );
    GroupBox2->setTitle( tr( "Technologies" ) );
    TextLabel1->setText( tr( "Installed Biometrics" ) );
    TextLabel2->setText( tr( "Current Handle:" ) );
    m_handleLabel->setText( tr( "0x00000000" ) );
    m_okButton->setText( tr( "Ok" ) );
    GroupBox3->setTitle( tr( "Authentication" ) );
    TextLabel4->setText( tr( "User Id" ) );
    m_enrollButton->setText( tr( "Enroll" ) );
    m_verifyButton->setText( tr( "Verify" ) );
}

