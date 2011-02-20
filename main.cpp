#include <QtGui/QApplication>
#include "mainwindow.h"

void loadStyleSheet(QApplication& app);
void loadTranslations(QApplication& app);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen* splash = new QSplashScreen;
    QPixmap px(":/images/soccer.svg");
    splash->showFullScreen();
    splash->setPixmap(px.scaled(QSize(splash->width(),splash->height())));

    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    splash->showMessage(QObject::tr("Loading..."), topRight);

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
