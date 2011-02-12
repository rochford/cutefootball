#include "settingsFrame.h"
#include "ui_settingsFrame.h"
#include "mainwindow.h"

settingsFrame::settingsFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::settingsFrame),
    m_extraTime(false)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(updateSettings()));
    connect(ui->buttonBox, SIGNAL(accepted()), parent, SLOT(hideSettingsFrame()));
    connect(ui->buttonBox, SIGNAL(rejected()), parent, SLOT(hideSettingsFrame()));
    updateSettings();
}

settingsFrame::~settingsFrame()
{
    delete ui;
}

void settingsFrame::updateSettings()
{
    m_gameLengthMinutes = ui->spinBox->value();
    m_soundEnabled = ui->checkBoxSound->isChecked();
    m_extraTime = ui->checkBoxExtraTime->isChecked();
    emit soundChanged(m_soundEnabled);
}

