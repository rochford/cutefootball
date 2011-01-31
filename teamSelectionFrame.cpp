
#include "teamSelectionFrame.h"
#include "ui_teamSelectionFrame.h"
#include "mainwindow.h"

TeamSelectionFrame::TeamSelectionFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::teamSelectionFrame)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), parent, SLOT(hideTeamSelectionFrame()));
    connect(ui->buttonBox, SIGNAL(accepted()), parent, SLOT(newGame()));
    connect(ui->buttonBox, SIGNAL(rejected()), parent, SLOT(hideTeamSelectionFrame()));
}

TeamSelectionFrame::~TeamSelectionFrame()
{
    delete ui;
}
