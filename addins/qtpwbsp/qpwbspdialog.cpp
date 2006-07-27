
#include "qpwbspdialog.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static QPixmap uic_load_pixmap_QpwbspDialog( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a QpwbspDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
QpwbspDialog::QpwbspDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "QpwbspDialog" );
    resize( 293, 41 ); 
    setCaption( trUtf8( "Enter password" ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 10, 65, 20 ) ); 
    TextLabel1->setText( trUtf8( "Password:" ) );

    m_okButton = new QPushButton( this, "m_okButton" );
    m_okButton->setGeometry( QRect( 244, 10, 40, 22 ) ); 
    m_okButton->setText( trUtf8( "Ok" ) );

    m_password = new QLineEdit( this, "m_password" );
    m_password->setGeometry( QRect( 80, 10, 150, 22 ) ); 
    m_password->setEchoMode( QLineEdit::Password );

    // signals and slots connections
    connect( m_okButton, SIGNAL( pressed() ), this, SLOT( accept() ) );
    connect( m_password, SIGNAL( returnPressed() ), this, SLOT( accept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
QpwbspDialog::~QpwbspDialog()
{
    // no need to delete child widgets, Qt does it all for us
}


#include "moc_qpwbspdialog.cpp"

extern "C"
int DoPasswordDialog(char *password, int maxPasswordLen) {

	QpwbspDialog *w = new QpwbspDialog;
	int result = 0;

	if (w->exec() == QDialog::Accepted) {
		strncpy(password, w->m_password->text(), maxPasswordLen);
		result = 1;
	} else {
		password[0] = '\0';
		result = 0;
	}

	delete w;
	return result;

}
