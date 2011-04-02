#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QFrame>

#include "mainwindow.h"

namespace Ui {
    class aboutFrame;
}

class aboutFrame : public QFrame
{
    Q_OBJECT

public:
    explicit aboutFrame(MWindow *parent = 0);
    ~aboutFrame();

public slots:
    void showFrame(MWindow::Frame f);
private:
    Ui::aboutFrame *uiAbout;
};
#endif // ABOUTDIALOG_H
