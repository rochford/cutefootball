#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"
#include "replay.h"

const int KLongPressValue = 800; // Ms

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent),
    m_keyEventTimer(NULL)
{
    setWindowTitle(tr("Cute Football"));

    QRectF footballGround(0,0,400,600);
    m_pitch = new Pitch(footballGround);

    m_keyEventTimer = new QTimer(this);
    m_keyEventTimer->setInterval(KGameRefreshRate);

    createActions();
    createKeyboardActions();

    connect(m_keyEventTimer, SIGNAL(timeout()), this, SLOT(repeatKeyEvent()));
    connect(m_newGameAction, SIGNAL(triggered()), m_pitch, SLOT(newGame()));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    setCentralWidget(m_pitch->view);
    m_pitch->view->show();
}

void MWindow::createActions()
{
    m_newGameAction = new QAction(QString(tr("New Game")), this);
    addAction(m_newGameAction);

    m_settingsAction = new QAction(QString(tr("Settings")), this);
//    addAction(m_settingsAction);

    m_aboutAction = new QAction(QString(tr("About")), this);
    addAction(m_aboutAction);

    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newGameAction);
//    m_fileMenu->addAction(m_settingsAction);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAction);
}

void MWindow::stopKeyEvent()
{
    if (m_keyEventTimer->isActive())
        m_keyEventTimer->stop();
}

void MWindow::createKeyboardActions()
{
    m_actions.insert( Qt::Key_W, North );
    m_actions.insert( Qt::Key_E, NorthEast );
    m_actions.insert( Qt::Key_D, East );
    m_actions.insert( Qt::Key_C, SouthEast );
    m_actions.insert( Qt::Key_X, South );
    m_actions.insert( Qt::Key_Z, SouthWest );
    m_actions.insert( Qt::Key_A, West );
    m_actions.insert( Qt::Key_Q, NorthWest );

    m_actions.insert( Qt::Key_S, Button );

    m_actions.insert( Qt::Key_R, Replay );
}

MWindow::~MWindow()
{
    delete m_pitch;
}

void MWindow::repeatKeyEvent()
{
    qDebug() << "repeatKeyEvent";
    m_pitch->action(m_lastAction);
    m_keyEventTimer->start();
}

void MWindow::keyPressEvent( QKeyEvent *event )
{
    if ( event->isAutoRepeat() || !m_actions.contains( event->key() ) ) {
        event->ignore();
        return;
    }

    qDebug() << "keyPressEvent";
    Action a = m_actions[ event->key() ];

    if ( m_pitch->replay_->isReplay() )
        // Not allowed to stop a replay!!!
        // pitch->replayStop();
        ;
    else {
        switch ( a )
        {
        case Button:
            m_elapsedTime.restart();
            break;
        case North:
        case NorthEast:
        case East:
        case SouthEast:
        case South:
        case SouthWest:
        case West:
        case NorthWest:
            // start a timer
            m_lastAction = a;
            m_pitch->action(a);
            m_keyEventTimer->start();
            break;
        case Replay:
            m_pitch->replayStart();
            break;
        default:
            break;
        }
    }
    event->accept();
}

void MWindow::keyReleaseEvent( QKeyEvent *event )
{
    if ( event->isAutoRepeat() || !m_actions.contains( event->key() ) ) {
        event->ignore();
        return;
    }
    qDebug() << "keyReleaseEvent";
    Action a = m_actions[ event->key() ];

    if ( a != Button ) {
        stopKeyEvent();
    } else {
        int elapsed = m_elapsedTime.elapsed();
        if ( elapsed > KLongPressValue )
            m_pitch->action(ButtonLongPress);
        else
            m_pitch->action(ButtonShortPress);
    }

    event->accept();
}

void MWindow::about()
{
    QMessageBox::about(this, tr("About Cute Football"),
                       tr("Cute Football is written by XXX"));
}
