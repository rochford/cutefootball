#include <QtGui>

#include "ball.h"
#include "team.h"
#include "soundeffects.h"

#include <QDebug>

Ball::Ball(Pitch* pitch)
    : QObject(),
    QGraphicsPixmapItem(QPixmap(QString(":/images/ball.png")),NULL),
    m_pitch(pitch),
    destination_(QPointF(0,0)),
    start_(m_pitch->m_scene->sceneRect().center()),
    step_(0),
    animation_(NULL),
    animationTimer_(NULL),
    controlledBy_(NULL)
{
    QBitmap bitmap = pixmap().createMaskFromColor(KCuteFootballMaskColor);
    pixmap().setMask(bitmap);

    setTransformOriginPoint(boundingRect().center());
    setPos(start_.x(), start_.y());
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setZValue(8);

    animation_ = new QGraphicsItemAnimation(this);
    animationTimer_ = new QTimeLine(1000, this);
    animationTimer_->setFrameRange(0, 40);

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
    QMap<MWindow::Action,QPointF> moveDistance;
    moveDistance.insert(MWindow::North, QPointF(0,-speed));
    moveDistance.insert(MWindow::NorthEast, QPointF(speed,-speed));
    moveDistance.insert(MWindow::East, QPointF(speed,0));
    moveDistance.insert(MWindow::SouthEast, QPointF(speed,speed));
    moveDistance.insert(MWindow::South, QPointF(0,speed));
    moveDistance.insert(MWindow::SouthWest, QPointF(-speed,speed));
    moveDistance.insert(MWindow::West, QPointF(-speed,0));
    moveDistance.insert(MWindow::NorthWest, QPointF(-speed,-speed));

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
        setRotation((step_%4)*90.0);
        moveBy(moveDistance[action].x(), moveDistance[action].y());
        break;
    case MWindow::Shot:
        // TODO shootBall(speed);
        break;
    }

    previousAction = action;
}

void Ball::kickBall(MWindow::Action action, QPointF destination)
{
    // calculate the difference between present and destination
    QPointF tmp = pos();

    const qreal stepX = (destination.x() - tmp.x()) / 40.0;
    const qreal stepY = (destination.y() - tmp.y()) / 40.0;

    for (int i = 0; i <= 40; ++i) {
        animation_->setPosAt(i / 40.0, QPointF(tmp.x() + stepX,
                                               tmp.y() + stepY));
        // Rotation in animations does not seem to work
        // animation_->setRotationAt(i / 40.0, i*90.0);
        tmp.setX(tmp.x() + stepX);
        tmp.setY(tmp.y() + stepY);
    }
    animationTimer_->stop();
    animationTimer_->start();

    if (action == MWindow::Shot)
        emit shot(destination);
    emit soundEvent(SoundEffects::BallKick);
}

void Ball::updateBall(int frame)
{
    // animation may no longer be running due to a goal
    if ( animationTimer_->state() == QTimeLine::Running ) {
        setPos(animation_->posAt(frame/40.0));
        // Rotation in animations does not seem to work
        // setRotation(animation_->rotationAt(frame/40.0));
    }
}

QVariant Ball::itemChange(GraphicsItemChange change, const QVariant &value)
 {
     if (change == ItemPositionChange && scene()) {
         step_++;

         // value is the new position.
         QPointF newPos = value.toPointF();
         QRectF rect = m_pitch->m_footballPitch->rect();

         // has a goal been scored?
         if (m_pitch->m_topGoal->contains(newPos)
             || m_pitch->m_bottomGoal->contains(newPos)) {
             animationTimer_->stop();
             emit goalScored(m_pitch->m_topGoal->contains(newPos));
             setControlledBy(NULL);
             return start_;
         }

         // is the ball out of bounds?
         if (!rect.contains(newPos)) {
             // no player controls ball now
             setControlledBy(NULL);

             // goal kick or corner?
             bool homeTeamTouchedLast = m_pitch->homeTeam()->teamHasBall_;

             // throw in?
             if (rect.right() < newPos.x()) {
                 m_pitch->setPiece(homeTeamTouchedLast ? m_pitch->awayTeam() : m_pitch->homeTeam(), Pitch::ThrowIn);
                 newPos.setX(m_pitch->m_footballPitch->rect().right());
                 return newPos;
             }
             if (rect.x() > newPos.x()) {
                 m_pitch->setPiece(homeTeamTouchedLast ? m_pitch->awayTeam() : m_pitch->homeTeam(), Pitch::ThrowIn);
                 newPos.setX(m_pitch->m_footballPitch->rect().left());
                 return newPos;
             }

             // goal kick?
             if ( rect.top() > newPos.y()) {
                 newPos.setX(m_pitch->m_topGoal->rect().x());
                 newPos.setY(m_pitch->m_topGoal->rect().y() + 20);
                 m_pitch->setPiece(homeTeamTouchedLast ? m_pitch->awayTeam() : m_pitch->homeTeam(), Pitch::GoalKick);
                 return newPos;
             }
             if (rect.bottom() < newPos.y()) {
                 newPos.setX(m_pitch->m_bottomGoal->rect().x());
                 newPos.setY(m_pitch->m_bottomGoal->rect().y() - 20);
                 m_pitch->setPiece(homeTeamTouchedLast ? m_pitch->awayTeam() : m_pitch->homeTeam(), Pitch::GoalKick);
                 return newPos;
             }
         }
     }
     return QGraphicsItem::itemChange(change, value);
 }
