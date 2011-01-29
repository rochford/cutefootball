#include "mainMenuFrame.h"
#include "ui_mainMenuFrame.h"
#include "mainwindow.h"

mainMenuFrame::mainMenuFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::mainMenuFrame)
{
    ui->setupUi(this);
    connect(ui->actionNewGame, SIGNAL(triggered()), parent, SLOT(newGame()));
    connect(ui->actionSettings, SIGNAL(triggered()), parent, SLOT(showSettingsFrame()));
    connect(ui->actionAbout, SIGNAL(triggered()), parent, SLOT(showAboutFrame()));
    connect(ui->actionQuit, SIGNAL(triggered()), parent, SLOT(close()));
}

mainMenuFrame::~mainMenuFrame()
{
    delete ui;
}
