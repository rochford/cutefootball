#include "referee.h"

#include "pitch.h"
#include "ball.h"

Referee::Referee(Pitch* pitch, QObject *parent)
    : Player("xxx",true,pitch,NULL,Player::LastDummy)

{
    setPixmap(QPixmap(QString(":/images/ref.png")));

    QPointF start(m_pitch->m_footballPitch->rect().center().x() + 25, m_pitch->m_footballPitch->rect().center().y() + 50);
    setPos(start);
}


void Referee::advance(int phase)
{
    if (!phase)
        return;

    MWindow::Action action = calculateAction(pos(), m_pitch->ball()->pos());

    // if within a few pixels then dont move towards ball anymore...
    int dx = abs(pos().x() - m_pitch->ball()->pos().x());
    int dy = abs(pos().y() - m_pitch->ball()->pos().y());

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
        setPixmap(QPixmap(m_images[action].at(m_step % 3)));
        moveBy(m_moveDistance[action].x(), m_moveDistance[action].y());
        break;
    }
}

void Referee::createPixmaps()
{
    QString ref(":/images/ref.png");

    PixmapList list;
    list << QPixmap(ref) << QPixmap(ref) << QPixmap(ref);

    m_images.insert(MWindow::North, list);
    m_images.insert(MWindow::NorthEast, list);
    m_images.insert(MWindow::East, list);
    m_images.insert(MWindow::SouthEast, list);
    m_images.insert(MWindow::South, list);
    m_images.insert(MWindow::SouthWest, list);
    m_images.insert(MWindow::West, list);
    m_images.insert(MWindow::NorthWest, list);
}
