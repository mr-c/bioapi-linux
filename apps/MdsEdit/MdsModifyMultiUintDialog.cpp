/****************************************************************************
** Form implementation generated from reading ui file 'MdsModifyMultiUintDialog.ui'
**
** Created: Mon Jun 13 00:52:56 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "MdsModifyMultiUintDialog.h"

#include <qvariant.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a MdsModifyMultiUintDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MdsModifyMultiUintDialog::MdsModifyMultiUintDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MdsModifyMultiUintDialog" );
    setIcon( QPixmap::fromMimeSource( "mdsedit.png" ) );

    m_attrName = new QLineEdit( this, "m_attrName" );
    m_attrName->setEnabled( FALSE );
    m_attrName->setGeometry( QRect( 20, 30, 470, 27 ) );
    m_attrName->setReadOnly( TRUE );

    m_cancelButton = new QPushButton( this, "m_cancelButton" );
    m_cancelButton->setGeometry( QRect( 380, 400, 108, 26 ) );

    m_attrValue3 = new QLineEdit( this, "m_attrValue3" );
    m_attrValue3->setGeometry( QRect( 40, 180, 450, 27 ) );

    m_attrValue4 = new QLineEdit( this, "m_attrValue4" );
    m_attrValue4->setGeometry( QRect( 40, 210, 450, 27 ) );

    m_attrValue5 = new QLineEdit( this, "m_attrValue5" );
    m_attrValue5->setGeometry( QRect( 40, 240, 450, 27 ) );

    m_attrValue6 = new QLineEdit( this, "m_attrValue6" );
    m_attrValue6->setGeometry( QRect( 40, 270, 450, 27 ) );

    m_attrValue7 = new QLineEdit( this, "m_attrValue7" );
    m_attrValue7->setGeometry( QRect( 40, 300, 450, 27 ) );

    m_attrValue8 = new QLineEdit( this, "m_attrValue8" );
    m_attrValue8->setGeometry( QRect( 40, 330, 450, 27 ) );

    m_attrValue9 = new QLineEdit( this, "m_attrValue9" );
    m_attrValue9->setGeometry( QRect( 40, 360, 450, 27 ) );

    m_attrValue1 = new QLineEdit( this, "m_attrValue1" );
    m_attrValue1->setGeometry( QRect( 40, 120, 450, 27 ) );

    m_attrValue2 = new QLineEdit( this, "m_attrValue2" );
    m_attrValue2->setGeometry( QRect( 40, 150, 450, 27 ) );

    m_attrValue0 = new QLineEdit( this, "m_attrValue0" );
    m_attrValue0->setGeometry( QRect( 40, 90, 450, 27 ) );

    m_okButton = new QPushButton( this, "m_okButton" );
    m_okButton->setGeometry( QRect( 260, 400, 108, 26 ) );
    m_okButton->setDefault( TRUE );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 20, 70, 248, 20 ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 20, 10, 120, 20 ) );
    TextLabel1->setFrameShape( QLabel::MShape );
    TextLabel1->setFrameShadow( QLabel::MShadow );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 12, 100, 30, 20 ) );

    TextLabel3_2 = new QLabel( this, "TextLabel3_2" );
    TextLabel3_2->setGeometry( QRect( 12, 130, 30, 20 ) );

    TextLabel3_3 = new QLabel( this, "TextLabel3_3" );
    TextLabel3_3->setGeometry( QRect( 12, 160, 30, 20 ) );

    TextLabel3_4 = new QLabel( this, "TextLabel3_4" );
    TextLabel3_4->setGeometry( QRect( 12, 190, 30, 20 ) );

    TextLabel3_5 = new QLabel( this, "TextLabel3_5" );
    TextLabel3_5->setGeometry( QRect( 12, 220, 30, 20 ) );

    TextLabel3_6 = new QLabel( this, "TextLabel3_6" );
    TextLabel3_6->setGeometry( QRect( 12, 250, 30, 20 ) );

    TextLabel3_7 = new QLabel( this, "TextLabel3_7" );
    TextLabel3_7->setGeometry( QRect( 12, 280, 30, 20 ) );

    TextLabel3_8 = new QLabel( this, "TextLabel3_8" );
    TextLabel3_8->setGeometry( QRect( 12, 310, 30, 20 ) );

    TextLabel3_9 = new QLabel( this, "TextLabel3_9" );
    TextLabel3_9->setGeometry( QRect( 12, 340, 30, 20 ) );

    TextLabel3_10 = new QLabel( this, "TextLabel3_10" );
    TextLabel3_10->setGeometry( QRect( 12, 370, 30, 20 ) );
    languageChange();
    resize( QSize(506, 439).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( m_cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( m_okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

    // tab order
    setTabOrder( m_attrValue0, m_attrValue1 );
    setTabOrder( m_attrValue1, m_attrValue2 );
    setTabOrder( m_attrValue2, m_attrValue3 );
    setTabOrder( m_attrValue3, m_attrValue4 );
    setTabOrder( m_attrValue4, m_attrValue5 );
    setTabOrder( m_attrValue5, m_attrValue6 );
    setTabOrder( m_attrValue6, m_attrValue7 );
    setTabOrder( m_attrValue7, m_attrValue8 );
    setTabOrder( m_attrValue8, m_attrValue9 );
    setTabOrder( m_attrValue9, m_okButton );
    setTabOrder( m_okButton, m_cancelButton );
    setTabOrder( m_cancelButton, m_attrName );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MdsModifyMultiUintDialog::~MdsModifyMultiUintDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MdsModifyMultiUintDialog::languageChange()
{
    setCaption( tr( "Modify Multiple Uint32s" ) );
    m_cancelButton->setText( tr( "Cancel" ) );
    m_okButton->setText( tr( "OK" ) );
    TextLabel2->setText( tr( "Attribute Values (in hexadecimal):" ) );
    TextLabel1->setText( tr( "Attribute Name:" ) );
    TextLabel3->setText( tr( "0x" ) );
    TextLabel3_2->setText( tr( "0x" ) );
    TextLabel3_3->setText( tr( "0x" ) );
    TextLabel3_4->setText( tr( "0x" ) );
    TextLabel3_5->setText( tr( "0x" ) );
    TextLabel3_6->setText( tr( "0x" ) );
    TextLabel3_7->setText( tr( "0x" ) );
    TextLabel3_8->setText( tr( "0x" ) );
    TextLabel3_9->setText( tr( "0x" ) );
    TextLabel3_10->setText( tr( "0x" ) );
}

