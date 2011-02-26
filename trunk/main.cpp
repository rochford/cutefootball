#include <QtGui/QApplication>
#include "mainwindow.h"

void loadStyleSheet(QApplication& app);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen* splash = new QSplashScreen;
    splash->showFullScreen();

    if (splash->size() == QSize(240,320)) {
        splash->setPixmap(QPixmap(":/images/splash240x320.svg"));
    } else if (splash->size() == QSize(320,240)) {
        splash->setPixmap(QPixmap(":/images/splash320x240.svg"));
    } else {
        QPixmap px = QPixmap(":/images/splash100x100.svg");
        splash->setPixmap(px.scaled(QSize(splash->width(),splash->height())));
    }

    loadStyleSheet(a);

    QString locale(QString("soccer_") + QLocale::system().name());
    QTranslator appTranslator;

    appTranslator.load(locale, ":/translations/");
    a.installTranslator(&appTranslator);

    MWindow window;
    window.showMaximized();
    splash->finish(&window);
    delete splash;

    return a.exec();
}

void loadStyleSheet(QApplication& app)
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    QFile f(":/mobile.qss");
#else
    QFile f(":/desktop.qss");
#endif
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = f.readAll();
    f.close();

    app.setStyleSheet(QString(data));
}
