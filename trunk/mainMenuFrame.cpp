#include "mainMenuFrame.h"
#include "ui_mainMenuFrame.h"
#include "mainwindow.h"
#include "version.h"

mainMenuFrame::mainMenuFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::mainMenuFrame)
{
    ui->setupUi(this);
    ui->versionLabel->setText(versionNumber.arg(KMajorVersion).arg(KMinorVersion));

    connect(ui->actionNewGame, SIGNAL(triggered()),
            parent, SLOT(showTeamSelectionFrame()));
    connect(ui->actionSettings, SIGNAL(triggered()),
            parent, SLOT(showSettingsFrame()));
    connect(ui->actionInputSettings, SIGNAL(triggered()),
            parent, SLOT(showInputSettingsFrame()));
//    connect(ui->actionAbout, SIGNAL(triggered()),
//            parent, SLOT(showAboutFrame()));
    connect(ui->actionHelp, SIGNAL(triggered()),
            parent,SLOT(showHelpFrame()));
    connect(ui->actionQuit, SIGNAL(triggered()),
            parent, SLOT(close()));
}

mainMenuFrame::~mainMenuFrame()
{
    delete ui;
}

