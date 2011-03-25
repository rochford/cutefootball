#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>
#include "ui_mainwindow.h"

class Pitch;
class SoundEffects;
class settingsFrame;
class helpFrame;
class inputSettingsFrame;
class mainMenuFrame;
class TeamSelectionFrame;
class HalfStatisticsFrame;
class InGameMenuFrame;

class MWindow : public QMainWindow
{
    Q_OBJECT

public:
    MWindow(QWidget *parent = 0);
    virtual ~MWindow();

    Pitch* pitch() { return m_pitch; }

    enum Action {
        East = 0,
        SouthEast = -45,
        South = -90,
        SouthWest = -135,
        West = -180,
        NorthWest = -225,
        North = -270,
        NorthEast = -315,

        Button,
        ButtonShortPress,
        ButtonLongPress,

        Shot,
        Pass,
        Tackle,

        TackleN,
        TackleNE,
        TackleE,
        TackleSE,
        TackleS,
        TackleSW,
        TackleW,
        TackleNW,

        // Due to loosing a tackle
        FallenOver,

// TODO        YellowCard,
// TODO        RedCard,

        // goal keeper moves
      DiveEast = 1,
      DiveWest = -179,

      Pause,
      NoAction
    };

    enum Frame {
        MainMenu,
        Settings,
        Help,
        InputSettings,
        TeamSelection,
        HalfTimeStatistics,
        GraphicsView,
        InGameMenu
    };

private:
    void createConnections();
    void removeContextMenus();

signals:
    void setFrame(MWindow::Frame f);

public slots:
    void showFrame(MWindow::Frame f);
    void newGame(int homeTeam, int awayTeam);

    void hideInputSettingsFrame(){ emit setFrame(MWindow::MainMenu);}
    void hideHelpFrame(){emit setFrame(MWindow::MainMenu);}
    void hideSettingsFrame(){ emit setFrame(MWindow::MainMenu);}
    void showSettingsFrame(){ emit setFrame(MWindow::Settings);}
    void showMainMenuFrame(){ emit setFrame(MWindow::MainMenu);}
    void hideTeamSelectionFrame(){ emit setFrame(MWindow::MainMenu);}
    void showTeamSelectionFrame(){ emit setFrame(MWindow::TeamSelection);}
    void showInputSettingsFrame() { emit setFrame(MWindow::InputSettings);}
    void showHelpFrame(){emit setFrame(MWindow::Help);}
    void showStatisticsFrame(){emit setFrame(MWindow::HalfTimeStatistics);}
    void hideStatisticsFrame();
    void showInGameMenu() { emit setFrame(MWindow::InGameMenu); }
    void hideInGameMenu();
    void displayHalfTimeStatistics(bool display);
    void showGraphicsViewFrame() { emit setFrame(MWindow::GraphicsView); }

protected:
    void resizeEvent(QResizeEvent *e);

private slots:
    void enableActions(bool gameInProgress);

private:
    Pitch *m_pitch;
    settingsFrame* m_settingsFrame;
    inputSettingsFrame* m_inputSettingsFrame;
    HalfStatisticsFrame* m_halfStatisticsFrame;
    helpFrame* m_helpFrame;
    TeamSelectionFrame* m_teamSelectionFrame;
    InGameMenuFrame* m_inGameMenuFrame;
    mainMenuFrame* m_mainMenuFrame;
    SoundEffects* m_soundEffects;
    bool m_gameInProgress;
public:
    Ui::MainWindow uiMainWindow;
    };

#endif // MAINWINDOW_H
