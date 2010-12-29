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

    inline void setStartingPosition() { setPos(start_.x(),start_.y()); }
    void advance(int phase);
    void moveBall(MWindow::Action action, int speed);
    enum { Type = UserType + 1 };
    int type() const { return Type; }

    void kickBall(MWindow::Action action, QPointF destination);
    inline Player* ballOwner() { return m_ballOwner; }
    inline void setBallOwner(Player* p) {
        m_ballOwner = p;
        m_lastPlayerToTouchBall = p;
    }
    Player* lastPlayerToTouchBall() { return m_lastPlayerToTouchBall; }
    inline void setNoBallOwner() { m_ballOwner = NULL; }

signals:
    void goalScored(bool topGoal);
    void shot(QPointF destination);
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
    QGraphicsItemAnimation *animation_;
    QTimeLine *animationTimer_;

    Player* m_lastPlayerToTouchBall; // can be null // NOT OWNED
};

#endif // BALL_H
