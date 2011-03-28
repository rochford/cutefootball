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

#include "helpFrame.h"
#include "ui_helpFrame.h"

helpFrame::helpFrame(MWindow *parent) :
    QFrame(parent),
    uiHelp(new Ui::helpFrame)
{
    uiHelp->setupUi(this);
    uiHelp->buttonBox->button(QDialogButtonBox::Ok)
            ->setText(tr("Main Menu"));
    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(uiHelp->buttonBox, SIGNAL(accepted()),
            parent, SLOT(hideHelpFrame()));
    connect(uiHelp->buttonBox, SIGNAL(rejected()),
            parent, SLOT(hideHelpFrame()));
}

helpFrame::~helpFrame()
{
    delete uiHelp;
}

void helpFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "helpFrame::showFrame" << f;
    if ( f == MWindow::Help )
        showMaximized();
    else
        setVisible(false);
}
