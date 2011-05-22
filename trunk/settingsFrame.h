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
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QFrame>
#include "mainwindow.h"

namespace Ui {
    class settingsFrame;
}

class settingsFrame : public QFrame
{
    Q_OBJECT

public:

    enum InputMethod {
        Keyboard,
        Mouse,
        Touch
    };

    enum ExtraTime {
        NoExtraTime,
        ExtraTimeAndPenalties,
        GoldenGoal
    };
    explicit settingsFrame(MWindow *parent = 0);
    ~settingsFrame();
    inline int gameLengthMinutes() const { return m_gameLengthMinutes; }
    inline bool soundEnabled() const { return m_soundEnabled; }
    inline ExtraTime extraTimeAllowed() const { return m_extraTime; }
    inline InputMethod inputMethod() const { return m_inputMethod; }
signals:
    void soundChanged(bool enabled);
    void inputMethodChanged(settingsFrame::InputMethod method);

public slots:
    void showFrame(MWindow::Frame f);

private slots:
    void updateSettings();

private:
    Ui::settingsFrame *ui;
    int m_gameLengthMinutes;
    bool m_soundEnabled;
    InputMethod m_inputMethod;
    ExtraTime m_extraTime;
};

#endif // SETTINGSDIALOG_H
