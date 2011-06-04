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
    loadStyleSheet(a);

    QString locale(QString("soccer_") + QLocale::system().name());
    QTranslator appTranslator;

    appTranslator.load(locale, ":/translations/");
    a.installTranslator(&appTranslator);

    MWindow window;
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
#if defined(SOCCER_VGA)
    window.setOrientation(MWindow::ScreenOrientationLockLandscape);
#endif // defined(SOCCER_VGA)
    window.showFullScreen();
#else
    window.show();
#endif

    return a.exec();
}

void loadStyleSheet(QApplication& app)
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
#if defined(SOCCER_VGA)
    QFile f(":/mobileVGA.qss");
#else
    QFile f(":/mobileQVGA.qss");
#endif
#else
    QFile f(":/desktop.qss");
#endif
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = f.readAll();
    f.close();

    app.setStyleSheet(QString(data));
}
