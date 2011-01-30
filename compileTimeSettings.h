#ifndef COMPILETIMESETTINGS_H
#define COMPILETIMESETTINGS_H

const QString versionNumber("version 0.1");

#include <QtGui>
// is indoor football rules
#define INDOOR

const int KHalfLength = 60*1000; // seconds
const QRgb KGoalKeeperColor = QColor(Qt::green).rgb();
const int KReplayFrameRate = 40.00; // ms
const int KOneSecondMs = 1000; // ms
const int KLongPressValue = 600; // Ms
const QPen KWhitePaintPen(QBrush(Qt::white),3);

const int KGameRefreshRate = 1000 / 24; // ms
const int KGameLength = KHalfLength*2;
const int KColumn = 5; // Left, l-Centre, centre, r-centre, Right
// goalkepper-defence-midfield-attack,attack-midfield-defence-goalkepper
const int KRow = 8;

const int KPlayerDefaultSpeed = 3;

// scale factor for players & ball
const qreal KScaleFactor = 1.0;

const QColor KCuteFootballMaskColor(103,123,9);

#endif // COMPILETIMESETTINGS_H
