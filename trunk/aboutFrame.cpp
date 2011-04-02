#include "aboutFrame.h"
#include "ui_aboutFrame.h"
#include "version.h"

aboutFrame::aboutFrame(MWindow *parent) :
    QFrame(parent),
    uiAbout(new Ui::aboutFrame)
{
    uiAbout->setupUi(this);
    uiAbout->versionLabel->setText(KVersionNumber);

    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));
    connect(uiAbout->mainMenuBtn, SIGNAL(clicked()),
            parent, SLOT(hideAboutFrame()));
}

aboutFrame::~aboutFrame()
{
    delete uiAbout;
}

void aboutFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "aboutFrame::showFrame" << f;
    if ( f == MWindow::About )
        showMaximized();
    else
        setVisible(false);
}
