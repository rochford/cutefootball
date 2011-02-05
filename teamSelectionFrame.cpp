
#include "teamSelectionFrame.h"
#include "ui_teamSelectionFrame.h"
#include "mainwindow.h"
#include "team.h"
#include "pitch.h"

TeamSelectionFrame::TeamSelectionFrame(MWindow *parent) :
    QFrame(parent),
    m_parent(parent),
    ui(new Ui::teamSelectionFrame)
{
    ui->setupUi(this);

    QStringList teamNames;
    foreach( Team* t, m_parent->pitch()->teams())
        teamNames.append(t->name());

    ui->m_homeTeamComboBox->addItems(teamNames);
    ui->m_homeTeamComboBox->setCurrentIndex(0);
    ui->m_awayTeamComboBox->addItems(teamNames);
    ui->m_awayTeamComboBox->setCurrentIndex(1);

    connect(ui->buttonBox, SIGNAL(accepted()), parent, SLOT(hideTeamSelectionFrame()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(startGame()));
    connect(ui->buttonBox, SIGNAL(rejected()), parent, SLOT(hideTeamSelectionFrame()));

    connect(ui->m_homeTeamComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(checkSelectedTeams(const QString &)));
    connect(ui->m_awayTeamComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(checkSelectedTeams(const QString &)));
}

TeamSelectionFrame::~TeamSelectionFrame()
{
    delete ui;
}

void TeamSelectionFrame::checkSelectedTeams(const QString &)
{
    if ( ui->m_homeTeamComboBox->currentText() == ui->m_awayTeamComboBox->currentText() )
        ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(false);
    else
        ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(true);
}

void TeamSelectionFrame::startGame()
{
    m_parent->newGame(ui->m_homeTeamComboBox->currentIndex(), ui->m_awayTeamComboBox->currentIndex());
}
