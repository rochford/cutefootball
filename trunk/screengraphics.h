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

    enum ScreenGraphicsMode {
        NormalMode,
        ReplayMode
        };

    ScreenGraphics(Pitch *p);
    virtual ~ScreenGraphics();
    void updatePosition();
    void setText(QString s);
    void setMode(ScreenGraphicsMode mode);

signals:

public slots:
    void changeFont();


private:
    Pitch *m_pitch;
    QGraphicsSimpleTextItem *m_scoreText;
    ScreenGraphicsMode m_mode;
    QTimer *m_timer;
};

#endif // SCREENGRAPHICS_H
