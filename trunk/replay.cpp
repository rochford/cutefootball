#include <QGraphicsItemAnimation>

#include <QDebug>

#include "replay.h"
#include "pitch.h"
#include "ball.h"
#include "Player.h"
#include "screengraphics.h"


Replay::Replay(Pitch* p,
               QObject *parent) :
    QObject(parent),
    pitch_(p),
    replayTimeLine_(NULL),
    frameCounter_(0.0)
{
    // replay will show 1 frame per second, for each second of the game
    replayTimeLine_ = new QTimeLine(KGameLength, this);
    replayTimeLine_->setFrameRange(0, KGameLength/KReplayFrameRate);
    replayTimeLine_->setUpdateInterval(KReplayFrameRate);
    replayTimeLine_->setCurveShape(QTimeLine::LinearCurve);
    replaySnapShotTimer_ = new QTimer(this);
    replaySnapShotTimer_->setInterval(KReplayFrameRate);

    connect(replaySnapShotTimer_, SIGNAL(timeout()), this, SLOT(makeReplaySnapshot()));
    connect(replayTimeLine_, SIGNAL(frameChanged(int)), this, SLOT(replayFrame(int)));
}

void Replay::replayStart()
{
    // dont make snapshots while replaying
    replaySnapShotTimer_->stop();

    // stop if previously started
    replayTimeLine_->stop();

    // only display last 5 seconds
    int startFrame = frameCounter_ - 5*(1000/KReplayFrameRate);

    replayTimeLine_->setFrameRange(startFrame, frameCounter_);
    replayTimeLine_->setDuration(5*1000);
    replayTimeLine_->start();
}

void Replay::replayStop()
{
    // all done, stop the time line now
    replayTimeLine_->stop();

    replaySnapShotTimer_->start();
    pitch_->replayStop();
}

void Replay::makeReplaySnapshot()
{
    // for each graphics item on the scene,
    // make an animation object
    // set the timeline value for the animation objects
    qreal f = frameCounter_ / (KGameLength/KReplayFrameRate);
    animationItems[0]->setPosAt(f, pitch_->ball()->pos());
    f = frameCounter_ / (KGameLength/KReplayFrameRate);
    int cnt = 1;
    foreach (Player *p, pitch_->m_players)
        animationItems[cnt++]->setPosAt(f, p->pos());

    frameCounter_++;
}

void Replay::replayFrame(int frame)
{
    pitch_->m_view->centerOn(pitch_->ball()->pos());
    pitch_->m_scoreText->updatePosition();

    if (frame == frameCounter_)
        replayStop();

    qreal f = frame/ (KGameLength/40.00);
    foreach (QGraphicsItemAnimation *anim, animationItems)
        anim->item()->setPos(anim->posAt(f));

    pitch_->m_scene->update();
}

void Replay::createAnimationItems()
{
    QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
    anim->setItem(pitch_->ball());
    anim->setTimeLine(replayTimeLine_);
    animationItems.append(anim);

    foreach (Player *p, pitch_->m_players) {
        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        anim->setTimeLine(replayTimeLine_);
        animationItems.append(anim);
    }
}
