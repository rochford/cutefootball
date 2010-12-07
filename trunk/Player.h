#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui>
#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <qmap.h>

#include "team.h"
#include "pitch.h"

typedef QList<QPixmap> PixmapList;

MWindow::Action calculateAction(QPointF source,
                                QPointF destination);
void teamColorTransform(QPixmap &pixmap, QString pix, QRgb colorFrom, QRgb colorTo);

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
    virtual void createMoves();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    inline int speed() const { return m_speed; }
    bool ballCollisionCheck();

    bool isManMarked() const;

    void move(MWindow::Action action);
    void movePlayer(MWindow::Action action);
    Player* findAvailableTeamMate(QPointF myPos) const;
    void specialAction(MWindow::Action action);
    void isTackled(bool defeated);

    bool withinShootingDistance() const;

public slots:
    void goalScored(bool isTopGoal);

private slots:
    void repeatKeyEvent();

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget);

    void pixmapInsert(MWindow::Action a,
                      QString s1,
                      QString s2,
                      QString s3,
                      QRgb teamColor);

private:
    void humanAdvance(int phase);
    void computerAdvance(int phase);
    void computerAdvanceWithBall();
    void computerAdvanceWithoutBall();

    void automove();

    void stopKeyEvent();
    void createKeyboardActions();

public:
    bool hasBall_;
    Team* team_;

    Role role_;

private:
    QString m_name;
    QString m_soundFile;
    // the previous action of this player
    MWindow::Action m_lastAction;
    QTime m_elapsedTime;
    QMap<int,MWindow::Action> m_actions;
    QTimer *m_keyEventTimer;

public:
    QRectF startPosition_;
    QRectF defencePosition_;
    QRectF attackPosition_;

protected:
    QMap<MWindow::Action,PixmapList> m_images;
    Pitch *m_pitch;
    QMap<MWindow::Action,QPointF> m_moveDistance;
    int m_step;
    QTimer *m_outOfAction;
    int m_speed;
};

#endif // PLAYER_H
