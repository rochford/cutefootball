
#include "goalkeeper.h"

#include "pitch.h"
#include "ball.h"
#include <QDebug>

GoalKeeper::GoalKeeper(QString name,
                       Pitch *pitch,
                       Team* team)
    : Player(name,true,pitch,team,Player::GoalKeeper)
{
    setPixmap(QPixmap(QString(":/images/keeperNorth.PNG")));
    connect(pitch_->getBall(), SIGNAL(shot(QPointF)), this, SLOT(shotAttempted(QPointF)));
}

void GoalKeeper::createPixmaps()
{
    QString gkN(":/images/keeperNorth.PNG");

    QPixmapList list;
    list << QPixmap(gkN) << QPixmap(gkN) << QPixmap(gkN);
    images_.insert(MWindow::North, list);
    images_.insert(MWindow::NorthEast, list);
    images_.insert(MWindow::East, list);
    images_.insert(MWindow::SouthEast, list);
    images_.insert(MWindow::South, list);
    images_.insert(MWindow::SouthWest, list);
    images_.insert(MWindow::West, list);
    images_.insert(MWindow::NorthWest, list);

    images_.insert(MWindow::TackleNorth, list);
    images_.insert(MWindow::TackleNorthEast, list);
    images_.insert(MWindow::TackleEast, list);
    images_.insert(MWindow::TackleSouthEast, list);
    images_.insert(MWindow::TackleSouth, list);
    images_.insert(MWindow::TackleSouthWest, list);
    images_.insert(MWindow::TackleWest, list);
    images_.insert(MWindow::TackleNorthWest, list);

    images_.insert(MWindow::GoalCelebration, list); // TODO

    images_.insert(MWindow::DiveLeft, list); // TODO
    images_.insert(MWindow::DiveRight, list); // TODO
}

void GoalKeeper::advance(int phase)
{
    if (!phase)
        return;
    if (outOfAction_->isActive())
        return;
    if ( hasBall_ )
         gkAdvanceWithBall();
    else
        gkAdvanceWithoutBall();
}

void GoalKeeper::gkAdvanceWithoutBall()
{
    if (!team_->teamHasBall_) {
        // if the ball enters the penalty area then go for it.

        if ( (team_->getDirection() == Team::SouthToNorth
                && pitch_->bottomPenaltyArea->contains(pitch_->getBall()->pos()) )
            || (team_->getDirection() == Team::NorthToSouth
                && pitch_->topPenaltyArea->contains(pitch_->getBall()->pos())) ) {
            MWindow::Action action;
            int dx = abs(pos().x() - pitch_->getBall()->pos().x());
            int dy = abs(pos().y() - pitch_->getBall()->pos().y());
            if ( pitch_->getBall()->controlledBy() && ( dx < 5) && (dy < 5) )
                action = MWindow::Tackle;
            else
                action = calculateAction(pos(), pitch_->getBall()->pos());

            move(action);
        }
    }
}

void GoalKeeper::gkAdvanceWithBall()
{
    move(MWindow::Pass);
}

void GoalKeeper::shotAttempted(QPointF dest)
{
    qDebug() << "shotAttempted to destination " << dest.x() << dest.y();
    if (team_->getDirection() == Team::SouthToNorth
        && pitch_->topGoal->contains(dest) ) {
        qDebug() << "shotAttempted need to save it  ";
    } else if (team_->getDirection() == Team::NorthToSouth
               && pitch_->bottomGoal->contains(dest) ) {
        qDebug() << "shotAttempted need to save it  ";
        move(MWindow::DiveLeft);

    }
}

void GoalKeeper::createMoves()
{
    moveDistance_.insert(MWindow::North, QPointF(0,-speed_));
    moveDistance_.insert(MWindow::NorthEast, QPointF(speed_,-speed_));
    moveDistance_.insert(MWindow::East, QPointF(speed_,0));
    moveDistance_.insert(MWindow::SouthEast, QPointF(speed_,speed_));
    moveDistance_.insert(MWindow::South, QPointF(0,speed_));
    moveDistance_.insert(MWindow::SouthWest, QPointF(-speed_,speed_));
    moveDistance_.insert(MWindow::West, QPointF(-speed_,0));
    moveDistance_.insert(MWindow::NorthWest, QPointF(-speed_,-speed_));

    moveDistance_.insert(MWindow::DiveLeft, QPointF(5*speed_,0));
    moveDistance_.insert(MWindow::DiveRight, QPointF(5*-speed_,0));
}
