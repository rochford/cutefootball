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

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent),
      m_gameInProgress(false)
{
    m_soundEffects = new SoundEffects(this);

    m_helpFrame = new helpFrame(this);
    m_inputSettingsFrame = new inputSettingsFrame(this);

    m_settingsFrame = new settingsFrame(this);
    m_mainMenuFrame = new mainMenuFrame(this);

    uiMainWindow.setupUi(this);
    // these frames are dependent on uiMainWindow actions, construct
    // after uiMainWindow
    m_halfStatisticsFrame = new HalfStatisticsFrame(this);
    m_inGameMenuFrame = new InGameMenuFrame(this);

    QRectF footballGround(0,0,300,400);
    m_pitch = new Pitch(footballGround,
                        uiMainWindow.m_graphicsView,
                        m_soundEffects,
                        m_settingsFrame);
    m_teamSelectionFrame = new TeamSelectionFrame(this);

    removeContextMenus();

    createConnections();

    m_soundEffects->startSound(SoundEffects::GameThemeTune);

    setCentralWidget( uiMainWindow.m_graphicsView );
    // showFrame(MWindow::MainMenu);
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

    connect(uiMainWindow.actionNew_Game, SIGNAL(triggered()),
            this, SLOT(showTeamSelectionFrame()));
    connect(uiMainWindow.actionSettings, SIGNAL(triggered()),
            this, SLOT(showSettingsFrame()));
    connect(uiMainWindow.actionInputSettings, SIGNAL(triggered()),
            this, SLOT(showInputSettingsFrame()));
    connect(uiMainWindow.actionHelp, SIGNAL(triggered()),
            this, SLOT(showHelpFrame()));
    connect(uiMainWindow.actionMainMenu, SIGNAL(triggered()),
            this, SLOT(showMainMenuFrame()));
    connect(uiMainWindow.actionMainMenu, SIGNAL(triggered()),
            m_pitch, SLOT(gameStop()));

    connect(uiMainWindow.actionContinue, SIGNAL(triggered()),
            this, SLOT(hideInGameMenu()));
    connect(uiMainWindow.actionContinue, SIGNAL(triggered()),
            this, SLOT(hideStatisticsFrame()));
    connect(uiMainWindow.actionQuit, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(m_pitch, SIGNAL(gameInProgress(bool)),
            this, SLOT(enableActions(bool)));
    connect(m_pitch, SIGNAL(displayHalfTimeStatistics(bool)),
            this, SLOT(displayHalfTimeStatistics(bool)));

    connect(m_pitch, SIGNAL(pauseGameClock()), this, SLOT(showInGameMenu()));
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
        m_halfStatisticsFrame->setHomeTeamName(m_pitch->homeTeam()->fullName());
        m_halfStatisticsFrame->setHomeTeamFlag(QString(":/images/flags/") + m_pitch->homeTeam()->fullName() + ".png");
        m_halfStatisticsFrame->setAwayTeamName(m_pitch->awayTeam()->fullName());
        m_halfStatisticsFrame->setAwayTeamFlag(QString(":/images/flags/") + m_pitch->awayTeam()->fullName() + ".png");

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
#if 0
    if (gameInProgress) {
        disconnect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
        connect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(showInGameMenu()));
    } else {
        connect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
        disconnect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(showInGameMenu()));
    }
#endif
    m_gameInProgress = gameInProgress;
    uiMainWindow.actionNew_Game->setEnabled(!gameInProgress);
    uiMainWindow.actionSettings->setEnabled(!gameInProgress);
    uiMainWindow.actionInputSettings->setEnabled(!gameInProgress);
    uiMainWindow.actionHelp->setEnabled(!gameInProgress);
    uiMainWindow.actionQuit->setEnabled(true);
    uiMainWindow.actionPause->setEnabled(gameInProgress);
    uiMainWindow.actionContinue->setEnabled(gameInProgress);

//    if (!gameInProgress)
//        showFrame(MWindow::HalfTimeStatistics);
}

void MWindow::showFrame(Frame f)
{
    qDebug() << "MWindow::showFrame " << f;
    if ( f == GraphicsView) {
        uiMainWindow.m_graphicsView->activateWindow();
        m_soundEffects->stopSound(SoundEffects::GameThemeTune);
        uiMainWindow.m_graphicsView->showMaximized();
        uiMainWindow.m_graphicsView->setFocus();
    } else {
        if ( f == MainMenu )
            enableActions(false);
        uiMainWindow.m_graphicsView->setVisible(false);
        uiMainWindow.m_graphicsView->clearFocus();
    }
}

void MWindow::hideStatisticsFrame()
{
    if (m_gameInProgress) {
        m_pitch->setState(Pitch::ReadyForNextHalf);
        emit setFrame(MWindow::GraphicsView);
    } else
        emit setFrame(MWindow::MainMenu);
}

void MWindow::newGame(int homeTeam, int awayTeam)
{
    showFrame(MWindow::GraphicsView);
    enableActions(true);
    m_pitch->newGame(homeTeam, awayTeam);
}

void MWindow::resizeEvent(QResizeEvent *e)
{
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w=0;
    foreach(w,widgets) {
        w->resize(e->size());
    }
}
