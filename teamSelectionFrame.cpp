
#include "teamSelectionFrame.h"
#include "ui_teamSelectionFrame.h"

#include "team.h"
#include "pitch.h"

TeamSelectionFrame::TeamSelectionFrame(MWindow *parent) :
    QFrame(parent),
    m_parent(parent),
    ui(new Ui::teamSelectionFrame)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)
            ->setText(tr("Play"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)
            ->setText(tr("Main Menu"));

    foreach( Team* t, m_parent->pitch()->teams()) {
        QString flagFile(QString(":/images/flags/") + t->fullName() + QString(".png"));
        ui->m_homeTeamComboBox->addItem(QIcon(QPixmap(flagFile)),t->briefName());
        ui->m_awayTeamComboBox->addItem(QIcon(QPixmap(flagFile)),t->briefName());
    }

    ui->teamSelectionErrorLabel->setVisible(false);
    ui->m_homeTeamComboBox->setCurrentIndex(0);
    updateHomeTeamDetails(0);
    ui->m_awayTeamComboBox->setCurrentIndex(1);
    updateAwayTeamDetails(1);

    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(ui->buttonBox, SIGNAL(accepted()),
            parent, SLOT(showGraphicsViewFrame()));
    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(startGame()));
    connect(ui->buttonBox, SIGNAL(rejected()),
            parent, SLOT(showMainMenuFrame()));

    connect(ui->m_homeTeamComboBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(checkSelectedTeams(const QString &)));
    connect(ui->m_awayTeamComboBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(checkSelectedTeams(const QString &)));

    connect(ui->m_homeTeamComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateHomeTeamDetails(int)));
    connect(ui->m_awayTeamComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateAwayTeamDetails(int)));
}

TeamSelectionFrame::~TeamSelectionFrame()
{
    delete ui;
}

void TeamSelectionFrame::updateHomeTeamDetails(int index)
{
    Team* t = m_parent->pitch()->teams().at(index);
    ui->m_homeTeamName->setText(t->fullName());
    ui->m_homeTeamRank->setText(tr("Rank: %1").arg(t->ranking()));
}

void TeamSelectionFrame::updateAwayTeamDetails(int index)
{
    Team* t = m_parent->pitch()->teams().at(index);
    ui->m_awayTeamName->setText(t->fullName());
    ui->m_awayTeamRank->setText(tr("Rank: %1").arg(t->ranking()));
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

void TeamSelectionFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "TeamSelectionFrame::showFrame" << f;
    if ( f == MWindow::TeamSelection )
        showMaximized();
    else
        setVisible(false);
}
