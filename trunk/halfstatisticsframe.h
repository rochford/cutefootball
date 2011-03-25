#ifndef HALFSTATISTICSFRAME_H
#define HALFSTATISTICSFRAME_H

#include <QFrame>

#include "ui_halfstatisticsframe.h"

#include "mainwindow.h"

class HalfStatisticsFrame : public QFrame
{
    Q_OBJECT

public:
    explicit HalfStatisticsFrame(MWindow *parent);
    ~HalfStatisticsFrame();
    void setHalfName(QString s) { ui->m_halfNameLabel->setText(s); }
    void setHomeTeamName(QString s) { ui->m_homeTeamName->setText(s); }
    void setAwayTeamName(QString s) { ui->m_awayTeamName->setText(s); }
    void setHomeTeamGoals(int n) { ui->m_homeGoals->setText(QString::number(n)); }
    void setAwayTeamGoals(int n) { ui->m_awayGoals->setText(QString::number(n)); }
    void setHomeTeamShots(int n) { ui->m_homeShots->setText(QString::number(n)); }
    void setAwayTeamShots(int n) { ui->m_awayShots->setText(QString::number(n)); }
    void setHomeTeamFlag(QString flag) { ui->homeFlag->setPixmap(flag);}
    void setAwayTeamFlag(QString flag) { ui->awayFlag->setPixmap(flag);}
public slots:
    void showFrame(MWindow::Frame f);

private:
    Ui::HalfStatisticsFrame *ui;
};

#endif // HALFSTATISTICSFRAME_H
