#include <qapplication.h>
#include "QSampleMainWndImpl.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    QSampleMainWndImpl *w = new QSampleMainWndImpl;
    w->show();
    w->initializeBioAPI();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
