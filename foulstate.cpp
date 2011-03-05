#include "foulstate.h"
#include "game.h"
#include "goalscoredstate.h"
#include "pitch.h"
#include "ball.h"
#include "Player.h"
#include "screengraphics.h"
#include "soccerutils.h"

FoulState::FoulState(Game *g, Pitch *p)
    : QState(g),
      m_game(g),
      m_pitch(p)
{
    m_takePositions = new QState(this);
    m_takeFreeKick = new QState(this);
    m_allDone = new QFinalState(this);
    setInitialState(m_takePositions);

    m_timeLineTakePositions = new QTimeLine(1000*2, this);
    m_timeLineTakePositions->setCurveShape(QTimeLine::LinearCurve);
    m_timeLineTakePositions->setFrameRange(0, 100);

    m_takePositions->addTransition(m_timeLineTakePositions, SIGNAL(finished()), m_takeFreeKick);
    connect(m_timeLineTakePositions, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));
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
    m_takeFreeKick->addTransition(m_pitch->ball(), SIGNAL(shot(Team*,QPointF)), m_allDone);
    connect(m_pitch->ball(), SIGNAL(shot(Team*,QPointF)),m_game, SLOT(continueGameClock()));

    m_game->pauseGameClock();
    createPlayerAnimationItems();
    m_timeLineTakePositions->start();
}

// animate from present player position to another point.
void FoulState::createPlayerAnimationItems()
{
    m_playerAnimationItems.clear(); // TODO XXX TIM delete all

    qDebug() << "FoulState::createPlayerAnimationItems start";
    qDebug() << m_game->m_foulingTeam->briefName() << m_game->m_foulingLocation;

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
            qDebug() << "move away from ball";
            stepX = (p->pos().x() - m_pitch->m_centerMark->rect().center().x() ) / 100;
            stepY = (p->pos().y() - m_pitch->m_centerMark->rect().center().y()) / 100;

        } else if (p == m_pitch->selectNearestPlayer(p->team())) {
            stepX = (p->pos().x() - m_game->m_foulingLocation.x()) / 100;
            stepY = (p->pos().y() - m_game->m_foulingLocation.y()) / 100;

            m_pitch->ball()->setRequiredNextAction(MWindow::Shot, p->team(), p);
        }
        MWindow::Action a = calculateAction(tmp, m_game->m_foulingLocation);
        p->movePlayer(a);

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
    m_pitch->m_scene->update();
}
