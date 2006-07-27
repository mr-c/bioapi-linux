/****************************************************************************
** Form implementation generated from reading ui file 'MDSEditAboutDlg.ui'
**
** Created: Mon Jun 13 00:52:56 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "MDSEditAboutDlg.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a MdsEditAboutDlg as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MdsEditAboutDlg::MdsEditAboutDlg( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MdsEditAboutDlg" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)2, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 400, 260 ) );
    setMaximumSize( QSize( 400, 260 ) );
    setIcon( QPixmap::fromMimeSource( "mdsedit.png" ) );

    m_pixmapLabel1 = new QLabel( this, "m_pixmapLabel1" );
    m_pixmapLabel1->setGeometry( QRect( 30, 20, 32, 32 ) );
    m_pixmapLabel1->setPixmap( QPixmap::fromMimeSource( "mdsedit.png" ) );
    m_pixmapLabel1->setScaledContents( TRUE );

    m_groupBox = new QGroupBox( this, "m_groupBox" );
    m_groupBox->setGeometry( QRect( 30, 90, 340, 151 ) );

    PixmapLabel3 = new QLabel( m_groupBox, "PixmapLabel3" );
    PixmapLabel3->setGeometry( QRect( 10, 50, 22, 22 ) );
    PixmapLabel3->setPixmap( QPixmap::fromMimeSource( "uint.png" ) );
    PixmapLabel3->setScaledContents( TRUE );

    PixmapLabel4 = new QLabel( m_groupBox, "PixmapLabel4" );
    PixmapLabel4->setGeometry( QRect( 10, 80, 22, 22 ) );
    PixmapLabel4->setPixmap( QPixmap::fromMimeSource( "multiuint.png" ) );
    PixmapLabel4->setScaledContents( TRUE );

    PixmapLabel5 = new QLabel( m_groupBox, "PixmapLabel5" );
    PixmapLabel5->setGeometry( QRect( 10, 110, 22, 22 ) );
    PixmapLabel5->setPixmap( QPixmap::fromMimeSource( "blob.png" ) );
    PixmapLabel5->setScaledContents( TRUE );

    PixmapLabel2 = new QLabel( m_groupBox, "PixmapLabel2" );
    PixmapLabel2->setGeometry( QRect( 10, 20, 22, 22 ) );
    PixmapLabel2->setPixmap( QPixmap::fromMimeSource( "string.png" ) );
    PixmapLabel2->setScaledContents( TRUE );

    TextLabel4 = new QLabel( m_groupBox, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 50, 80, 90, 20 ) );

    TextLabel3 = new QLabel( m_groupBox, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 50, 50, 90, 20 ) );

    m_label2 = new QLabel( m_groupBox, "m_label2" );
    m_label2->setGeometry( QRect( 50, 20, 90, 20 ) );

    TextLabel5 = new QLabel( m_groupBox, "TextLabel5" );
    TextLabel5->setGeometry( QRect( 50, 110, 90, 20 ) );

    m_okButton = new QPushButton( this, "m_okButton" );
    m_okButton->setGeometry( QRect( 280, 20, 90, 30 ) );
    m_okButton->setDefault( TRUE );

    m_label1 = new QLabel( this, "m_label1" );
    m_label1->setGeometry( QRect( 70, 20, 190, 20 ) );

    TextLabel6 = new QLabel( this, "TextLabel6" );
    TextLabel6->setGeometry( QRect( 70, 40, 65, 20 ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 70, 60, 300, 20 ) );
    languageChange();
    resize( QSize(400, 260).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( m_okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MdsEditAboutDlg::~MdsEditAboutDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MdsEditAboutDlg::languageChange()
{
    setCaption( tr( "About MDS Editor" ) );
    m_groupBox->setTitle( tr( "Legend:" ) );
    PixmapLabel5->setText( QString::null );
    TextLabel4->setText( tr( "Multi-Uint32" ) );
    TextLabel3->setText( tr( "Uint32" ) );
    m_label2->setText( tr( "String" ) );
    TextLabel5->setText( tr( "Blob" ) );
    m_okButton->setText( tr( "OK" ) );
    m_label1->setText( tr( "MDS Edit Application for Unix" ) );
    TextLabel6->setText( tr( "Version 0.1" ) );
    TextLabel1->setText( tr( "(c) International Biometric Group, 2002" ) );
}

