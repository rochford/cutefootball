#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QFile f(":/stylesheet.qss");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = f.readAll();
    f.close();

    QApplication a(argc, argv);
    a.setStyleSheet(QString(data));

    QString locale("soccer_" + QLocale::system().name());
    QTranslator appTranslator;
    appTranslator.load(locale, ":/translations/");
    a.installTranslator(&appTranslator);

    MWindow window;
    window.showMaximized();

    return a.exec();
}


