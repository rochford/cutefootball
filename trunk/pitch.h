#ifndef PITCH_H
#define PITCH_H

#include <QtGui>

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QList>

#include "compileTimeSettings.h"
#include "mainwindow.h"
#include "team.h" // Team::Direction
#include "soundeffects.h"

#include <QStateMachine>

class QGraphicsEllipseItem;
class QGraphicsLineItem;
class QGraphicsView;
class QGraphicsScene;
class QTimeLine;

class Ball;
class Player;
class Team;
#ifdef REFEREE_USED
class Referee;
#endif //
class Replay;
class ScreenGraphics;
class Game;
class settingsDialog;

class Pitch : public QObject
{
    Q_OBJECT
public:
    enum SetPiece {
        KickOff,
        GoalKick,
        Corner,
        Penalty,
        ThrowIn,
        Foul
    };

    Pitch(const QRectF& footballGroundRect,
          QWidget* frame,
          settingsDialog* settingsDlg);
    ~Pitch();

    inline Ball* ball() const { return m_ball; }
#ifdef REFEREE_USED
    inline Referee* referee() const { return m_referee; }
#endif //
    Player* selectNearestPlayer(Team* team);

    void setPlayerStartPositions(Team *team);
    void setPlayerDefendPositions(Team *team);
    void setPlayerAttackPositions(Team *team);
    void setPiece(Team* t, SetPiece s);

    inline Team* homeTeam() const { return m_homeTeam; }
    inline Team* awayTeam() const { return m_awayTeam; }
    void updateDisplayTime(int timeLeftMs);
    inline QPointF pitchEntrancePoint() const { return m_entrancePoint; }
    inline Replay* replay() const { return m_replay; }
    void playGameSound(SoundEffects::GameSound s);

public slots:
    void newGame();

    void hasBallCheck();
    void selectNearestPlayer();

    void replayStart();
    void replayStop();

    void gameStarted();
    void gameStopped();

private slots:
    void removePlayers();

signals:
   void gameInProgress(bool playing);

private:
    void createTeamPlayers(Team *team);
    void layoutPitch();

public:
    QList<Player*> m_players;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QGraphicsRectItem *m_footballPitch;

    QGraphicsRectItem *m_bottomGoal;
    QGraphicsRectItem *m_topGoal;
    QGraphicsRectItem *m_bottomPenaltyArea;
    QGraphicsRectItem *m_topPenaltyArea;
    QGraphicsLineItem *m_centerLine;
    QGraphicsEllipseItem *m_centerCircle;
    ScreenGraphics *m_scoreText;
    QRectF m_pitchArea[KRow][KColumn];
    SoundEffects* m_soundEffects;
private:
    Replay* m_replay;

    Team *m_homeTeam;
    Team *m_awayTeam;
    Ball *m_ball;
#ifdef REFEREE_USED
    Referee *m_referee;
#endif //
    QTimer *m_motionTimer;

    QStateMachine *m_game;
    Game *m_firstHalfState;
    Game *m_secondHalfState;
    QFinalState *m_allDone;
    QPointF m_entrancePoint;

    QWidget *m_menuFrame;
    settingsDialog *m_settingsDlg;
    QGraphicsProxyWidget *m_proxyMenuFrame;
    QGraphicsProxyWidget *m_proxySettingsDlg;
};

#endif // PITCH_H
