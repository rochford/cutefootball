/*
 * Copyright 2010,2011 Timothy Rochford
 *
 *    This file is part of CuteFootball.
 *
 *    CuteFootball is free software: you can redistribute it and/or modify
 *    it under the terms of the Lesser GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    CuteFootball is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Lesser  GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef COMPILETIMESETTINGS_H
#define COMPILETIMESETTINGS_H

#include <QtGui>

const int KHalfLength = 60*1000; // seconds
//const QRgb KDefaultHairColor = QColor(195,74,74).rgb();
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
