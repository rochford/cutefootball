#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>
#include "ui_mainMenu.h"
#include "ui_mainwindow.h"

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
    void createConnections();

private slots:
    void about();
    void isPlaying(bool playing);
    void buttonClickedNoise();
    void showSettingsDialog();
    void enableActions(bool gameInProgress);

private:
    Pitch *m_pitch;
    settingsDialog* m_settingsDialog;
    SoundEffects* m_soundEffects;

    QMap<int,Action> m_actions;

    Ui::Frame ui;
    QFrame* m_mainMenuFrame;

    Ui::MainWindow uiMainWindow;
    };

#endif // MAINWINDOW_H
