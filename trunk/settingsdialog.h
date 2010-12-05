#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class settingsDialog;
}

class settingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDialog(QWidget *parent = 0);
    ~settingsDialog();
    int gameLengthMinutes() const { return m_gameLengthMinutes; }

private slots:
    void updateSettings();

private:
    Ui::settingsDialog *ui;
    int m_gameLengthMinutes;
};

#endif // SETTINGSDIALOG_H
