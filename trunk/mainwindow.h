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

        // Due to loosing a tackle
        FallenOver,

#ifndef INDOOR
         ThrownIn,
#endif // INDOOR

#ifdef REPLAY_FEATURE
        // replay last n seconds
        Replay,
#endif // REPLAY_FEATURE
// TODO        YellowCard,
// TODO        RedCard,

        // goal keeper moves
// TODO       DiveLeft,
// TODO        DiveRight
    };

#ifdef REPLAY_FEATURE
protected:
    virtual void keyPressEvent( QKeyEvent *event );
#endif // REPLAY_FEATURE

private:
#ifdef REPLAY_FEATURE
    void createKeyboardActions();
#endif // REPLAY_FEATURE
    void createConnections();

private slots:
    void about();
    void buttonClickedNoise();
    void showSettingsDialog();
    void enableActions(bool gameInProgress);
#ifdef REPLAY_FEATURE
    void isPlaying(bool playing);
#endif // REPLAY_FEATURE

private:
    Pitch *m_pitch;
    settingsDialog* m_settingsDialog;
    SoundEffects* m_soundEffects;
#ifdef REPLAY_FEATURE
    QMap<int,Action> m_actions;
#endif // REPLAY_FEATURE
    Ui::Frame ui;
    QFrame* m_mainMenuFrame;

    Ui::MainWindow uiMainWindow;
    };

#endif // MAINWINDOW_H
