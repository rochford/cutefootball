#ifndef SCREENGRAPHICS_H
#define SCREENGRAPHICS_H

#include <QObject>

class QGraphicsSimpleTextItem;

class Pitch;

class ScreenGraphics
{
public:
    ScreenGraphics(Pitch *p);
    virtual ~ScreenGraphics();
    void updatePosition();
    void setText(QString s);

private:
    Pitch *m_pitch;
    QGraphicsSimpleTextItem *m_scoreText;
};

#endif // SCREENGRAPHICS_H
