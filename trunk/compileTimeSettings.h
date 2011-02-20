#ifndef COMPILETIMESETTINGS_H
#define COMPILETIMESETTINGS_H

#include <QtGui>

const int KMajorVersion = 0;
const int KMinorVersion = 5;
const QString versionNumber(QObject::tr("version %1.%2"));

const int KHalfLength = 60*1000; // seconds
const QRgb KGoalKeeperShirtColor = QColor(Qt::green).rgb();
const QRgb KGoalKeeperShortColor = QColor(Qt::black).rgb();
const int KOneSecondMs = 1000; // ms
const int KLongPressValue = 600; // Ms
const QPen KWhitePaintPen(QBrush(Qt::white),3);

const int KGameRefreshRate = 1000 / 20; // ms
const int KColumn = 5; // Left, l-Centre, centre, r-centre, Right
// goalkepper-defence-midfield-attack,attack-midfield-defence-goalkepper
const int KRow = 8;

const int KPlayerDefaultSpeed = 3;

// scale factor for players & ball
const qreal KScaleFactor = 1.0;

const QColor KCuteFootballMaskColor(103,123,9);

#endif // COMPILETIMESETTINGS_H
