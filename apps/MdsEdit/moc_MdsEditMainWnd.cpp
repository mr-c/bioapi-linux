/****************************************************************************
** MdsEditMainWnd meta object code from reading C++ file 'MdsEditMainWnd.h'
**
** Created: Mon Jun 13 01:15:18 2005
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.4   edited Jan 21 18:14 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "MdsEditMainWnd.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MdsEditMainWnd::className() const
{
    return "MdsEditMainWnd";
}

QMetaObject *MdsEditMainWnd::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MdsEditMainWnd( "MdsEditMainWnd", &MdsEditMainWnd::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MdsEditMainWnd::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MdsEditMainWnd", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MdsEditMainWnd::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MdsEditMainWnd", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MdsEditMainWnd::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"helpAbout", 0, 0 };
    static const QUParameter param_slot_1[] = {
	{ 0, &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_1 = {"slotAttributeDoubleClicked", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ 0, &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_2 = {"slotAttrSelectionChanged", 1, param_slot_2 };
    static const QUMethod slot_3 = {"slotFindAction", 0, 0 };
    static const QUMethod slot_4 = {"slotFindNextAction", 0, 0 };
    static const QUMethod slot_5 = {"slotModifyAttributeAction", 0, 0 };
    static const QUMethod slot_6 = {"slotNewRecordAction", 0, 0 };
    static const QUParameter param_slot_7[] = {
	{ 0, &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_7 = {"slotSelectionChanged", 1, param_slot_7 };
    static const QUMethod slot_8 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "helpAbout()", &slot_0, QMetaData::Public },
	{ "slotAttributeDoubleClicked(QListViewItem*)", &slot_1, QMetaData::Public },
	{ "slotAttrSelectionChanged(QListViewItem*)", &slot_2, QMetaData::Public },
	{ "slotFindAction()", &slot_3, QMetaData::Public },
	{ "slotFindNextAction()", &slot_4, QMetaData::Public },
	{ "slotModifyAttributeAction()", &slot_5, QMetaData::Public },
	{ "slotNewRecordAction()", &slot_6, QMetaData::Public },
	{ "slotSelectionChanged(QListViewItem*)", &slot_7, QMetaData::Public },
	{ "languageChange()", &slot_8, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"MdsEditMainWnd", parentObject,
	slot_tbl, 9,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MdsEditMainWnd.setMetaObject( metaObj );
    return metaObj;
}

void* MdsEditMainWnd::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MdsEditMainWnd" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool MdsEditMainWnd::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: helpAbout(); break;
    case 1: slotAttributeDoubleClicked((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    case 2: slotAttrSelectionChanged((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    case 3: slotFindAction(); break;
    case 4: slotFindNextAction(); break;
    case 5: slotModifyAttributeAction(); break;
    case 6: slotNewRecordAction(); break;
    case 7: slotSelectionChanged((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    case 8: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool MdsEditMainWnd::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MdsEditMainWnd::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool MdsEditMainWnd::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
