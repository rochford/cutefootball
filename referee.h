#ifndef REFEREE_H
#define REFEREE_H

#include <QtGui>
#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

class Pitch;

class Referee : public QGraphicsPixmapItem
{
public:
    explicit Referee(Pitch* pitch, QObject *parent = 0);

    enum { Type = UserType + 3 };
    int type() const
        { return Type;}

    void advance(int phase);

    enum RefereeAction {
        North,
        NorthEast,
        East,
        SouthEast,
        South,
        SouthWest,
        West,
        NorthWest,

        YellowCard,
        RedCard,

        DoNothing };

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
signals:

public slots:

private:
    Pitch *pitch_;
    int speed_;
    int step_;

    QMap<RefereeAction,QPointF> moveDistance_;
};

#endif // REFEREE_H
