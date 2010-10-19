#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <qmap.h>

class Pitch;

class MWindow : public QMainWindow
{
    Q_OBJECT

public:
    MWindow(QWidget *parent = 0);
    virtual ~MWindow();

    void createActions();
    void createStatusBar();

    enum Action {
        West = 0,
        NorthWest,
        NorthEast,
        SouthEast,
        SouthWest,
        East,
        North,
        South,

        Button,

        Shoot,
        Pass,
        Tackle,
        DoNothing };

private slots:
    void startedGame();
    void pausedGame();
    void continueGame();

    void repeatKeyEvent();
    void stopKeyEvent();

protected:
    virtual void keyPressEvent( QKeyEvent *event );
    virtual void keyReleaseEvent( QKeyEvent *event );

private:
    void createKeyboardActions();

private:
    Pitch *pitch;

    QMap<int,Action> actions;
    QTimer *keyEventTimer;
    Action lastAction;

    QAction* newGameAction;
    QAction* pauseGameAction;
    QAction* continueGameAction;

    QMenu* fileMenu;
    QMenu* helpMenu;

    QLabel* scoreLabel;

    bool buttonPressed_;
    };

#endif // MAINWINDOW_H
