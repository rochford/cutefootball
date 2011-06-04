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

#ifndef ONSCREENBUTTONSFRAME_H
#define ONSCREENBUTTONSFRAME_H

#include <QFrame>
#include "settingsFrame.h"

namespace Ui {
    class OnScreenButtonsFrame;
}

class Pitch;

class OnScreenButtonsFrame : public QFrame
{
    Q_OBJECT

public:
    explicit OnScreenButtonsFrame(Pitch* p, QWidget *parent = 0);
    ~OnScreenButtonsFrame();
    void refresh();
public slots:
    void setInputMethod(settingsFrame::InputMethod method);

private:
    Ui::OnScreenButtonsFrame *ui;
    Pitch* m_pitch;
    settingsFrame::InputMethod m_inputMethod;
};

#endif // ONSCREENBUTTONSFRAME_H
