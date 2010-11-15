#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>

class Pitch;
class Replay;

class MWindow : public QMainWindow
{
    Q_OBJECT

public:
    MWindow(QWidget *parent = 0);
    virtual ~MWindow();

    void createActions();

    enum Action {
        West = 0,
        NorthWest,
        NorthEast,
        SouthEast,
        SouthWest,
        East,
        North,
        South,

        Button,
        ButtonShortPress,
        ButtonLongPress,

        Shot,
        Pass,
        Tackle,

        TackleWest,
        TackleNorthWest,
        TackleNorthEast,
        TackleSouthEast,
        TackleSouthWest,
        TackleEast,
        TackleNorth,
        TackleSouth,

        ThrownIn,

        GoalCelebration,

        // replay last n seconds
        Replay,

        YellowCard,
        RedCard,

        // goal keeper moves
        DiveLeft,
        DiveRight,

        DoNothing };

private slots:
    void startedGame();
    void pausedGame();
    void continueGame();

    void repeatKeyEvent();
    void stopKeyEvent();

protected:
    virtual void keyPressEvent( QKeyEvent *event );
    virtual void keyReleaseEvent( QKeyEvent *event );

private:
    void createKeyboardActions();

private:
    Pitch *pitch;

    QMap<int,Action> actions;
    QTimer *keyEventTimer;
    Action lastAction;

    QAction* newGameAction;
    QAction* pauseGameAction;
    QAction* continueGameAction;

    QMenu* fileMenu;
    QMenu* helpMenu;

    bool buttonPressed_;
    QTime elapsedTime_;
    };

#endif // MAINWINDOW_H
