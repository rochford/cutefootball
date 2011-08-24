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

#ifndef PITCHSCENE_H
#define PITCHSCENE_H

#include <QGraphicsScene>

#include "settingsFrame.h"

class QTimer;
class Pitch;

class PitchScene : public QGraphicsScene
{
    Q_OBJECT;
public:
    explicit PitchScene(const QRectF& footballGroundRect,
                        Pitch* pitch,
                        QObject *parent = 0);
    virtual ~PitchScene();
public slots:
    void setInputMethod(settingsFrame::InputMethod method) { m_inputMethod = method; }

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);

private:
    Pitch* m_pitch;
    settingsFrame::InputMethod m_inputMethod;
    QTimer* m_doubleTapTimer;
    QPointF m_lastTapPoint;
};

#endif // PITCHSCENE_H
