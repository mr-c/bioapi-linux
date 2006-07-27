#include "stdafx.h"
#include <qapplication.h>
#include "MdsEditMainWnd.h"
#include "MdsEditDoc.h"
#include "MdsEditView.h"

QMdsEditDoc *g_doc = NULL;
MdsEditView *g_view = NULL;

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );

    g_doc = new QMdsEditDoc();
    g_view = new MdsEditView;
    g_view->show();
    g_view->refresh();

    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

    int result =  a.exec();
    delete g_doc;

    return result;
}
