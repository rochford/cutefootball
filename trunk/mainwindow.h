#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>
#include "ui_mainwindow.h"
#include "ui_exitconfirmationdialog.h"

class Pitch;
class SoundEffects;
class settingsFrame;
// class aboutFrame;
class helpFrame;
class inputSettingsFrame;
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
        East = 0,
        SouthEast = 45,
        South = 90,
        SouthWest = 135,
        West = 180,
        NorthWest = 225,
        North = 270,
        NorthEast = 315,

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
      DiveEast,
      DiveWest,

      NoAction
    };

private:
    enum Frame {
        MainMenu,
        Settings,
//        About,
        Help,
        InputSettings,
        TeamSelection,
        GraphicsView
    };

    void createConnections();
    void removeContextMenus();

public slots:
    void showFrame(Frame f);
    void newGame(int homeTeam, int awayTeam);
//    void hideAboutFrame(){showFrame(MWindow::MainMenu);}
    void hideInputSettingsFrame(){showFrame(MWindow::MainMenu);}
    void hideHelpFrame(){showFrame(MWindow::MainMenu);}
    void hideSettingsFrame(){ showFrame(MWindow::MainMenu);}
    void showSettingsFrame(){ showFrame(MWindow::Settings);}
    void hideTeamSelectionFrame(){ showFrame(MWindow::MainMenu);}
    void showTeamSelectionFrame(){ showFrame(MWindow::TeamSelection);}
    void showInputSettingsFrame() { showFrame(MWindow::InputSettings);}
    void showHelpFrame(){showFrame(MWindow::Help);}
    // void showAboutFrame(){showFrame(MWindow::About);}

protected:
    void resizeEvent(QResizeEvent *e);

private slots:
    void enableActions(bool gameInProgress);
    void checkClose(int result);

private:
    Pitch *m_pitch;
    settingsFrame* m_settingsFrame;
    inputSettingsFrame* m_inputSettingsFrame;
//    aboutFrame* m_aboutFrame;
    helpFrame* m_helpFrame;
    TeamSelectionFrame* m_teamSelectionFrame;
    mainMenuFrame* m_mainMenuFrame;
    SoundEffects* m_soundEffects;
    bool m_gameInProgress;

    Ui::MainWindow uiMainWindow;

    Ui::ExitConfirmationDialog uiExitConfirmationDialog;
    QDialog* m_exitDialog;
    };

#endif // MAINWINDOW_H
