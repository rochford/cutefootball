#ifndef SCREENGRAPHICS_H
#define SCREENGRAPHICS_H

#include <QObject>

class QGraphicsSimpleTextItem;
class QTimer;

class Pitch;

class ScreenGraphics : public QObject
{
    Q_OBJECT
public:

    ScreenGraphics(Pitch *p);
    virtual ~ScreenGraphics();
    void updatePosition();
    void setText(QString s);

signals:

public slots:
    void changeFont();


private:
    Pitch *m_pitch;
    QGraphicsSimpleTextItem *m_scoreText;
    QTimer *m_timer;
};

#endif // SCREENGRAPHICS_H
