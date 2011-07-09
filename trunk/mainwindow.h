/*
 * Copyright 2010,2011 Timothy Rochford
 *
 *    This file is part of CuteFootball.
 *
 *    CuteFootball is free software: you can redistribute it and/or modify
 *    it under the terms of the Lesser GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    CuteFootball is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Lesser GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>
class TeamSelectionFrame;

class aboutFrame;
class Pitch;
class SoundEffects;
class settingsFrame;
class helpFrame;
class inputSettingsFrame;
class mainMenuFrame;
class HalfStatisticsFrame;
class InGameMenuFrame;

namespace Ui {
    class MainWindow;
}

class MWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    MWindow(QWidget *parent = 0);
    virtual ~MWindow();

    void setOrientation(ScreenOrientation orientation);
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
        InGameMenu,
        About
    };

private:
    void createConnections();
    void removeContextMenus();

signals:
    void setFrame(MWindow::Frame f);

public slots:
    void showFrame(MWindow::Frame f);
    void newGame();

    inline void setComputerTeam(int index) { m_awayTeamIndex = index; }
    inline int computerTeam() const { return m_awayTeamIndex; }
    inline void setPlayerTeam(int index){ m_homeTeamIndex = index; }
    inline int playerTeam() const { return m_homeTeamIndex; }
    void hideInputSettingsFrame(){ emit setFrame(MWindow::MainMenu);}
    void hideHelpFrame(){emit setFrame(MWindow::MainMenu);}
    void hideSettingsFrame(){ emit setFrame(MWindow::MainMenu);}
    void showSettingsFrame(){ emit setFrame(MWindow::Settings);}
    void showMainMenuFrame(){ emit setFrame(MWindow::MainMenu);}
    void hideTeamSelectionFrame(){ emit setFrame(MWindow::MainMenu);}
    void showSingleGameTeamSelection();
    void showInputSettingsFrame() { emit setFrame(MWindow::InputSettings);}
    void showHelpFrame(){emit setFrame(MWindow::Help);}
    void showStatisticsFrame(){emit setFrame(MWindow::HalfTimeStatistics);}
    void hideStatisticsFrame();
    void showAboutFrame(){emit setFrame(MWindow::About);}
    void hideAboutFrame() { emit setFrame(MWindow::MainMenu);}
    void showInGameMenu() { emit setFrame(MWindow::InGameMenu); }
    void hideInGameMenu();
    void displayHalfTimeStatistics(bool display);
    void showGraphicsViewFrame() { emit setFrame(MWindow::GraphicsView); }
    void showComputerTeamSelection();
    void showCupTeamSelection();


protected:
    void resizeEvent(QResizeEvent *e);

private slots:
    void enableActions(bool gameInProgress);

private:
    int m_homeTeamIndex;
    int m_awayTeamIndex;
    Pitch *m_pitch;
    settingsFrame* m_settingsFrame;
    inputSettingsFrame* m_inputSettingsFrame;
    HalfStatisticsFrame* m_halfStatisticsFrame;
    helpFrame* m_helpFrame;
    TeamSelectionFrame* m_teamSelectionFrame;
    InGameMenuFrame* m_inGameMenuFrame;
    mainMenuFrame* m_mainMenuFrame;
    SoundEffects* m_soundEffects;
    aboutFrame* m_aboutFrame;
    bool m_gameInProgress;
public:
    Ui::MainWindow* uiMainWindow;
    };

#endif // MAINWINDOW_H
