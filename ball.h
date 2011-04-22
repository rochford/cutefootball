/*
 * Copyright 2010,2011 Timothy Rochford
 *
 *    This file is part of CuteFootball.
 *
 *    CuteFootball is free software: you can redistribute it and/or modify
 *    it under the terms of the Lesser GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    CuteFootball is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Lesser  GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


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

    void setPos(const QPointF pos) {
        QGraphicsPixmapItem::setPos(pos);
        m_positionLocked = false;
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    }
    void moveBall(MWindow::Action action, int speed);
    enum { Type = UserType + 1 };
    int type() const { return Type; }

    void kickBall(MWindow::Action action, QPointF destination);
    inline Player* ballOwner() const { return m_ballOwner; }
    void setBallOwner(Player* p);
    inline Player* lastPlayerToTouchBall() const { return m_lastPlayerToTouchBall; }
    inline void setNoBallOwner() { m_ballOwner = NULL; }

    // the next actions on the ball must be "a", performed by player "p".
    // "p" may be null, Team must be a valid value
    void setRequiredNextAction(MWindow::Action a, Team* t, Player* p);

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
    // present speed of ball
    qreal m_velocity;
    // angle of direction of travel
    qreal m_angle;
    // last position of the ball
    QPointF m_lastPos;

    // ball position fixed until this is false
    bool m_positionLocked;

    Pitch *m_pitch; // NOT OWNED
    QPointF m_destination;
    Player *m_ballOwner; // NOT OWNED
    QTimer* m_ballOwnerTimer;

    int step_;

    // ball passing animation
    QGraphicsItemAnimation *m_animation;
    QTimeLine *m_animationTimeLine;

    Player* m_lastPlayerToTouchBall; // can be null // NOT OWNED

    MWindow::Action m_requiredNextAction;
    Player* m_requiredNextActionPlayer; // can be null // NOT OWNED
};

#endif // BALL_H
