#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>
#include "ui_mainMenu.h"

class Pitch;
class SoundEffects;
class settingsDialog;

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

private:
    void createKeyboardActions();
    void createActions();
    void createConnections();

private slots:
    void about();
    void isPlaying(bool playing);
    void buttonClickedNoise();
    void showSettingsDialog();
    void enableActions(bool gameInProgress);

private:
    Pitch *m_pitch;

    QMap<int,Action> m_actions;

    Ui::Frame ui;

    QFrame* m_mainMenuFrame;

    QAction* m_newGameAction;
    QAction* m_quitAction;
    QAction* m_replayAction;
    QAction* m_settingsAction;
    QAction* m_aboutAction;

    settingsDialog* m_settingsDialog;

    QMenu* m_fileMenu;
    QMenu* m_gameMenu;
    QMenu* m_helpMenu;

    SoundEffects* m_soundEffects;
    };

#endif // MAINWINDOW_H
