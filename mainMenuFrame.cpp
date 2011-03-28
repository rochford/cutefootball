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
#include "mainMenuFrame.h"
#include "ui_mainMenuFrame.h"
#include "version.h"

mainMenuFrame::mainMenuFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::mainMenuFrame)
{
    ui->setupUi(this);
    ui->versionLabel->setText(versionNumber.arg(KBuildNumber));
    ui->m_settingsBtn->setVisible(false);

    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(ui->actionNewGame, SIGNAL(triggered()),
            parent, SLOT(showTeamSelectionFrame()));
    connect(ui->actionSettings, SIGNAL(triggered()),
            parent, SLOT(showSettingsFrame()));
    connect(ui->actionInputSettings, SIGNAL(triggered()),
            parent, SLOT(showInputSettingsFrame()));
    connect(ui->actionHelp, SIGNAL(triggered()),
            parent,SLOT(showHelpFrame()));
    connect(ui->actionQuit, SIGNAL(triggered()),
            parent, SLOT(close()));
}

mainMenuFrame::~mainMenuFrame()
{
    delete ui;
}

void mainMenuFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "mainMenuFrame::showFrame" << f;
    if ( f == MWindow::MainMenu ) {
        ui->m_newGameBtn->setFocus();
        showMaximized();
    } else
        setVisible(false);
}

