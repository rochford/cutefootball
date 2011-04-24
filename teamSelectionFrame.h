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
#ifndef TEAMSELECTIONFRAME_H
#define TEAMSELECTIONFRAME_H

#include <QFrame>
#include "mainwindow.h"

namespace Ui {
    class teamSelectionFrame;
}

class TeamSelectionFrame : public QFrame
{
    Q_OBJECT

public:
    enum TeamSelectionState {
        PlayerTeamOnly,
        PlayerAndComputerTeam,
        ComputerTeamOnly
        };

    explicit TeamSelectionFrame(MWindow *parent = 0);
    ~TeamSelectionFrame();

public slots:
    void showFrame(MWindow::Frame f);
    void setTeamSelectionState(TeamSelectionState state);

private slots:
    void checkSelectedTeams(const QString &);
    void updateTeamDetails(int index);
    void startGame();
    void setTeam();

private:
    MWindow* m_parent;
    Ui::teamSelectionFrame *ui;
    TeamSelectionState m_selection;
};

#endif // TEAMSELECTIONFRAME_H
