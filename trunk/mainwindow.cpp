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
#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"
#include "settingsFrame.h"
#include "halfstatisticsframe.h"
#include "inputSettingsFrame.h"
#include "helpFrame.h"
#include "teamSelectionFrame.h"
#include "mainMenuFrame.h"
#include "ingamemenuframe.h"
#include "aboutFrame.h"
#include "ui_mainwindow.h"

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent),
      m_homeTeamIndex(-1),
      m_awayTeamIndex(-1),
      m_gameInProgress(false),
      uiMainWindow(new Ui::MainWindow)
{
    m_soundEffects = new SoundEffects(this);

    m_helpFrame = new helpFrame(this);
    m_inputSettingsFrame = new inputSettingsFrame(this);
    m_aboutFrame = new aboutFrame(this);

    m_settingsFrame = new settingsFrame(this);
    m_mainMenuFrame = new mainMenuFrame(this);

    uiMainWindow->setupUi(this);
    // these frames are dependent on uiMainWindow actions, construct
    // after uiMainWindow
    m_halfStatisticsFrame = new HalfStatisticsFrame(this);

    QRectF footballGround(0,0,340,420);
    m_pitch = new Pitch(footballGround,
                        uiMainWindow->m_graphicsView,
                        m_soundEffects,
                        m_settingsFrame,
                        uiMainWindow->statusBar);
    m_teamSelectionFrame = new TeamSelectionFrame(this);
    m_inGameMenuFrame = new InGameMenuFrame(this);

    removeContextMenus();

    createConnections();

    m_soundEffects->startSound(SoundEffects::GameThemeTune);

    setCentralWidget( uiMainWindow->m_graphicsView );
    showFrame(MWindow::MainMenu);
    emit setFrame(MWindow::MainMenu);
}

MWindow::~MWindow()
{
    delete m_soundEffects;
    delete m_settingsFrame;
    delete m_halfStatisticsFrame;
    delete m_helpFrame;
    delete m_teamSelectionFrame;
    delete m_mainMenuFrame;
    delete m_inGameMenuFrame;
    delete m_aboutFrame;
    delete m_inputSettingsFrame;
    delete m_pitch;
    delete uiMainWindow;
}

void MWindow::removeContextMenus()
{
    /* Remove context menu from the all widgets. */
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w=0;
    foreach(w,widgets) {
        w->setContextMenuPolicy(Qt::NoContextMenu);
    }
}

