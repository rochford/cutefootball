#ifndef PITCH_H
#define PITCH_H

#include <QObject>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QList>
#include <QTimeLine>

#include "mainwindow.h"
#include "team.h" // Team::Direction

class Ball;
class Player;
class Team;
class Referee;

const int KGameRefreshRate = 1000 / 24; // ms
const int KGameLength = 80*1000; // 80 seconds
const int KColumn = 3; // Left, Centre, Right
// goalkepper-defence-midfield-attack,attack-midfield-defence-goalkepper
const int KRow = 8;

const int KPlayerDefaultSpeed = 2;

// scale factor for players & ball
const qreal KScaleFactor = 1.6;

class Pitch : public QObject
{
    Q_OBJECT;
public:
    enum SetPiece {
        GoalKick,
        Corner,
        Penalty,
        ThrowIn,
        Foul
    };

    enum Game {
        NotStarted,
        FirstHalfOver,
        SecondHalfOver,
        Finished,
        Paused, // whole system paused
        GoalScored // reason for another kickoff
    };
    Pitch(const QRectF& footballGroundRect);
    ~Pitch();
    void action(MWindow::Action act);
    Ball* getBall() const { return ball_; }
    void kickOff(Game state);

    Player* selectNearestPlayer(Team* team);

    void setPlayerStartPositions(Team *team);
    void setPlayerDefendPositions(Team *team);
    void setPlayerAttackPositions(Team *team);
    void setPiece(Team::Direction, SetPiece s);

    void replayStart();
    bool isReplay() const { return replayTimeLine_->state() == QTimeLine::Running; }


public slots:
    void newGame();
    void pausedGame();
    void continueGame();

    void hasBallCheck();
    void kickOff();
    void selectNearestPlayer();
    void updateDisplayTime();

    void decrementGameTime();

    void goalScored(bool isLeftGoal);

    void makeReplaySnapshot();
    void replayFrame(int frame);

signals:
   void focusedPlayerChanged();

private:
    void createTeamPlayers(Team *team);
    void removePlayers();

public:
    QList<Player*> players_;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsRectItem *footballPitch_;
    Team *homeTeam_;
    Team *awayTeam_;
private:
    Ball *ball_;
    Referee *referee_;
    QTimer *motionTimer_;
    // Issues timeout every second.
    // used to decrement the amount of game time left
    QTimer *gameTimer_;
    QTimer *replaySnapShotTimer_;
    Game nextGameState_;

    // amounts of MS left in this half
    int remainingTimeInHalfMs_;

    Player *lastNearestPlayer; // NOT OWNED
    int remainingGameTime_;

    QTimeLine* replayTimeLine_;
    qreal frameCounter_;
    QList<QGraphicsItemAnimation*> animationItems;
public:
    QGraphicsRectItem *bottomGoal;
    QGraphicsRectItem *topGoal;
    QGraphicsRectItem *bottomPenaltyArea;
    QGraphicsRectItem *topPenaltyArea;
    QGraphicsSimpleTextItem *scoreText_;
    QRectF pitchArea[KRow][KColumn];
};

#endif // PITCH_H
