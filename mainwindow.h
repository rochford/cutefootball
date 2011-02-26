#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>
#include "ui_mainwindow.h"
#include "ui_exitconfirmationdialog.h"

class Pitch;
class SoundEffects;
class settingsFrame;
class aboutFrame;
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
      DiveEast,
      DiveWest
    };

private:
    enum Frame {
        MainMenu,
        Settings,
        About,
        Help,
        InputSettings,
        TeamSelection,
        GraphicsView
    };

    void createConnections();
    void removeContextMenus();

public slots:
    void hideTeamSelectionFrame();
    void hideSettingsFrame();
    void hideAboutFrame();
    void hideHelpFrame();
    void hideInputSettingsFrame();
    void showFrame(Frame f);
    void newGame(int homeTeam, int awayTeam);

protected:
    void resizeEvent(QResizeEvent *e);

private slots:
    void showAboutFrame();
    void showHelpFrame();
    void showInputSettingsFrame();
    void showSettingsFrame();
    void showTeamSelectionFrame();
    void enableActions(bool gameInProgress);
    void checkClose(int result);

private:
    Pitch *m_pitch;
    settingsFrame* m_settingsFrame;
    inputSettingsFrame* m_inputSettingsFrame;
    aboutFrame* m_aboutFrame;
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
