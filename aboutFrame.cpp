#include "aboutFrame.h"
#include "ui_aboutFrame.h"
#include "mainwindow.h"
#include "version.h"

aboutFrame::aboutFrame(MWindow *parent) :
    QFrame(parent),
    uiAbout(new Ui::aboutFrame)
{
    uiAbout->setupUi(this);
    uiAbout->versionLabel->setText(versionNumber.arg(KMajorVersion).arg(KMinorVersion));
    uiAbout->buttonBox->button(QDialogButtonBox::Ok)
            ->setText(tr("Main Menu"));

    connect(uiAbout->buttonBox, SIGNAL(accepted()), parent, SLOT(hideAboutFrame()));
    connect(uiAbout->buttonBox, SIGNAL(rejected()), parent, SLOT(hideAboutFrame()));
}

aboutFrame::~aboutFrame()
{
    delete uiAbout;
}
