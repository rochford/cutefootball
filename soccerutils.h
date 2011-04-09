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
 *    Lesser GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef SOCCERUTILS_H
#define SOCCERUTILS_H

#include "mainwindow.h"
#include "Player.h"

Player::Role mapPositionStringToRole(QString positionString);
QColor skinColorMapping(QString col);
int calculateTackleRotationFromLastAction(MWindow::Action lastAction);
MWindow::Action calculateTackleActionFromLastAction(MWindow::Action lastAction);

void teamColorTransform(QPixmap &pixmap,
                        QString pix,
                        QRgb colorFrom,
                        QRgb shirtColor,
                        QRgb shortColor,
                        QRgb hairColor,
                        QRgb skinColor);

MWindow::Action calculateAction(QPointF source,
                                QPointF destination);



#endif // SOCCERUTILS_H
