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
#include "teamSelectionFrame.h"
#include "ui_teamSelectionFrame.h"

#include "team.h"
#include "pitch.h"

TeamSelectionFrame::TeamSelectionFrame(MWindow *parent) :
    QFrame(parent),
    m_parent(parent),
    ui(new Ui::teamSelectionFrame),
    m_selection(PlayerTeamOnly)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)
            ->setText(tr("Continue"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)
            ->setText(tr("Main Menu"));

    foreach( Team* t, m_parent->pitch()->teams()) {
        ui->m_teamComboBox->addItem(QIcon(QPixmap(t->flag())),t->briefName());
    }

    ui->teamSelectionErrorLabel->setVisible(false);
    ui->m_teamComboBox->setCurrentIndex(0);
    updateTeamDetails(0);

    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(ui->buttonBox, SIGNAL(rejected()),
            parent, SLOT(showMainMenuFrame()));
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(setTeam()));

    connect(ui->m_teamComboBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(checkSelectedTeams(const QString &)));
    connect(ui->m_teamComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateTeamDetails(int)));
}

TeamSelectionFrame::~TeamSelectionFrame()
{
    delete ui;
}

void TeamSelectionFrame::setTeam()
{
    //
    switch(m_selection) {
    case ComputerTeamOnly:
        m_parent->setComputerTeam(ui->m_teamComboBox->currentIndex());
        break;
    case PlayerAndComputerTeam:
    case PlayerTeamOnly:
        m_parent->setPlayerTeam(ui->m_teamComboBox->currentIndex());
        break;
    default:
        break;
    }
}

void TeamSelectionFrame::updateTeamDetails(int index)
{
    Team* t = m_parent->pitch()->teams().at(index);
    ui->m_teamName->setText(t->localisedName());
    ui->m_teamRank->setText(tr("Rank: %1").arg(t->ranking()));
}

void TeamSelectionFrame::checkSelectedTeams(const QString &)
{
    if ( ui->m_teamComboBox->currentIndex() == m_parent->playerTeam()
        && m_selection == TeamSelectionFrame::ComputerTeamOnly ) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->teamSelectionErrorLabel->setVisible(true);
    } else {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->teamSelectionErrorLabel->setVisible(false);
    }
}

void TeamSelectionFrame::startGame()
{
    if (m_selection == PlayerTeamOnly)
        m_parent->setComputerTeam(0); // TODO pick a random team for cup competition
    m_parent->newGame();
    m_parent->showGraphicsViewFrame();
}

void TeamSelectionFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "TeamSelectionFrame::showFrame" << f;
    if ( f == MWindow::TeamSelection ) {
        ui->m_teamComboBox->setCurrentIndex(0);
        updateTeamDetails(0);

        ui->buttonBox->button(QDialogButtonBox::Ok)->setFocus();
        showMaximized();
    } else
        setVisible(false);
}

void TeamSelectionFrame::setTeamSelectionState(TeamSelectionState state)
{
        m_selection = state;
        if (state == TeamSelectionFrame::PlayerTeamOnly ) {
            ui->m_teamToSelect->setText(tr("Select Player team:"));
            ui->buttonBox->button(QDialogButtonBox::Ok)
                    ->setText(tr("Play"));

            disconnect(ui->buttonBox, SIGNAL(accepted()),
                    m_parent, SLOT(showComputerTeamSelection()));
            connect(ui->buttonBox, SIGNAL(accepted()),
                    this, SLOT(startGame()));
         } else if (state == TeamSelectionFrame::PlayerAndComputerTeam) {
            ui->m_teamToSelect->setText(tr("Select Player team:"));
            ui->buttonBox->button(QDialogButtonBox::Ok)
                    ->setText(tr("Continue"));

            disconnect(ui->buttonBox, SIGNAL(accepted()),
                    this, SLOT(startGame()));
            connect(ui->buttonBox, SIGNAL(accepted()),
                    m_parent, SLOT(showComputerTeamSelection()));
        }  else if (state == TeamSelectionFrame::ComputerTeamOnly) {
            ui->m_teamToSelect->setText(tr("Select Computer team:"));
            ui->buttonBox->button(QDialogButtonBox::Ok)
                    ->setText(tr("Play"));

             disconnect(ui->buttonBox, SIGNAL(accepted()),
                 m_parent, SLOT(showComputerTeamSelection()));
             connect(ui->buttonBox, SIGNAL(accepted()),
                 this, SLOT(startGame()));
     }
}

