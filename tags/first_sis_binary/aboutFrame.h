#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QFrame>

class MWindow;

namespace Ui {
    class aboutFrame;
}

class aboutFrame : public QFrame
{
    Q_OBJECT

public:
    explicit aboutFrame(MWindow *parent = 0);
    ~aboutFrame();

private:
    Ui::aboutFrame *uiAbout;
};
#endif // ABOUTDIALOG_H
