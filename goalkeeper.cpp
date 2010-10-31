#include "goalkeeper.h"

#include "pitch.h"
#include "ball.h"


GoalKeeper::GoalKeeper(Pitch *pitch,
                       Team* team)
    : Player(true,pitch,team,Player::GoalKeeper)
{
    setPixmap(QPixmap(QString(":/images/keeperNorth.PNG")));
}

void GoalKeeper::createPixmaps()
{
    QString gkN(":/images/keeperNorth.PNG");

    QStringList list;
    list << gkN << gkN << gkN;
    images_.insert(MWindow::North, list);
    images_.insert(MWindow::NorthEast, list);
    images_.insert(MWindow::East, list);
    images_.insert(MWindow::SouthEast, list);
    images_.insert(MWindow::South, list);
    images_.insert(MWindow::SouthWest, list);
    images_.insert(MWindow::West, list);
    images_.insert(MWindow::NorthWest, list);
}

void GoalKeeper::advance(int phase)
{
    if (!phase)
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
            if ( pitch_->getBall()->controlledBy() && ( dx < 15) && (dy < 15) )
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
