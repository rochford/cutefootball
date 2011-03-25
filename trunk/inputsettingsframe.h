#ifndef INPUTSETTINGSFRAME_H
#define INPUTSETTINGSFRAME_H

#include <QFrame>

#include "mainwindow.h"

namespace Ui {
    class inputSettingsFrame;
}

class inputSettingsFrame : public QFrame
{
    Q_OBJECT

public:
    explicit inputSettingsFrame(MWindow *parent = 0);
    ~inputSettingsFrame();
public slots:
    void showFrame(MWindow::Frame f);
private:
    Ui::inputSettingsFrame *ui;
};

#endif // INPUTSETTINGSFRAME_H
