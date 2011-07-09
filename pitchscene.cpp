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

#include "pitchscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include "pitch.h"
#include "Player.h"
#include "soccerutils.h"

PitchScene::PitchScene(const QRectF& footballGroundRect,
                       Pitch* pitch,
                       QObject *parent) :
    QGraphicsScene(footballGroundRect),
    m_pitch(pitch),
    m_inputMethod(settingsFrame::Keyboard)
{
    setBackgroundBrush(QBrush(QPixmap(QString(":/images/pitch4.png"))));
    // disable focus selection by user pressing scene items
    setStickyFocus(true);
}

void PitchScene::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    qDebug() << "mousePressEvent " << e->buttonDownScenePos(Qt::LeftButton);
    if (m_inputMethod != settingsFrame::Mouse)
        return;

    Player* p = static_cast<Player*>(focusItem());
    if (!p)
        return;
    p->mousePressEvent(e);
}

