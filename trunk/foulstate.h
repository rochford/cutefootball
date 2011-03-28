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

#ifndef FOULSTATE_H
#define FOULSTATE_H

#include <QTimer>
#include <QtGlobal>
#include <QStateMachine>
#include <QFinalState>

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QList>
#include <QGraphicsItemAnimation>
#include <QDebug>

class QTimeLine;

class Pitch;
class GameHalf;
class GoalScoredState;

class FoulState : public QState
{
    Q_OBJECT
public:
    FoulState(GameHalf *g, Pitch *p);
    ~FoulState();

public slots:
    void playFrame(int frame);

protected:
    void onEntry (QEvent * event );

private:
    void createPlayerAnimationItems();

private:
    QState *m_takePositions;
    QState *m_takeFreeKick;
    QFinalState *m_allDone;
    GameHalf *m_game;
    Pitch* m_pitch;
    QTimeLine *m_timeLineTakePositions;
    QList<QGraphicsItemAnimation*> m_playerAnimationItems;
};

#endif // FOULSTATE_H
