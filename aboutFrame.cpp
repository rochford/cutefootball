#include "aboutFrame.h"
#include "ui_aboutFrame.h"
#include "mainwindow.h"

aboutFrame::aboutFrame(MWindow *parent) :
    QFrame(parent),
    uiAbout(new Ui::aboutFrame)
{
    uiAbout->setupUi(this);
    connect(uiAbout->buttonBox, SIGNAL(accepted()), parent, SLOT(hideAboutFrame()));
    connect(uiAbout->buttonBox, SIGNAL(rejected()), parent, SLOT(hideAboutFrame()));
}

aboutFrame::~aboutFrame()
{
    delete uiAbout;
}
