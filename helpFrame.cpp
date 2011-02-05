
#include "helpFrame.h"
#include "ui_helpFrame.h"
#include "mainwindow.h"

helpFrame::helpFrame(MWindow *parent) :
    QFrame(parent),
    uiHelp(new Ui::helpFrame)
{
    uiHelp->setupUi(this);
    connect(uiHelp->buttonBox, SIGNAL(accepted()), parent, SLOT(hideHelpFrame()));
    connect(uiHelp->buttonBox, SIGNAL(rejected()), parent, SLOT(hideHelpFrame()));
}

helpFrame::~helpFrame()
{
    delete uiHelp;
}
