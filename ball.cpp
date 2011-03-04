#include <QtGui>

#include "ball.h"
#include "team.h"
#include "soundeffects.h"
#include "Player.h"
#include <cassert> // assert

#include <QDebug>

Ball::Ball(Pitch* pitch)
    : QObject(),
    QGraphicsPixmapItem(QPixmap(QString(":/images/ball.png")),NULL),
    m_pitch(pitch),
    destination_(QPointF(0,0)),
    step_(0),
    m_animation(NULL),
    m_animationTimer(NULL),
    m_ballOwner(NULL),
    m_lastPlayerToTouchBall(NULL),
    m_positionLocked(false),
    m_requiredNextAction(MWindow::NoAction)
{
    m_ballOwnerTimer = new QTimer(this);
    m_ballOwnerTimer->setInterval(250);
    m_ballOwnerTimer->start();

    QSize pixmapSize = pixmap().size();
    pixmapSize.scale(QSizeF(20,20).toSize(), Qt::KeepAspectRatio);

    QBitmap bitmap = pixmap().createMaskFromColor(KCuteFootballMaskColor);
    pixmap().setMask(bitmap);

    setTransformOriginPoint(boundingRect().center());
//    QGraphicsPixmapItem::setPos();
    setZValue(Pitch::ZBall);

    m_animation = new QGraphicsItemAnimation(this);
    m_animationTimer = new QTimeLine(1000, this);
    m_animationTimer->setFrameRange(0, 40);

    m_animation->setItem(this);
    m_animation->setTimeLine(m_animationTimer);
    connect(m_animationTimer, SIGNAL(frameChanged(int)), this, SLOT(updateBall(int)));
    connect(m_ballOwnerTimer, SIGNAL(timeout()), this, SLOT(updateBallOwner()));
}

Ball::~Ball()
{
    m_moveDistance.clear();
    m_ballOwnerTimer->stop();
    delete m_ballOwnerTimer;
    delete m_animation;
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
                setBallOwner(p);
                break;
            }
            if ( m_ballOwner )
                 m_ballOwner->setHasBall(false);
            setBallOwner(p);
            p->setHasBall(true);
        }
    }
}

QRectF Ball::boundingRect() const
{
    return QRectF(-9, -9,
                  9, 9);
}

void Ball::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget)
{
    // Draw QGraphicsPixmapItem face
    painter->drawPixmap(boundingRect().toRect(), pixmap());
}

QPainterPath Ball::shape() const
{
    QPainterPath path;
    path.addRect(-10, -10,
                 10, 10);
    return path;
}

void Ball::moveBall(MWindow::Action action, int speed)
{
    if (m_positionLocked) {
        qDebug() << "moveBall positionLocked";
        return;
    }
    if (m_requiredNextAction != MWindow::NoAction &&
        action != m_requiredNextAction ) {
        qDebug() << "moveBall specialAction required -> return";
        return;
    }
    if (!m_ballOwner) {
        qDebug() << "moveBall no ball owner";
        assert(0);
        return;
    }

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
        moveBy(m_moveDistance[action].x(), m_moveDistance[action].y());
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
    QPointF tmp(pos());

    m_animationTimer->stop();

    qreal stepX = (destination.x() - tmp.x()) / 40.0;
    qreal stepY = (destination.y() - tmp.y()) / 40.0;
    QPointF diff(stepX,stepY);

    for (int i = 0; i <= 40; ++i) {
//        QPointF newPos = tmp + QPointF(stepX,stepY);
        m_animation->setPosAt(i / 40.0, tmp + diff);
        // Rotation in animations does not seem to work
        // animation_->setRotationAt(i / 40.0, i*90.0);
        tmp = tmp + diff;
    }
    m_animationTimer->start();

    if (action == MWindow::Shot )
        emit shot(team, destination);
    emit soundEvent(SoundEffects::BallKick);
}

void Ball::updateBall(int frame)
{
  //  qDebug() << "Ball::updateBall" << frame;
    // animation may no longer be running due to a goal
    if ( (m_animationTimer->state() == QTimeLine::Running) && !m_positionLocked ) {
        QPointF newPos = m_animation->posAt(frame/40.0);
        if (!m_pitch->m_footballPitch->contains(newPos))
            qDebug() << "Ball::updateBall XXX error" << frame;
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
             emit soundEvent(SoundEffects::Goal);
             emit soundEvent(SoundEffects::Whistle);
             setNoBallOwner();
             m_animationTimer->stop();
             m_lastPos = newPos;
             return newPos;
         }

        if (!pitchRect.contains(newPos) && !(m_pitch->m_topGoal->contains(newPos)
                                             || m_pitch->m_bottomGoal->contains(newPos))) {
            qreal dx = newPos.x() - m_lastPos.x();
            qreal dy = newPos.y() - m_lastPos.y();
            qDebug() << "Ball::itemChange not in pitch, dx" << dx << "," << dy;
            m_animationTimer->stop();
            emit soundEvent(SoundEffects::BallRebound);
            return QPointF(m_lastPos.x() - dx*1.5,m_lastPos.y()-dy*1.5);
        } else {
            m_lastPos = newPos;
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
 }

void Ball::setBallOwner(Player* p)
{
    if (p) {
        m_ballOwner = p;
        m_lastPlayerToTouchBall = p;

        const qreal speed = p->speed();
        m_moveDistance.clear();
        m_moveDistance.insert(MWindow::North, QPointF(0,-speed));
        m_moveDistance.insert(MWindow::NorthEast, QPointF(speed,-speed));
        m_moveDistance.insert(MWindow::East, QPointF(speed,0));
        m_moveDistance.insert(MWindow::SouthEast, QPointF(speed,speed));
        m_moveDistance.insert(MWindow::South, QPointF(0,speed));
        m_moveDistance.insert(MWindow::SouthWest, QPointF(-speed,speed));
        m_moveDistance.insert(MWindow::West, QPointF(-speed,0));
        m_moveDistance.insert(MWindow::NorthWest, QPointF(-speed,-speed));
    }
}

void Ball::setnextActionMustBe(MWindow::Action a, Team* t, Player* p)
{
    qDebug() << "Ball::setnextActionMustBe";

}
