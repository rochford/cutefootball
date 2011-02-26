

#include "inputsettingsframe.h"
#include "ui_inputSettingsFrame.h"
#include "mainwindow.h"

inputSettingsFrame::inputSettingsFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::inputSettingsFrame)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)
            ->setText(tr("Main Menu"));

    connect(ui->buttonBox, SIGNAL(accepted()), parent, SLOT(hideHelpFrame()));
    connect(ui->buttonBox, SIGNAL(rejected()), parent, SLOT(hideHelpFrame()));
}

inputSettingsFrame::~inputSettingsFrame()
{
    delete ui;
}
