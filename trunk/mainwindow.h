#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>
#include "ui_mainwindow.h"

class Pitch;
class SoundEffects;
class settingsFrame;
class aboutFrame;
class mainMenuFrame;
class TeamSelectionFrame;

class MWindow : public QMainWindow
{
    Q_OBJECT

public:
    MWindow(QWidget *parent = 0);
    virtual ~MWindow();

    Pitch* pitch() { return m_pitch; }

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

// TODO        YellowCard,
// TODO        RedCard,

        // goal keeper moves
// TODO       DiveLeft,
// TODO        DiveRight
    };

private:
    enum Frame {
        MainMenu,
        Settings,
        About,
        TeamSelection,
        GraphicsView
    };

    void createConnections();

public slots:
    void hideTeamSelectionFrame();
    void hideSettingsFrame();
    void hideAboutFrame();
    void showFrame(Frame f);
    void newGame(int homeTeam, int awayTeam);

private slots:

    void showAboutFrame();
    void buttonClickedNoise();
    void showSettingsFrame();
    void showTeamSelectionFrame();
    void enableActions(bool gameInProgress);


private:
    Pitch *m_pitch;
    settingsFrame* m_settingsFrame;
    aboutFrame* m_aboutFrame;
    TeamSelectionFrame* m_teamSelectionFrame;
    mainMenuFrame* m_mainMenuFrame;
    SoundEffects* m_soundEffects;

    Ui::MainWindow uiMainWindow;
    };

#endif // MAINWINDOW_H
