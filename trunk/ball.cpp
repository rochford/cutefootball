#include <QtGui>

#include "ball.h"
#include "team.h"
#include "soundeffects.h"
#include "Player.h"

#include <QDebug>

Ball::Ball(Pitch* pitch)
    : QObject(),
    QGraphicsPixmapItem(QPixmap(QString(":/images/ball.png")),NULL),
    m_pitch(pitch),
    destination_(QPointF(0,0)),
    start_(m_pitch->m_scene->sceneRect().center()),
    step_(0),
    animation_(NULL),
    m_animationTimer(NULL),
    m_ballOwner(NULL),
    m_lastPlayerToTouchBall(NULL),
    m_positionLocked(false)
{
    m_ballOwnerTimer = new QTimer(this);
    m_ballOwnerTimer->setInterval(250);
    m_ballOwnerTimer->start();

    QBitmap bitmap = pixmap().createMaskFromColor(KCuteFootballMaskColor);
    pixmap().setMask(bitmap);

    setTransformOriginPoint(boundingRect().center());
    setStartingPosition();
    setZValue(Pitch::ZBall);

    animation_ = new QGraphicsItemAnimation(this);
    m_animationTimer = new QTimeLine(1000, this);
    m_animationTimer->setFrameRange(0, 40);

    animation_->setItem(this);
    animation_->setTimeLine(m_animationTimer);
    connect(m_animationTimer, SIGNAL(frameChanged(int)), this, SLOT(updateBall(int)));
    connect(m_ballOwnerTimer, SIGNAL(timeout()), this, SLOT(updateBallOwner()));
}

Ball::~Ball()
{
    m_ballOwnerTimer->stop();
    delete m_ballOwnerTimer;
    delete animation_;
    delete m_animationTimer;
}

void Ball::updateBallOwner()
{
    // find a player who is near the ball.
    // that player, now controls the ball
    QList<QGraphicsItem*> list = m_pitch->m_scene->collidingItems(this,
                                     Qt::IntersectsItemBoundingRect);

    foreach(QGraphicsItem* i, list) {
        Player* p = qgraphicsitem_cast<Player*>(i);
        if (p) {
            if ( p == m_ballOwner ) {
                p->setHasBall(true);
                break;
            }
            if ( m_ballOwner )
                 m_ballOwner->setHasBall(false);
            m_ballOwner = p;
            p->setHasBall(true);
        }
    }
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
    if (m_positionLocked) {
        return;
    }

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
    if (m_positionLocked)
        return;

    Team* team(NULL);
    if ( m_ballOwner ) {
        m_ballOwner->setHasBall(false);
        team = m_ballOwner->team();
    }
    setNoBallOwner();

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
    m_animationTimer->stop();
    m_animationTimer->start();

    if (action == MWindow::Shot)
        emit shot(team, destination);
    emit soundEvent(SoundEffects::BallKick);
}

void Ball::updateBall(int frame)
{
    qDebug() << "Ball::updateBall" << frame;
    // animation may no longer be running due to a goal
    if ( (m_animationTimer->state() == QTimeLine::Running) && !m_positionLocked ) {
        QPointF newPos = animation_->posAt(frame/40.0);
        setPos(newPos);
        // Rotation in animations does not seem to work
        // setRotation(animation_->rotationAt(frame/40.0));
    }
}

QVariant Ball::itemChange(GraphicsItemChange change, const QVariant &value)
 {
     if (change == ItemPositionChange && scene() ) {
         if (m_positionLocked)
             return m_lastPos;

         step_++;

         // Ball is either on the pitch or in a goal, otherwise return last position
         // value is the new position.
         QPointF newPos = value.toPointF();
         QRectF pitchRect = m_pitch->m_footballPitch->rect();

         // has a goal been scored?
         if (m_pitch->m_topGoal->contains(newPos)
             || m_pitch->m_bottomGoal->contains(newPos)) {
             setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
             m_positionLocked = true;
             qDebug() << "Ball::itemChange goal scored";
             emit goalScored(m_pitch->m_topGoal->contains(newPos));
             setBallOwner(NULL);
             m_animationTimer->stop();
             m_lastPos = newPos;
             return newPos;
         }

        if (!pitchRect.contains(newPos) && !(m_pitch->m_topGoal->contains(newPos)
                                             || m_pitch->m_bottomGoal->contains(newPos))) {
            qDebug() << "Ball::itemChange not in pitch";
            m_animationTimer->stop();
            return m_lastPos;
        } else {
            m_lastPos = newPos;
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
 }
