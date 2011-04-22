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
#include <QFinalState>

#include "goalscoredstate.h"
#include "game.h"
#include "pitch.h"
#include "ball.h"
#include "Player.h"
#include "screengraphics.h"
#include "soccerutils.h"

GoalScoredState::GoalScoredState(GameHalf *g, Pitch *p)
    : QState(g),
      m_game(g),
      m_pitch(p)
{
    m_celebrate = new QState(this);
    m_returnToPosition = new QState(this);
    m_allDone = new QFinalState(this);
    setInitialState(m_celebrate);

    m_timeLineCelebrate = new QTimeLine(1000*3, this);
    m_timeLineCelebrate->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineCelebrate->setFrameRange(0, 100);

    m_timeLineReturnStartPositions = new QTimeLine(1000*3, this);
    m_timeLineReturnStartPositions->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineReturnStartPositions->setFrameRange(0, 100);

    m_celebrate->addTransition(m_timeLineCelebrate, SIGNAL(finished()), m_returnToPosition);
    m_returnToPosition->addTransition(m_timeLineReturnStartPositions, SIGNAL(finished()), m_allDone);

    connect(m_timeLineCelebrate, SIGNAL(frameChanged(int)), this, SLOT(playFrameCelebrate(int)));
    connect(m_timeLineCelebrate, SIGNAL(finished()), this, SLOT(createTakePositionAnimation()));
    connect(m_timeLineReturnStartPositions, SIGNAL(frameChanged(int)), this, SLOT(playFramePositions(int)));
    connect(m_timeLineReturnStartPositions, SIGNAL(finished()), m_game, SLOT(kickOff()));
    connect(m_pitch, SIGNAL(pauseGameClock()), this, SLOT(pauseGameClock()));
    connect(m_pitch, SIGNAL(continueGameClock()), this, SLOT(continueGameClock()));
}

void GoalScoredState::pauseGameClock()
{
    qDebug() << "GoalScoredState::pauseGameClock";
    if (m_timeLineCelebrate->state() == QTimeLine::Running)
        m_timeLineCelebrate->setPaused(true);
    else if (m_timeLineReturnStartPositions->state() == QTimeLine::Running)
        m_timeLineReturnStartPositions->setPaused(true);
}

void GoalScoredState::continueGameClock()
{
    qDebug() << "GoalScoredState::continueGameClock";
    if (m_timeLineCelebrate->state() == QTimeLine::Paused)
        m_timeLineCelebrate->setPaused(false);
    else if (m_timeLineReturnStartPositions->state() == QTimeLine::Paused)
        m_timeLineReturnStartPositions->setPaused(false);
}

void GoalScoredState::createTakePositionAnimation()
{
    createPlayerAnimationItems(TakePositions);
    m_timeLineReturnStartPositions->start();
}

void GoalScoredState::onEntry(QEvent * /* event */)
{
    qDebug() << "GoalScoredState::onEntry parent " << parent()->objectName();
    m_pitch->m_screenGraphicsLabel->setGraphics(ScreenGraphics::GoalScored);
    m_pitch->updateDisplayTime(m_game->remainingTimeInHalfMs());
    m_game->pauseGameClock();
    createPlayerAnimationItems(Celebrate);
    m_timeLineCelebrate->start();

}

// animate from present player position to another point.
void GoalScoredState::createPlayerAnimationItems(GameState g)
{
    m_playerAnimationItems.clear(); // TODO XXX TIM delete all

    foreach (Player *p, m_pitch->m_players) {
        if ( g == Celebrate && !p->team()->scoredLastGoal() )
            continue;

        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        m_playerAnimationItems.append(anim);

        QPointF tmp;
        qreal stepX;
        qreal stepY;

        switch( g )
        {
        case TakePositions:
        {
            anim->setTimeLine(m_timeLineReturnStartPositions);
            tmp = p->pos();
            stepX = ( p->m_startPositionRectF.center().x() - tmp.x()) / 100.0;
            stepY = ( p->m_startPositionRectF.center().y() - tmp.y()) / 100.0;
            MWindow::Action a = calculateAction(tmp, p->m_startPositionRectF.center());
            p->movePlayer(a);
        }
            break;

        case Celebrate:
            {
            anim->setTimeLine(m_timeLineCelebrate);
            tmp = p->pos();
            stepX = ( m_pitch->m_footballPitch->rect().center().x() - tmp.x() ) / 100.0;
            stepY = ( m_pitch->m_footballPitch->rect().center().y() - tmp.y() ) / 100.0;
            MWindow::Action a = calculateAction(tmp, m_pitch->m_footballPitch->rect().center());
            p->movePlayer(a);
            }
            break;

        default:
            Q_ASSERT(0);
            break;
        }

        for (int i = 0; i < 100; ++i) {
            anim->setPosAt(i / 100.0, QPointF(tmp.x() + stepX,
                                              tmp.y() + stepY));
            tmp.setX(tmp.x() + stepX);
            tmp.setY(tmp.y() + stepY);
        }
    }
}

void GoalScoredState::playFrameCelebrate(int frame)
{
    //qDebug() << "playFrameCelebrate" << frame;
    qreal f = frame/ 100.00;

    if ( frame > 30 )
        m_pitch->centerOn( m_pitch->ball()->lastPlayerToTouchBall() );

    foreach (QGraphicsItemAnimation *anim, m_playerAnimationItems)
        anim->item()->setPos(anim->posAt(f));
    m_pitch->m_scene->update();
}

void GoalScoredState::playFramePositions(int frame)
{
    //qDebug() << "playFramePositions" << frame;
    qreal f = frame/ 100.00;

//    if ( frame > 80 )
//        m_pitch->centerOn( m_pitch->m_centerMark->pos() );

    foreach (QGraphicsItemAnimation *anim, m_playerAnimationItems)
        anim->item()->setPos(anim->posAt(f));
    m_pitch->m_scene->update();
}

