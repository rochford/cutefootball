#ifndef TEAMSELECTIONFRAME_H
#define TEAMSELECTIONFRAME_H

#include <QFrame>
#include "mainwindow.h"

namespace Ui {
    class teamSelectionFrame;
}

class TeamSelectionFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TeamSelectionFrame(MWindow *parent = 0);
    ~TeamSelectionFrame();

public slots:
    void showFrame(MWindow::Frame f);

private slots:
    void checkSelectedTeams(const QString &);
    void updateHomeTeamDetails(int index);
    void updateAwayTeamDetails(int index);
    void startGame();

private:
    MWindow* m_parent;
    Ui::teamSelectionFrame *ui;
};

#endif // TEAMSELECTIONFRAME_H
