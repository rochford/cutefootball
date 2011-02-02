#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile f(":/stylesheet.qss");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = f.readAll();
    f.close();
    a.setStyleSheet(QString(data));

    MWindow w; 

#ifdef Q_OS_SYMBIAN
    w.showMaximized();
#else
    w.show();
#endif

    return a.exec();
}
