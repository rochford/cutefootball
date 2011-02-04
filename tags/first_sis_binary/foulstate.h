#ifndef FOULSTATE_H
#define FOULSTATE_H

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

class FoulState : public QState
{
    Q_OBJECT
public:
    FoulState(Game *g, Pitch *p);
    ~FoulState() {}

    enum GameState {
        TakePositions
    };
public slots:
    void playFrame(int frame);

protected:
    void onEntry (QEvent * event );

private:
    void createPlayerAnimationItems(GameState g);

private:
    QState *m_takePositions;
    QFinalState *m_allDone;
    Game *m_game;
    Pitch* m_pitch;
    QTimeLine *m_timeLineTakePositions;
    QList<QGraphicsItemAnimation*> m_playerAnimationItems;
};

#endif // FOULSTATE_H
