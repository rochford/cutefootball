#include "referee.h"

#include "pitch.h"
#include "ball.h"

Referee::Referee(Pitch* pitch, QObject *parent)
    : Player("xxx",true,pitch,NULL,Player::LastDummy)

{
    setPixmap(QPixmap(QString(":/images/ref.png")));

    QPointF start(pitch_->m_footballPitch->rect().center().x() + 25, pitch_->m_footballPitch->rect().center().y() + 50);
    setPos(start);
}


void Referee::advance(int phase)
{
    if (!phase)
        return;

    MWindow::Action action = calculateAction(pos(), pitch_->getBall()->pos());

    // if within a few pixels then dont move towards ball anymore...
    int dx = abs(pos().x() - pitch_->getBall()->pos().x());
    int dy = abs(pos().y() - pitch_->getBall()->pos().y());

    // if too close, then move away from ball (do reverse action)
    if (dx < 15 || dy < 25) {
        switch(action)
        {
        case MWindow::North:
            action = MWindow::South;
            break;
        case MWindow::NorthEast:
            action = MWindow::SouthWest;
            break;
        case MWindow::East:
            action = MWindow::West;
            break;
        case MWindow::SouthEast:
            action = MWindow::NorthWest;
            break;
        case MWindow::South:
            action = MWindow::North;
            break;
        case MWindow::SouthWest:
            action = MWindow::NorthEast;
            break;
        case MWindow::West:
            action = MWindow::East;
            break;
        case MWindow::NorthWest:
            action = MWindow::SouthEast;
            break;
        }
    }

    // make the move
    switch(action)
    {
    case MWindow::North:
    case MWindow::NorthEast:
    case MWindow::East:
    case MWindow::SouthEast:
    case MWindow::South:
    case MWindow::SouthWest:
    case MWindow::West:
    case MWindow::NorthWest:
   //     setPixmap(QPixmap(images_[action].at(step % 3)));
        moveBy(moveDistance_[action].x(), moveDistance_[action].y());
        break;
    }
}

void Referee::createPixmaps()
{
    QString ref(":/images/ref.png");

    QPixmapList list;
    list << QPixmap(ref) << QPixmap(ref) << QPixmap(ref);
    images_.insert(MWindow::North, list);
    images_.insert(MWindow::NorthEast, list);
    images_.insert(MWindow::East, list);
    images_.insert(MWindow::SouthEast, list);
    images_.insert(MWindow::South, list);
    images_.insert(MWindow::SouthWest, list);
    images_.insert(MWindow::West, list);
    images_.insert(MWindow::NorthWest, list);
}
