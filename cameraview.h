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
