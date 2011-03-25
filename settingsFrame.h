#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QFrame>
#include "mainwindow.h"

namespace Ui {
    class settingsFrame;
}

class settingsFrame : public QFrame
{
    Q_OBJECT

public:

    enum ExtraTime {
        NoExtraTime,
        ExtraTimeAndPenalties,
        GoldenGoal
    };
    explicit settingsFrame(MWindow *parent = 0);
    ~settingsFrame();
    int gameLengthMinutes() const { return m_gameLengthMinutes; }
    bool soundEnabled() const { return m_soundEnabled; }
    ExtraTime extraTimeAllowed() const { return m_extraTime; }
signals:
    void soundChanged(bool enabled);

public slots:
    void showFrame(MWindow::Frame f);

private slots:
    void updateSettings();

private:
    Ui::settingsFrame *ui;
    int m_gameLengthMinutes;
    bool m_soundEnabled;
    ExtraTime m_extraTime;
};

#endif // SETTINGSDIALOG_H
