#include "game.h"
#include "goalscoredstate.h"
#include "foulstate.h"
#include "pitch.h"
#include "ball.h"
#include "Player.h"
#include "team.h"
#include "screengraphics.h"
#include "soccerutils.h"

Game::Game(Pitch* p,
     QString stateName,
     bool isFirstHalf,
     bool isExtraTime)
    : QState(),
    m_pitch(p),
    m_isFirstHalf(isFirstHalf),
    m_isExtraTime(isExtraTime),
    m_remainingTimeInHalfMs( KHalfLength )
{
    setObjectName(stateName);
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
    m_goalScoredState = new GoalScoredState(this, m_pitch);
    m_foulState = new FoulState(this, m_pitch);
    m_allDoneState = new QFinalState(this);
    setInitialState(m_startState);

    m_startState->addTransition(m_timeLineTakePositions, SIGNAL(finished()), m_playingState);
    m_playingState->addTransition(this, SIGNAL(halfOver(QString)), m_halfEndState);
    m_goalScoredState->addTransition(m_goalScoredState, SIGNAL(finished()), m_playingState);
    m_foulState->addTransition(m_foulState, SIGNAL(finished()), m_playingState);
    m_halfEndState->addTransition(m_timeLineLeavePitch, SIGNAL(finished()), m_allDoneState);

    connect(this, SIGNAL(halfOver(QString)), this, SLOT(startPlayersLeavePitchAnim(QString)));
    connect(m_timeLineLeavePitch, SIGNAL(finished()), m_pitch, SLOT(showHalfStatisticsFrame()));
 //   connect(m_timeLineLeavePitch, SIGNAL(finished()), m_pitch, SLOT(hideHalfStatisticsFrame()));

    connect(m_timeLineTakePositions, SIGNAL(finished()), m_pitch, SLOT(centerOnBall()));
    connect(m_timeLineTakePositions, SIGNAL(finished()), this, SLOT(kickOff()));
    connect(m_timeLineTakePositions, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));
    connect(m_timeLineLeavePitch, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));

    connect(m_1second, SIGNAL(timeout()), this, SLOT(decrementGameTime()));

    connect(m_playingState, SIGNAL(entered()), m_pitch, SLOT(gameStarted()));
    connect(m_pitch, SIGNAL(foul(Team*,QPointF)), this, SLOT(foulCaused(Team*,QPointF)));
}

void Game::foulCaused(Team* orig, QPointF location)
{
    qDebug() << "Game::foulCaused";
    m_foulingTeam = orig;
    m_foulingLocation = location;
    m_pitch->ball()->setPos(m_foulingLocation);
}

Game::~Game()
{
    pauseGameClock();
    delete m_1second;

    delete m_timeLineTakePositions;
    delete m_timeLineLeavePitch;
}



void Game::decrementGameTime()
{
    m_remainingTimeInHalfMs = m_remainingTimeInHalfMs - 1000;
    m_pitch->updateDisplayTime(m_remainingTimeInHalfMs);
    if (m_remainingTimeInHalfMs == 0) {
        // players are now allowed off the pitch
        foreach (Player *p, m_pitch->m_players) {
            p->setAllowedOffPitch(true);
        }
        emit halfOver(objectName());
    }
}

void Game::startPlayersLeavePitchAnim(QString /* halfName */)
{
    createPlayerAnimationItems(HalfOver);
    m_timeLineLeavePitch->start();
    m_pitch->centerOnBall(false);
    pauseGameClock();
}

void Game::pauseGameClock()
{
    if (m_1second->isActive())
        m_1second->stop();
}

void Game::continueGameClock()
{
    if (!m_1second->isActive())
        m_1second->start();
}

void Game::kickOff()
{
    foreach (Player *p, m_pitch->m_players)
        p->setAllowedOffPitch(false);

    m_pitch->setPiece(m_pitch->homeTeam(), Pitch::KickOff);

    m_playingState->addTransition(m_pitch, SIGNAL(foul(Team*,QPointF)), m_foulState);
    m_playingState->addTransition(m_pitch->ball(), SIGNAL(goalScored(bool)), m_goalScoredState);

    m_1second->start();
}

void Game::playFrame(int frame)
{
    m_pitch->m_view->centerOn(m_playerAnimationItems.last()->item());
    m_pitch->m_scoreText->updatePosition();
    qreal f = frame/ 100.00;
    foreach (QGraphicsItemAnimation *anim, m_playerAnimationItems)
        anim->item()->setPos(anim->posAt(f));

    m_pitch->m_scene->update();
}

// animate from present player position to another point.
void Game::createPlayerAnimationItems(GameState g)
{
    m_playerAnimationItems.clear(); // TODO XXX TIM delete all

    foreach (Player *p, m_pitch->m_players) {
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
            stepX = ( m_pitch->pitchEntrancePoint().x() - tmp.x() ) / 100.0;
            stepY = ( m_pitch->pitchEntrancePoint().y() - tmp.y() ) / 100.0;
            MWindow::Action a = calculateAction(tmp, m_pitch->pitchEntrancePoint());
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

void Game::onEntry(QEvent * /* event */)
{
    qDebug() << "Game::onEntry " << objectName();
    m_pitch->updateDisplayTime(remainingTimeInHalfMs());
    if (  m_isFirstHalf ) {
        m_pitch->homeTeam()->setDirection(Team::NorthToSouth);
        m_pitch->setPlayerStartPositions(m_pitch->homeTeam());

        m_pitch->awayTeam()->setDirection(Team::SouthToNorth);
        m_pitch->setPlayerStartPositions(m_pitch->awayTeam());
    } else {
        m_pitch->awayTeam()->setDirection(Team::NorthToSouth);
        m_pitch->setPlayerStartPositions(m_pitch->awayTeam());

        m_pitch->homeTeam()->setDirection(Team::SouthToNorth);
        m_pitch->setPlayerStartPositions(m_pitch->homeTeam());
    }
    createPlayerAnimationItems(TakePositions);
    m_timeLineTakePositions->start();
    m_pitch->centerOnBall(false);
}

void Game::onExit(QEvent * /* event */)
{
    qDebug() << "Game::onExit() " << objectName();
    if ((objectName() == KSecondHalfET) && m_pitch->extraTimeAllowed()) {
        if ( m_pitch->extraTime() )
            m_pitch->setPenaltyShootOut(true);
        else
            m_pitch->setPenaltyShootOut(false);
    } else if (objectName() == KPenaltyShootOut)
        m_pitch->setPenaltyShootOut(false);

    m_pitch->m_scene->removeItem(m_pitch->ball());

    pauseGameClock();
}


