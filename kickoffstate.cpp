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

#include "kickoffstate.h"
#include "game.h"
#include "pitch.h"
#include "pitchscene.h"
#include "ball.h"
#include "Player.h"
#include "team.h"
#include "screengraphics.h"
#include "soccerutils.h"

KickOffState::KickOffState(GameHalf *g, Pitch *p, QObject *parent) :
    QState(g),
    m_game(g),
    m_pitch(p),
    m_teamToKickOff(NULL)
{
    m_takePositions = new QState(this);
    m_takeKickOff = new QState(this);
    m_allDone = new QFinalState(this);
    setInitialState(m_takePositions);

    m_timeLineTakePositions = new QTimeLine(1000*4, this);
    m_timeLineTakePositions->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineTakePositions->setFrameRange(0, 100);

    m_takePositions->addTransition(m_timeLineTakePositions, SIGNAL(finished()), m_takeKickOff);
    m_takeKickOff->addTransition(m_pitch->ball(), SIGNAL(pass(Team*,QPointF)), m_allDone);
    connect(m_timeLineTakePositions, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));
    connect(m_timeLineTakePositions, SIGNAL(finished()), this, SLOT(prepareForKickOff()));
    connect(m_pitch, SIGNAL(kickOff(Team*)), this, SLOT(setTeamToKickOff(Team*)));

    connect(m_pitch, SIGNAL(pauseGameClock()), this, SLOT(pauseGameClock()));
    connect(m_pitch, SIGNAL(continueGameClock()), this, SLOT(continueGameClock()));
}

KickOffState::~KickOffState()
{
    delete m_timeLineTakePositions;
    delete m_takePositions;
    delete m_takeKickOff;
    delete m_allDone;
}

void KickOffState::prepareForKickOff()
{
    qDebug() << "KickOffState::prepareForKickOff";

    foreach (Player *p, m_pitch->m_players){
        p->setHasBall(false);
        p->setAllowedInCenterCircle(false);
    }

    Q_ASSERT(m_kickOffPlayer);
    Q_ASSERT(m_kickOffSupportPlayer);
    m_kickOffPlayer->setAllowedInCenterCircle(true);
    m_kickOffSupportPlayer->setAllowedInCenterCircle(true);

    qDebug() << "KickOffState::prepareForKickOff " << m_kickOffPlayer->name();

    // Only 2 players from the team taking the kick off are allowed within the center circle.
    // all others are not allowed in that area.
    // TODO
    m_pitch->ball()->setRequiredNextAction(MWindow::Pass, m_teamToKickOff, m_kickOffPlayer);
    m_kickOffPlayer->setRequiredNextAction(MWindow::Pass);

    m_pitch->scene()->addItem(m_pitch->ball());
    m_pitch->ball()->setPos(m_pitch->scene()->sceneRect().center());
    m_pitch->centerOn(m_pitch->ball());
}

void KickOffState::onEntry(QEvent * /* event */)
{
    m_kickOffPlayer = NULL;
    m_kickOffSupportPlayer = NULL;
    m_pitch->scene()->removeItem(m_pitch->ball());
    qDebug() << "KickOffState::onEntry";
    m_game->pauseGameClock();

    m_pitch->setGraphics(ScreenGraphics::KickOff);
    m_pitch->updateDisplayTime(m_game->remainingTimeInHalfMs());

    // players allowed to wander thru circle
    foreach (Player *p, m_pitch->m_players)
        p->setAllowedInCenterCircle(true);

    createPlayerAnimationItems();
    m_pitch->centerOn(m_playerAnimationItems.last()->item());
    m_timeLineTakePositions->start();
}

void KickOffState::onExit(QEvent* /* event */)
{
    qDebug() << "KickOffState::onExit";

    m_pitch->setGraphics(ScreenGraphics::ScoreText);
    m_pitch->updateDisplayTime(m_game->remainingTimeInHalfMs());
    m_game->continueGameClock();

    foreach (Player *p, m_pitch->m_players) {
        p->setAllowedOffPitch(false);
        p->setAllowedInCenterCircle(true);
        }
}

// animate from present player position to another point.
void KickOffState::createPlayerAnimationItems()
{
    m_playerAnimationItems.clear(); // TODO XXX TIM delete all

    qDebug() << "KickOffState::createPlayerAnimationItems start";

    // find 2 players from the m_teamToKickOff to take the kick off

    foreach (Player *p, m_pitch->m_players) {
        if (p->team() == m_teamToKickOff) {
            // last player on the team takes kickoff
            if (!m_kickOffSupportPlayer && p->m_role != Player::GoalKeeper) {
                m_kickOffSupportPlayer = p;
            } else
                m_kickOffPlayer = p;
        }
    }
    Q_ASSERT(m_kickOffPlayer);
    Q_ASSERT(m_kickOffSupportPlayer);

    foreach (Player *p, m_pitch->m_players) {
        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        m_playerAnimationItems.append(anim);

        qreal stepX;
        qreal stepY;
        qDebug() << "KickOffState::createPlayerAnimationItems name " << p->name();
        QPointF tmp(p->pos());
        anim->setTimeLine(m_timeLineTakePositions);
        MWindow::Action a;
        if ( p == m_kickOffPlayer ) {
            p->setDestination(m_pitch->scene()->sceneRect().center());
            stepX = ( m_pitch->scene()->sceneRect().center().x() - tmp.x()) / 100.0;
            stepY = ( m_pitch->scene()->sceneRect().center().y() - tmp.y()) / 100.0;
            a = calculateAction(tmp, m_pitch->scene()->sceneRect().center());
        } else if ( p == m_kickOffSupportPlayer ) {
            p->setDestination(QPointF(m_pitch->scene()->sceneRect().center().x() - 25,
                                      m_pitch->scene()->sceneRect().center().y()));
            stepX = ( ( m_pitch->scene()->sceneRect().center().x() - 25) - tmp.x()) / 100.0;
            stepY = ( m_pitch->scene()->sceneRect().center().y() - tmp.y()) / 100.0;
            a = calculateAction(tmp, m_pitch->scene()->sceneRect().center());
        } else {
            p->setDestination(p->m_startPositionRectF.center());
            stepX = ( p->m_startPositionRectF.center().x() - tmp.x()) / 100.0;
            stepY = ( p->m_startPositionRectF.center().y() - tmp.y()) / 100.0;
            a = calculateAction(tmp, p->m_startPositionRectF.center());
        }
        p->movePlayer(a);

        for (int i = 0; i < 100; ++i) {
            QPointF newPos(tmp.x() + stepX,tmp.y() + stepY);
            anim->setPosAt(i / 100.0, newPos);
            tmp = newPos;
        }
    }
}

void KickOffState::playFrame(int frame)
{
    qreal f = frame/ 100.00;
    foreach (QGraphicsItemAnimation *anim, m_playerAnimationItems)
        anim->item()->setPos(anim->posAt(f));
    m_pitch->scene()->update();
}

void KickOffState::setTeamToKickOff(Team* t)
{
    if (t){
        qDebug() << "KickOffState::setTeamToKickOff" << t->fullName() << " to kickoff";
        m_teamToKickOff = t;
    }
}

void KickOffState::pauseGameClock()
{
    qDebug() << "KickOffState::pauseGameClock";
    if (m_timeLineTakePositions->state() == QTimeLine::Running)
        m_timeLineTakePositions->setPaused(true);
}

void KickOffState::continueGameClock()
{
    qDebug() << "KickOffState::continueGameClock";
    if (m_timeLineTakePositions->state() == QTimeLine::Paused)
        m_timeLineTakePositions->setPaused(false);
}
