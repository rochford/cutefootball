
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
    pixmapInsert(MWindow::North, "playerNorth.PNG", "playerNorth1.PNG", "playerNorth2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::NorthEast, "playerNorthEast.PNG", "playerNorthEast1.PNG", "playerNorthEast2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::East, "playerEast.PNG", "playerEast1.PNG", "playerEast2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::SouthEast, "playerSouthEast.PNG", "playerSouthEast1.PNG", "playerSouthEast2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::South, "playerSouth.PNG", "playerSouth1.PNG", "playerSouth2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::SouthWest, "playerSouthWest.PNG", "playerSouthWest1.PNG", "playerSouthWest2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::West, "playerWest.PNG", "playerWest1.PNG", "playerWest2.PNG", KGoalKeeperColor);
    pixmapInsert(MWindow::NorthWest, "playerNorthWest.PNG", "playerNorthWest1.PNG", "playerNorthWest2.PNG", KGoalKeeperColor);

    pixmapInsert(MWindow::ThrownIn, "playerNorthWest.PNG", "playerNorthWest1.PNG", "playerNorthWest2.PNG", KGoalKeeperColor); // TODO XXX TIM

    pixmapInsert(MWindow::TackleNorth, "tackleNorth.PNG", "tackleNorth.PNG", "tackleNorth.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleNorthEast, "tackleNorthEast.PNG", "tackleNorthEast.PNG", "tackleNorthEast.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleEast, "tackleEast.PNG", "tackleEast.PNG", "tackleEast.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleSouthEast, "tackleSouthEast.PNG", "tackleSouthEast.PNG", "tackleSouthEast.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleSouth, "tackleSouth.PNG", "tackleSouth.PNG", "tackleSouth.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleSouthWest, "tackleSouthWest.PNG", "tackleSouthWest.PNG", "tackleSouthWest.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleWest, "tackleWest.PNG", "tackleWest.PNG", "tackleWest.PNG", KGoalKeeperColor); // TODO XXX TIM
    pixmapInsert(MWindow::TackleNorthWest, "tackleNorthWest.PNG", "tackleNorthWest.PNG", "tackleNorthWest.PNG", KGoalKeeperColor); // TODO XXX TIM

    pixmapInsert(MWindow::GoalCelebration, "playerNorth.PNG", "playerNorth1.PNG", "playerNorth2.PNG", KGoalKeeperColor); // TODO XXX TIM

    pixmapInsert(MWindow::DiveLeft, "playerNorth.PNG", "playerNorth1.PNG", "playerNorth2.PNG", KGoalKeeperColor); // TODO
    pixmapInsert(MWindow::DiveRight, "playerNorth.PNG", "playerNorth1.PNG", "playerNorth2.PNG", KGoalKeeperColor); // TODO

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

            int dx = abs(pos().x() - m_pitch->ball()->pos().x());
            int dy = abs(pos().y() - m_pitch->ball()->pos().y());
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

