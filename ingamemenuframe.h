#ifndef INGAMEMENUFRAME_H
#define INGAMEMENUFRAME_H

#include <QFrame>

#include "ui_ingamemenuframe.h"
#include "mainwindow.h"
#include "pitch.h"

class InGameMenuFrame : public QFrame
{
    Q_OBJECT

public:
    explicit InGameMenuFrame(MWindow *parent = 0);
    ~InGameMenuFrame();
    void setHomeTeamGoals(int n) { ui->homeGoals->setText(QString::number(n)); }
    void setAwayTeamGoals(int n) { ui->awayGoals->setText(QString::number(n)); }
    void setHomeTeamFlag(QString flag) { ui->homeFlag->setPixmap(flag);}
    void setAwayTeamFlag(QString flag) { ui->awayFlag->setPixmap(flag);}

public slots:
    void showFrame(MWindow::Frame f);

private:
    Ui::InGameMenuFrame *ui;
    Pitch& m_pitch;
};

#endif // INGAMEMENUFRAME_H
