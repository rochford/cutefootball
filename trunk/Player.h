#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui>
#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include "team.h"
#include "pitch.h"

MWindow::Action calculateAction(QPointF source,
                                QPointF destination);

class Player : public QObject,
               public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    enum Role {GoalKeeper = 1,
               LeftDefence,
               LeftCentralDefence,
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
           bool computerControlled,
           Pitch* pitch,
           Team* team,
           Player::Role role);
    enum { Type = UserType + 2 };

    virtual int type() const
        { return Type;}

    virtual void advance(int phase);
    virtual void createPixmaps();

    int speed() { return speed_; }
    bool ballCollisionCheck();

    bool isManMarked() const;

    void move(MWindow::Action action);

    void setHumanControlled(bool human)
    {
        humanControlled_ = human;
    }
    bool humanControlled() { return humanControlled_; }
    void movePlayer(MWindow::Action action);
    Player* findAvailableTeamMate() const;
    void specialAction(MWindow::Action action);
    void isTackled(bool defeated);

    bool withinShootingDistance() const;

public slots:
    void goalScored(bool isTopGoal);

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget);

private:
    void humanAdvance(int phase);
    void computerAdvance(int phase);
    void computerAdvanceWithBall();
    void computerAdvanceWithoutBall();

    void automove();

    void createMoves();
    void pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3);

public:
    bool hasBall_;
    Team* team_;

    Role role_;
private:
    QString name_;
    bool humanControlled_;
    bool computerControlled;
    int speed_;
    // the previous action of this player
    MWindow::Action lastAction_;

public:
    QRectF startPosition_;
    QRectF defencePosition_;
    QRectF attackPosition_;
protected:
    QMap<MWindow::Action,QStringList> images_;
    Pitch *pitch_;
    QMap<MWindow::Action,QPointF> moveDistance_;
    int step_;
    QTimer *outOfAction_;
};

#endif // PLAYER_H