void MWindow::createConnections()
{
    connect(this, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(uiMainWindow->actionSingleGame, SIGNAL(triggered()),
            this, SLOT(showSingleGameTeamSelection()));
    connect(uiMainWindow->actionGameCup, SIGNAL(triggered()),
            this, SLOT(showCupTeamSelection()));
    connect(uiMainWindow->actionSettings, SIGNAL(triggered()),
            this, SLOT(showSettingsFrame()));
    connect(uiMainWindow->actionInputSettings, SIGNAL(triggered()),
            this, SLOT(showInputSettingsFrame()));
    connect(uiMainWindow->actionHelp, SIGNAL(triggered()),
            this, SLOT(showHelpFrame()));
    connect(uiMainWindow->actionMainMenu, SIGNAL(triggered()),
            this, SLOT(showMainMenuFrame()));
    connect(uiMainWindow->actionMainMenu, SIGNAL(triggered()),
            m_pitch, SLOT(gameStop()));
    connect(uiMainWindow->actionAbout, SIGNAL(triggered()),
            this, SLOT(showAboutFrame()));

    connect(uiMainWindow->actionContinue, SIGNAL(triggered()),
            this, SLOT(hideInGameMenu()));
    connect(uiMainWindow->actionQuit, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(m_pitch, SIGNAL(gameInProgress(bool)),
            this, SLOT(enableActions(bool)));

    connect(m_pitch, SIGNAL(pauseGameClock()),
            this, SLOT(showInGameMenu()));
}

void MWindow::hideInGameMenu()
{
    emit setFrame(MWindow::GraphicsView);
    m_pitch->continueGame();
}

void MWindow::displayHalfTimeStatistics(bool display)
{
    if (display) {
        // m_halfStatisticsFrame->setHalfName(halfName);
        m_halfStatisticsFrame->setHomeTeamName(m_pitch->homeTeam()->localisedName());
        m_halfStatisticsFrame->setHomeTeamFlag(m_pitch->homeTeam()->flag());
        m_halfStatisticsFrame->setAwayTeamName(m_pitch->awayTeam()->localisedName());
        m_halfStatisticsFrame->setAwayTeamFlag(m_pitch->awayTeam()->flag());

        m_halfStatisticsFrame->setHomeTeamGoals(m_pitch->homeTeam()->m_goals);
        m_halfStatisticsFrame->setAwayTeamGoals(m_pitch->awayTeam()->m_goals);

        m_halfStatisticsFrame->setHomeTeamShots(m_pitch->homeTeam()->shots());
        m_halfStatisticsFrame->setAwayTeamShots(m_pitch->awayTeam()->shots());

        showStatisticsFrame();
    } else
        hideStatisticsFrame();
}

void MWindow::enableActions(bool gameInProgress)
{
    qDebug() << "MWindow::enableActions" << gameInProgress;
    m_gameInProgress = gameInProgress;
    uiMainWindow->actionSingleGame->setEnabled(!gameInProgress);
    uiMainWindow->actionGameCup->setEnabled(!gameInProgress);
    uiMainWindow->actionSettings->setEnabled(!gameInProgress);
    uiMainWindow->actionInputSettings->setEnabled(!gameInProgress);
    uiMainWindow->actionHelp->setEnabled(!gameInProgress);
    uiMainWindow->actionAbout->setEnabled(!gameInProgress);
    uiMainWindow->actionQuit->setEnabled(true);
    uiMainWindow->actionPause->setEnabled(gameInProgress);
    uiMainWindow->actionContinue->setEnabled(gameInProgress);

    // TODO this needs to be moved to be triggered by game FSM
    // complete.
    // dont want to show this frame if user paused and returned
    // to main menu
    if (!gameInProgress)
        displayHalfTimeStatistics(true);
}

void MWindow::showFrame(Frame f)
{
    qDebug() << "MWindow::showFrame " << f;
    if ( f == GraphicsView) {
        uiMainWindow->m_graphicsView->activateWindow();
        m_soundEffects->stopSound(SoundEffects::GameThemeTune);
        uiMainWindow->m_graphicsView->showMaximized();
        uiMainWindow->m_graphicsView->setFocus();
    } else {
        uiMainWindow->m_graphicsView->setVisible(false);
        uiMainWindow->m_graphicsView->clearFocus();
    }
}

void MWindow::hideStatisticsFrame()
{
    qDebug() << "MWindow::hideStatisticsFrame " << m_gameInProgress;
    if (m_gameInProgress)
        emit setFrame(MWindow::GraphicsView);
    else
        emit setFrame(MWindow::MainMenu);
}

void MWindow::newGame()
{
    showFrame(MWindow::GraphicsView);
    enableActions(true);
    m_pitch->newGame(playerTeam(), computerTeam()); // TODO
}

void MWindow::resizeEvent(QResizeEvent *e)
{
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w=0;
    foreach(w,widgets) {
        if (dynamic_cast<QPushButton*>(w)==NULL)
            w->resize(e->size());
    }
    e->accept();
}

void MWindow::setOrientation(ScreenOrientation orientation)
{
#ifdef Q_OS_SYMBIAN
    if (orientation != ScreenOrientationAuto) {
#if defined(ORIENTATIONLOCK)
        const CAknAppUiBase::TAppUiOrientation uiOrientation =
                (orientation == ScreenOrientationLockPortrait) ? CAknAppUi::EAppUiOrientationPortrait
                    : CAknAppUi::EAppUiOrientationLandscape;
        CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
        TRAPD(error,
            if (appUi)
                appUi->SetOrientationL(uiOrientation);
        );
        Q_UNUSED(error)
#else // ORIENTATIONLOCK
        qWarning("'ORIENTATIONLOCK' needs to be defined on Symbian when locking the orientation.");
#endif // ORIENTATIONLOCK
    }
#elif defined(Q_WS_MAEMO_5)
    Qt::WidgetAttribute attribute;
    switch (orientation) {
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_Maemo5PortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_Maemo5LandscapeOrientation;
        break;
    case ScreenOrientationAuto:
    default:
        attribute = Qt::WA_Maemo5AutoOrientation;
        break;
    }
    setAttribute(attribute, true);
#else // Q_OS_SYMBIAN
    Q_UNUSED(orientation);
#endif // Q_OS_SYMBIAN
}

void MWindow::showCupTeamSelection()
    {
    m_teamSelectionFrame->setTeamSelectionState(TeamSelectionFrame::PlayerTeamOnly);
    emit setFrame(MWindow::TeamSelection);
    }

void MWindow::showSingleGameTeamSelection()
    {
    m_teamSelectionFrame->setTeamSelectionState(TeamSelectionFrame::PlayerAndComputerTeam);
    emit setFrame(MWindow::TeamSelection);
    }

void MWindow::showComputerTeamSelection()
    {
    m_teamSelectionFrame->setTeamSelectionState(TeamSelectionFrame::ComputerTeamOnly);
    emit setFrame(MWindow::TeamSelection);
    }
