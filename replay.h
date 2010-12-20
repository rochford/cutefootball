#ifndef REPLAY_H
#define REPLAY_H

#ifdef REPLAY_FEATURE

#include <QObject>
#include <QList>
#include <QTimeLine>
#include <QTimer>

class QGraphicsItemAnimation;
class Pitch;

class Replay : public QObject
{
    Q_OBJECT
public:
    explicit Replay(Pitch* p, QObject *parent = 0);

    void replayStart();
    void replayStop();
    bool isReplay() const { return replayTimeLine_->state() == QTimeLine::Running; }
    void createAnimationItems();

signals:

public slots:
    void replayFrame(int frame);
    void makeReplaySnapshot();

public:
    Pitch* pitch_;
    QTimeLine* replayTimeLine_;
    QTimer *replaySnapShotTimer_;
    qreal frameCounter_;
    QList<QGraphicsItemAnimation*> animationItems;
};

#endif // REPLAY_FEATURE

#endif // REPLAY_H
