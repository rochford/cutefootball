#include <QStateMachine>
#include <QFinalState>
#include <QHistoryState>

#include "game.h"
#include "goalscoredstate.h"
#include "foulstate.h"
#include "pitch.h"
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
    m_1second->setInterval(1000);

    m_timeLineTakePositions = new QTimeLine(1000*5, this);
    m_timeLineTakePositions->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineTakePositions->setFrameRange(0, 100);

    m_timeLineLeavePitch = new QTimeLine(1000*5, this);
    m_timeLineLeavePitch->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineLeavePitch->setFrameRange(0, 100);

    m_startState = new QState(this);
    m_playingState = new QState(this);
    m_halfEndState = new QState(this);
    m_goalScoredState = new GoalScoredState(this, &m_pitch);
    m_foulState = new FoulState(this, &m_pitch);
    m_allDoneState = new QFinalState(this);
    setInitialState(m_startState);

    m_startState->addTransition(m_timeLineTakePositions, SIGNAL(finished()), m_playingState);
    m_playingState->addTransition(this, SIGNAL(halfOver(QString)), m_halfEndState);
    m_goalScoredState->addTransition(m_goalScoredState, SIGNAL(finished()), m_playingState);
    m_foulState->addTransition(m_foulState, SIGNAL(finished()), m_playingState);
    m_halfEndState->addTransition(m_timeLineLeavePitch, SIGNAL(finished()), m_allDoneState);

    connect(this, SIGNAL(halfOver(QString)),
            this, SLOT(startPlayersLeavePitchAnim(QString)));

    connect(m_timeLineTakePositions, SIGNAL(finished()), this, SLOT(kickOff()));
    connect(m_timeLineTakePositions, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));
    connect(m_timeLineLeavePitch, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));

    connect(m_1second, SIGNAL(timeout()),
            this, SLOT(decrementGameTime()));

//    connect(m_playingState, SIGNAL(entered()), m_pitch, SLOT(gameStarted()));
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

    delete m_timeLineTakePositions;
    delete m_timeLineLeavePitch;
}

void GameHalf::decrementGameTime()
{
    qDebug() << "GameHalf::decrementGameTime";
    m_remainingTimeInHalfMs = m_remainingTimeInHalfMs - 1000;
    m_pitch.updateDisplayTime(m_remainingTimeInHalfMs);
    if (m_remainingTimeInHalfMs == 0) {
        // players are now allowed off the pitch
        foreach (Player *p, m_pitch.m_players) {
            p->setAllowedOffPitch(true);
        }
        qDebug() << "GameHalf::decrementGameTime half over " << objectName();
        emit halfOver(objectName());
    }
}

void GameHalf::startPlayersLeavePitchAnim(QString /* halfName */)
{
    qDebug() << "GameHalf::startPlayersLeavePitchAnim " << objectName();
    createPlayerAnimationItems(HalfOver);
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
    if (m_timeLineTakePositions->state() == QTimeLine::Running)
        m_timeLineTakePositions->setPaused(true);
    else if (m_timeLineLeavePitch->state() == QTimeLine::Running)
        m_timeLineLeavePitch->setPaused(true);
}

void GameHalf::continueGameClock()
{
    if (m_game->currentState() != this)
        return;

    qDebug() << "GameHalf::continueGameClock() "<< objectName();
    if (m_timeLineTakePositions->state() == QTimeLine::Paused) {
        m_timeLineTakePositions->setPaused(false);
    } else if (m_timeLineLeavePitch->state() == QTimeLine::Paused) {
        m_timeLineLeavePitch->setPaused(false);
    } else if (!m_1second->isActive())
            m_1second->start();
}

void GameHalf::kickOff()
{
    qDebug() << "GameHalf::kickoff " << objectName();

    foreach (Player *p, m_pitch.m_players)
        p->setAllowedOffPitch(false);
    m_pitch.centerOn(m_pitch.ball());

    m_pitch.setPiece(m_pitch.homeTeam(), Pitch::KickOff);

    m_playingState->addTransition(&m_pitch, SIGNAL(foul(Team*,QPointF)), m_foulState);
    m_playingState->addTransition(m_pitch.ball(), SIGNAL(goalScored(bool)), m_goalScoredState);

    m_1second->start();
}

void GameHalf::playFrame(int frame)
{
    qDebug() << "GameHalf::playFrame " << objectName();

    m_pitch.centerOn(m_playerAnimationItems.last()->item());

    qreal f = frame/ 100.00;
    foreach (QGraphicsItemAnimation *anim, m_playerAnimationItems)
        anim->item()->setPos(anim->posAt(f));

    m_pitch.m_scene->update();
}

// animate from present player position to another point.
void GameHalf::createPlayerAnimationItems(GameState g)
{
    qDebug() << "GameHalf::createPlayerAnimationItems " << objectName();
    m_playerAnimationItems.clear(); // TODO XXX TIM delete all

    foreach (Player *p, m_pitch.m_players) {
        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        m_playerAnimationItems.append(anim);

        QPointF tmp(p->pos());
        qreal stepX;
        qreal stepY;

        switch(g)
        {
        case TakePositions:
            {
            anim->setTimeLine(m_timeLineTakePositions);
            stepX = ( p->m_startPositionRectF.center().x() - tmp.x()) / 100.0;
            stepY = ( p->m_startPositionRectF.center().y() - tmp.y()) / 100.0;
            MWindow::Action a = calculateAction(tmp, p->m_startPositionRectF.center());
            p->movePlayer(a);
            }
            break;

        case HalfOver:
            {
            anim->setTimeLine(m_timeLineLeavePitch);
            stepX = ( m_pitch.pitchEntrancePoint().x() - tmp.x() ) / 100.0;
            stepY = ( m_pitch.pitchEntrancePoint().y() - tmp.y() ) / 100.0;
            MWindow::Action a = calculateAction(tmp, m_pitch.pitchEntrancePoint());
            p->setAllowedOffPitch(true);
            p->movePlayer(a);
            }
            break;

        default:
            break;
        }

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
    createPlayerAnimationItems(TakePositions);
    m_timeLineTakePositions->start();
}

void GameHalf::onExit(QEvent * /* event */)
{
    qDebug() << "GameHalf::onExit() " << objectName();
    if ((objectName() == KSecondHalfET) && m_pitch.extraTimeAllowed()) {
        if ( m_pitch.extraTime() )
            m_pitch.setPenaltyShootOut(true);
        else
            m_pitch.setPenaltyShootOut(false);
    } else if (objectName() == KPenaltyShootOut)
        m_pitch.setPenaltyShootOut(false);

    m_pitch.m_scene->removeItem(m_pitch.ball());

    if (m_1second->isActive())
        m_1second->stop();
}


