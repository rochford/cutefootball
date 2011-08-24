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

ScreenGraphics::ScreenGraphics(Pitch& p,
                               QStatusBar& bar)
  : m_pitch(p),
    m_type(ScoreText),
    m_bar(bar),
    m_SGFhomeTeamFlag(new QLabel()),
    m_SGFhomeTeamName(new QLabel()),
    m_SGFhomeTeamGoals(new QLabel()),
    m_SGFawayTeamFlag(new QLabel()),
    m_SGFawayTeamName(new QLabel()),
    m_SGFawayTeamGoals(new QLabel()),
    m_Time(new QLabel())
{
    m_SGFhomeTeamFlag->setObjectName("homeTeamFlag");
    m_SGFawayTeamFlag->setObjectName("awayTeamFlag");
    m_SGFhomeTeamName->setObjectName("homeTeamName");
    m_SGFawayTeamName->setObjectName("awayTeamName");
    m_SGFhomeTeamGoals->setObjectName("homeTeamGoals");
    m_SGFawayTeamGoals->setObjectName("awayTeamGoals");
    m_Time->setObjectName("scoreText");
#if defined(SOCCER_VGA)
    m_pauseBtn = new QPushButton(tr("Pause"), this);
    connect(m_pauseBtn, SIGNAL(pressed()), &m_pitch, SLOT(pause()));
#endif
}

ScreenGraphics::~ScreenGraphics()
{
    removeWidgets();
    delete m_SGFhomeTeamFlag;
    delete m_SGFhomeTeamName;
    delete m_SGFhomeTeamGoals;
    delete m_SGFawayTeamFlag;
    delete m_SGFawayTeamName;
    delete m_SGFawayTeamGoals;
    delete m_Time;
#if defined(SOCCER_VGA)
    delete m_pauseBtn;
#endif
}

void ScreenGraphics::update(QString s)
{
    m_SGFhomeTeamGoals->setText(QString::number(m_pitch.homeTeam()->m_goals));
    m_SGFawayTeamGoals->setText(QString::number(m_pitch.awayTeam()->m_goals));

    QString str;
    switch(m_type)
        {
        case Foul:
            str = QObject::tr("Foul");
            break;
        case GoalScored:
            str = QObject::tr("Goal");
            break;
        case KickOff:
            str = QObject::tr("Kick Off");
            break;
        default:
        case ScoreText:
            str += s;
            break;
        }
    m_Time->setText(str);
}

void ScreenGraphics::setGraphics(ScreenGraphicsType type)
{
    m_type = type;
}

void ScreenGraphics::setTeams(Team* home, Team* away)
{
    m_SGFhomeTeamFlag->setPixmap(
                QPixmap(home->flag()).scaled(20,15));
    m_SGFhomeTeamName->setText(home->briefName());
    m_SGFawayTeamFlag->setPixmap(
                QPixmap(away->flag()).scaled(20,15));
    m_SGFawayTeamName->setText(away->briefName());

    addWidgets();
}

void ScreenGraphics::addWidgets()
{
#if defined(SOCCER_VGA)
    m_bar.addWidget(m_pauseBtn);
    m_pauseBtn->show();
#endif
    m_bar.addWidget(m_SGFhomeTeamFlag);
    m_bar.addWidget(m_SGFhomeTeamName);
    m_bar.addWidget(m_SGFhomeTeamGoals);
    m_bar.addWidget(m_SGFawayTeamFlag);
    m_bar.addWidget(m_SGFawayTeamName);
    m_bar.addWidget(m_SGFawayTeamGoals);
    m_bar.addWidget(m_Time);
    m_SGFhomeTeamFlag->show();
    m_SGFhomeTeamName->show();
    m_SGFhomeTeamGoals->show();
    m_SGFawayTeamFlag->show();
    m_SGFawayTeamName->show();
    m_SGFawayTeamGoals->show();
    m_Time->show();
}

void ScreenGraphics::removeWidgets()
{
#if defined(SOCCER_VGA)
    m_bar.removeWidget(m_pauseBtn);
#endif
    m_bar.removeWidget(m_SGFhomeTeamFlag);
    m_bar.removeWidget(m_SGFhomeTeamName);
    m_bar.removeWidget(m_SGFhomeTeamGoals);
    m_bar.removeWidget(m_SGFawayTeamFlag);
    m_bar.removeWidget(m_SGFawayTeamName);
    m_bar.removeWidget(m_SGFawayTeamGoals);
    m_bar.removeWidget(m_Time);
}
