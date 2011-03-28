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
#ifndef TEAM_H
#define TEAM_H

#include <QObject>
#include <QColor>
#include <QString>
#include <QList>

class Team;

class TeamManager {
public:
    TeamManager() {};
    ~TeamManager() {};
    QList<Team*> teams() { return m_teams; }
    Team* at(int index) { return m_teams.at(index); }
    void createTeams();
private:
    enum TeamFileFormat {
        Ranking = 0,
        BriefName,
        FullName,
        ShirtColor,
        ShortColor,
        PlayerSpeed
    };

   QList<Team*> m_teams;
};

class Team : public QObject {
    Q_OBJECT;
public:
    // the attacking direction
    enum Direction { SouthToNorth, NorthToSouth };
    Team(int ranking,
         QString briefName,
         QString teamName,
         QColor shirtColor,
         QColor shortColor,
         int playerSpeed);
    QString fullName() const { return m_name; }
    void setDirection(Direction dir) { m_direction = dir; }
    Direction getDirection() const { return m_direction; }
    void setHasBall(bool hasBall);
    bool scoredLastGoal() const { return m_scoredLastGoal; }
    void setShots(int newValue) { m_shotCount = newValue; }
    inline int shots() const { return m_shotCount; }
    void newGame() { m_goals = 0; m_scoredLastGoal = false; m_teamHasBall = false; m_shotCount = 0; }
    inline bool teamHasBall() const { return m_teamHasBall; }
    inline qreal speed() const { return m_speed; }
    inline QString briefName() const { return m_briefName; }
    inline int ranking() const { return m_rank; }

public slots:
    void goalScored(bool isNorthGoal);

public:
    QColor m_shirtColor;
    QColor m_shortColor;
    Direction m_direction;
    int m_goals;

private:
    QString m_briefName;
    QString m_name;
    bool m_teamHasBall;
    // this team scored the last goal
    bool m_scoredLastGoal;
    int m_shotCount;
    qreal m_speed;
    // the teams ranking
    int m_rank;
};


#endif // TEAM_H
