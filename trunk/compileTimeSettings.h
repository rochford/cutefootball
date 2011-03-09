#ifndef COMPILETIMESETTINGS_H
#define COMPILETIMESETTINGS_H

#include <QtGui>

const int KHalfLength = 60*1000; // seconds
const QRgb KGoalKeeperShirtColor = QColor(Qt::green).rgb();
const QRgb KGoalKeeperShortColor = QColor(Qt::black).rgb();
const int KOneSecondMs = 1000; // ms
const int KLongPressValue = 400; // Ms

const QFont KPlayerToolTipFont("Times",7, QFont::Bold);
const QPen KPlayerNameFocused(QBrush(Qt::yellow),4);
const QPen KPlayerNameUnfocused(QBrush(Qt::white),4);

const QPen KWhitePaintPen(QBrush(Qt::white),3);
const QBrush KFocusPlayerBrush(Qt::white, Qt::Dense3Pattern);

const int KGameRefreshRate = 1000 / 25; // ms
const int KKeyPressRefreshRate = 1000 / 25; // ms

const int KColumn = 5; // Left, l-Centre, centre, r-centre, Right
// goalkepper-defence-midfield-attack,attack-midfield-defence-goalkepper
const int KRow = 8;

// scale factor for players & ball
//const qreal KScaleFactor = 1.0;

const QColor KCuteFootballMaskColor(103,123,9);

// distance opposing team needs to be away from ball when foul
const qreal KFoulDistance = 25.0;

#endif // COMPILETIMESETTINGS_H
