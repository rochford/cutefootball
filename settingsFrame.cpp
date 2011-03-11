#include "settingsFrame.h"
#include "ui_settingsFrame.h"
#include "mainwindow.h"

settingsFrame::settingsFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::settingsFrame),
    m_extraTime(NoExtraTime)
{
    ui->setupUi(this);
//    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Save)
            ->setText(tr("Save"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)
            ->setText(tr("Main Menu"));

//    connect(ui->gameLengthSlider, SIGNAL(sliderMoved(int)),ui->buttonBox->button(QDialogButtonBox::Save),SLOT(setEnabled(bool)));
//    connect(ui->checkBoxSound, SIGNAL(stateChanged(int)),ui->buttonBox->button(QDialogButtonBox::Save),SLOT(setEnabled(bool)));
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
    m_gameLengthMinutes = ui->gameLengthLcdNumber->value();
    m_soundEnabled = ui->checkBoxSound->isChecked();
//    m_extraTime = (ExtraTime) ui->extraTimeComboBox->currentIndex();
    emit soundChanged(m_soundEnabled);
}

