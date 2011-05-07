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
#include "cameraview.h"
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>

#include <QDebug>

CameraView::CameraView(QGraphicsView& v,
                       QObject *parent) :
    QObject(parent),
    m_view(v),
    m_object(NULL),
    m_position(QPointF())
{
}

void CameraView::centerOn(QGraphicsItem *object)
{
//    qDebug() << "CameraView::centerOn(QGraphicsItem *object)";
    m_object = object;
    m_position = QPointF();
    m_view.centerOn(object);
}

void CameraView::centerOn(QPointF position)
{
//    qDebug() << "CameraView::centerOn(QPointF position)";
    m_position = position;
    m_object = NULL;
    m_view.centerOn(position);
}

QPointF CameraView::topLeft() const
{
    return m_view.mapToScene(m_view.rect().topLeft());
}

QPointF CameraView::center() const
{
    return m_view.mapToScene(m_view.rect().center() );
}

void CameraView::update()
{
    if (m_object) {
//        qDebug() << "CameraView::update - object";
        m_view.centerOn(m_object);
    } else  {
//        qDebug() << "CameraView::update - pos";
        m_view.centerOn(m_position);
    }

    //    if (m_cameraView->centeredItem() == m_ball) {
    //        m_cameraView->centerOn(m_ball);
    //    }
    //    m_screenGraphicsFrameProxy->setPos(m_cameraView->topLeft());

    QPointF point;
    foreach(PositionedProxyWidget item, m_widgets) {
        switch (item.viewPos) {
        case TopLeft:
            point = topLeft();
            break;
        case Center:
            point = center();
            point = QPointF(point.x() - item.widget->rect().width() / 2,point.y());
            break;
        default:
            point = topLeft();
            break;
        }

        item.widget->setPos(point);
    }
}

void CameraView::appendProxyWidget(QGraphicsProxyWidget *item, ViewPosition viewPos )
{
      PositionedProxyWidget pw;
      pw.viewPos = viewPos;
      pw.widget = item;

      m_widgets.append(pw);
}

