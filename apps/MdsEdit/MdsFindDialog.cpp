/****************************************************************************
** Form implementation generated from reading ui file 'MdsFindDialog.ui'
**
** Created: Mon Jun 13 00:52:56 2005
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "MdsFindDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a MdsFindDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MdsFindDialog::MdsFindDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MdsFindDialog" );
    setIcon( QPixmap::fromMimeSource( "searchfind" ) );

    m_cancelButton = new QPushButton( this, "m_cancelButton" );
    m_cancelButton->setGeometry( QRect( 380, 50, 108, 26 ) );

    m_okButton = new QPushButton( this, "m_okButton" );
    m_okButton->setGeometry( QRect( 380, 10, 108, 26 ) );
    m_okButton->setDefault( TRUE );

    m_scopeFlags = new QGroupBox( this, "m_scopeFlags" );
    m_scopeFlags->setGeometry( QRect( 10, 40, 350, 50 ) );

    m_useAttrName = new QCheckBox( m_scopeFlags, "m_useAttrName" );
    m_useAttrName->setGeometry( QRect( 10, 20, 150, 20 ) );

    m_useAttrValue = new QCheckBox( m_scopeFlags, "m_useAttrValue" );
    m_useAttrValue->setGeometry( QRect( 180, 20, 140, 20 ) );

    m_matchCase = new QCheckBox( this, "m_matchCase" );
    m_matchCase->setGeometry( QRect( 20, 120, 210, 20 ) );

    m_searchString = new QLineEdit( this, "m_searchString" );
    m_searchString->setGeometry( QRect( 70, 10, 290, 22 ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 10, 50, 20 ) );

    m_matchWholeString = new QCheckBox( this, "m_matchWholeString" );
    m_matchWholeString->setGeometry( QRect( 20, 100, 210, 20 ) );
    languageChange();
    resize( QSize(500, 148).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( m_okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( m_cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( m_searchString, SIGNAL( returnPressed() ), this, SLOT( accept() ) );

    // tab order
    setTabOrder( m_searchString, m_useAttrName );
    setTabOrder( m_useAttrName, m_useAttrValue );
    setTabOrder( m_useAttrValue, m_matchWholeString );
    setTabOrder( m_matchWholeString, m_matchCase );
    setTabOrder( m_matchCase, m_okButton );
    setTabOrder( m_okButton, m_cancelButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MdsFindDialog::~MdsFindDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MdsFindDialog::languageChange()
{
    setCaption( tr( "Find" ) );
    m_cancelButton->setText( tr( "Cancel" ) );
    m_okButton->setText( tr( "Find" ) );
    m_scopeFlags->setTitle( tr( "Examine:" ) );
    m_useAttrName->setText( tr( "Attribute Name" ) );
    m_useAttrValue->setText( tr( "Attribute Value" ) );
    m_matchCase->setText( tr( "Match case" ) );
    TextLabel1->setText( tr( "String:" ) );
    m_matchWholeString->setText( tr( "Match whole string only" ) );
}

