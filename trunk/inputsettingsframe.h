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
