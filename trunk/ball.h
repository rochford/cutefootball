#ifndef BALL_H
#define BALL_H

#include <QtGui>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include <QGraphicsPixmapItem>
#include <QGraphicsItemAnimation>
#include <QTimeLine>

#include "mainwindow.h"
#include "pitch.h"
#include "soundeffects.h"

class Player;

class Ball : public QObject,
             public QGraphicsPixmapItem
{
    Q_OBJECT;
public:
    Ball(Pitch* pitch);
    ~Ball();

    void setStartingPosition() {
        setPos(start_.x(),start_.y());
        m_positionLocked = false;
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    }
    void advance(int phase);
    void moveBall(MWindow::Action action, int speed);
    enum { Type = UserType + 1 };
    int type() const { return Type; }

    void kickBall(MWindow::Action action, QPointF destination);
    inline Player* ballOwner() const { return m_ballOwner; }
    inline void setBallOwner(Player* p) {
        m_ballOwner = p;
        m_lastPlayerToTouchBall = p;
    }
    Player* lastPlayerToTouchBall() const { return m_lastPlayerToTouchBall; }
    inline void setNoBallOwner() {
        qDebug() << "setNoBallOwner";
        m_ballOwner = NULL;
    }

signals:
    void goalScored(bool topGoal);
    void shot(Team* teamShooting, QPointF destination);
    void soundEvent(SoundEffects::GameSound);

public slots:
    void updateBall(int frame);
    void updateBallOwner();

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QTimer* m_ballOwnerTimer;

    // ball position fixed until this is false
    bool m_positionLocked;

    Pitch *m_pitch; // NOT OWNED
    MWindow::Action previousAction;
    QPointF destination_;
    QPointF currentPosition_;
    Player *m_ballOwner; // NOT OWNED

    // starting position of ball at kick off
    QPointF start_;
    int step_;
    // last position of the ball
    QPointF m_lastPos;

    // ball passing animation
    QGraphicsItemAnimation *m_animation;
    QTimeLine *m_animationTimer;

    Player* m_lastPlayerToTouchBall; // can be null // NOT OWNED
};

#endif // BALL_H
