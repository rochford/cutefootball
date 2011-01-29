#ifndef MAINMENUDIALOG_H
#define MAINMENUDIALOG_H

#include <QFrame>

class MWindow;

namespace Ui {
    class mainMenuFrame;
}

class mainMenuFrame : public QFrame
{
    Q_OBJECT

public:
    explicit mainMenuFrame(MWindow *parent = 0);
    ~mainMenuFrame();

private:
    Ui::mainMenuFrame *ui;
};
#endif // MAINMENUDIALOG_H
