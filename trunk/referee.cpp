#include "referee.h"

#include "pitch.h"
#include "ball.h"

namespace {
    Referee::RefereeAction calculateAction(QPointF source,
                                    QPointF destination)
    {
        const int dx = source.x() - destination.x();
        const int dy = source.y() - destination.y();
        if ( dx > 0 && dy == 0 )
            return Referee::West;
        else if ( dx >= 0 && dy < 0)
            return Referee::SouthWest;
        else if ( dx > 0 && dy > 0)
            return Referee::NorthWest;
        else if ( dx <= 0 && dy == 0)
            return Referee::East;
        else if ( dx < 0 && dy > 0)
            return Referee::NorthEast;
        else if ( dx < 0 && dy < 0)
            return Referee::SouthEast;
        else if ( dx == 0 && dy >= 0 )
            return Referee::North;
        else if ( dx == 0 && dy <= 0 )
            return Referee::South;
    }
}

Referee::Referee(Pitch* pitch, QObject *parent) :
    QGraphicsPixmapItem(NULL),
    pitch_(pitch),
    speed_(KPlayerDefaultSpeed),
    step_(0)
{
    setPixmap(QPixmap(QString(":/images/ref.png")));
    moveDistance_.insert(North, QPointF(0,-speed_));
    moveDistance_.insert(NorthEast, QPointF(speed_,-speed_));
    moveDistance_.insert(East, QPointF(speed_,0));
    moveDistance_.insert(SouthEast, QPointF(speed_,speed_));
    moveDistance_.insert(South, QPointF(0,speed_));
    moveDistance_.insert(SouthWest, QPointF(-speed_,speed_));
    moveDistance_.insert(West, QPointF(-speed_,0));
    moveDistance_.insert(NorthWest, QPointF(-speed_,-speed_));

    QPointF start(pitch_->footballPitch_->rect().center().x() + 25, pitch_->footballPitch_->rect().center().y() + 50);
    setPos(start);
}

QRectF Referee::boundingRect() const
{
    return QRectF(-18*KScaleFactor, -18*KScaleFactor,
                  18*KScaleFactor, 18*KScaleFactor);
}

QPainterPath Referee::shape() const
{
    QPainterPath path;
    path.addRect(-18*KScaleFactor, -18*KScaleFactor,
                 18*KScaleFactor, 18*KScaleFactor);
    return path;
}

void Referee::advance(int phase)
{
    if (!phase)
        return;

    RefereeAction action = calculateAction(pos(), pitch_->getBall()->pos());
    step_++;

    // if within a few pixels then dont move towards ball anymore...
    int dx = abs(pos().x() - pitch_->getBall()->pos().x());
    int dy = abs(pos().y() - pitch_->getBall()->pos().y());

    // if too close, then move away from ball (do reverse action)
    if (dx < 15 || dy < 25) {
        switch(action)
        {
        case North:
            action = South;
            break;
        case NorthEast:
            action = SouthWest;
            break;
        case East:
            action = West;
            break;
        case SouthEast:
            action = NorthWest;
            break;
        case South:
            action = North;
            break;
        case SouthWest:
            action = NorthEast;
            break;
        case West:
            action = East;
            break;
        case NorthWest:
            action = SouthEast;
            break;
        }
    }

    // make the move
    switch(action)
    {
    case North:
    case NorthEast:
    case East:
    case SouthEast:
    case South:
    case SouthWest:
    case West:
    case NorthWest:
   //     setPixmap(QPixmap(images_[action].at(step % 3)));
        moveBy(moveDistance_[action].x(), moveDistance_[action].y());
        break;
    }
}

void Referee::paint(QPainter *painter,
                    const QStyleOptionGraphicsItem *option,
                    QWidget *widget)
{
    QSize pixmapSize = pixmap().size();
    pixmapSize.scale(QSizeF(36*KScaleFactor,36*KScaleFactor).toSize(), Qt::KeepAspectRatio);

    // Draw QGraphicsPixmapItem face
    painter->drawPixmap(boundingRect().toRect(), pixmap());
}
