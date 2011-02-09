
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
    pixmapInsert(MWindow::North, "pN.PNG", "pN1.PNG", "pN2.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor);
    pixmapInsert(MWindow::NorthEast, "pNE.PNG", "pNE1.PNG", "pNE2.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor);
    pixmapInsert(MWindow::East, "pE.PNG", "pE1.PNG", "pE2.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor);
    pixmapInsert(MWindow::SouthEast, "pSE.PNG", "pSE1.PNG", "pSE2.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor);
    pixmapInsert(MWindow::South, "pS.PNG", "pS1.PNG", "pS2.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor);
    pixmapInsert(MWindow::SouthWest, "pSW.PNG", "pSW1.PNG", "pSW2.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor);
    pixmapInsert(MWindow::West, "pW.PNG", "pW1.PNG", "pW2.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor);
    pixmapInsert(MWindow::NorthWest, "pNW.PNG", "pNW1.PNG", "pNW2.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor);

    pixmapInsert(MWindow::Tackle, "tackleN.PNG", "tackleN.PNG", "tackleN.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor); // TODO XXX TIM
    pixmapInsert(MWindow::FallenOver, "pNW.PNG", "pNW1.PNG", "pNW2.PNG", KGoalKeeperShirtColor, KGoalKeeperShortColor); // TODO XXX TIM
    // set default pixmap
    setPixmap(m_images[MWindow::North].at(0));
}

void GoalKeeper::pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3, QRgb goalKeeperShirtColor, QRgb goalKeeperShortColor)
{
    QString s(":/images/red/");

    QString n1(s), n2(s), n3(s);
    QPixmap p1, p2, p3;

    teamColorTransform(p1, n1.append(s1), qRgb(255, 0, 0), goalKeeperShirtColor, goalKeeperShortColor);
    teamColorTransform(p2, n2.append(s2), qRgb(255, 0, 0), goalKeeperShirtColor, goalKeeperShortColor);
    teamColorTransform(p3, n3.append(s3), qRgb(255, 0, 0), goalKeeperShirtColor, goalKeeperShortColor);

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
    if ( hasBall() )
         gkAdvanceWithBall();
    else
        gkAdvanceWithoutBall();
}

void GoalKeeper::gkAdvanceWithoutBall()
{
//    if (!m_team->teamHasBall()) {
        // if the ball enters the penalty area then go for it, otherwise return to goal line
        Team::Direction dir = m_team->getDirection();
        MWindow::Action action;

            if ( (dir == Team::SouthToNorth
                && m_pitch->m_bottomPenaltyArea->contains(m_pitch->ball()->pos()) )
            || (dir == Team::NorthToSouth
                && m_pitch->m_topPenaltyArea->contains(m_pitch->ball()->pos())) ) {

            qreal dx = abs(pos().x() - m_pitch->ball()->pos().x());
            qreal dy = abs(pos().y() - m_pitch->ball()->pos().y());
            if ( m_pitch->ball()->ballOwner() && ( dx < 15) && (dy < 15) )
                action = MWindow::Tackle;
            else
                action = calculateAction(pos(), m_pitch->ball()->pos());
        } else {
            QPointF ownGoal;
            action = calculateAction( pos(), m_startPositionRectF.center() );
        }
        move(action);
//    }
}

void GoalKeeper::gkAdvanceWithBall()
{
    Team::Direction dir = m_team->getDirection();
    if ( dir == Team::SouthToNorth )
        m_lastAction = MWindow::North;
    else
        m_lastAction = MWindow::South;
    move(MWindow::Shot);
}


