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
#include <QStateMachine>
#include <QFinalState>
#include <QHistoryState>

#include "game.h"
#include "goalscoredstate.h"
#include "foulstate.h"
#include "kickoffstate.h"
#include "pitch.h"
#include "pitchscene.h"
#include "ball.h"
#include "Player.h"
#include "team.h"
#include "screengraphics.h"
#include "soccerutils.h"

const QString KFirstHalf(QObject::tr("First half"));
const QString KSecondHalf(QObject::tr("Second half"));
const QString KFirstHalfET(QObject::tr("Extra Time First half"));
const QString KSecondHalfET(QObject::tr("Extra Time Second half"));
const QString KPenaltyShootOut(QObject::tr("penalty shoot out"));

Game::Game(QStateMachine& fsm, Pitch& p)
    : QState(),
      m_fsm(fsm),
      m_pitch(p),
      m_currentState(NULL)
{
    qDebug() << "Game::Game";
    m_pausedState = new QState();

    m_firstHalfState = new GameHalf(this, m_pitch,
                                    KFirstHalf, true, false);
    m_secondHalfState = new GameHalf(this, m_pitch,
                                     KSecondHalf, false, false);
    m_extraTimeFirstHalfState = new GameHalf(this, m_pitch,
                                             KFirstHalfET, true, true);
    m_extraTimeSecondHalfState = new GameHalf(this, m_pitch,
                                              KSecondHalfET, false, true);
    m_penaltiesState = new GameHalf(this, m_pitch,
                                    KPenaltyShootOut, true, true);

    m_allDone = new QFinalState(this);

    m_prePausedState = new QHistoryState(this);
    m_prePausedState->setHistoryType(QHistoryState::DeepHistory);
    m_prePausedState->setDefaultState(m_firstHalfState);

    m_fsm.addState(this);

    m_fsm.addState(m_pausedState);

    m_fsm.setInitialState(this);
    setInitialState(m_firstHalfState);

    m_pausedState->addTransition(&m_pitch,
                                 SIGNAL(continueGameClock()),
                                 m_prePausedState);
    m_firstHalfState->addTransition(m_firstHalfState, SIGNAL(finished()),
                                    m_secondHalfState);
    m_secondHalfState->addTransition(m_secondHalfState, SIGNAL(finished()),
                                     m_allDone);
    m_extraTimeFirstHalfState->addTransition(m_extraTimeFirstHalfState,
                                             SIGNAL(finished()),
                                             m_extraTimeSecondHalfState);
    m_extraTimeSecondHalfState->addTransition(m_extraTimeSecondHalfState,
                                              SIGNAL(finished()),
                                              m_allDone);
    m_penaltiesState->addTransition(m_penaltiesState, SIGNAL(finished()),
                                    m_allDone);
    connect(m_secondHalfState, SIGNAL(finished()),
            &m_pitch, SLOT(gameStop()));
}

Game::~Game()
{
    m_currentState = NULL;
    delete m_pausedState;
    delete m_firstHalfState;
    delete m_secondHalfState;
    delete m_extraTimeFirstHalfState;
    delete m_extraTimeSecondHalfState;
    delete m_penaltiesState;
    delete m_allDone;
}
void Game::setHalfLength(int totalGameInMinutes)
{
    m_firstHalfState->setGameLength(totalGameInMinutes);
    m_secondHalfState->setGameLength(totalGameInMinutes);
    m_extraTimeFirstHalfState->setGameLength(1);
    m_extraTimeSecondHalfState->setGameLength(1);
}

void Game::setTeamToKickOff(Team* t)
{
    m_firstHalfState->setTeamToKickOff(t);
    m_secondHalfState->setTeamToKickOff(t);
    m_extraTimeFirstHalfState->setTeamToKickOff(t);
    m_extraTimeSecondHalfState->setTeamToKickOff(t);
}

GameHalf::GameHalf(Game* parent,
                   Pitch& p,
                   QString stateName,
                   bool isFirstHalf,
                   bool isExtraTime)
    : QState(parent),  
    m_game(parent),
    m_pitch(p),
    m_isFirstHalf(isFirstHalf),
    m_isExtraTime(isExtraTime),
    m_remainingTimeInHalfMs( KHalfLength )
{
    setObjectName(stateName);
    qDebug() << "GameHalf::GameHalf " << objectName();
    m_1second = new QTimer(this);
    m_1second->setInterval(KOneSecondMs);

    m_timeLineLeavePitch = new QTimeLine(KOneSecondMs*5, this);
    m_timeLineLeavePitch->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineLeavePitch->setFrameRange(0, 100);

    m_kickOffState = new KickOffState(this, &m_pitch);


    m_playingState = new QState(this);
    m_halfEndState = new QState(this);
    m_goalScoredState = new GoalScoredState(this, &m_pitch);
    m_foulState = new FoulState(this, &m_pitch);
    m_allDoneState = new QFinalState(this);
    setInitialState(m_kickOffState);

    m_kickOffState->addTransition(m_kickOffState, SIGNAL(finished()), m_playingState);

    m_playingState->addTransition(this, SIGNAL(halfOver(QString)), m_halfEndState);
    m_playingState->addTransition(&m_pitch, SIGNAL(foul(Team*,QPointF)), m_foulState);
    m_playingState->addTransition(m_pitch.ball(), SIGNAL(goalScored(bool)), m_goalScoredState);

    m_goalScoredState->addTransition(m_goalScoredState, SIGNAL(finished()), m_kickOffState);

    m_foulState->addTransition(m_foulState, SIGNAL(finished()), m_playingState);

    m_halfEndState->addTransition(m_timeLineLeavePitch, SIGNAL(finished()), m_allDoneState);


    connect(this, SIGNAL(halfOver(QString)),
            this, SLOT(startPlayersLeavePitchAnim(QString)));
    connect(m_timeLineLeavePitch, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));

    connect(m_1second, SIGNAL(timeout()),
            this, SLOT(decrementGameTime()));

    connect(&m_pitch, SIGNAL(foul(Team*,QPointF)),
            this, SLOT(foulCaused(Team*,QPointF)));
    connect(&m_pitch, SIGNAL(pauseGameClock()),
            this, SLOT(pauseGameClock()));
    connect(&m_pitch, SIGNAL(continueGameClock()),
            this, SLOT(continueGameClock()));
}

