#include <QDebug>

#include "mainwindow.h"
#include "pitch.h"

MWindow::MWindow(QWidget *parent)
    : QMainWindow(parent),
    keyEventTimer(new QTimer),
    buttonPressed_(false)
{
    QRectF footballGround(0,0,400,600);
    pitch = new Pitch(footballGround);

    createActions();
    createKeyboardActions();

    connect(keyEventTimer, SIGNAL(timeout()), this, SLOT(repeatKeyEvent()));

    setCentralWidget(pitch->view);
    pitch->view->show();
    pitch->newGame();
}

void MWindow::startedGame()
{
    pauseGameAction->setEnabled(true);
    continueGameAction->setEnabled(false);
}
void MWindow::pausedGame()
{
    pauseGameAction->setEnabled(false);
    continueGameAction->setEnabled(true);
}
void MWindow::continueGame()
{
    pauseGameAction->setEnabled(true);
    continueGameAction->setEnabled(false);
}

void MWindow::createActions()
{
    newGameAction = new QAction(QString("New Game"), this);
    addAction(newGameAction);

    pauseGameAction = new QAction(QString("Pause Game"), this);
    pauseGameAction->setEnabled(false);
    addAction(pauseGameAction);

    continueGameAction = new QAction(QString("Continue Game"), this);
    continueGameAction->setEnabled(false);
    addAction(continueGameAction);

    connect(newGameAction, SIGNAL(triggered()), pitch, SLOT(newGame()));
    connect(newGameAction, SIGNAL(triggered()), this, SLOT(startedGame()));
    connect(pauseGameAction, SIGNAL(triggered()), pitch, SLOT(pausedGame()));
    connect(pauseGameAction, SIGNAL(triggered()), this, SLOT(pausedGame()));
    connect(continueGameAction, SIGNAL(triggered()), pitch, SLOT(continueGame()));
    connect(continueGameAction, SIGNAL(triggered()), this, SLOT(continueGame()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newGameAction);
    fileMenu->addAction(pauseGameAction);
    fileMenu->addAction(continueGameAction);

    helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MWindow::stopKeyEvent()
{
    if (keyEventTimer->isActive())
        keyEventTimer->stop();
}

void MWindow::createKeyboardActions()
{
    actions.insert( Qt::Key_W, North );
    actions.insert( Qt::Key_E, NorthEast );
    actions.insert( Qt::Key_D, East );
    actions.insert( Qt::Key_C, SouthEast );
    actions.insert( Qt::Key_X, South );
    actions.insert( Qt::Key_Z, SouthWest );
    actions.insert( Qt::Key_A, West );
    actions.insert( Qt::Key_Q, NorthWest );
    actions.insert( Qt::Key_S, Button );

    actions.insert( Qt::Key_R, Replay );
}

MWindow::~MWindow()
{
    delete pitch;
}

void MWindow::repeatKeyEvent()
{
    pitch->action(lastAction);
    keyEventTimer->start(KGameRefreshRate);
}

void MWindow::keyPressEvent( QKeyEvent *event )
{
    if ( event->isAutoRepeat() || !actions.contains( event->key() ) ) {
        event->ignore();
        return;
    }

    qDebug() << "keyPressEvent start";

    Action a = actions[ event->key() ];

    switch ( a )
    {
    case Button:
        pitch->action(a);
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
        lastAction = a;
        pitch->action(a);
        keyEventTimer->start(KGameRefreshRate);
        break;
    case Replay:
        pitch->replayStart();
        break;
    default:
        break;
    }
    event->accept();
    qDebug() << "keyPressEvent end";
}

void MWindow::keyReleaseEvent( QKeyEvent *event )
{
    if ( event->isAutoRepeat() || !actions.contains( event->key() ) ) {
        event->ignore();
        return;
    }

    Action a = actions[ event->key() ];

    if ( a != Button )
        stopKeyEvent();

    event->accept();
}
