#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"
#include "settingsFrame.h"
#include "aboutFrame.h"
#include "inputSettingsFrame.h"
#include "helpFrame.h"
#include "teamSelectionFrame.h"
#include "mainMenuFrame.h"

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_soundEffects = new SoundEffects(this);

    m_aboutFrame = new aboutFrame(this);
    m_helpFrame = new helpFrame(this);
    m_inputSettingsFrame = new inputSettingsFrame(this);

    m_settingsFrame = new settingsFrame(this);
    m_mainMenuFrame = new mainMenuFrame(this);

    uiMainWindow.setupUi(this);

    QRectF footballGround(0,0,300,400);
    m_pitch = new Pitch(footballGround,
                        uiMainWindow.m_graphicsView,
                        m_soundEffects,
                        m_settingsFrame);
    m_teamSelectionFrame = new TeamSelectionFrame(this);


    createConnections();
    setCentralWidget( uiMainWindow.m_graphicsView );
    showFrame(MWindow::MainMenu);
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
    connect(uiMainWindow.actionNew_Game, SIGNAL(triggered()), this, SLOT(showTeamSelectionFrame()));
    connect(uiMainWindow.actionSettings, SIGNAL(triggered()), this, SLOT(showSettingsFrame()));
    connect(uiMainWindow.actionInputSettings, SIGNAL(triggered()), this, SLOT(showInputSettingsFrame()));
    connect(uiMainWindow.actionAbout, SIGNAL(triggered()), this, SLOT(showAboutFrame()));
    connect(uiMainWindow.actionHelp, SIGNAL(triggered()), this, SLOT(showHelpFrame()));
    connect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    connect(m_pitch, SIGNAL(gameInProgress(bool)), this, SLOT(enableActions(bool)));
}

void MWindow::enableActions(bool gameInProgress)
{
    uiMainWindow.actionNew_Game->setEnabled(!gameInProgress);
    uiMainWindow.actionSettings->setEnabled(!gameInProgress);
    uiMainWindow.actionInputSettings->setEnabled(!gameInProgress);
    uiMainWindow.actionAbout->setEnabled(!gameInProgress);
    uiMainWindow.actionHelp->setEnabled(!gameInProgress);
    uiMainWindow.actionQuit->setEnabled(true);

    uiMainWindow.menubar->setEnabled(true);
    uiMainWindow.menuAbout->setEnabled(!gameInProgress);

    if (!gameInProgress)
        showFrame(MWindow::MainMenu);
}

void MWindow::hideAboutFrame()
{
    showFrame(MWindow::MainMenu);
}
void MWindow::hideInputSettingsFrame()
{
    showFrame(MWindow::MainMenu);
}

void MWindow::hideHelpFrame()
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

void MWindow::showInputSettingsFrame()
{
    showFrame(MWindow::InputSettings);
}

void MWindow::showFrame(Frame f)
{
    uiMainWindow.menubar->setVisible(false);
    m_mainMenuFrame->setVisible(false);
    m_aboutFrame->setVisible(false);
    m_helpFrame->setVisible(false);
    uiMainWindow.m_graphicsView->setVisible(false);
    m_settingsFrame->setVisible(false);
    m_inputSettingsFrame->setVisible(false);
    m_teamSelectionFrame->setVisible(false);

    switch (f) {
    case MWindow::About:
        m_aboutFrame->setVisible(true);
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
        m_mainMenuFrame->setVisible(true);
        break;
    case MWindow::GraphicsView:
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
        // Toggle softkey visibility
        setWindowFlags( windowFlags() ^ Qt::WindowSoftkeysVisibleHint );
        showFullScreen();
#endif //
        uiMainWindow.menubar->setVisible(true);
        uiMainWindow.menuAbout->setVisible(false);
        uiMainWindow.m_graphicsView->setVisible(true);
        uiMainWindow.m_graphicsView->setFocus();
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
    delete m_helpFrame;
    delete m_teamSelectionFrame;
    delete m_mainMenuFrame;
}

void MWindow::newGame(int homeTeam, int awayTeam)
{
    showFrame(MWindow::GraphicsView);
    enableActions(true);
    m_pitch->newGame(homeTeam, awayTeam);
}

void MWindow::showHelpFrame()
{
    showFrame(MWindow::Help);
}

void MWindow::showAboutFrame()
{
    showFrame(MWindow::About);
}


