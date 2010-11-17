#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>

class Pitch;

class MWindow : public QMainWindow
{
    Q_OBJECT

public:
    MWindow(QWidget *parent = 0);
    virtual ~MWindow();

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

protected:
    virtual void keyPressEvent( QKeyEvent *event );
    virtual void keyReleaseEvent( QKeyEvent *event );

private:
    void createKeyboardActions();
    void stopKeyEvent();
    void createActions();

private slots:
    void repeatKeyEvent();
    void about();

private:
    Pitch *m_pitch;

    QMap<int,Action> m_actions;
    QTimer *m_keyEventTimer;
    Action m_lastAction;

    QAction* m_newGameAction;
    QAction* m_settingsAction;
    QAction* m_aboutAction;

    QMenu* m_fileMenu;
    QMenu* m_helpMenu;

    QTime m_elapsedTime;
    };

#endif // MAINWINDOW_H
