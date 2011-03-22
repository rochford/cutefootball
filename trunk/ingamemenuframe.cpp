#include "ingamemenuframe.h"
#include "ui_ingamemenuframe.h"
#include "mainwindow.h"

InGameMenuFrame::InGameMenuFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::InGameMenuFrame)
{
    ui->setupUi(this);
    connect(ui->continueBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow.actionContinue,
            SLOT(trigger()));
    connect(ui->exitBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow.actionQuit,
            SLOT(trigger()));
}

InGameMenuFrame::~InGameMenuFrame()
{
    delete ui;
}
