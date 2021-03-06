/*
 * Copyright 2010,2011 Timothy Rochford
 *
 *    This file is part of CuteFootball.
 *
 *    CuteFootball is free software: you can redistribute it and/or modify
 *    it under the terms of the Lesser GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    CuteFootball is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Lesser GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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
#include "screengraphics.h"
#include <QStateMachine>

class QGraphicsEllipseItem;
class QGraphicsLineItem;
class QGraphicsView;
class QGraphicsScene;
class QTimeLine;
class QStatusBar;

class Ball;
class Player;
class Team;
class TeamManager;
class ScreenGraphics;
class OnScreenButtonsFrame;
class Game;
class GameHalf;
class PitchScene;

class Pitch : public QObject
{
    Q_OBJECT
public:
    enum State{
        Pause,
        ReadyForNextHalf
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
        ZOnScreenBtns,
        ZMenus,
        ZMainMenu
    };

    Pitch(const QRectF& footballGroundRect,
          QGraphicsView* view,
          SoundEffects* se,
          settingsFrame* settingsDlg,
          QStatusBar* bar);
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
    void centerOn(QGraphicsItem* item);
    PitchScene* scene() const { return m_scene; }
    QRectF footballPitch() const
        { return m_footballPitch; }
    void setGraphics(ScreenGraphics::ScreenGraphicsType s)
        { m_statusBarInfo->setGraphics(s);}
    QPointF centerMark() const { return m_centerMark; }

public slots:
    void centerOn();
    void pause();
    void continueGame();
    void newGame(int homeTeam, int awayTeam);
    void selectNearestPlayer();
    // causes the game to be stopped
    void gameStop();
    void countShots(Team* team, QPointF dest);

private slots:
    void hasBallCheck();
    void gameStarted();
    // called when game FSM has been stopped
    void gameStopped();

signals:
    void continueGameClock();
    void pauseGameClock();
    void gameInProgress(bool playing);
    void foul(Team* originatingTeam, QPointF foulLocation);
    void kickOff(Team* teamToKickOff);

private:
    void createTeamPlayers(Team *team);
    void layoutPitch();
    void setPlayerDefendZone(Player *p);

    inline bool penalties() const {  return m_isPenalties; }

public:
    QList<Player*> m_players;
    QRectF *m_bottomGoal;
    QRectF *m_topGoal;
    QPainterPath m_bottomPenaltyArea;
    QPainterPath m_topPenaltyArea;
    QPainterPath m_centerCircle;

private:
    PitchScene *m_scene;
    QRectF m_footballPitch;

    QPointF m_centerMark;

    ScreenGraphics *m_statusBarInfo;
    QRectF m_pitchArea[KRow][KColumn];
    SoundEffects* m_soundEffects;  // NOT OWNED

    QGraphicsView *m_view;  // NOT OWNED
    TeamManager* m_teamMgr;

    Team *m_homeTeam;
    Team *m_awayTeam;
    Ball *m_ball;
    QTimer *m_motionTimer;

    State m_state;
    QStateMachine *m_gameFSM;
    // top level state, contains gameHalfs
    Game* m_game;
    bool m_isPenalties;

    QPointF m_entrancePoint;

    settingsFrame *m_settingsFrame;

    QStatusBar* m_bar;
    // item to center the graphics view on
    QGraphicsItem* m_centerOn;
};

#endif // PITCH_H
