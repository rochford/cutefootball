#include "ingamemenuframe.h"

InGameMenuFrame::InGameMenuFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::InGameMenuFrame)
{
    ui->setupUi(this);
    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));
    connect(ui->continueBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow.actionContinue,
            SLOT(trigger()));
    connect(ui->mainMenuBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow.actionMainMenu,
            SLOT(trigger()));
}

InGameMenuFrame::~InGameMenuFrame()
{
    delete ui;
}

void InGameMenuFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "InGameMenuFrame::showFrame " << f;
    if ( f == MWindow::InGameMenu ) {
//        setHomeTeamGoals(m_pitch->homeTeam()->m_goals);
//        setAwayTeamGoals(m_pitch->awayTeam()->m_goals);
//        setHomeTeamFlag(QString(":/images/flags/") + m_pitch->homeTeam()->fullName() + ".png");
//        setAwayTeamFlag(QString(":/images/flags/") + m_pitch->awayTeam()->fullName() + ".png");
        showMaximized();
    } else
        setVisible(false);
}
