
#include "goalkeeper.h"

#include "pitch.h"
#include "ball.h"
#include <QDebug>

GoalKeeper::GoalKeeper(QString name,
                       Pitch *pitch,
                       Team* team)
    : Player(name,true,pitch,team,Player::GoalKeeper)
{
}

void GoalKeeper::createPixmaps()
{
    pixmapInsert(MWindow::North, "pN.PNG", "pN1.PNG", "pN2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::NorthEast, "pNE.PNG", "pNE1.PNG", "pNE2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::East, "pE.PNG", "pE1.PNG", "pE2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::SouthEast, "pSE.PNG", "pSE1.PNG", "pSE2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::South, "pS.PNG", "pS1.PNG", "pS2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::SouthWest, "pSW.PNG", "pSW1.PNG", "pSW2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::West, "pW.PNG", "pW1.PNG", "pW2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::NorthWest, "pNW.PNG", "pNW1.PNG", "pNW2.PNG", KGoalKeeperColor);

    pixmapInsert(MWindow::ThrownIn, "pNW.PNG", "pNW1.PNG", "pNW2.PNG", KGoalKeeperColor); // TODO XXX TIM

    pixmapInsert(MWindow::TackleNorth, "tackleN.PNG", "tackleN.PNG", "tackleN.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleNorthEast, "tackleNE.PNG", "tackleNE.PNG", "tackleNE.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleEast, "tackleE.PNG", "tackleE.PNG", "tackleE.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleSouthEast, "tackleSE.PNG", "tackleSE.PNG", "tackleSE.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleSouth, "tackleS.PNG", "tackleS.PNG", "tackleS.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleSouthWest, "tackleSW.PNG", "tackleSW.PNG", "tackleSW.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleWest, "tackleW.PNG", "tackleW.PNG", "tackleW.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleNorthWest, "tackleNW.PNG", "tackleNW.PNG", "tackleNW.PNG", KGoalKeeperColor); // TODO XXX TIM

    pixmapInsert(MWindow::GoalCelebration, "pN.PNG", "pN1.PNG", "pN2.PNG", KGoalKeeperColor); // TODO XXX TIM

    pixmapInsert(MWindow::DiveLeft, "pN.PNG", "pN1.PNG", "pN2.PNG", KGoalKeeperColor); // TODO
    pixmapInsert(MWindow::DiveRight, "pN.PNG", "pN1.PNG", "pN2.PNG", KGoalKeeperColor); // TODO

    // set default pixmap
    setPixmap(m_images[MWindow::North].at(0));
}

void GoalKeeper::pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3, QRgb goalKeeperColor)
{
    QString s(":/images/red/");

    QString n1(s), n2(s), n3(s);
    QPixmap p1, p2, p3;

    teamColorTransform(p1, n1.append(s1), qRgb(255, 0, 0), goalKeeperColor);
    teamColorTransform(p2, n2.append(s2), qRgb(255, 0, 0), goalKeeperColor);
    teamColorTransform(p3, n3.append(s3), qRgb(255, 0, 0), goalKeeperColor);

    QList<QPixmap> list;
    list << p1 <<  p2 << p3;
    m_images.insert(a, list);
}

void GoalKeeper::advance(int phase)
{
    if (!phase)
        return;
    if (m_outOfAction->isActive())
        return;
    if ( hasBall_ )
         gkAdvanceWithBall();
    else
        gkAdvanceWithoutBall();
}

void GoalKeeper::gkAdvanceWithoutBall()
{
//    if (!team_->teamHasBall_) {
        // if the ball enters the penalty area then go for it, otherwise return to goal line
        Team::Direction dir = team_->getDirection();
        MWindow::Action action;

            if ( (dir == Team::SouthToNorth
                && m_pitch->m_bottomPenaltyArea->contains(m_pitch->ball()->pos()) )
            || (dir == Team::NorthToSouth
                && m_pitch->m_topPenaltyArea->contains(m_pitch->ball()->pos())) ) {

            qreal dx = abs(pos().x() - m_pitch->ball()->pos().x());
            qreal dy = abs(pos().y() - m_pitch->ball()->pos().y());
            if ( m_pitch->ball()->controlledBy() && ( dx < 5) && (dy < 5) )
                action = MWindow::Tackle;
            else
                action = calculateAction(pos(), m_pitch->ball()->pos());
        } else {
            QPointF ownGoal;
            if ( dir == Team::SouthToNorth )
                ownGoal = m_pitch->m_bottomPenaltyArea->rect().center();
            else
                ownGoal = m_pitch->m_topPenaltyArea->rect().center();
            action = calculateAction(pos(), ownGoal);
        }
        move(action);
//    }
}

void GoalKeeper::gkAdvanceWithBall()
{
    move(MWindow::Shot);
}

void GoalKeeper::createMoves()
{
    Player::createMoves();

    m_moveDistance.insert(MWindow::DiveLeft, QPointF(5*m_speed,0));
    m_moveDistance.insert(MWindow::DiveRight, QPointF(5*-m_speed,0));
}

