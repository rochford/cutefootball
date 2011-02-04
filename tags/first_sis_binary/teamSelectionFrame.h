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
private:
    Ui::teamSelectionFrame *ui;
};

#endif // TEAMSELECTIONFRAME_H
