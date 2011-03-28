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
#include "settingsFrame.h"
#include "ui_settingsFrame.h"
#include "mainwindow.h"

settingsFrame::settingsFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::settingsFrame),
    m_extraTime(NoExtraTime)
{
    ui->setupUi(this);
//    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Save)
            ->setText(tr("Save"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)
            ->setText(tr("Main Menu"));
    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

//    connect(ui->gameLengthSlider, SIGNAL(sliderMoved(int)),ui->buttonBox->button(QDialogButtonBox::Save),SLOT(setEnabled(bool)));
//    connect(ui->checkBoxSound, SIGNAL(stateChanged(int)),ui->buttonBox->button(QDialogButtonBox::Save),SLOT(setEnabled(bool)));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(updateSettings()));
    connect(ui->buttonBox, SIGNAL(accepted()), parent, SLOT(hideSettingsFrame()));
    connect(ui->buttonBox, SIGNAL(rejected()), parent, SLOT(hideSettingsFrame()));
    updateSettings();
}

settingsFrame::~settingsFrame()
{
    delete ui;
}

void settingsFrame::updateSettings()
{
    m_gameLengthMinutes = ui->gameLengthLcdNumber->value();
    m_soundEnabled = ui->checkBoxSound->isChecked();
//    m_extraTime = (ExtraTime) ui->extraTimeComboBox->currentIndex();
    emit soundChanged(m_soundEnabled);
}

void settingsFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "settingsFrame::showFrame" << f;
    if ( f == MWindow::Settings )
        showMaximized();
    else
        setVisible(false);
}
