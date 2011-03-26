#include "mainMenuFrame.h"
#include "ui_mainMenuFrame.h"
#include "version.h"

mainMenuFrame::mainMenuFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::mainMenuFrame)
{
    ui->setupUi(this);
    ui->versionLabel->setText(versionNumber.arg(KBuildNumber));
    ui->m_settingsBtn->setVisible(false);

    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(ui->actionNewGame, SIGNAL(triggered()),
            parent, SLOT(showTeamSelectionFrame()));
    connect(ui->actionSettings, SIGNAL(triggered()),
            parent, SLOT(showSettingsFrame()));
    connect(ui->actionInputSettings, SIGNAL(triggered()),
            parent, SLOT(showInputSettingsFrame()));
    connect(ui->actionHelp, SIGNAL(triggered()),
            parent,SLOT(showHelpFrame()));
    connect(ui->actionQuit, SIGNAL(triggered()),
            parent, SLOT(close()));
}

mainMenuFrame::~mainMenuFrame()
{
    delete ui;
}

void mainMenuFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "mainMenuFrame::showFrame" << f;
    if ( f == MWindow::MainMenu ) {
        ui->m_newGameBtn->setFocus();
        showMaximized();
    } else
        setVisible(false);
}

