#ifndef INPUTSETTINGSFRAME_H
#define INPUTSETTINGSFRAME_H

#include <QFrame>

class MWindow;

namespace Ui {
    class inputSettingsFrame;
}

class inputSettingsFrame : public QFrame
{
    Q_OBJECT

public:
    explicit inputSettingsFrame(MWindow *parent = 0);
    ~inputSettingsFrame();

private:
    Ui::inputSettingsFrame *ui;
};

#endif // INPUTSETTINGSFRAME_H
