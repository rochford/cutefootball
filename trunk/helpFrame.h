#ifndef HELPFRAME_H
#define HELPFRAME_H

#include <QFrame>

class MWindow;

namespace Ui {
    class helpFrame;
}

class helpFrame : public QFrame
{
    Q_OBJECT

public:
    explicit helpFrame(MWindow *parent = 0);
    ~helpFrame();

private:
    Ui::helpFrame *uiHelp;
};

#endif // HELPFRAME_H
