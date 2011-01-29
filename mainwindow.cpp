#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"
#include "settingsFrame.h"
#include "aboutFrame.h"
#include "mainMenuFrame.h"

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_soundEffects = new SoundEffects(this);

    m_aboutFrame = new aboutFrame(this);
    m_settingsFrame = new settingsFrame(this);
    m_mainMenuFrame = new mainMenuFrame(this);
    uiMainWindow.setupUi(this);
    uiMainWindow.menubar->setVisible(false);

    QRectF footballGround(0,0,400,600);
    m_pitch = new Pitch(footballGround,
                        uiMainWindow.m_graphicsView,
                        m_soundEffects,
                        m_settingsFrame);

    createConnections();
    setCentralWidget( uiMainWindow.m_graphicsView );
    showMainMenuFrame();
}

void MWindow::createConnections()
{
    connect(uiMainWindow.actionNew_Game, SIGNAL(triggered()), this, SLOT(newGame()));
    connect(uiMainWindow.actionNew_Game, SIGNAL(triggered()), m_pitch, SLOT(newGame()));
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
}

void MWindow::hideAboutFrame()
{
    showMainMenuFrame();
}

void MWindow::hideSettingsFrame()
{
    showMainMenuFrame();
}

void MWindow::showSettingsFrame()
{
    m_mainMenuFrame->setVisible(false);
    m_aboutFrame->setVisible(false);
    uiMainWindow.m_graphicsView->setVisible(false);
    m_settingsFrame->setVisible(true);
}

void MWindow::showMainMenuFrame()
{
    m_aboutFrame->setVisible(false);
    m_settingsFrame->setVisible(false);
    uiMainWindow.m_graphicsView->setVisible(false);
    m_mainMenuFrame->setVisible(true);
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
    delete m_mainMenuFrame;
}

void MWindow::newGame()
{
    m_settingsFrame->setVisible(false);
    m_aboutFrame->setVisible(false);
    m_mainMenuFrame->setVisible(false);
    uiMainWindow.menubar->setVisible(true);
    uiMainWindow.m_graphicsView->setVisible(true);
    enableActions(true);
    m_pitch->newGame();
}

void MWindow::showAboutFrame()
{
    m_settingsFrame->setVisible(false);
    m_mainMenuFrame->setVisible(false);
    uiMainWindow.m_graphicsView->setVisible(false);
    m_aboutFrame->setVisible(true);
}


