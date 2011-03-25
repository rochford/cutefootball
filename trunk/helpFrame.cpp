
#include "helpFrame.h"
#include "ui_helpFrame.h"

helpFrame::helpFrame(MWindow *parent) :
    QFrame(parent),
    uiHelp(new Ui::helpFrame)
{
    uiHelp->setupUi(this);
    uiHelp->buttonBox->button(QDialogButtonBox::Ok)
            ->setText(tr("Main Menu"));
    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(uiHelp->buttonBox, SIGNAL(accepted()),
            parent, SLOT(hideHelpFrame()));
    connect(uiHelp->buttonBox, SIGNAL(rejected()),
            parent, SLOT(hideHelpFrame()));
}

helpFrame::~helpFrame()
{
    delete uiHelp;
}

void helpFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "helpFrame::showFrame" << f;
    if ( f == MWindow::Help )
        showMaximized();
    else
        setVisible(false);
}
