#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"
#include "replay.h"
#include "settingsDialog.h"

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Cute Football"));

    m_soundEffects = new SoundEffects(this);

    m_frame = new QFrame();
    ui.setupUi(m_frame);
    m_frame->setVisible(true);
    m_frame->setEnabled(true);

    m_settingsDialog = new settingsDialog(this);
    m_settingsDialog->setVisible(false);

    QRectF footballGround(0,0,400,600);
    m_pitch = new Pitch(footballGround, m_frame, m_settingsDialog);

    createActions();
    ui.m_newGameBtn->setText(m_newGameAction->text());
    ui.m_settingsBtn->setText(m_settingsAction->text());
    ui.m_quitBtn->setText(m_quitAction->text());
    ui.m_aboutBtn->setText(m_aboutAction->text());
    createKeyboardActions();

    createConnections();

    setCentralWidget(m_pitch->m_view);
    m_pitch->m_view->show();
}

void MWindow::createConnections()
{
    connect(m_newGameAction, SIGNAL(triggered()), m_pitch, SLOT(newGame()));
    connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
    connect(m_replayAction, SIGNAL(triggered()), m_pitch, SLOT(replayStart()));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_pitch, SIGNAL(gameInProgress(bool)), this, SLOT(isPlaying(bool)));

    connect(ui.m_newGameBtn, SIGNAL(clicked(bool)), m_newGameAction, SIGNAL(triggered()));
    connect(ui.m_newGameBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClickedNoise()));
    connect(ui.m_settingsBtn, SIGNAL(clicked(bool)), m_settingsAction, SIGNAL(triggered()));
    connect(ui.m_settingsBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClickedNoise()));
    connect(ui.m_quitBtn, SIGNAL(clicked(bool)), m_quitAction, SIGNAL(triggered()));
    connect(ui.m_quitBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClickedNoise()));
    connect(ui.m_aboutBtn, SIGNAL(clicked(bool)), m_aboutAction, SIGNAL(triggered()));
    connect(ui.m_aboutBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClickedNoise()));

    connect(m_pitch, SIGNAL(gameInProgress(bool)), this, SLOT(enableActions(bool)));
}

void MWindow::enableActions(bool gameInProgress)
{
    m_newGameAction->setEnabled(!gameInProgress);
    m_settingsAction->setEnabled(!gameInProgress);
    m_aboutAction->setEnabled(!gameInProgress);
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
    m_gameMenu->setEnabled(playing);
    m_replayAction->setEnabled(playing);
}

void MWindow::createActions()
{
    m_newGameAction = new QAction(QString(tr("New Game")), this);
    addAction(m_newGameAction);
    ui.m_newGameBtn->addAction(m_newGameAction);

    m_replayAction = new QAction(QString(tr("Replay")), this);
    m_replayAction->setEnabled(false);

    m_settingsAction = new QAction(QString(tr("Settings")), this);
    addAction(m_settingsAction);
    ui.m_settingsBtn->addAction(m_settingsAction);

    m_aboutAction = new QAction(QString(tr("About")), this);
    addAction(m_aboutAction);
    ui.m_aboutBtn->addAction(m_aboutAction);

    m_quitAction = new QAction(QString(tr("Quit")), this);
    addAction(m_quitAction);
    ui.m_quitBtn->addAction(m_quitAction);

    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newGameAction);
    m_fileMenu->addAction(m_settingsAction);
    m_fileMenu->addAction(m_quitAction);

    m_gameMenu = menuBar()->addMenu(tr("&Game"));
    m_gameMenu->addAction(m_replayAction);
    m_gameMenu->setEnabled(false);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAction);
}

void MWindow::createKeyboardActions()
{
    m_actions.insert( Qt::Key_R, Replay );
}

MWindow::~MWindow()
{
    delete m_pitch;
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
