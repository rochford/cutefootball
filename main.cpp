#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MWindow w; 
#ifdef Q_OS_SYMBIAN
    w.showMaximized();
#else
    w.show();
#endif

    return a.exec();
}
