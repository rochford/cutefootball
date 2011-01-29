#include "settingsFrame.h"
#include "ui_settingsFrame.h"
#include "mainwindow.h"

settingsFrame::settingsFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::settingsFrame)
{
    ui->setupUi(this);
    m_gameLengthMinutes = ui->spinBox->value();
    m_soundEnabled = ui->checkBox->isChecked();
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(updateSettings()));
    connect(ui->buttonBox, SIGNAL(accepted()), parent, SLOT(hideSettingsFrame()));
    connect(ui->buttonBox, SIGNAL(rejected()), parent, SLOT(hideSettingsFrame()));
}

settingsFrame::~settingsFrame()
{
    delete ui;
}

void settingsFrame::updateSettings()
{
    m_gameLengthMinutes = ui->spinBox->value();
    m_soundEnabled = ui->checkBox->isChecked();
    emit soundChanged(m_soundEnabled);
}

