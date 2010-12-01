#ifndef COMPILETIMESETTINGS_H
#define COMPILETIMESETTINGS_H

#include <QtGui>

// #define REFEREE_USED
// #define SHOW_PLAYER_NAMES_AS_TOOLTIPS

const int KHalfLength = 60*1000; // seconds
const QRgb KGoalKeeperColor = QColor(Qt::green).rgb();
const int KReplayFrameRate = 40.00; // ms
const int KOneSecondMs = 1000; // ms
const int KLongPressValue = 800; // Ms
const QPen KWhitePaintPen(QBrush(Qt::white),3);

const int KGameRefreshRate = 1000 / 24; // ms
const int KGameLength = KHalfLength*2;
const int KColumn = 5; // Left, l-Centre, centre, r-centre, Right
// goalkepper-defence-midfield-attack,attack-midfield-defence-goalkepper
const int KRow = 8;

const int KPlayerDefaultSpeed = 2;

// scale factor for players & ball
const qreal KScaleFactor = 1.6;

const QColor KHomeTeamColor = Qt::blue;
const QColor KAwayTeamColor = Qt::yellow;
const QColor KCuteFootballMaskColor(125,150,0);

#endif // COMPILETIMESETTINGS_H
