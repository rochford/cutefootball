#include "halfstatisticsframe.h"
#include "mainwindow.h"

HalfStatisticsFrame::HalfStatisticsFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::HalfStatisticsFrame)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()),
            parent, SLOT(hideStatisticsFrame()));
    connect(ui->buttonBox, SIGNAL(rejected()),
            parent, SLOT(hideStatisticsFrame()));
}

HalfStatisticsFrame::~HalfStatisticsFrame()
{
    delete ui;
}
