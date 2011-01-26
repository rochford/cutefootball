#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"
#include "settingsDialog.h"

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_soundEffects = new SoundEffects(this);

    uiMainWindow.setupUi(this);

    m_settingsDialog = new settingsDialog(this);
    m_settingsDialog->setVisible(false);

    QRectF footballGround(0,0,400,600);
    m_pitch = new Pitch(footballGround,
                        uiMainWindow.m_graphicsView,
                        m_soundEffects,
                        m_settingsDialog);

    createConnections();

    setCentralWidget( uiMainWindow.m_graphicsView );
}

void MWindow::createConnections()
{
    connect(uiMainWindow.actionNew_Game, SIGNAL(triggered()), m_pitch, SLOT(newGame()));
    connect(uiMainWindow.actionSettings, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
    connect(uiMainWindow.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    connect(m_pitch, SIGNAL(gameInProgress(bool)), this, SLOT(enableActions(bool)));
}

void MWindow::enableActions(bool gameInProgress)
{
    uiMainWindow.actionNew_Game->setEnabled(!gameInProgress);
    uiMainWindow.actionSettings->setEnabled(!gameInProgress);
    uiMainWindow.actionAbout->setEnabled(!gameInProgress);
}

void MWindow::showSettingsDialog()
{
    m_settingsDialog->setVisible(true);
}

void MWindow::buttonClickedNoise()
{
    m_soundEffects->soundEvent(SoundEffects::BallKick);
}

MWindow::~MWindow()
{
    delete m_pitch;
    delete m_soundEffects;
    delete m_settingsDialog;
}

void MWindow::about()
{
    QMessageBox::about(this, tr("About Cute Football"),
                       tr("Cute Football is written by Tim"));
}


