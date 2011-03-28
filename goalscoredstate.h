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

#ifndef GOALSCOREDSTATE_H
#define GOALSCOREDSTATE_H

#include <QList>
#include <QtGlobal>
#include <QStateMachine>
#include <QGraphicsItemAnimation>

class QTimeLine;
class QFinalState;

class GameHalf;
class Pitch;

class GoalScoredState : public QState
{
    Q_OBJECT
public:
    GoalScoredState(GameHalf *g, Pitch *p);
    ~GoalScoredState() {}

    enum GameState {
        TakePositions,
        Celebrate,
    };
public slots:
    void playFrameCelebrate(int frame);
    void playFramePositions(int frame);
    void createTakePositionAnimation();

    void pauseGameClock();
    void continueGameClock();

protected:
    void onEntry ( QEvent * event );

private:
    void createPlayerAnimationItems(GameState g);

private:
    QState *m_celebrate;
    QState *m_returnToPosition;
    QFinalState *m_allDone;
    GameHalf *m_game;
    Pitch* m_pitch;
    QTimeLine *m_timeLineCelebrate;
    QTimeLine *m_timeLineReturnStartPositions;
    QList<QGraphicsItemAnimation*> m_playerAnimationItems;
};

#endif // GOALSCOREDSTATE_H
