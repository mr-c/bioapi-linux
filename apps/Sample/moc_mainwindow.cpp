/****************************************************************************
** QSampleMainWnd meta object code from reading C++ file 'mainwindow.h'
**
** Created: Sun Oct 21 22:51:36 2012
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "mainwindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QSampleMainWnd::className() const
{
    return "QSampleMainWnd";
}

QMetaObject *QSampleMainWnd::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QSampleMainWnd( "QSampleMainWnd", &QSampleMainWnd::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QSampleMainWnd::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QSampleMainWnd", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QSampleMainWnd::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QSampleMainWnd", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QSampleMainWnd::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"slotVerifyButtonPressed", 0, 0 };
    static const QUMethod slot_1 = {"slotEnrollButtonPressed", 0, 0 };
    static const QUParameter param_slot_2[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"slotSelectedBSPChanged", 1, param_slot_2 };
    static const QUMethod slot_3 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "slotVerifyButtonPressed()", &slot_0, QMetaData::Public },
	{ "slotEnrollButtonPressed()", &slot_1, QMetaData::Public },
	{ "slotSelectedBSPChanged(int)", &slot_2, QMetaData::Public },
	{ "languageChange()", &slot_3, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"QSampleMainWnd", parentObject,
	slot_tbl, 4,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QSampleMainWnd.setMetaObject( metaObj );
    return metaObj;
}

void* QSampleMainWnd::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QSampleMainWnd" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool QSampleMainWnd::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: slotVerifyButtonPressed(); break;
    case 1: slotEnrollButtonPressed(); break;
    case 2: slotSelectedBSPChanged((int)static_QUType_int.get(_o+1)); break;
    case 3: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QSampleMainWnd::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool QSampleMainWnd::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool QSampleMainWnd::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
