/****************************************************************************
** Form implementation generated from reading ui file 'MdsModifyUintDialog.ui'
**
** Created: Mon Jun 13 00:52:56 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "MdsModifyUintDialog.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a MdsModifyUintDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MdsModifyUintDialog::MdsModifyUintDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MdsModifyUintDialog" );
    setIcon( QPixmap::fromMimeSource( "mdsedit.png" ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 20, 10, 120, 20 ) );

    m_attrName = new QLineEdit( this, "m_attrName" );
    m_attrName->setEnabled( FALSE );
    m_attrName->setGeometry( QRect( 20, 30, 470, 27 ) );
    m_attrName->setReadOnly( TRUE );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 20, 70, 240, 20 ) );

    m_cancelButton = new QPushButton( this, "m_cancelButton" );
    m_cancelButton->setGeometry( QRect( 380, 130, 108, 26 ) );

    m_attrValue = new QLineEdit( this, "m_attrValue" );
    m_attrValue->setGeometry( QRect( 40, 90, 450, 27 ) );

    m_okButton = new QPushButton( this, "m_okButton" );
    m_okButton->setGeometry( QRect( 260, 130, 108, 26 ) );
    m_okButton->setDefault( TRUE );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 12, 100, 30, 20 ) );
    languageChange();
    resize( QSize(507, 166).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( m_cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( m_okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

    // tab order
    setTabOrder( m_attrName, m_attrValue );
    setTabOrder( m_attrValue, m_okButton );
    setTabOrder( m_okButton, m_cancelButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MdsModifyUintDialog::~MdsModifyUintDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MdsModifyUintDialog::languageChange()
{
    setCaption( tr( "Modify Uint32" ) );
    TextLabel1->setText( tr( "Attribute Name:" ) );
    TextLabel2->setText( tr( "Attribute Value (in hexadecimal):" ) );
    m_cancelButton->setText( tr( "Cancel" ) );
    m_okButton->setText( tr( "OK" ) );
    TextLabel3->setText( tr( "0x" ) );
}

