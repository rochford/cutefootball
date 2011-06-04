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

#include "goalkeeper.h"
#include "soccerutils.h"

#include "pitch.h"
#include "ball.h"
#include <QDebug>

const int KDiveDistance(25);

GoalKeeper::GoalKeeper(QString name,
                       int number,
                       Pitch *pitch,
                       Team* team,
                       QColor hairColor,
                       QColor skinColor)
    : Player(name,number,true,pitch,
          team,
          5.0,
          Player::GoalKeeper,
          hairColor,
          skinColor)
{
    connect(pitch->ball(), SIGNAL(shot(Team*,QPointF)),
            this,SLOT(goalAttempt(Team*,QPointF)));
}

void GoalKeeper::goalAttempt(Team* t, QPointF dest)
{
    // if the ball enters the penalty area then go for it, otherwise return to goal line
    Team::Direction dir = m_team->getDirection();
    MWindow::Action action;

    if ( (dir == Team::SouthToNorth
        && m_pitch->m_bottomPenaltyArea->contains(dest)
    || (dir == Team::NorthToSouth
        && m_pitch->m_topPenaltyArea->contains(dest)) ) ){

        qDebug() << "need to save it";
        m_destination = dest;
        action = calculateAction(pos(), m_destination);
        if (action == MWindow::East) {
            m_destination = QPointF(pos().x()+KDiveDistance,pos().y());
            move(MWindow::DiveEast);
        }
        else if (action == MWindow::West) {
            m_destination = QPointF(pos().x()-KDiveDistance,pos().y());
            move(MWindow::DiveWest);
        }
    }
}

void GoalKeeper::createPixmaps()
{
    pixmapInsert(MWindow::North, "pN.PNG", "pN1.PNG", "pN2.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::NorthEast, "pNE.PNG", "pNE1.PNG", "pNE2.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::East, "pE.PNG", "pE1.PNG", "pE2.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::SouthEast, "pSE.PNG", "pSE1.PNG", "pSE2.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::South, "pS.PNG", "pS1.PNG", "pS2.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::SouthWest, "pSW.PNG", "pSW1.PNG", "pSW2.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::West, "pW.PNG", "pW1.PNG", "pW2.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::NorthWest, "pNW.PNG", "pNW1.PNG", "pNW2.PNG", KGKShirtColor, KGKShortColor);

    pixmapInsert(MWindow::TackleN, "tackleN.PNG", "tackleN.PNG", "tackleN.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::TackleNE, "tackleNE.PNG", "tackleNE.PNG", "tackleNE.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::TackleNW, "tackleNW.PNG", "tackleNW.PNG", "tackleNW.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::TackleE, "tackleE.PNG", "tackleE.PNG", "tackleE.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::TackleSE, "tackleSE.PNG", "tackleSE.PNG", "tackleSE.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::TackleS, "tackleS.PNG", "tackleS.PNG", "tackleS.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::TackleSW, "tackleSW.PNG", "tackleSW.PNG", "tackleSW.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::TackleW, "tackleW.PNG", "tackleW.PNG", "tackleW.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::FallenOver, "pTackled.PNG", "pTackled.PNG", "pTackled.PNG", KGKShirtColor, KGKShortColor);

    pixmapInsert(MWindow::DiveEast, "diveE.PNG", "diveE.PNG", "diveE.PNG", KGKShirtColor, KGKShortColor);
    pixmapInsert(MWindow::DiveWest, "diveW.PNG", "diveW.PNG", "diveW.PNG", KGKShirtColor, KGKShortColor);

    // set default pixmap
    setPixmap(m_images[MWindow::North].at(0));
}

void GoalKeeper::pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3, QRgb goalKeeperShirtColor, QRgb goalKeeperShortColor)
{
    QString s(":/images/red/");

    QString n1(s), n2(s), n3(s);
    QPixmap p1, p2, p3;

    teamColorTransform(p1, n1.append(s1), qRgb(255, 0, 0), goalKeeperShirtColor, goalKeeperShortColor, m_hairColor.rgb(), m_skinColor.rgb());
    teamColorTransform(p2, n2.append(s2), qRgb(255, 0, 0), goalKeeperShirtColor, goalKeeperShortColor, m_hairColor.rgb(), m_skinColor.rgb());
    teamColorTransform(p3, n3.append(s3), qRgb(255, 0, 0), goalKeeperShirtColor, goalKeeperShortColor, m_hairColor.rgb(), m_skinColor.rgb());

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
    if ( hasBall() ) {
  //      qDebug() << "GoalKeeper::advance has ball";
        gkAdvanceWithBall();
    } else {
        // qDebug() << "GoalKeeper::advance without ball";
        gkAdvanceWithoutBall();
    }
}

void GoalKeeper::gkAdvanceWithoutBall()
{
    // if the ball enters the penalty area then go for it, otherwise return to goal line
    Team::Direction dir = m_team->getDirection();
    MWindow::Action action;

//    QPointF dst;
    if ( (dir == Team::SouthToNorth
        && m_pitch->m_bottomPenaltyArea->contains(m_pitch->ball()->pos()) )
    || (dir == Team::NorthToSouth
        && m_pitch->m_topPenaltyArea->contains(m_pitch->ball()->pos())) )
        //qDebug() << "GoalKeeper::gkAdvanceWithoutBall";
        m_destination = m_pitch->ball()->pos();
    else {
        m_destination = m_startPositionRectF.center();
    }
    action = calculateAction(pos(), m_destination);
    move(action);
}

void GoalKeeper::gkAdvanceWithBall()
{
//    qDebug() << "GoalKeeper::gkAdvanceWithBall";
    Team::Direction dir = m_team->getDirection();
    if ( dir == Team::SouthToNorth )
        m_lastAction = MWindow::North;
    else
        m_lastAction = MWindow::South;
    move(MWindow::Pass);
}


