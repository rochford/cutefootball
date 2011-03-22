#ifndef INGAMEMENUFRAME_H
#define INGAMEMENUFRAME_H

#include <QFrame>

class MWindow;

namespace Ui {
    class InGameMenuFrame;
}

class InGameMenuFrame : public QFrame
{
    Q_OBJECT

public:
    explicit InGameMenuFrame(MWindow *parent = 0);
    ~InGameMenuFrame();

private:
    Ui::InGameMenuFrame *ui;
};

#endif // INGAMEMENUFRAME_H
