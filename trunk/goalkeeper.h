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
 *    Lesser  GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GOALKEEPER_H
#define GOALKEEPER_H

#include <QtGui>

#include "Player.h"

class Pitch;
class Team;

class GoalKeeper : public Player
{
    Q_OBJECT
public:
    explicit GoalKeeper(QString name,
                        int number,
                        Pitch* pitch,
                        Team* team,
                        QColor hairColor,
                        QColor skinColor);

    // goal keepers are players too
    enum { Type = UserType + 2 };
    int type() const
        { return Type;}

public slots:
    void goalAttempt(Team* t, QPointF dest);

public:
    void createPixmaps();
    void advance(int phase);
    void createMoves();
    void pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3, QRgb goalKeeperShirtColor, QRgb goalKeeperShortColor);

private:
    void gkAdvanceWithBall();
    void gkAdvanceWithoutBall();
};

#endif // GOALKEEPER_H
