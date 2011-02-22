
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

    foreach( Team* t, m_parent->pitch()->teams()) {
        QString flagFile(QString(":/images/flags/") + t->fullName() + QString(".png"));
        ui->m_homeTeamComboBox->addItem(QIcon(QPixmap(flagFile)),t->briefName());
        ui->m_awayTeamComboBox->addItem(QIcon(QPixmap(flagFile)),t->briefName());
    }

    ui->teamSelectionErrorLabel->setVisible(false);
    ui->m_homeTeamComboBox->setCurrentIndex(0);
    ui->m_awayTeamComboBox->setCurrentIndex(12);

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
    if ( ui->m_homeTeamComboBox->currentText() == ui->m_awayTeamComboBox->currentText() ) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->teamSelectionErrorLabel->setVisible(true);
    } else {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->teamSelectionErrorLabel->setVisible(false);
    }
}

void TeamSelectionFrame::startGame()
{
    m_parent->newGame(ui->m_homeTeamComboBox->currentIndex(), ui->m_awayTeamComboBox->currentIndex());
}
