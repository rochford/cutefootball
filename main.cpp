/*
 * Copyright 2010,2011 Timothy Rochford
 *
 *    This file is part of CuteFootball.
 *
 *    CuteFootball is free software: you can redistribute it and/or modify
 *    it under the terms of the Lesser GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    CuteFootball is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Lesser GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <QtGui/QApplication>
#include "mainwindow.h"

void loadStyleSheet(QApplication& app);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen* splash = new QSplashScreen;
#if defined(Q_OS_SYMBIAN)
    splash->showFullScreen();
#else
    splash->show();
#endif

    QSize screenSize(splash->size());
    if (screenSize == QSize(240,320)) {
        splash->setPixmap(QPixmap(":/images/splash240x320.svg"));
    } else if (screenSize == QSize(320,240)) {
        splash->setPixmap(QPixmap(":/images/splash320x240.svg"));
    } else if (screenSize.width() > screenSize.height()) {
        QPixmap px = QPixmap(":/images/splash320x240.svg");
        splash->setPixmap(px.scaled(QSize(splash->width(),splash->height())));
    } else if (screenSize.width() < screenSize.height()) {
        QPixmap px = QPixmap(":/images/splash240x320.svg");
        splash->setPixmap(px.scaled(screenSize));
    } else {
        QPixmap px = QPixmap(":/images/splash100x100.svg");
        splash->setPixmap(px.scaled(screenSize));
    }

    loadStyleSheet(a);

    QString locale(QString("soccer_") + QLocale::system().name());
    QTranslator appTranslator;

    appTranslator.load(locale, ":/translations/");
    a.installTranslator(&appTranslator);

    MWindow window;
#if defined(Q_OS_SYMBIAN)
    window.showFullScreen();
#else
    window.show();
#endif
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
