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
#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QPointF>

class QGraphicsItem;
class QGraphicsProxyWidget;



class CameraView : public QObject
{
    Q_OBJECT
public:
    explicit CameraView(QGraphicsView& v,
                        QObject *parent = 0);

    enum ViewPosition {
        TopLeft
    };
    struct PositionedProxyWidget {
        QGraphicsProxyWidget *widget;
        ViewPosition viewPos;
    };

    void centerOn(QGraphicsItem* object);
    void centerOn(QPointF position);
    // returns the top left of the view in scene co-ordinates
    QPointF topLeft() const;
    QGraphicsItem* centeredItem() const { return m_object; }
    void appendProxyWidget(QGraphicsProxyWidget *item, ViewPosition viewPos);
signals:

public slots:
    void update();
private:
    QGraphicsView& m_view;
    QGraphicsItem* m_object; // NOT OWNED
    QPointF m_position;
    QList<PositionedProxyWidget> m_widgets;

};

#endif // CAMERAVIEW_H
