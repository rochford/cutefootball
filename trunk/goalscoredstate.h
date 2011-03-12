#ifndef GOALSCOREDSTATE_H
#define GOALSCOREDSTATE_H

#include <QList>
#include <QtGlobal>
#include <QStateMachine>
#include <QFinalState>
#include <QGraphicsItemAnimation>

class QTimeLine;

class Game;
class Pitch;

class GoalScoredState : public QState
{
    Q_OBJECT
public:
    GoalScoredState(Game *g, Pitch *p);
    ~GoalScoredState() {}

    enum GameState {
        TakePositions,
        Celebrate,
    };
public slots:
    void playFrameCelebrate(int frame);
    void playFramePositions(int frame);
    void createTakePositionAnimation();

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

#endif // GOALSCOREDSTATE_H
