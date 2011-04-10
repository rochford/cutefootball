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
#include "ingamemenuframe.h"

InGameMenuFrame::InGameMenuFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::InGameMenuFrame),
    m_pitch(*(parent->pitch()))
{
    ui->setupUi(this);
    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));
    connect(ui->continueBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow.actionContinue,
            SLOT(trigger()));
    connect(ui->mainMenuBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow.actionMainMenu,
            SLOT(trigger()));
}

InGameMenuFrame::~InGameMenuFrame()
{
    delete ui;
}

void InGameMenuFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "InGameMenuFrame::showFrame " << f;
    if ( f == MWindow::InGameMenu ) {
        setHomeTeamGoals(m_pitch.homeTeam()->m_goals);
        setAwayTeamGoals(m_pitch.awayTeam()->m_goals);
        setHomeTeamFlag(m_pitch.homeTeam()->flag());
        setAwayTeamFlag(m_pitch.awayTeam()->flag());
        showMaximized();
    } else
        setVisible(false);
}
