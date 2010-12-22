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
#ifdef REPLAY_FEATURE
class Replay;
#endif // REPLAY_FEATURE
class ScreenGraphics;
class Game;
class settingsDialog;

class Pitch : public QObject
{
    Q_OBJECT
public:
    enum SetPiece {
#ifndef INDOOR
        ThrowIn,
        GoalKick,
        Corner,
#endif // INDOOR
        KickOff,
        Penalty,
        Foul
    };

    Pitch(const QRectF& footballGroundRect,
          QGraphicsView* view,
          SoundEffects* se,
          QWidget* frame,
          settingsDialog* settingsDlg);
    ~Pitch();

    inline Ball* ball() const { return m_ball; }
    Player* selectNearestPlayer(Team* team);

    void setPlayerStartPositions(Team *team);
    void setPiece(Team* t, SetPiece s);

    inline Team* homeTeam() const { return m_homeTeam; }
    inline Team* awayTeam() const { return m_awayTeam; }
    void updateDisplayTime(int timeLeftMs);
    inline QPointF pitchEntrancePoint() const { return m_entrancePoint; }
#ifdef REPLAY_FEATURE
    inline Replay* replay() const { return m_replay; }
#endif // REPLAY_FEATURE
    void playGameSound(SoundEffects::GameSound s);

public slots:
    void newGame();

    void hasBallCheck();
    void selectNearestPlayer();
#ifdef REPLAY_FEATURE
    void replayStart();
    void replayStop();
#endif // REPLAY_FEATURE
    void gameStarted();
    void gameStopped();

private slots:
    void removePlayers();

signals:
   void gameInProgress(bool playing);

private:
    void createTeamPlayers(Team *team);
    void layoutPitch();
    void createTeams();

public:
    QList<Player*> m_players;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;  // NOT OWNED
    QGraphicsRectItem *m_footballPitch;

    QGraphicsRectItem *m_bottomGoal;
    QGraphicsRectItem *m_topGoal;
    QGraphicsRectItem *m_bottomPenaltyArea;
    QGraphicsRectItem *m_topPenaltyArea;
    QGraphicsLineItem *m_centerLine;
    QGraphicsEllipseItem *m_centerCircle;
    ScreenGraphics *m_scoreText;
    QRectF m_pitchArea[KRow][KColumn];
    SoundEffects* m_soundEffects;  // NOT OWNED
private:
    QList<Team*> m_teams;
#ifdef REPLAY_FEATURE
    Replay* m_replay;
#endif // REPLAY_FEATURE
    Team *m_homeTeam;
    Team *m_awayTeam;
    Ball *m_ball;
    QTimer *m_motionTimer;

    QStateMachine *m_game;
    Game *m_firstHalfState;
    Game *m_secondHalfState;
    Game *m_extraTimeFirstHalfState;
    Game *m_extraTimeSecondHalfState;
    QFinalState *m_allDone;
    QPointF m_entrancePoint;

    QWidget *m_menuFrame;
    settingsDialog *m_settingsDlg;
    QGraphicsProxyWidget *m_proxyMenuFrame;
    QGraphicsProxyWidget *m_proxySettingsDlg;
};

#endif // PITCH_H
