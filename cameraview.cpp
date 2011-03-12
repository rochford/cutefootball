#include "cameraview.h"
#include <QGraphicsItem>

CameraView::CameraView(QGraphicsView& v,
                       QObject *parent) :
    QObject(parent),
    m_view(v)
{
}

void CameraView::centerOn(QGraphicsItem *object)
{
    m_object = object;
    m_view.centerOn(object);
}

void CameraView::centerOn(QPointF position)
{
    m_view.centerOn(position);
}

QPointF CameraView::topLeft() const
{
    return m_view.mapToScene(m_view.rect().topLeft());
}

