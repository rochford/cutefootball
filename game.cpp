#include "game.h"
#include "pitch.h"
#include "ball.h"
#include "Player.h"
#include "referee.h"
#include "replay.h"

const int KHalfLength = 60*1000; // seconds

GoalScoredState::GoalScoredState(Game *g, Pitch *p)
    : m_game(g),
    m_pitch(p)
{
    m_celebrate = new QState(this);
    m_returnToPosition = new QState(this);
    m_allDone = new QFinalState(this);
    setInitialState(m_celebrate);

    m_timeLineCelebrate = new QTimeLine(1000*6, this);
    m_timeLineCelebrate->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineCelebrate->setFrameRange(0, 100);

    m_timeLineReturnStartPositions = new QTimeLine(1000*6, this);
    m_timeLineReturnStartPositions->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineReturnStartPositions->setFrameRange(0, 100);

    m_celebrate->addTransition(m_timeLineCelebrate, SIGNAL(finished()), m_returnToPosition);
    m_returnToPosition->addTransition(m_timeLineReturnStartPositions, SIGNAL(finished()), m_allDone);

    connect(m_timeLineCelebrate, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));
    connect(m_timeLineCelebrate, SIGNAL(finished()), m_timeLineReturnStartPositions, SLOT(start()));
    connect(m_timeLineReturnStartPositions, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));
    connect(m_timeLineReturnStartPositions, SIGNAL(finished()), m_game, SLOT(kickOff()));
}

void GoalScoredState::onEntry(QEvent *event)
{
    qDebug() << "GoalScoredState onEntry";
    createPlayerAnimationItems(Celebrate);
    m_timeLineCelebrate->start();
}

// animate from present player position to another point.
void GoalScoredState::createPlayerAnimationItems(GameState g)
{
    m_playerAnimationItems.clear(); // TODO XXX TIM delete all

    foreach (Player *p, m_pitch->players_) {
        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        m_playerAnimationItems.append(anim);

        QPointF tmp;
        qreal stepX;
        qreal stepY;

        switch(g)
        {
        case TakePositions:
            anim->setTimeLine(m_timeLineReturnStartPositions);
            tmp = p->pos();
            stepX = ( p->startPosition_.center().x() - tmp.x()) / 100.0;
            stepY = ( p->startPosition_.center().y() - tmp.y()) / 100.0;
            break;

        case Celebrate:
            anim->setTimeLine(m_timeLineCelebrate);
            tmp = p->pos();
            stepX = ( 0 - tmp.x() ) / 100.0;
            stepY = ( 0 - tmp.y() ) / 100.0;
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

void GoalScoredState::playFrame(int frame)
{
    qDebug() << "GoalScoredState frame" << frame;
    foreach (QGraphicsItemAnimation *anim, m_playerAnimationItems) {
        qreal f = frame/ 100.00;

        anim->item()->setPos(anim->posAt(f));
    }
    m_pitch->scene->update();
}

Game::Game(Pitch* p,
     QString stateName,
     bool isFirstHalf)
    : QState(),
    m_pitch(p),
    m_stateName(stateName),
    m_remainingTimeInHalfMs(KHalfLength)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(KHalfLength);

    m_1second = new QTimer(this);
    m_1second->setInterval(1000);

    m_timeLineTakePositions = new QTimeLine(1000*6, this);
    m_timeLineTakePositions->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineTakePositions->setFrameRange(0, 100);

    m_timeLineLeavePitch = new QTimeLine(1000*6, this);
    m_timeLineLeavePitch->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineLeavePitch->setFrameRange(0, 100);

    m_startState = new QState(this);
    m_playingState = new QState(this);
    m_halfEndState = new QState(this);
    m_goalScoredState = new GoalScoredState(this, m_pitch);
    m_allDoneState = new QFinalState(this);
    setInitialState(m_startState);

    m_startState->addTransition(m_timeLineTakePositions, SIGNAL(finished()), m_playingState);
    m_playingState->addTransition(m_timer, SIGNAL(timeout()), m_halfEndState);
    m_goalScoredState->addTransition(m_goalScoredState, SIGNAL(finished()), m_playingState);
    m_halfEndState->addTransition(m_timeLineLeavePitch, SIGNAL(finished()), m_allDoneState);

    connect(m_timer,SIGNAL(timeout()), this, SLOT(startPlayersLeavePitchAnim()));

    connect(m_timeLineTakePositions, SIGNAL(finished()), this, SLOT(kickOff()));
    connect(m_timeLineTakePositions, SIGNAL(finished()), m_timer, SLOT(m_startState()));
    connect(m_timeLineTakePositions, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));
    connect(m_timeLineLeavePitch, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));

    connect(m_1second, SIGNAL(timeout()), this, SLOT(decrementGameTime()));
}

