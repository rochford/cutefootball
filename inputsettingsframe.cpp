

#include "inputsettingsframe.h"
#include "ui_inputSettingsFrame.h"

inputSettingsFrame::inputSettingsFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::inputSettingsFrame)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)
            ->setText(tr("Main Menu"));
    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(ui->buttonBox, SIGNAL(accepted()), parent, SLOT(hideHelpFrame()));
    connect(ui->buttonBox, SIGNAL(rejected()), parent, SLOT(hideHelpFrame()));
}

inputSettingsFrame::~inputSettingsFrame()
{
    delete ui;
}

void inputSettingsFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "inputSettingsFrame::showFrame " << f;
    if ( f == MWindow::InputSettings )
        showMaximized();
    else
        setVisible(false);
}
