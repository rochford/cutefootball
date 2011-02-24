#include <QtGui/QApplication>
#include "mainwindow.h"

void loadStyleSheet(QApplication& app);
void loadTranslations(QApplication& app);

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

    loadTranslations(a);
    loadStyleSheet(a);

    MWindow window;
    window.showMaximized();
    splash->finish(&window);
    delete splash;

    return a.exec();
}

void loadStyleSheet(QApplication& app)
{
    QFile f(":/stylesheet.qss");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = f.readAll();
    f.close();

    app.setStyleSheet(QString(data));
}

void loadTranslations(QApplication& app)
{
    QString locale("soccer_" + QLocale::system().name());
    QTranslator appTranslator;
    appTranslator.load(locale, ":/translations/");
    app.installTranslator(&appTranslator);
}
