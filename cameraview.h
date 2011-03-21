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

    void centerOn(QGraphicsItem* object);
    void centerOn(QPointF position);
    // returns the top left of the view in scene co-ordinates
    QPointF topLeft() const;
    QGraphicsItem* centeredItem() const { return m_object; }
    void appendProxyWidget(QGraphicsProxyWidget *item, QPointF viewPosition);
signals:

public slots:
    void update();
private:
    QGraphicsView& m_view;
    QGraphicsItem* m_object; // NOT OWNED
    QPointF m_position;
    QList<QGraphicsProxyWidget *> m_widgets;

};

#endif // CAMERAVIEW_H
