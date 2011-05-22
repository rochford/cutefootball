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

#include "onscreenbuttonsframe.h"
#include "ui_onscreenbuttonsframe.h"

OnScreenButtonsFrame::OnScreenButtonsFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OnScreenButtonsFrame)
{
    ui->setupUi(this);
//    connect(ui->actionPause, SIGNAL(triggered()), m_pitch, SLOT(pause()));
}

OnScreenButtonsFrame::~OnScreenButtonsFrame()
{
    delete ui;
}

void OnScreenButtonsFrame::refresh()
{
    if (m_inputMethod != settingsFrame::Touch)
        return;

    resize(100,40);
}

void OnScreenButtonsFrame::setInputMethod(settingsFrame::InputMethod method)
{
    m_inputMethod = method;
    if (m_inputMethod != settingsFrame::Touch) {
        setEnabled(false);
        setVisible(false);
    } else {
        setEnabled(true);
        setVisible(true);
    }
}
