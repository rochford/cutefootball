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
        GraphicsView
    };

    void createConnections();

public slots:
    void hideSettingsFrame();
    void hideAboutFrame();
    void showFrame(Frame f);

private slots:
    void newGame();
    void showAboutFrame();
    void buttonClickedNoise();
    void showSettingsFrame();
    void enableActions(bool gameInProgress);

private:
    Pitch *m_pitch;
    settingsFrame* m_settingsFrame;
    aboutFrame* m_aboutFrame;
    mainMenuFrame* m_mainMenuFrame;
    SoundEffects* m_soundEffects;

    Ui::MainWindow uiMainWindow;
    };

#endif // MAINWINDOW_H
