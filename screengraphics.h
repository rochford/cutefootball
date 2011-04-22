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
#ifndef SCREENGRAPHICS_H
#define SCREENGRAPHICS_H

#include <QFrame>

#include "ui_screengraphicsframe.h"

namespace Ui {
    class ScreenGraphicsFrame;
}


class Team;
class Pitch;

class ScreenGraphics : public QFrame
{
public:
    enum ScreenGraphicsType {
        ScoreText,
        GoalScored,
        Foul
    };

    ScreenGraphics(Pitch *p);
    virtual ~ScreenGraphics();
    void update(QString s);
    void setTeams(Team* home, Team* away);

public slots:
    void setGraphics(ScreenGraphicsType type);

private:
    Ui::ScreenGraphicsFrame *ui;
    Pitch *m_pitch;
    ScreenGraphicsType m_type;
};

#endif // SCREENGRAPHICS_H
