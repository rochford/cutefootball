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
#include "foulstate.h"
#include "game.h"
#include "goalscoredstate.h"
#include "pitch.h"
#include "pitchscene.h"
#include "ball.h"
#include "Player.h"
#include "screengraphics.h"
#include "soccerutils.h"

FoulState::FoulState(GameHalf *g, Pitch *p)
    : QState(g),
      m_game(g),
      m_pitch(p),
      m_freeKickTaker(NULL)
{
    m_takePositions = new QState(this);
    m_takeFreeKick = new QState(this);
    m_allDone = new QFinalState(this);
    setInitialState(m_takePositions);

    m_timeLineTakePositions = new QTimeLine(1000*4, this);
    m_timeLineTakePositions->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineTakePositions->setFrameRange(0, 100);

    m_takePositions->addTransition(m_timeLineTakePositions, SIGNAL(finished()), m_takeFreeKick);
    m_takeFreeKick->addTransition(m_pitch->ball(), SIGNAL(pass(Team*,QPointF)), m_allDone);

//    connect(m_pitch->ball(), SIGNAL(shot(Team*,QPointF)),m_game, SLOT(continueGameClock()));
    connect(m_timeLineTakePositions, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));
    connect(m_timeLineTakePositions, SIGNAL(finished()), this, SLOT(prepareForFreeKick()));

    connect(m_pitch, SIGNAL(pauseGameClock()), this, SLOT(pauseGameClock()));
    connect(m_pitch, SIGNAL(continueGameClock()), this, SLOT(continueGameClock()));
}

FoulState::~FoulState()
{
    delete m_timeLineTakePositions;
    delete m_takePositions;
    delete m_takeFreeKick;
    delete m_allDone;
}

void FoulState::onEntry(QEvent * /* event */)
{
    qDebug() << "FoulState::onEntry";

    m_pitch->scene()->removeItem(m_pitch->ball());
    m_game->pauseGameClock();

    m_pitch->setGraphics(ScreenGraphics::Foul);
    m_pitch->updateDisplayTime(m_game->remainingTimeInHalfMs());

    m_freeKickTaker = NULL;
    m_pitch->ball()->setNoBallOwner();

    createPlayerAnimationItems();
    m_timeLineTakePositions->start();
}

void FoulState::onExit(QEvent* /* event */)
{
    qDebug() << "FoulState::onExit";
    m_freeKickTaker = NULL;

    m_game->continueGameClock();
    m_pitch->setGraphics(ScreenGraphics::ScoreText);
    m_pitch->updateDisplayTime(m_game->remainingTimeInHalfMs());
}

// animate from present player position to another point.
void FoulState::createPlayerAnimationItems()
{
    m_playerAnimationItems.clear(); // TODO XXX TIM delete all

    qDebug() << "FoulState::createPlayerAnimationItems start";
    qDebug() << m_game->m_foulingTeam->briefName() << m_game->m_foulingLocation;
    Team* teamToTakeFreeKick(NULL);
    if (m_pitch->homeTeam() == m_game->m_foulingTeam)
        teamToTakeFreeKick = m_pitch->awayTeam();
    else
        teamToTakeFreeKick = m_pitch->homeTeam();

    m_freeKickTaker = m_pitch->selectNearestPlayer(teamToTakeFreeKick);

    // move the captain to take the freekick near the m_foulingLocation
    // add a second delay prior to able to kick ball

    foreach (Player *p, m_pitch->m_players) {
        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        m_playerAnimationItems.append(anim);

        qDebug() << "FoulState::createPlayerAnimationItems name " << p->name() << " pos=" << p->pos();
        QPointF tmp(p->pos());
        qreal stepX(1.0);
        qreal stepY(1.0);
        anim->setTimeLine(m_timeLineTakePositions);
        if (p->team() == m_game->m_foulingTeam) {
            // move player away from ball
            qDebug() << p->name() << " move away from ball";
            stepX = (p->pos().x() - m_pitch->centerMark().x() ) / 100;
            stepY = (p->pos().y() - m_pitch->centerMark().y()) / 100;
            MWindow::Action a = calculateAction(tmp, m_pitch->centerMark());
            p->movePlayer(a);

        } /* else if (p == m_freeKickTaker) {

            stepX = (p->pos().x() - m_game->m_foulingLocation.x()) / 100;
            stepY = (p->pos().y() - m_game->m_foulingLocation.y()) / 100;

            MWindow::Action a = calculateAction(tmp, m_game->m_foulingLocation);
            p->movePlayer(a);
        } */ else {
            // nothing to do for other players on the team taking free-kick
            p->m_hasBall = false;
            continue;
        }

        for (int i = 0; i < 100; ++i) {
            anim->setPosAt(i / 100.0, QPointF(tmp.x() + stepX,
                                              tmp.y() + stepY));
            tmp.setX(tmp.x() + stepX);
            tmp.setY(tmp.y() + stepY);
        }
    }
}

void FoulState::playFrame(int frame)
{
    qreal f = frame/ 100.00;
    foreach (QGraphicsItemAnimation *anim, m_playerAnimationItems)
        anim->item()->setPos(anim->posAt(f));
    m_pitch->scene()->update();
}

void FoulState::prepareForFreeKick()
{
    // set the ball location to be the foul point.
    m_pitch->scene()->addItem(m_pitch->ball());
    m_pitch->ball()->setPos(m_game->m_foulingLocation);

    m_freeKickTaker = m_pitch->selectNearestPlayer(m_freeKickTaker->team());
    qDebug() << "FoulState::prepareForFreeKick " << m_freeKickTaker->name();
    m_pitch->ball()->setBallOwner(m_freeKickTaker);

    m_pitch->ball()->setRequiredNextAction(MWindow::Pass, m_freeKickTaker->team(), m_freeKickTaker);
    m_freeKickTaker->setRequiredNextAction(MWindow::Pass);
}

void FoulState::pauseGameClock()
{
    qDebug() << "FoulState::pauseGameClock";
    if (m_timeLineTakePositions->state() == QTimeLine::Running)
        m_timeLineTakePositions->setPaused(true);
}

void FoulState::continueGameClock()
{
    qDebug() << "FoulState::continueGameClock";
    if (m_timeLineTakePositions->state() == QTimeLine::Paused)
        m_timeLineTakePositions->setPaused(false);
}