void GameHalf::foulCaused(Team* orig, QPointF location)
{
    //qDebug() << "Game::foulCaused";
    m_foulingTeam = orig;
    m_foulingLocation = location;
    m_pitch.ball()->setPos(m_foulingLocation);
}

GameHalf::~GameHalf()
{
//    pauseGameClock();
    delete m_1second;

    delete m_timeLineLeavePitch;
}

void GameHalf::decrementGameTime()
{
    m_remainingTimeInHalfMs = m_remainingTimeInHalfMs - KOneSecondMs;
    m_pitch.updateDisplayTime(m_remainingTimeInHalfMs);
    if (m_remainingTimeInHalfMs == 0) {
        // players are now allowed off the pitch
        foreach (Player *p, m_pitch.m_players) {
            p->setAllowedOffPitch(true);
        }
        //qDebug() << "GameHalf::decrementGameTime half over " << objectName();
        emit halfOver(objectName());
    }
}

void GameHalf::startPlayersLeavePitchAnim(QString /* halfName */)
{
    //qDebug() << "GameHalf::startPlayersLeavePitchAnim " << objectName();
    createPlayerAnimationItems();
    if (m_1second->isActive())
        m_1second->stop();
    m_timeLineLeavePitch->start();
}

void GameHalf::pauseGameClock()
{
    if (m_game->currentState() != this)
        return;

    qDebug() << "GameHalf::pauseGameClock() " << objectName();
    if (m_1second->isActive())
        m_1second->stop();
    else if (m_timeLineLeavePitch->state() == QTimeLine::Running)
        m_timeLineLeavePitch->setPaused(true);
}

void GameHalf::continueGameClock()
{
    if (m_game->currentState() != this)
        return;

    if (m_timeLineLeavePitch->state() == QTimeLine::Paused) {
        m_timeLineLeavePitch->setPaused(false);
    } else if (!m_1second->isActive())
            m_1second->start();
}

void GameHalf::playFrame(int frame)
{
    m_pitch.centerOn(m_playerAnimationItems.last()->item());

    qreal f = frame/ 100.00;
    foreach (QGraphicsItemAnimation *anim, m_playerAnimationItems)
        anim->item()->setPos(anim->posAt(f));

    m_pitch.scene()->update();
}

// animate from present player position to another point.
void GameHalf::createPlayerAnimationItems()
{
    //qDebug() << "GameHalf::createPlayerAnimationItems " << objectName();
    m_playerAnimationItems.clear(); // TODO XXX TIM delete all

    foreach (Player *p, m_pitch.m_players) {
        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        m_playerAnimationItems.append(anim);
        anim->setTimeLine(m_timeLineLeavePitch);

        QPointF tmp(p->pos());
        qreal stepX = ( m_pitch.pitchEntrancePoint().x() - tmp.x() ) / 100.0;
        qreal stepY = ( m_pitch.pitchEntrancePoint().y() - tmp.y() ) / 100.0;
        MWindow::Action a = calculateAction(tmp, m_pitch.pitchEntrancePoint());
        p->setAllowedOffPitch(true);
        p->movePlayer(a);

        for (int i = 0; i < 100; ++i) {
            QPointF newPos(tmp.x() + stepX,tmp.y() + stepY);
            anim->setPosAt(i / 100.0, newPos);
            tmp = newPos;
        }
    }
}

void GameHalf::onEntry(QEvent * /* event */)
{
    qDebug() << "GameHalf::onEntry " << objectName();
    m_game->setCurrentState(this);

    m_pitch.setGraphics(ScreenGraphics::ScoreText);
    m_pitch.updateDisplayTime(remainingTimeInHalfMs());
    if (  m_isFirstHalf ) {
        m_pitch.homeTeam()->setDirection(Team::NorthToSouth);
        m_pitch.setPlayerStartPositions(m_pitch.homeTeam());

        m_pitch.awayTeam()->setDirection(Team::SouthToNorth);
        m_pitch.setPlayerStartPositions(m_pitch.awayTeam());
    } else {
        m_pitch.awayTeam()->setDirection(Team::NorthToSouth);
        m_pitch.setPlayerStartPositions(m_pitch.awayTeam());

        m_pitch.homeTeam()->setDirection(Team::SouthToNorth);
        m_pitch.setPlayerStartPositions(m_pitch.homeTeam());
    }
}

void GameHalf::onExit(QEvent * /* event */)
{
    //qDebug() << "GameHalf::onExit() " << objectName();
    if ((objectName() == KSecondHalfET) && m_pitch.extraTimeAllowed()) {
        if ( m_pitch.extraTime() )
            m_pitch.setPenaltyShootOut(true);
        else
            m_pitch.setPenaltyShootOut(false);
    } else if (objectName() == KPenaltyShootOut)
        m_pitch.setPenaltyShootOut(false);

    m_pitch.scene()->removeItem(m_pitch.ball());

    if (m_1second->isActive())
        m_1second->stop();
}


