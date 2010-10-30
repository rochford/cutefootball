#include <QtGui>
#include "ball.h"
#include "team.h"

#include <QDebug>

Ball::Ball(Pitch* pitch)
    : QObject(),
    QGraphicsPixmapItem(QPixmap(QString(":/images/ball.png")),NULL),
    pitch_(pitch),
    destination_(QPointF(0,0)),
    start_(pitch_->scene->sceneRect().center()),
    step_(0),
    animation_(NULL),
    animationTimer_(NULL),
    controlledBy_(NULL)
{
    setPos(start_.x(), start_.y());
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setZValue(8);

    animation_ = new QGraphicsItemAnimation(this);
    animationTimer_ = new QTimeLine(1000, this);
    animationTimer_->setFrameRange(0, 20);

    animation_->setItem(this);
    animation_->setTimeLine(animationTimer_);
    connect(animationTimer_, SIGNAL(frameChanged(int)), this, SLOT(updateBall(int)));
}

Ball::~Ball()
{
    // do nothing
}

QRectF Ball::boundingRect() const
{
    return QRectF(-9*KScaleFactor, -9*KScaleFactor,
                  9*KScaleFactor, 9*KScaleFactor);
}

void Ball::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget)
{
    switch(step_ % 4) {
    case 0:
        setPixmap(QPixmap(QString(":/images/ball.png")));
        break;
    case 1:
        setPixmap(QPixmap(QString(":/images/ball1.png")));
        break;
    case 2:
        setPixmap(QPixmap(QString(":/images/ball2.png")));
        break;
    case 3:
        setPixmap(QPixmap(QString(":/images/ball3.png")));
        break;
    }
    // Scale QGraphicsPixmapItem to wanted 'size' and keep the aspect ratio
    QSize pixmapSize = pixmap().size();
    pixmapSize.scale(QSizeF(20*KScaleFactor,20*KScaleFactor).toSize(), Qt::KeepAspectRatio);

    // Draw QGraphicsPixmapItem face
    painter->drawPixmap(boundingRect().toRect(), pixmap());
}

QPainterPath Ball::shape() const
{
    QPainterPath path;
    path.addRect(-10*KScaleFactor, -10*KScaleFactor,
                 10*KScaleFactor, 10*KScaleFactor);
    return path;
}

void Ball::advance(int phase)
{
    if (!phase)
        return;
}

void Ball::moveBall(MWindow::Action action, int speed)
{
    QMap<MWindow::Action,QPointF> moveDistance_;
    moveDistance_.insert(MWindow::North, QPointF(0,-speed));
    moveDistance_.insert(MWindow::NorthEast, QPointF(speed,-speed));
    moveDistance_.insert(MWindow::East, QPointF(speed,0));
    moveDistance_.insert(MWindow::SouthEast, QPointF(speed,speed));
    moveDistance_.insert(MWindow::South, QPointF(0,speed));
    moveDistance_.insert(MWindow::SouthWest, QPointF(-speed,speed));
    moveDistance_.insert(MWindow::West, QPointF(-speed,0));
    moveDistance_.insert(MWindow::NorthWest, QPointF(-speed,-speed));

    // old position
    QPointF oldPos=pos();
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
        moveBy(moveDistance_[action].x(), moveDistance_[action].y());
        break;
    case MWindow::Shoot:
        // TODO shootBall(speed);
        break;
    }

    previousAction = action;
}

void Ball::moveBall(MWindow::Action action, QPointF destination)
{
    // calculate the difference between present and destination
    QPointF tmp = pos();

    const int stepX = (destination.x() - tmp.x()) / 20;
    const int stepY = (destination.y() - tmp.y()) / 20;

    for (int i = 0; i < 20.0; ++i) {
        animation_->setPosAt(i / 20.0, QPointF(tmp.x() + stepX,
                                               tmp.y() + stepY));
        tmp.setX(tmp.x() + stepX);
        tmp.setY(tmp.y() + stepY);
    }
    animationTimer_->stop();
    animationTimer_->start();
}

void Ball::updateBall(int frame)
{
    // animation may no longer be running due to a goal
    if ( animationTimer_->state() == QTimeLine::Running ) {
        setPos(animation_->posAt(frame/20.0));
    }
}

QVariant Ball::itemChange(GraphicsItemChange change, const QVariant &value)
 {
     if (change == ItemPositionChange && scene()) {
         step_++;

         // value is the new position.
         QPointF newPos = value.toPointF();
         QRectF rect = pitch_->footballPitch_->rect();

         // has a goal been scored?
         if (pitch_->topGoal->contains(newPos)
             || pitch_->bottomGoal->contains(newPos)) {
             animationTimer_->stop();
             emit goalScored(pitch_->topGoal->contains(newPos));
             setControlledBy(NULL);
             return start_;
         }

         // is the ball out of bounds?
         if (!rect.contains(newPos)) {
             // no player controls ball now
             setControlledBy(NULL);

             // goal kick or corner?
             bool homeTeamTouchedLast = pitch_->homeTeam_->teamHasBall_;
             Team::Direction dir;
             if (homeTeamTouchedLast)
                 dir = pitch_->awayTeam_->getDirection();
             else
                 dir = pitch_->homeTeam_->getDirection();

             // throw in?
             if (rect.right() < newPos.x()) {
                 pitch_->setPiece(dir, Pitch::ThrowIn);
                 newPos.setX(pitch_->footballPitch_->rect().right());
                 return newPos;
             }
             if (rect.x() > newPos.x()) {
                 pitch_->setPiece(dir, Pitch::ThrowIn);
                 newPos.setX(pitch_->footballPitch_->rect().x());
                 return newPos;
             }
         }
     }
     return QGraphicsItem::itemChange(change, value);
 }
