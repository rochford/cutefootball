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
    void setHomeTeamFlag(QString flag) { ui->homeFlag->setPixmap(QPixmap(flag).scaled(40,30));}
    void setAwayTeamFlag(QString flag) { ui->awayFlag->setPixmap(QPixmap(flag).scaled(40,30));}

public slots:
    void showFrame(MWindow::Frame f);

private:
    Ui::InGameMenuFrame *ui;
    Pitch& m_pitch;
};

#endif // INGAMEMENUFRAME_H
