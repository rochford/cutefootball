#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"
#include "replay.h"
#include "settingsDialog.h"

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_soundEffects = new SoundEffects(this);

    m_mainMenuFrame = new QFrame();
    ui.setupUi(m_mainMenuFrame);

    uiMainWindow.setupUi(this);

    m_mainMenuFrame->setVisible(true);
    m_mainMenuFrame->setEnabled(true);

    m_settingsDialog = new settingsDialog(this);
    m_settingsDialog->setVisible(false);

    QRectF footballGround(0,0,400,600);
    m_pitch = new Pitch(footballGround,
                        uiMainWindow.m_graphicsView,
                        m_soundEffects,
                        m_mainMenuFrame,
                        m_settingsDialog);

    ui.m_newGameBtn->setText(uiMainWindow.actionNew_Game->text());
    ui.m_settingsBtn->setText(uiMainWindow.actionSettings->text());
    ui.m_quitBtn->setText(uiMainWindow.actionQuit->text());
    ui.m_aboutBtn->setText(uiMainWindow.actionAbout->text());
    ui.m_newGameBtn->addAction(uiMainWindow.actionNew_Game);
    ui.m_settingsBtn->addAction(uiMainWindow.actionSettings);
    ui.m_aboutBtn->addAction(uiMainWindow.actionAbout);
    ui.m_quitBtn->addAction(uiMainWindow.actionQuit);

    createKeyboardActions();

    createConnections();

    setCentralWidget(uiMainWindow.m_graphicsView);
}

void MWindow::createConnections()
{
    connect(uiMainWindow.actionNew_Game, SIGNAL(triggered()), m_pitch, SLOT(newGame()));
    connect(uiMainWindow.actionSettings, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
    connect(uiMainWindow.actionReplay, SIGNAL(triggered()), m_pitch, SLOT(replayStart()));
    connect(uiMainWindow.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(uiMainWindow.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_pitch, SIGNAL(gameInProgress(bool)), this, SLOT(isPlaying(bool)));

    connect(ui.m_newGameBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClickedNoise()));
    connect(ui.m_newGameBtn, SIGNAL(clicked(bool)), uiMainWindow.actionNew_Game, SIGNAL(triggered()));
    connect(ui.m_settingsBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClickedNoise()));
    connect(ui.m_settingsBtn, SIGNAL(clicked(bool)), uiMainWindow.actionSettings, SIGNAL(triggered()));
    connect(ui.m_quitBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClickedNoise()));
    connect(ui.m_quitBtn, SIGNAL(clicked(bool)), uiMainWindow.actionQuit, SIGNAL(triggered()));
    connect(ui.m_aboutBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClickedNoise()));
    connect(ui.m_aboutBtn, SIGNAL(clicked(bool)), uiMainWindow.actionAbout, SIGNAL(triggered()));

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

void MWindow::isPlaying(bool playing)
{
    uiMainWindow.menuGame->setEnabled(playing);
    uiMainWindow.actionReplay->setEnabled(playing);
}

void MWindow::createKeyboardActions()
{
    m_actions.insert( Qt::Key_R, Replay );
}

MWindow::~MWindow()
{
    delete m_pitch;
    delete m_soundEffects;
    delete m_mainMenuFrame;
    delete m_settingsDialog;
}

void MWindow::keyPressEvent( QKeyEvent *event )
{
    if ( event->isAutoRepeat() || !m_actions.contains( event->key() ) ) {
        event->ignore();
        return;
    }

    Action a = m_actions[ event->key() ];

    if ( m_pitch->replay()->isReplay() )
        // Not allowed to stop a replay!!!
        // pitch->replayStop();
        ;
    else {
        switch ( a )
        {
        case Replay:
            m_pitch->replayStart();
            break;
        default:
            break;
        }
    }
    event->accept();
}

void MWindow::about()
{
    QMessageBox::about(this, tr("About Cute Football"),
                       tr("Cute Football is written by XXX"));
}
