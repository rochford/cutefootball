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
    inline void setNoBallOwner() {
        qDebug() << "setNoBallOwner";
        m_ballOwner = NULL;
    }
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
    QTimer* m_ballOwnerTimer;

    // ball position fixed until this is false
    bool m_positionLocked;

    Pitch *m_pitch; // NOT OWNED
    MWindow::Action previousAction;
    QPointF destination_;
    QPointF currentPosition_;
    Player *m_ballOwner; // NOT OWNED

    int step_;
    // last position of the ball
    QPointF m_lastPos;

    // ball passing animation
    QGraphicsItemAnimation *m_animation;
    QTimeLine *m_animationTimer;

    Player* m_lastPlayerToTouchBall; // can be null // NOT OWNED

    QMap<MWindow::Action,QPointF> m_moveDistance;

    MWindow::Action m_requiredNextAction;
    Player* m_requiredNextActionPlayer; // can be null // NOT OWNED
};

#endif // BALL_H
