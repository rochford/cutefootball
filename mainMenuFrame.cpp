#include "mainMenuFrame.h"
#include "ui_mainMenuFrame.h"
#include "mainwindow.h"

mainMenuFrame::mainMenuFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::mainMenuFrame)
{
    ui->setupUi(this);
#if defined(Q_OS_SYMBIAN) or defined(Q_WS_SIMULATOR)
    ui->verticalSpacer_2->changeSize(1,10);
#endif
    connect(ui->actionNewGame, SIGNAL(triggered()), parent, SLOT(showTeamSelectionFrame()));
    connect(ui->actionSettings, SIGNAL(triggered()), parent, SLOT(showSettingsFrame()));
    connect(ui->actionAbout, SIGNAL(triggered()), parent, SLOT(showAboutFrame()));
    connect(ui->actionQuit, SIGNAL(triggered()), parent, SLOT(close()));
}

mainMenuFrame::~mainMenuFrame()
{
    delete ui;
}
