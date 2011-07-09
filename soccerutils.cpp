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
#include <QtGui>
#include "soccerutils.h"
#include "compileTimeSettings.h"

Player::Role mapPositionStringToRole(QString positionString)
{
    QMap<QString, Player::Role> map;
    map.insert( "GK", Player::GoalKeeper );

    map.insert( "LB", Player::LeftDefence );
    map.insert( "LD", Player::LeftCentralDefence );
    map.insert( "CD", Player::CentralDefence );
    map.insert( "RD", Player::RightCentralDefence );
    map.insert( "RB", Player::RightDefence );

    map.insert( "LM", Player::LeftMidfield );
    map.insert( "CM", Player::CentralMidfield );
    map.insert( "RM", Player::RightMidfield );

    map.insert( "LF", Player::LeftAttack );
    map.insert( "CF", Player::CentralAttack );
    map.insert( "RF", Player::RightAttack );

    return map[positionString];
}

QColor skinColorMapping(QString col)
{
    if (col == "pink")
        return QColor(255,200,156);
    else if (col == "tan")
        return QColor(242,160,94);
    else // brown
        return QColor(146,96,56);
}

void teamColorTransform(QPixmap &pixmap, QString pix,
                        QRgb colorFrom,
                        QRgb shirtColor, QRgb shortColor,
                        QRgb hairColor, QRgb skinColor)
{
    QImage img(pix);
    for (int w = 0; w < img.width(); w++) {
        for (int h = 0; h < img.height(); h++) {
            QRgb rgb = img.pixel(w, h);
            if (qRed(rgb) > 250 && qBlue(rgb) < 8 && qGreen(rgb) < 8)
                img.setPixel(QPoint(w,h), shirtColor);
            else if (qRed(rgb) < 8 && qBlue(rgb) < 8 && qGreen(rgb) > 250)
                img.setPixel(QPoint(w,h), shortColor);
            else if (qRed(rgb) == 255 && qBlue(rgb) == 122 && qGreen(rgb) == 40)
                img.setPixel(QPoint(w,h), hairColor);
            else if (qRed(rgb) == 255 && qBlue(rgb) == 156 && qGreen(rgb) == 200)
                img.setPixel(QPoint(w,h), skinColor);
        }
    }
    pixmap = QPixmap::fromImage(img);
}



MWindow::Action calculateAction(QPointF source,
                                QPointF destination)

{
    QPointF diff(source - destination);

    const qreal dx = diff.x();
    const qreal dy = diff.y();

    if (dx > 0 && dy == 0)
        return MWindow::West;
    else if (dx < 0 && dy == 0)
        return MWindow::East;
    else if (dx > 0 && dy < 0)
        return MWindow::SouthWest;
    else if (dx > 0 && dy > 0)
        return MWindow::NorthWest;
    else if (dx < 0 && dy > 0)
        return MWindow::NorthEast;
    else if (dx < 0 && dy < 0)
        return MWindow::SouthEast;
    else if (dx == 0 && dy > 0)
        return MWindow::North;
    else if (dx == 0 && dy < 0)
        return MWindow::South;
    else
        return MWindow::NoAction;
}

int calculateTackleRotationFromLastAction(MWindow::Action lastAction)
{
    int action;
    switch(lastAction) {
    case MWindow::North:
        action = 0;
        break;
    case MWindow::NorthEast:
        action = 45;
        break;
    case MWindow::East:
        action = 90;
        break;
    case MWindow::SouthEast:
        action = 135;
        break;
    case MWindow::South:
        action = 180;
        break;
    case MWindow::SouthWest:
        action = 225;
        break;
    case MWindow::West:
        action = 270;
        break;
    case MWindow::NorthWest:
        action = 315;
        break;
    default:
        break;
    }
    return action;
}

MWindow::Action calculateTackleActionFromLastAction(MWindow::Action lastAction)
{
    MWindow::Action action;

    switch(lastAction) {
    case MWindow::North:
        action = MWindow::TackleN;
        break;
    case MWindow::NorthEast:
        action = MWindow::TackleNE;
        break;
    case MWindow::East:
        action = MWindow::TackleE;
        break;
    case MWindow::SouthEast:
        action = MWindow::TackleSE;
        break;
    case MWindow::South:
        action = MWindow::TackleS;
        break;
    case MWindow::SouthWest:
        action = MWindow::TackleSW;
        break;
    case MWindow::West:
        action = MWindow::TackleW;
        break;
    case MWindow::NorthWest:
        action = MWindow::TackleNW;
        break;
    default:
        action = MWindow::TackleN;
        break;
    }
    return action;
}

