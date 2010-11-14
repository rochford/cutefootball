#ifndef PITCH_H
#define PITCH_H

#include <QtGui>

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QList>

#include "mainwindow.h"
#include "team.h" // Team::Direction

#include <QStateMachine>

class QGraphicsEllipseItem;
class QGraphicsLineItem;
class QGraphicsView;
class QGraphicsScene;
class QTimeLine;

class Ball;
class Player;
class Team;
class Referee;
class Replay;
class ScreenGraphics;
class Game;

const int KGameRefreshRate = 1000 / 24; // ms
const int KGameLength = 80*1000; // 80 seconds
const int KColumn = 5; // Left, l-Centre, centre, r-centre, Right
// goalkepper-defence-midfield-attack,attack-midfield-defence-goalkepper
const int KRow = 8;

const int KPlayerDefaultSpeed = 2;

// scale factor for players & ball
const qreal KScaleFactor = 1.6;


class Pitch : public QObject
{
    Q_OBJECT
public:
    enum SetPiece {
        GoalKick,
        Corner,
        Penalty,
        ThrowIn,
        Foul
    };

    Pitch(const QRectF& footballGroundRect);
    ~Pitch();

    void action(MWindow::Action act);

    inline Ball* getBall() const { return ball_; }
    inline Referee* referree() const { return referee_; }

    Player* selectNearestPlayer(Team* team);

    void setPlayerStartPositions(Team *team);
    void setPlayerDefendPositions(Team *team);
    void setPlayerAttackPositions(Team *team);
    void setPiece(Team* t, SetPiece s);

    void replayStart();
    void replayStop();

    inline Team* homeTeam() { return homeTeam_; }
    inline Team* awayTeam() { return awayTeam_; }

public slots:
    void newGame();
    void pausedGame();
    void continueGame();

    void hasBallCheck();
    void selectNearestPlayer();
    void updateDisplayTime();
    void decrementGameTime();

signals:
   void focusedPlayerChanged();

private:
    void createTeamPlayers(Team *team);
    void removePlayers();
    void layoutPitch();

public:
    QList<Player*> players_;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsRectItem *footballPitch_;

    QGraphicsRectItem *bottomGoal;
    QGraphicsRectItem *topGoal;
    QGraphicsRectItem *bottomPenaltyArea;
    QGraphicsRectItem *topPenaltyArea;
    QGraphicsLineItem *centerLine_;
    QGraphicsEllipseItem *centerCircle_;
    ScreenGraphics *scoreText_;
    QRectF pitchArea[KRow][KColumn];

    Replay* replay_;
private:
    Team *homeTeam_;
    Team *awayTeam_;
    Ball *ball_;
    Referee *referee_;
    QTimer *motionTimer_;
    // Issues timeout every second.
    // used to decrement the amount of game time left
    QTimer *gameTimer_;

    // amounts of MS left in this half
    int remainingTimeInHalfMs_;

    Player *lastNearestPlayer_; // NOT OWNED
    int remainingGameTime_;

    QStateMachine *game;
    Game *firstHalfState;
    Game *secondHalfState;
    QFinalState *allDone;
};

#endif // PITCH_H
