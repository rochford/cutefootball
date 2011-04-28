/*
 * Copyright 2010,2011 Timothy Rochford
 *
 *    This file is part of CuteFootball.
 *
 *    CuteFootball is free software: you can redistribute it and/or modify
 *    it under the terms of the Lesser GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    CuteFootball is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Lesser GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
//    void setHalfName(QString s) { ui->m_halfNameLabel->setText(s); }
    void setHomeTeamName(QString s) { ui->m_homeTeamName->setText(s); }
    void setAwayTeamName(QString s) { ui->m_awayTeamName->setText(s); }
    void setHomeTeamGoals(int n) { ui->m_homeGoals->setText(QString::number(n)); }
    void setAwayTeamGoals(int n) { ui->m_awayGoals->setText(QString::number(n)); }
    void setHomeTeamShots(int n) { ui->m_homeShots->setText(QString::number(n)); }
    void setAwayTeamShots(int n) { ui->m_awayShots->setText(QString::number(n)); }
    void setHomeTeamFlag(QString flag) { ui->homeFlag->setPixmap(QPixmap(flag).scaled(40,30));}
    void setAwayTeamFlag(QString flag) { ui->awayFlag->setPixmap(QPixmap(flag).scaled(40,30));}
public slots:
    void showFrame(MWindow::Frame f);

private:
    Ui::HalfStatisticsFrame *ui;
};

#endif // HALFSTATISTICSFRAME_H
