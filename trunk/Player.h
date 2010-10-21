#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui>
#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>


#include "team.h"
#include "pitch.h"

class Player : public QObject,
               public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    enum Role {GoalKeeper = 1,
               LeftDefence,
               RightDefence,
               LeftMidfield,
               CentralMidfield,
               RightMidfield,
               LeftAttack,
               CentralAttack,
               RightAttack,
               LastDummy };

    Player(bool computerControlled,
           Pitch* pitch,
           Team* team,
           Player::Role role);
    enum { Type = UserType + 2 };
    int type() const
        { return Type;}

    void advance(int phase);
    int speed() { return speed_; }
    bool ballCollisionCheck();

    bool isManMarked() const;

    void move(MWindow::Action action);

    void setHumanControlled(bool human)
    {
        humanControlled_ = human;
        buttonDown_ = false;
    }
    bool humanControlled() { return humanControlled_; }
    void movePlayer(MWindow::Action action);
    Player* findAvailableTeamMate() const;
    void specialAction(MWindow::Action action);
    void isTackled(bool defeated);

    bool withinShootingDistance() const;

public slots:
    void goalScored(bool isLeftGoal);

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    void humanAdvance(int phase);
    void computerAdvance(int phase);
    void computerAdvanceWithBall();
    void computerAdvanceWithoutBall();

    void automove();
    bool canPass() const;

    void createMoves();
    void createPixmaps();

public:
    bool hasBall_;
    Team* team_;

    Role role_;
private:
    Pitch *pitch_;

    bool humanControlled_;
    bool computerControlled;
    QColor color;
    int speed_;
    // the previous action of this player
    MWindow::Action lastAction_;

    int step;

    bool buttonDown_;
    QMap<MWindow::Action,QStringList> images_;
    QMap<MWindow::Action,QPointF> moveDistance_;
    QTimer *outOfAction_;

public:
    QRectF startPosition_;
    QRectF defencePosition_;
    QRectF attackPosition_;

};

#endif // PLAYER_H
