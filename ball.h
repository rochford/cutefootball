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

class Player;

class Ball : public QObject,
             public QGraphicsPixmapItem
{
    Q_OBJECT;
public:
    Ball(Pitch* pitch);
    ~Ball();

    void setStartingPosition() { setPos(start_.x(),start_.y()); }
    void advance(int phase);
    void moveBall(MWindow::Action action, int speed);
    enum { Type = UserType + 1 };
    int type() const { return Type; }

    void moveBall(MWindow::Action action, QPointF destination);
    Player* controlledBy() { return controlledBy_; }
    void setControlledBy(Player* p) { controlledBy_ = p; }

signals:
    void goalScored(bool leftGoal);

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
    Pitch *pitch_; // NOT OWNED
    MWindow::Action previousAction;
    QPointF destination_;
    QPointF currentPosition_;
    Player *controlledBy_; // NOT OWNED

    // starting position of ball at kick off
    QPointF start_;
    int step_;

    // ball passing animation
    QGraphicsItemAnimation *animation_;
    QTimeLine *animationTimer_;
};

#endif // BALL_H
