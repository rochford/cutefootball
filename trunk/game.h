#ifndef GAME_H
#define GAME_H

#include <QTimer>
#include <QtGlobal>
#include <QStateMachine>

// #include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QList>
#include <QGraphicsItemAnimation>
#include <QDebug>

class QTimeLine;
class QFinalState;
class QHistoryState;

class Pitch;
class GameHalf;
class GoalScoredState;
class FoulState;
class Team;

class Game : public QState
{
    Q_OBJECT
public:
    Game(QStateMachine& fsm, Pitch& p);
    ~Game();
    void setHalfLength(int totalGameInMinutes);
    inline GameHalf* firstHalfState() const
        { return m_firstHalfState;}
    inline GameHalf* extraTimeFirstHalfState() const
        { return m_extraTimeFirstHalfState;}
    inline GameHalf* penaltiesState() const
        { return m_penaltiesState;}
    inline GameHalf* currentState() const
        { return m_currentState; }
    void setCurrentState(GameHalf* half)
        { m_currentState = half; }

private:
    QStateMachine& m_fsm;
    Pitch& m_pitch;

    // pointer to the current state
    GameHalf* m_currentState; // NOT OWNED
    // paused state
    QState *m_pausedState;
    // The m_game state prior to the pause
    QHistoryState* m_prePausedState;

    GameHalf *m_firstHalfState;
    GameHalf *m_secondHalfState;
    GameHalf *m_extraTimeFirstHalfState;
    GameHalf *m_extraTimeSecondHalfState;
    GameHalf *m_penaltiesState;
    QFinalState *m_allDone;
};

class GameHalf : public QState
{
    Q_OBJECT
public:
    GameHalf(Game* parent, Pitch& p, QString stateName, bool isFirstHalf, bool isExtraTime);
    ~GameHalf();

    enum GameState {
        GoalScored, // reason for another kickoff
        TakePositions,
        HalfOver
    };
    inline int remainingTimeInHalfMs() const
        { return m_remainingTimeInHalfMs; }
    inline void setGameLength(int totalGameInMinutes)
        { m_remainingTimeInHalfMs = (totalGameInMinutes * 60 *1000)/2.0; }
signals:
    void halfOver(QString halfName);

public slots:
    void playFrame(int frame);
    void startPlayersLeavePitchAnim(QString halfName);
    void kickOff();
    void decrementGameTime();
    void foulCaused(Team* orig, QPointF location);

    // stops the gameClock
    void pauseGameClock();
    void continueGameClock();

protected:
    void onEntry ( QEvent * event );
    void onExit ( QEvent * event );

private:
    void createPlayerAnimationItems(GameState g);

private:
    // the parent state
    Game* m_game;

    Pitch& m_pitch;
    bool m_isFirstHalf;
    bool m_isExtraTime;

    QTimer *m_1second;
    // amounts of MS left in this half
    int m_remainingTimeInHalfMs;

    QState *m_startState;
    QState *m_playingState;
    QState *m_halfEndState;
    GoalScoredState *m_goalScoredState;
    FoulState* m_foulState;
    QFinalState *m_allDoneState;

    QTimeLine *m_timeLineTakePositions;
    QTimeLine *m_timeLineLeavePitch;
    QList<QGraphicsItemAnimation*> m_playerAnimationItems;

public:
    Team* m_foulingTeam;
    QPointF m_foulingLocation;
};

#endif // GAME_H

