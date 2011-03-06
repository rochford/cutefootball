#include "screengraphics.h"

#include "pitch.h"
#include "team.h"

ScreenGraphics::ScreenGraphics(Pitch *p)
  :  QFrame(NULL),
    m_pitch(p),
    ui(new Ui::ScreenGraphicsFrame)
{
    ui->setupUi(this);
}

ScreenGraphics::~ScreenGraphics()
{
//    delete m_scoreText;
}

void ScreenGraphics::update(QString s)
{
    resize(160,50);
    ui->scoreTextLabel->setVisible(true);

    ui->scoreTextLabel->setText(s);

    ui->SGFhomeTeamName->setText(m_pitch->homeTeam()->briefName() + " " + QString::number(m_pitch->homeTeam()->m_goals));
//    ui->SGFhomeGoals->setText("" /* QString::number(m_pitch->homeTeam()->m_goals) */);
    ui->SGFawayTeamName->setText(m_pitch->awayTeam()->briefName()+ " " + QString::number(m_pitch->awayTeam()->m_goals));
//    ui->SGFawayGoals->setText("" /* + QString::number(m_pitch->awayTeam()->m_goals) */);
}

void ScreenGraphics::setTeams(Team* home, Team* away)
{
    ui->SGFhomeTeamFlag->setPixmap(QPixmap(QString(":/images/flags/")+home->fullName()+".png").scaled(20,15));
    ui->SGFhomeTeamName->setText(home->briefName());
    ui->SGFawayTeamFlag->setPixmap(QPixmap(QString(":/images/flags/")+away->fullName()+".png").scaled(20,15));
    ui->SGFawayTeamName->setText(away->briefName());
}

