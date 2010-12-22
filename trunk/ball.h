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
    inline Player* controlledBy() { return controlledBy_; }
    inline void setControlledBy(Player* p) { controlledBy_ = p; }
    inline void setControlledByNobody() { controlledBy_ = NULL; }

signals:
    void goalScored(bool topGoal);
    void shot(QPointF destination);
    void soundEvent(SoundEffects::GameSound);

public slots:
    void updateBall(int frame);

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    Pitch *m_pitch; // NOT OWNED
    MWindow::Action previousAction;
    QPointF destination_;
    QPointF currentPosition_;
    Player *controlledBy_; // NOT OWNED

    // starting position of ball at kick off
    QPointF start_;
    int step_;
    // last position of the ball
    QPointF m_lastPos;

    // ball passing animation
    QGraphicsItemAnimation *animation_;
    QTimeLine *animationTimer_;
};

#endif // BALL_H
