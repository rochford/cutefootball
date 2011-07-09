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
#include "halfstatisticsframe.h"
#include "ui_mainwindow.h"

HalfStatisticsFrame::HalfStatisticsFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::HalfStatisticsFrame)
{
    ui->setupUi(this);

    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    /*
    connect(ui->continueBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow->actionContinue,
            SLOT(trigger()));
            */
    connect(ui->mainMenuBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow->actionMainMenu,
            SLOT(trigger()));
}

HalfStatisticsFrame::~HalfStatisticsFrame()
{
    delete ui;
}

void HalfStatisticsFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "HalfStatisticsFrame::showFrame" << f;
    if ( f == MWindow::HalfTimeStatistics )
        showMaximized();
    else
        setVisible(false);
}
