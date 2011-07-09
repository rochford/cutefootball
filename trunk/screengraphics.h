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
#ifndef SCREENGRAPHICS_H
#define SCREENGRAPHICS_H

#include <QtGui>
#include <QStatusBar>

class Pitch;
class Team;

class ScreenGraphics : public QObject
{
    Q_OBJECT;
public:
    enum ScreenGraphicsType {
        ScoreText,
        GoalScored,
        Foul,
        KickOff
    };

    ScreenGraphics(Pitch& p, QStatusBar& bar);
    ~ScreenGraphics();
    void update(QString s);
    void setTeams(Team* home, Team* away);
    void addWidgets();
    void removeWidgets();

public slots:
    void setGraphics(ScreenGraphicsType type);

private:
    Pitch& m_pitch; // NOT OWNED
    ScreenGraphicsType m_type;
    QStatusBar& m_bar; // NOT OWNED

    QLabel* mSGFhomeTeamFlag;
    QLabel* mSGFhomeTeamName;
    QLabel* mSGFhomeTeamGoals;
    QLabel* mSGFawayTeamFlag;
    QLabel* mSGFawayTeamName;
    QLabel* mSGFawayTeamGoals;
    QLabel* mTime;
};

#endif // SCREENGRAPHICS_H
