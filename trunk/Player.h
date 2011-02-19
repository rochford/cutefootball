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
void teamColorTransform(QPixmap &pixmap, QString pix, QRgb colorFrom, QRgb shirtColor, QRgb shortColor);

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
           bool computerControlled,
           Pitch* pitch,
           Team* team,
           Player::Role role);
    virtual ~Player();

    enum { Type = UserType + 2 };

    virtual int type() const
        { return Type;}

    virtual void advance(int phase);
    virtual void createPixmaps();
    virtual void createMoves();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    inline int speed() const { return m_speed; }
    bool ballCollisionCheck() const;
    bool playerCollisionCheck() const;

    void move(MWindow::Action action, QPointF destination= QPointF(0.0,0.0));
    void movePlayer(MWindow::Action action, QPointF destination= QPointF(0.0,0.0));
    Player* findAvailableTeamMate(QPointF myPos) const;
    void specialAction(MWindow::Action action);
    void setTackled();

    bool withinShootingDistance() const;

    inline void setHasBall(bool hasBall) { m_hasBall = hasBall; }
    inline bool hasBall() const { return m_hasBall; }
    Team* team() const { return m_team; }

private slots:
    void repeatKeyEvent();
    void allowedOffPitch(bool allowedOffPitch);

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
    QMap<MWindow::Action,QPointF> m_moveDistance;
    int m_step;
    QTimer *m_outOfAction;
    bool m_allowedOffPitch;
    int m_speed;
};

#endif // PLAYER_H
