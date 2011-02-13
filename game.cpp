#include "game.h"
#include "goalscoredstate.h"
#include "foulstate.h"
#include "pitch.h"
#include "ball.h"
#include "Player.h"
#include "screengraphics.h"

Game::Game(Pitch* p,
     QString stateName,
     bool isFirstHalf,
     bool isExtraTime)
    : QState(),
    m_pitch(p),
    m_stateName(stateName),
    m_isFirstHalf(isFirstHalf),
    m_isExtraTime(isExtraTime),
    m_remainingTimeInHalfMs( KHalfLength )
{
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
    m_playingState->addTransition(this, SIGNAL(halfOver()), m_halfEndState);
    m_goalScoredState->addTransition(m_goalScoredState, SIGNAL(finished()), m_playingState);
    m_foulState->addTransition(m_foulState, SIGNAL(finished()), m_playingState);
    m_halfEndState->addTransition(m_timeLineLeavePitch, SIGNAL(finished()), m_allDoneState);

    connect(this, SIGNAL(halfOver()), this, SLOT(startPlayersLeavePitchAnim()));
    connect(this, SIGNAL(halfOver()), m_pitch, SLOT(showHalfStatisticsFrame()));
    connect(m_timeLineLeavePitch, SIGNAL(finished()), m_pitch, SLOT(hideHalfStatisticsFrame()));

    connect(m_timeLineTakePositions, SIGNAL(finished()), this, SLOT(kickOff()));
    connect(m_timeLineTakePositions, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));
    connect(m_timeLineLeavePitch, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));

    connect(m_1second, SIGNAL(timeout()), this, SLOT(decrementGameTime()));

    connect(m_playingState, SIGNAL(entered()), m_pitch, SLOT(gameStarted()));
}

Game::~Game()
{
    m_1second->stop();
    delete m_1second;

    delete m_timeLineTakePositions;
    delete m_timeLineLeavePitch;
}

void Game::setGameLength(int totalGameInMinutes)
{
     m_remainingTimeInHalfMs = (totalGameInMinutes * 60 *1000)/2.0;
}

void Game::decrementGameTime()
{
    m_remainingTimeInHalfMs = m_remainingTimeInHalfMs - 1000;
    m_pitch->updateDisplayTime(m_remainingTimeInHalfMs);
    if (m_remainingTimeInHalfMs == 0)
        emit halfOver();
}

void Game::startPlayersLeavePitchAnim()
{
    createPlayerAnimationItems(HalfOver);
    m_timeLineLeavePitch->start();
    m_1second->stop();
}

void Game::stopGameClock()
{
    if (m_1second->isActive())
        m_1second->stop();
}

void Game::kickOff()
{
    qDebug() << "Game::kickOff()";
    m_pitch->setPiece(m_pitch->homeTeam(), Pitch::KickOff);

    m_playingState->addTransition(m_pitch, SIGNAL(foul()), m_foulState);
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

        QPointF tmp;
        qreal stepX;
        qreal stepY;

        switch(g)
        {
        case TakePositions:
            {
            anim->setTimeLine(m_timeLineTakePositions);
            tmp = p->pos();
            stepX = ( p->m_startPositionRectF.center().x() - tmp.x()) / 100.0;
            stepY = ( p->m_startPositionRectF.center().y() - tmp.y()) / 100.0;
            MWindow::Action a = calculateAction(tmp, p->m_startPositionRectF.center());
            p->movePlayer(a);
            }
            break;

        case HalfOver:
            {
            anim->setTimeLine(m_timeLineLeavePitch);
            tmp = p->pos();
            stepX = ( m_pitch->pitchEntrancePoint().x() - tmp.x() ) / 100.0;
            stepY = ( m_pitch->pitchEntrancePoint().y() - tmp.y() ) / 100.0;
            MWindow::Action a = calculateAction(tmp, m_pitch->pitchEntrancePoint());
            p->movePlayer(a);
            }
            break;

        default:
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

void Game::onEntry(QEvent * /* event */)
{
    if (  m_isFirstHalf ) { // m_stateName == QString(tr("First half")) ) {
        m_pitch->homeTeam()->setDirection(Team::NorthToSouth);
        m_pitch->setPlayerStartPositions(m_pitch->homeTeam());

        m_pitch->awayTeam()->setDirection(Team::SouthToNorth);
        m_pitch->setPlayerStartPositions(m_pitch->awayTeam());
        createPlayerAnimationItems(TakePositions);
        m_timeLineTakePositions->start();
    } else { // if (m_stateName == QString(tr("Second half"))) {
        m_pitch->awayTeam()->setDirection(Team::NorthToSouth);
        m_pitch->setPlayerStartPositions(m_pitch->awayTeam());

        m_pitch->homeTeam()->setDirection(Team::SouthToNorth);
        m_pitch->setPlayerStartPositions(m_pitch->homeTeam());
        createPlayerAnimationItems(TakePositions);
        m_timeLineTakePositions->start();
    }
}

void Game::onExit(QEvent * /* event */)
{
    qDebug() << "Game::onExit() " << m_stateName;
    if (m_stateName == QString(tr("extra time second half")) && m_pitch->extraTimeAllowed()) {
        if ( m_pitch->extraTime() )
            m_pitch->setPenaltyShootOut(true);
        else
            m_pitch->setPenaltyShootOut(false);
    } else if (m_stateName == QString(tr("penalty shoot out")))
        m_pitch->setPenaltyShootOut(false);

    m_pitch->m_scene->removeItem(m_pitch->ball());

    m_1second->stop();
}