void Game::decrementGameTime()
{
    m_remainingTimeInHalfMs = m_remainingTimeInHalfMs - 1000;
    m_pitch->updateDisplayTime(m_remainingTimeInHalfMs);
}

void Game::startPlayersLeavePitchAnim()
{
    qDebug() << "startPlayersLeavePitchAnim";
    createPlayerAnimationItems(HalfOver);
    m_timeLineLeavePitch->start();
    m_pitch->getBall()->setVisible(false);
    m_1second->stop();
    m_pitch->replay_->replaySnapShotTimer_->stop();
}

void Game::kickOff()
{
    qDebug() << "kickOff";
    foreach (Player *p, m_pitch->players_) {
            p->hasBall_ = false;
            p->setPos(p->startPosition_.center());
        }
    m_pitch->homeTeam()->setHasBall(true);
    m_pitch->awayTeam()->setHasBall(false);
    m_pitch->scene->addItem(m_pitch->getBall());
    m_pitch->getBall()->setStartingPosition();

    // playing state transition to goalScoreState is not working
#if 1
    connect(m_pitch->getBall(), SIGNAL(goalScored(bool)), this, SLOT(kickOff()));
#else
    playing->addTransition(m_pitch->getBall(), SIGNAL(goalScored(bool)), m_goalScoredState);
#endif //

    m_pitch->scene->addItem(m_pitch->referee());

    m_1second->start();
    m_pitch->replay_->replaySnapShotTimer_->start();
}

void Game::playFrame(int frame)
{
    qDebug() << "playFrame frame" << frame;
    foreach (QGraphicsItemAnimation *anim, m_playerAnimationItems) {
        qreal f = frame/ 100.00;

        anim->item()->setPos(anim->posAt(f));
    }
    m_pitch->scene->update();
}

// animate from present player position to another point.
void Game::createPlayerAnimationItems(GameState g)
{
    m_playerAnimationItems.clear(); // TODO XXX TIM delete all

    foreach (Player *p, m_pitch->players_) {
        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        m_playerAnimationItems.append(anim);

        QPointF tmp;
        qreal stepX;
        qreal stepY;

        switch(g)
        {
        case TakePositions:
            anim->setTimeLine(m_timeLineTakePositions);
            tmp = p->pos();
            stepX = ( p->startPosition_.center().x() - tmp.x()) / 100.0;
            stepY = ( p->startPosition_.center().y() - tmp.y()) / 100.0;
            break;

        case HalfOver:
            anim->setTimeLine(m_timeLineLeavePitch);
            tmp = p->pos();
            stepX = ( 0 - tmp.x() ) / 100.0;
            stepY = ( 0 - tmp.y() ) / 100.0;
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

void Game::onEntry(QEvent *event)
{
    qDebug() << m_stateName << "onEntry";
    m_timer->start();
    m_pitch->homeTeam()->setDirection(Team::NorthToSouth);
    m_pitch->setPlayerStartPositions(m_pitch->homeTeam());
    m_pitch->setPlayerAttackPositions(m_pitch->homeTeam());
    m_pitch->setPlayerDefendPositions(m_pitch->homeTeam());

    m_pitch->awayTeam()->setDirection(Team::SouthToNorth);
    m_pitch->setPlayerStartPositions(m_pitch->awayTeam());
    m_pitch->setPlayerAttackPositions(m_pitch->awayTeam());
    m_pitch->setPlayerDefendPositions(m_pitch->awayTeam());

    createPlayerAnimationItems(TakePositions);
    m_timeLineTakePositions->start();
    m_pitch->getBall()->setVisible(true);
}

void Game::onExit(QEvent *event)
{
    qDebug() << m_stateName << "onExit";
    m_pitch->scene->removeItem(m_pitch->getBall());

    m_1second->stop();
    m_timer->stop();
}
