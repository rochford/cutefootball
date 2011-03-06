#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"
#include "settingsFrame.h"
//#include "aboutFrame.h"
#include "halfstatisticsframe.h"
#include "inputSettingsFrame.h"
#include "helpFrame.h"
#include "teamSelectionFrame.h"
#include "mainMenuFrame.h"

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent),
      m_gameInProgress(false)
{
    m_soundEffects = new SoundEffects(this);

//    m_aboutFrame = new aboutFrame(this);
    m_halfStatisticsFrame = new HalfStatisticsFrame(this);
    m_helpFrame = new helpFrame(this);
    m_inputSettingsFrame = new inputSettingsFrame(this);

    m_settingsFrame = new settingsFrame(this);
    m_mainMenuFrame = new mainMenuFrame(this);

    uiMainWindow.setupUi(this);

    m_exitDialog = new QDialog();
    uiExitConfirmationDialog.setupUi(m_exitDialog);

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
    showFrame(MWindow::MainMenu);
}

MWindow::~MWindow()
{
    delete m_soundEffects;
    delete m_settingsFrame;
    delete m_halfStatisticsFrame;
//    delete m_aboutFrame;
    delete m_helpFrame;
    delete m_teamSelectionFrame;
    delete m_mainMenuFrame;
    delete m_exitDialog;
    //    delete m_pitch;
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
void MWindow::checkClose(int result)
{
    if (QDialog::Accepted == result)
        close();
}

void MWindow::createConnections()
{
    connect(uiMainWindow.actionNew_Game, SIGNAL(triggered()), this, SLOT(showTeamSelectionFrame()));
    connect(uiMainWindow.actionSettings, SIGNAL(triggered()), this, SLOT(showSettingsFrame()));
    connect(uiMainWindow.actionInputSettings, SIGNAL(triggered()), this, SLOT(showInputSettingsFrame()));
//    connect(uiMainWindow.actionAbout, SIGNAL(triggered()), this, SLOT(showAboutFrame()));
    connect(uiMainWindow.actionHelp, SIGNAL(triggered()), this, SLOT(showHelpFrame()));

    // this connection is created when a game is not in progress
    connect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    // this connection is created when a game is in progress
    //connect(uiMainWindow.actionQuit, SIGNAL(triggered()), m_exitDialog, SLOT(show()));

    connect(uiExitConfirmationDialog.actionContinue, SIGNAL(triggered()), this, SLOT(close()));
    connect(uiExitConfirmationDialog.actionMainMenu, SIGNAL(triggered()), this, SLOT(close()));
    connect(uiExitConfirmationDialog.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    connect(m_pitch, SIGNAL(gameInProgress(bool)), this, SLOT(enableActions(bool)));
    connect(m_pitch, SIGNAL(displayHalfTimeStatistics(bool)), this, SLOT(displayHalfTimeStatistics(bool)));

    connect(m_pitch, SIGNAL(pauseGame()), m_exitDialog, SLOT(show()));
}

void MWindow::displayHalfTimeStatistics(bool display)
{
    qDebug() << "MWindow::displayHalfTimeStatistics";
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
    if (gameInProgress) {
        disconnect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
        connect(uiMainWindow.actionQuit, SIGNAL(triggered()), m_exitDialog, SLOT(show()));
    } else {
        connect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
        disconnect(uiMainWindow.actionQuit, SIGNAL(triggered()), m_exitDialog, SLOT(show()));
    }
    m_gameInProgress = gameInProgress;
    uiMainWindow.actionNew_Game->setEnabled(!gameInProgress);
    uiMainWindow.actionSettings->setEnabled(!gameInProgress);
    uiMainWindow.actionInputSettings->setEnabled(!gameInProgress);
//    uiMainWindow.actionAbout->setEnabled(!gameInProgress);
    uiMainWindow.actionHelp->setEnabled(!gameInProgress);
    uiMainWindow.actionQuit->setEnabled(true);
    uiMainWindow.actionPause->setEnabled(!gameInProgress);

    uiMainWindow.menubar->setEnabled(true);
    uiMainWindow.menuAbout->setEnabled(!gameInProgress);

    if (!gameInProgress)
        showFrame(MWindow::HalfTimeStatistics);
}

void MWindow::showFrame(Frame f)
{
    /* Remove context menu from the all widgets. */
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w=0;
    foreach(w,widgets) {
        if (w->hasFocus())
            qDebug() << w->objectName() << "start has focus!!!";
    }

    uiMainWindow.menubar->setVisible(false);
    m_mainMenuFrame->setVisible(false);
//    m_aboutFrame->setVisible(false);
    m_helpFrame->setVisible(false);
    uiMainWindow.m_graphicsView->setVisible(false);
    m_settingsFrame->setVisible(false);
    m_inputSettingsFrame->setVisible(false);
    m_teamSelectionFrame->setVisible(false);
    m_halfStatisticsFrame->setVisible(false);
    uiMainWindow.m_graphicsView->clearFocus();

    switch (f) {
#if 0
    case MWindow::About:
        m_aboutFrame->setVisible(true);
        break;
#endif
    case MWindow::HalfTimeStatistics:
        m_halfStatisticsFrame->setVisible(true);
        break;
    case MWindow::Help:
        m_helpFrame->setVisible(true);
        break;
    case MWindow::InputSettings:
        m_inputSettingsFrame->setVisible(true);
        break;
    case MWindow::Settings:
        m_settingsFrame->setVisible(true);
        break;
    case MWindow::MainMenu:
        m_mainMenuFrame->showMaximized();
        break;
    case MWindow::GraphicsView:
        m_soundEffects->stopSound(SoundEffects::GameThemeTune);
        uiMainWindow.menubar->setVisible(true);
        uiMainWindow.menuAbout->setVisible(false);
        //uiMainWindow.m_graphicsView->setVisible(true);
        uiMainWindow.m_graphicsView->showMaximized();
        uiMainWindow.m_graphicsView->setFocus();
        break;
    case MWindow::TeamSelection:
        m_teamSelectionFrame->setVisible(true);
        break;
    default:
        break;
    }
    w=0;
    foreach(w,widgets) {
        if (w->hasFocus())
            qDebug() << w->objectName() << "end has focus!!!";
    }
}

void MWindow::hideStatisticsFrame()
{
    qDebug() << "MWindow::hideStatisticsFrame";
    if (m_gameInProgress) {
        m_pitch->setState(Pitch::Continue);
        showFrame(MWindow::GraphicsView);
    } else
        showFrame(MWindow::MainMenu);
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
