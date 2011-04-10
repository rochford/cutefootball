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
#include "screengraphics.h"

#include "pitch.h"
#include "team.h"

ScreenGraphics::ScreenGraphics(Pitch *p)
  :  QFrame(NULL),
    m_pitch(p),
    ui(new Ui::ScreenGraphicsFrame)
{
    ui->setupUi(this);
}

ScreenGraphics::~ScreenGraphics()
{
//    delete m_scoreText;
}

void ScreenGraphics::update(QString s)
{
    resize(160,50);
    ui->scoreTextLabel->setVisible(true);

    ui->scoreTextLabel->setText(s);

    ui->SGFhomeTeamName->setText(/* m_pitch->homeTeam()->briefName() + " " + */ QString::number(m_pitch->homeTeam()->m_goals));
//    ui->SGFhomeGoals->setText("" /* QString::number(m_pitch->homeTeam()->m_goals) */);
    ui->SGFawayTeamName->setText(/* m_pitch->awayTeam()->briefName()+ " " + */ QString::number(m_pitch->awayTeam()->m_goals));
//    ui->SGFawayGoals->setText("" /* + QString::number(m_pitch->awayTeam()->m_goals) */);
}

void ScreenGraphics::setTeams(Team* home, Team* away)
{
    ui->SGFhomeTeamFlag->setPixmap(
                QPixmap(home->flag()).scaled(20,15));
    ui->SGFhomeTeamName->setText(home->briefName());
    ui->SGFawayTeamFlag->setPixmap(
                QPixmap(away->flag()).scaled(20,15));
    ui->SGFawayTeamName->setText(away->briefName());
}

