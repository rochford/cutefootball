#ifndef GAME_H
#define GAME_H

#include <QTimer>
#include <QtGlobal>
#include <QStateMachine>
#include <QFinalState>

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QList>
#include <QGraphicsItemAnimation>
#include <QDebug>

class QTimeLine;

class Pitch;
class Game;
class GoalScoredState;
class FoulState;

class Game : public QState
{
    Q_OBJECT
public:
    Game(Pitch *p, QString stateName, bool isFirstHalf, bool isExtraTime);
    ~Game();

    enum GameState {
        GoalScored, // reason for another kickoff
        TakePositions,
        HalfOver
    };
    // stops the gameClock
    void stopGameClock();
    void setGameLength(int totalGameInMinutes);
    inline int remainingTimeInHalfMs() { return m_remainingTimeInHalfMs; }

signals:
    void halfOver();

public slots:
    void playFrame(int frame);
    void startPlayersLeavePitchAnim();
    void kickOff();
    void decrementGameTime();

protected:
    void onEntry ( QEvent * event );
    void onExit ( QEvent * event );

private:
    void createPlayerAnimationItems(GameState g);

private:
    Pitch* m_pitch;
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
};

#endif // GAME_H

