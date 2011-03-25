#include "halfstatisticsframe.h"

HalfStatisticsFrame::HalfStatisticsFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::HalfStatisticsFrame)
{
    ui->setupUi(this);

    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(ui->continueBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow.actionContinue,
            SLOT(trigger()));
    connect(ui->mainMenuBtn,
            SIGNAL(clicked()),
            parent->uiMainWindow.actionMainMenu,
            SLOT(trigger()));
}

HalfStatisticsFrame::~HalfStatisticsFrame()
{
    delete ui;
}

void HalfStatisticsFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "HalfStatisticsFrame::showFrame" << f;
    if ( f == MWindow::HalfTimeStatistics )
        showMaximized();
    else
        setVisible(false);
}
