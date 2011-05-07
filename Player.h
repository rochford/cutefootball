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
 *    Lesser GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui>
#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <qmap.h>

#include "team.h"
#include "pitch.h"

class QFocusEvent;

typedef QList<QPixmap> PixmapList;

class Player : public QObject,
               public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    enum Role {GoalKeeper = 1,
               LeftDefence,
               LeftCentralDefence,
               CentralDefence,
               RightCentralDefence,
               RightDefence,
               LeftMidfield,
               CentralMidfield,
               RightMidfield,
               LeftAttack,
               CentralAttack,
               RightAttack,
               LastDummy };

    Player(QString name,
           int number,
           bool computerControlled,
           Pitch* pitch,
           Team* team,
           qreal speed,
           Player::Role role,
           QColor hairColor,
           QColor skinColor);
    virtual ~Player();

    enum { Type = UserType + 2 };

    virtual int type() const
        { return Type;}

    virtual void advance(int phase);
    virtual void createPixmaps();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

protected: // From QGraphicsItem
    void focusInEvent(QFocusEvent * event);
    void focusOutEvent(QFocusEvent * event);

public:
    bool ballCollisionCheck() const;
    bool playerCollisionCheck() const;

    void move(MWindow::Action action, QPointF destination= QPointF(0.0,0.0));
    void movePlayer(MWindow::Action action, QPointF destination= QPointF(0.0,0.0));
    Player* findAvailableTeamMate(QPointF myPos) const;
    void specialAction(MWindow::Action action);
    void setTackled();

    bool withinShootingDistance() const;

    inline qreal speed() const { return m_speed; }
    inline void setHasBall(bool hasBall) { m_hasBall = hasBall; }
    inline bool hasBall() const { return m_hasBall; }
    inline Team* team() const { return m_team; }

    inline void setAllowedOffPitch(bool isAllowed) { m_allowedOffPitch = isAllowed; }
    inline void setAllowedInCenterCircle(bool isAllowed) { m_allowedInCenterCircle = isAllowed; }

    inline QString name() const { return m_name; }

private slots:
    void repeatKeyEvent();
    void standupPlayer();
    void foulEventStart(Team* t, QPointF foulLocation);

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void pixmapInsert(MWindow::Action a,
                      QString s1,
                      QString s2,
                      QString s3,
                      QRgb shirtColor,
                      QRgb shortColor);

private:
    void humanAdvance(int phase);
    void computerAdvance(int phase);
    void computerAdvanceWithBall();
    void computerAdvanceWithoutBall();

    void automove();

    void stopKeyEvent();
    void createKeyboardActions();
    QPointF calculateDestination(MWindow::Action act);

public:
    bool m_hasBall;
    Role m_role;
    QRectF m_startPositionRectF;
    // player will defend this zone of pitch
    QRectF m_defendZone;

private:
    QString m_name;
    int m_number;

    QTime m_elapsedTime;
    QMap<int,MWindow::Action> m_actions;
    QTimer *m_keyEventTimer;
    // last position of the player
    QPointF m_lastPos;

protected:
    Team* m_team;
    // the previous action of this player
    MWindow::Action m_lastAction;
    QMap<MWindow::Action,PixmapList> m_images;
    Pitch *m_pitch;
    int m_step;
    QTimer *m_outOfAction;

    bool m_allowedOffPitch;
    bool m_allowedInCenterCircle;

    qreal m_speed;

    QColor m_hairColor;
    QColor m_skinColor;

    // Displaying player name/number:
    QString m_toolTipText;
    QPen m_toolTipPen;
    QPointF m_toolTipTextPos;
};

#endif // PLAYER_H
