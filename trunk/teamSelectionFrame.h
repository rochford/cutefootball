#ifndef TEAMSELECTIONFRAME_H
#define TEAMSELECTIONFRAME_H

#include <QFrame>
class MWindow;

namespace Ui {
    class teamSelectionFrame;
}

class TeamSelectionFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TeamSelectionFrame(MWindow *parent = 0);
    ~TeamSelectionFrame();
private slots:
    void checkSelectedTeams(const QString &);
    void startGame();

private:
    MWindow* m_parent;
    Ui::teamSelectionFrame *ui;
};

#endif // TEAMSELECTIONFRAME_H
