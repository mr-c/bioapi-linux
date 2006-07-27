/****************************************************************************
** QpwbspDialog meta object code from reading C++ file 'qpwbspdialog.h'
**
** Created: Mon Jun 13 03:49:58 2005
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.4   edited Jan 21 18:14 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "qpwbspdialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QpwbspDialog::className() const
{
    return "QpwbspDialog";
}

QMetaObject *QpwbspDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QpwbspDialog( "QpwbspDialog", &QpwbspDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QpwbspDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QpwbspDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QpwbspDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QpwbspDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QpwbspDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"QpwbspDialog", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QpwbspDialog.setMetaObject( metaObj );
    return metaObj;
}

void* QpwbspDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QpwbspDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool QpwbspDialog::qt_invoke( int _id, QUObject* _o )
{
    return QDialog::qt_invoke(_id,_o);
}

bool QpwbspDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool QpwbspDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool QpwbspDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
