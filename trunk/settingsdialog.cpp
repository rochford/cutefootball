#include "settingsdialog.h"
#include "ui_settingsdialog.h"

settingsDialog::settingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDialog)
{
    ui->setupUi(this);
    m_gameLengthMinutes = ui->spinBox->value();
    m_soundEnabled = ui->checkBox->isChecked();
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(updateSettings()));
}

settingsDialog::~settingsDialog()
{
    delete ui;
}

void settingsDialog::updateSettings()
{
    m_gameLengthMinutes = ui->spinBox->value();
    m_soundEnabled = ui->checkBox->isChecked();
    emit soundChanged(m_soundEnabled);
}

