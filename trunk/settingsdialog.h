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
    bool soundEnabled() const { return m_soundEnabled; }

signals:
    void soundChanged(bool enabled);

private slots:
    void updateSettings();

private:
    Ui::settingsDialog *ui;
    int m_gameLengthMinutes;
    bool m_soundEnabled;
};

#endif // SETTINGSDIALOG_H
