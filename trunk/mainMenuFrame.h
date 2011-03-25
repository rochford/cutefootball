#ifndef MAINMENUDIALOG_H
#define MAINMENUDIALOG_H

#include <QFrame>

#include "mainwindow.h"

namespace Ui {
    class mainMenuFrame;
}

class mainMenuFrame : public QFrame
{
    Q_OBJECT

public:
    explicit mainMenuFrame(MWindow *parent = 0);
    ~mainMenuFrame();
public slots:
    void showFrame(MWindow::Frame f);

private:
    Ui::mainMenuFrame *ui;
};
#endif // MAINMENUDIALOG_H
