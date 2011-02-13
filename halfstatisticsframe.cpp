#include "halfstatisticsframe.h"


HalfStatisticsFrame::HalfStatisticsFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::HalfStatisticsFrame)
{
    ui->setupUi(this);
}

HalfStatisticsFrame::~HalfStatisticsFrame()
{
    delete ui;
}
