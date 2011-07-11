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
    mSGFhomeTeamFlag(new QLabel()),
    mSGFhomeTeamName(new QLabel()),
    mSGFhomeTeamGoals(new QLabel()),
    mSGFawayTeamFlag(new QLabel()),
    mSGFawayTeamName(new QLabel()),
    mSGFawayTeamGoals(new QLabel()),
    mTime(new QLabel())
{
    mSGFhomeTeamFlag->setObjectName("homeTeamFlag");
    mSGFawayTeamFlag->setObjectName("awayTeamFlag");
    mSGFhomeTeamName->setObjectName("homeTeamName");
    mSGFawayTeamName->setObjectName("awayTeamName");
    mSGFhomeTeamGoals->setObjectName("homeTeamGoals");
    mSGFawayTeamGoals->setObjectName("awayTeamGoals");
    mTime->setObjectName("scoreText");
}

ScreenGraphics::~ScreenGraphics()
{
    removeWidgets();
    delete mSGFhomeTeamFlag;
    delete mSGFhomeTeamName;
    delete mSGFhomeTeamGoals;
    delete mSGFawayTeamFlag;
    delete mSGFawayTeamName;
    delete mSGFawayTeamGoals;
    delete mTime;
}

void ScreenGraphics::update(QString s)
{
    mSGFhomeTeamGoals->setText(QString::number(m_pitch.homeTeam()->m_goals));
    mSGFawayTeamGoals->setText(QString::number(m_pitch.awayTeam()->m_goals));

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
    mTime->setText(str);
}

void ScreenGraphics::setGraphics(ScreenGraphicsType type)
{
    m_type = type;
}

void ScreenGraphics::setTeams(Team* home, Team* away)
{
    mSGFhomeTeamFlag->setPixmap(
                QPixmap(home->flag()).scaled(20,15));
    mSGFhomeTeamName->setText(home->briefName());
    mSGFawayTeamFlag->setPixmap(
                QPixmap(away->flag()).scaled(20,15));
    mSGFawayTeamName->setText(away->briefName());

    addWidgets();
}

void ScreenGraphics::addWidgets()
{
    m_bar.addWidget(mSGFhomeTeamFlag);
    m_bar.addWidget(mSGFhomeTeamName);
    m_bar.addWidget(mSGFhomeTeamGoals);
    m_bar.addWidget(mSGFawayTeamFlag);
    m_bar.addWidget(mSGFawayTeamName);
    m_bar.addWidget(mSGFawayTeamGoals);
    m_bar.addWidget(mTime);
    mSGFhomeTeamFlag->show();
    mSGFhomeTeamName->show();
    mSGFhomeTeamGoals->show();
    mSGFawayTeamFlag->show();
    mSGFawayTeamName->show();
    mSGFawayTeamGoals->show();
    mTime->show();
}

void ScreenGraphics::removeWidgets()
{
    m_bar.removeWidget(mSGFhomeTeamFlag);
    m_bar.removeWidget(mSGFhomeTeamName);
    m_bar.removeWidget(mSGFhomeTeamGoals);
    m_bar.removeWidget(mSGFawayTeamFlag);
    m_bar.removeWidget(mSGFawayTeamName);
    m_bar.removeWidget(mSGFawayTeamGoals);
    m_bar.removeWidget(mTime);
}
