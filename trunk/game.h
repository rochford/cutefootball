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

class GoalScoredState : public QState
{
    Q_OBJECT
public:
    GoalScoredState(Game *g, Pitch *p);
    ~GoalScoredState() {};

    enum GameState {
        TakePositions,
        Celebrate,
    };
public slots:
    void playFrame(int frame);

protected:
    void onEntry ( QEvent * event );

private:
    void createPlayerAnimationItems(GameState g);

private:
    QState *m_celebrate;
    QState *m_returnToPosition;
    QFinalState *m_allDone;
    Game *m_game;
    Pitch* m_pitch;
    QTimeLine *m_timeLineCelebrate;
    QTimeLine *m_timeLineReturnStartPositions;
    QList<QGraphicsItemAnimation*> m_playerAnimationItems;
};

class Game : public QState
{
    Q_OBJECT
public:
    Game(Pitch *p, QString stateName, bool isFirstHalf);
    ~Game() { qDebug() << stateName_ <<"dtor"; m_timer->stop(); }

    enum GameState {
        GoalScored, // reason for another kickoff
        TakePositions,
        HalfOver
    };

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
    QString stateName_;

    QTimer *m_timer;
    QTimer *m_1second;
    // amounts of MS left in this half
    int m_remainingTimeInHalfMs;

    QState *start;
    QState *playing;
    QState *end;
    GoalScoredState *m_goalScoredState;
    QFinalState *m_allDone;

    QTimeLine *m_timeLineTakePositions;
    QTimeLine *m_timeLineLeavePitch;
    QList<QGraphicsItemAnimation*> m_playerAnimationItems;
};

#endif // GAME_H

