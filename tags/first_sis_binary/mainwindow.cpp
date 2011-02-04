#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"
#include "settingsFrame.h"
#include "aboutFrame.h"
#include "teamSelectionFrame.h"
#include "mainMenuFrame.h"

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_soundEffects = new SoundEffects(this);

    m_aboutFrame = new aboutFrame(this);
    m_settingsFrame = new settingsFrame(this);
    m_mainMenuFrame = new mainMenuFrame(this);
    m_teamSelectionFrame = new TeamSelectionFrame(this);

    uiMainWindow.setupUi(this);

    QRectF footballGround(0,0,400,600);
    m_pitch = new Pitch(footballGround,
                        uiMainWindow.m_graphicsView,
                        m_soundEffects,
                        m_settingsFrame);

    createConnections();
    setCentralWidget( uiMainWindow.m_graphicsView );
    showFrame(MWindow::MainMenu);
}

void MWindow::createConnections()
{
    connect(uiMainWindow.actionNew_Game, SIGNAL(triggered()), this, SLOT(showTeamSelectionFrame()));
//    connect(uiMainWindow.actionNew_Game, SIGNAL(triggered()), m_pitch, SLOT(newGame()));
    connect(uiMainWindow.actionSettings, SIGNAL(triggered()), this, SLOT(showSettingsFrame()));
    connect(uiMainWindow.actionAbout, SIGNAL(triggered()), this, SLOT(showAboutFrame()));
    connect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    connect(m_pitch, SIGNAL(gameInProgress(bool)), this, SLOT(enableActions(bool)));
}

void MWindow::enableActions(bool gameInProgress)
{
    uiMainWindow.actionNew_Game->setEnabled(!gameInProgress);
    uiMainWindow.actionSettings->setEnabled(!gameInProgress);
    uiMainWindow.actionAbout->setEnabled(!gameInProgress);
    uiMainWindow.actionQuit->setEnabled(true);
    if (!gameInProgress)
        showFrame(MWindow::MainMenu);
}

void MWindow::hideAboutFrame()
{
    showFrame(MWindow::MainMenu);
}

void MWindow::hideSettingsFrame()
{
    showFrame(MWindow::MainMenu);
}

void MWindow::showSettingsFrame()
{
    showFrame(MWindow::Settings);
}

void MWindow::hideTeamSelectionFrame()
{
    showFrame(MWindow::MainMenu);
}

void MWindow::showTeamSelectionFrame()
{
    showFrame(MWindow::TeamSelection);
}

void MWindow::showFrame(Frame f)
{
    uiMainWindow.menubar->setVisible(false);
    m_mainMenuFrame->setVisible(false);
    m_aboutFrame->setVisible(false);
    uiMainWindow.m_graphicsView->setVisible(false);
    m_settingsFrame->setVisible(false);
    m_teamSelectionFrame->setVisible(false);

    switch (f) {
    case MWindow::About:
        m_aboutFrame->setVisible(true);
        break;
    case MWindow::Settings:
        m_settingsFrame->setVisible(true);
        break;
    case MWindow::MainMenu:
        m_mainMenuFrame->setVisible(true);
        break;
    case MWindow::GraphicsView:
        uiMainWindow.menubar->setVisible(true);
        uiMainWindow.m_graphicsView->setVisible(true);
        break;
    case MWindow::TeamSelection:
        m_teamSelectionFrame->setVisible(true);
        break;
    default:
        break;
    }
}

void MWindow::buttonClickedNoise()
{
    m_soundEffects->soundEvent(SoundEffects::BallKick);
}

MWindow::~MWindow()
{
    delete m_pitch;
    delete m_soundEffects;
    delete m_settingsFrame;
    delete m_aboutFrame;
    delete m_teamSelectionFrame;
    delete m_mainMenuFrame;
}

void MWindow::newGame()
{
    showFrame(MWindow::GraphicsView);
    enableActions(true);
    m_pitch->newGame();
}

void MWindow::showAboutFrame()
{
    showFrame(MWindow::About);
}


