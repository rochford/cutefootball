#ifndef HELPFRAME_H
#define HELPFRAME_H

#include <QFrame>
#include "mainwindow.h"

namespace Ui {
    class helpFrame;
}

class helpFrame : public QFrame
{
    Q_OBJECT

public:
    explicit helpFrame(MWindow *parent = 0);
    ~helpFrame();
public slots:
    void showFrame(MWindow::Frame f);

private:
    Ui::helpFrame *uiHelp;
};

#endif // HELPFRAME_H
