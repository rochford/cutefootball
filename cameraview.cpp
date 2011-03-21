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
    qDebug() << "CameraView::centerOn(QGraphicsItem *object)";
    m_object = object;
    m_position = QPointF();
    m_view.centerOn(object);
}

void CameraView::centerOn(QPointF position)
{
    qDebug() << "CameraView::centerOn(QPointF position)";
    m_position = position;
    m_object = NULL;
    m_view.centerOn(position);
}

QPointF CameraView::topLeft() const
{
    return m_view.mapToScene(m_view.rect().topLeft());
}

void CameraView::update()
{
    if (m_object) {
        qDebug() << "CameraView::update - object";
        m_view.centerOn(m_object);
    } else  {
        qDebug() << "CameraView::update - pos";
        m_view.centerOn(m_position);
    }

    //    if (m_cameraView->centeredItem() == m_ball) {
    //        m_cameraView->centerOn(m_ball);
    //    }
    //    m_screenGraphicsFrameProxy->setPos(m_cameraView->topLeft());

    foreach(QGraphicsProxyWidget *item, m_widgets) {
        item->setPos(topLeft());
    }
}

void CameraView::appendProxyWidget(QGraphicsProxyWidget *item, QPointF /* viewPosition */)
{
    m_widgets.append(item);
}

