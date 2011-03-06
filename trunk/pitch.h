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
#include "settingsFrame.h"
#include <QStateMachine>

class QGraphicsEllipseItem;
class QGraphicsLineItem;
class QGraphicsView;
class QGraphicsScene;
class QTimeLine;

class Ball;
class Player;
class Team;
class TeamManager;
class ScreenGraphics;
class Game;

const QString KFirstHalf(QObject::tr("First half"));
const QString KSecondHalf(QObject::tr("Second half"));
const QString KFirstHalfET(QObject::tr("Extra Time First half"));
const QString KSecondHalfET(QObject::tr("Extra Time Second half"));
const QString KPenaltyShootOut(QObject::tr("penalty shoot out"));

class Pitch : public QObject
{
    Q_OBJECT
public:
    enum State {
        Pause,
        Continue
    };

    enum SetPiece {
        KickOff,
        Penalty,
        Foul
    };

    enum ZValues {
        ZStadium = 1,
        ZPitchGrass,
        ZPitchMarkings,
        ZComputerControlledPlayer,
        ZHumanControlledPlayer,
        ZFocusedPlayer,
        ZBall,
        ZGoalObject,
        ZScoreText,
        ZMenus,
        ZMainMenu
    };

    Pitch(const QRectF& footballGroundRect,
          QGraphicsView* view,
          SoundEffects* se,
          settingsFrame* settingsDlg);
    ~Pitch();

    inline Ball* ball() const { return m_ball; }
    Player* selectNearestPlayer(Team* team);

    void setPlayerStartPositions(Team *team);
    void setPiece(Team* t, SetPiece s, QPointF foulLocation = QPointF(-1.0,-1.0));

    QList<Team*> teams() { return m_teamMgr->teams(); }
    inline Team* homeTeam() const { return m_homeTeam; }
    inline Team* awayTeam() const { return m_awayTeam; }

    void updateDisplayTime(int timeLeftMs);
    inline QPointF pitchEntrancePoint() const { return m_entrancePoint; }
    inline bool extraTimeAllowed() const { return m_settingsFrame->extraTimeAllowed(); }
    inline void setPenaltyShootOut(bool penalties) { m_isPenalties = penalties; }
    inline bool extraTime() const {
        return (homeTeam()->m_goals == awayTeam()->m_goals); }
    inline void centerOnBall(bool centerOnBall) { m_centerOnBall = centerOnBall; }

public slots:
    void newGame(int homeTeam, int awayTeam);
    void setState(Pitch::State s) { m_state = s; emit stateChanged(m_state);}

    void hasBallCheck();
    void selectNearestPlayer();
    void gameStarted();
    void gameStopped();

    void showHalfStatisticsFrame();
    void hideHalfStatisticsFrame();

    inline void centerOnBall() { m_centerOnBall = true; }
    void countShots(Team* team, QPointF dest);

signals:
   void gameInProgress(bool playing);
   void foul(Team* originatingTeam, QPointF foulLocation);
   void displayHalfTimeStatistics(bool);
   void stateChanged(State);

private:
    void createTeamPlayers(Team *team);
    void layoutPitch();
    void setPlayerDefendZone(Player *p);
    QStringList parsePlayers(QString teamName);

    inline bool penalties() const {  return m_isPenalties; }

public:
    QList<Player*> m_players;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;  // NOT OWNED
    QGraphicsRectItem *m_footballPitch;

    QGraphicsPixmapItem* m_grass;
    QGraphicsRectItem *m_bottomGoal;
    QGraphicsRectItem *m_topGoal;
    QGraphicsPathItem *m_bottomPenaltyArea;
    QGraphicsPathItem *m_topPenaltyArea;
    QGraphicsLineItem *m_centerLine;
    QGraphicsEllipseItem *m_centerCircle;
    QGraphicsEllipseItem *m_centerMark;
    ScreenGraphics *m_scoreText;
    QRectF m_pitchArea[KRow][KColumn];
    SoundEffects* m_soundEffects;  // NOT OWNED
    QGraphicsProxyWidget *m_screenGraphicsFrameProxy;

private:
    TeamManager* m_teamMgr;

    Team *m_homeTeam;
    Team *m_awayTeam;
    Ball *m_ball;
    QTimer *m_motionTimer;

    State m_state;
    QStateMachine *m_game;
    Game *m_firstHalfState;
    Game *m_secondHalfState;
    QFinalState *m_allDone;
    Game *m_extraTimeFirstHalfState;
    Game *m_extraTimeSecondHalfState;
    Game *m_penaltiesState;
    bool m_isPenalties;

    QPointF m_entrancePoint;

    settingsFrame *m_settingsFrame;

    ScreenGraphics* m_screenGraphicsLabel;

    // true if the ball should in centered on
    bool m_centerOnBall;
};

#endif // PITCH_H
