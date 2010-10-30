#include <QtGui>
#include <QtGlobal>
#include <QDebug>

#include <QGraphicsItemAnimation>

#include "pitch.h"
#include "ball.h"
#include "Player.h"
#include "team.h"
#include "referee.h"
#include "goalkeeper.h"

const int KReplayFrameRate = 40.00; // ms
const int KReallyHighZValue = 10;

const QPen KWhitePaintPen(QBrush(Qt::white),3);
#ifdef Q_OS_SYMBIAN
const QFont KFont("Times", 9, QFont::Bold);
#else
const QFont KFont("Times", 12, QFont::Bold);
#endif // Q_OS_SYMBIAN

Pitch::Pitch(const QRectF& footballGroundRect)
  : QObject(),
    scene(new QGraphicsScene(footballGroundRect)),
    view(new QGraphicsView(scene)),
    motionTimer_(NULL),
    gameTimer_(NULL),
    nextGameState_(NotStarted),
    lastNearestPlayer(NULL),
    bottomGoal(NULL),
    topGoal(NULL),
    replayTimeLine_(NULL),
    frameCounter_(0.0),
    remainingTimeInHalfMs_(KGameLength)
{
    // replay will show 1 frame per second, for each second of the game
    replayTimeLine_ = new QTimeLine(KGameLength, this);
    replayTimeLine_->setFrameRange(0, KGameLength/KReplayFrameRate);
    replayTimeLine_->setUpdateInterval(KReplayFrameRate);
    replayTimeLine_->setCurveShape(QTimeLine::LinearCurve);

    scene->setBackgroundBrush(QPixmap(QString(":/images/pitch2.GIF")));

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setAutoFillBackground(false);

    motionTimer_ = new QTimer(this);
    motionTimer_->setInterval(KGameRefreshRate);
    gameTimer_ = new QTimer(this);
    gameTimer_->setInterval(1000);
    replaySnapShotTimer_ = new QTimer(this);

    // create the pitch
    footballPitch_ = scene->addRect(30, 30, scene->width()-60, scene->height() -60,
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );

    // divide the pitch into areas
    // makes it easier for computer based movement
    QPointF tlTopHalf = footballPitch_->rect().topLeft();
    QPointF brBottomHalf = footballPitch_->rect().bottomRight();
    QPointF brTopHalf = brBottomHalf - QPointF(footballPitch_->rect().height()/2,0);
    QPointF tlBottomHalf = tlTopHalf + QPointF(footballPitch_->rect().height()/2,0);

    const int w = footballPitch_->rect().width();
    const int h = footballPitch_->rect().height();

    for (int row = 0; row < KRow; row++) {
        for (int col = 0; col < KColumn; col++) {
            qreal tlx = (w / KColumn) * col;
            qreal tly = (h / KRow ) * row;
            QPointF tl(tlTopHalf + QPointF(tlx,tly));
            QPointF br(tl + QPointF(w/KColumn,h/KRow));
            pitchArea[row][col] = QRectF(tl, br);
        }
    }

    // simple text
    scoreText_ = scene->addSimpleText(QString("XXX"),KFont);
    scoreText_->setPos(view->mapToScene(view->rect().topLeft()));
    scoreText_->setZValue(KReallyHighZValue);

    // create the goals
    bottomGoal = scene->addRect((scene->width() / 2)-30, scene->height()-30,60,25,
                   QPen(Qt::black),
                   QBrush(Qt::black,Qt::CrossPattern) );
    topGoal = scene->addRect((scene->width() / 2)-30,5,60,25,
                   QPen(Qt::black),
                   QBrush(Qt::black,Qt::CrossPattern) );

    // penalty areas
    topPenaltyArea = scene->addRect((scene->width() / 2)-70, 30,
                                    140, 100,
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );
    bottomPenaltyArea = scene->addRect((scene->width() / 2)-70, scene->height()-130,
                                       140, 100,
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );
    connect(motionTimer_, SIGNAL(timeout()), scene, SLOT(advance()));
    connect(motionTimer_, SIGNAL(timeout()), scene, SLOT(update()));
    connect(motionTimer_, SIGNAL(timeout()), this, SLOT(hasBallCheck()));
    connect(motionTimer_, SIGNAL(timeout()), this, SLOT(selectNearestPlayer()));
    connect(gameTimer_, SIGNAL(timeout()), this, SLOT(updateDisplayTime()));

    connect(gameTimer_, SIGNAL(timeout()), this, SLOT(decrementGameTime()));

    connect(replaySnapShotTimer_, SIGNAL(timeout()), this, SLOT(makeReplaySnapshot()));
    connect(replayTimeLine_, SIGNAL(frameChanged(int)), this, SLOT(replayFrame(int)));
}

Player* Pitch::selectNearestPlayer(Team* team)
{
    qreal nearest = 0xffff;
    Player *nearestPlayer(NULL);
    Ball *ball = getBall();
    // nobody has ball, select the closest hometeam
    foreach (Player *p, players_) {
        if ( p->team_== team ) {

            // dont select the goal keeper
            if (p->role_ == Player::GoalKeeper)
                continue;

            if (!nearestPlayer)
                nearestPlayer = p;

            if ( p->hasBall_ ) {
                nearestPlayer = p;
                break;
            }

            p->setHumanControlled(false);

            const int dx = p->pos().x() - ball->pos().x();
            const int dy = p->pos().y() - ball->pos().y();
            if ( (qAbs(dx*dx)+qAbs(dy*dy)) < nearest) {
                nearestPlayer = p;
                nearest = qAbs(qAbs(dx*dx)+qAbs(dy*dy));
            }
        }
    }
    return nearestPlayer;
}

void Pitch::setPiece(Team::Direction, SetPiece s)
{
}

void Pitch::selectNearestPlayer()
{
    Player *p = selectNearestPlayer(homeTeam_);
    if (p) {
        scene->setFocusItem(p);
        p->setHumanControlled(true);
        if ( lastNearestPlayer != p ) {
            this->lastNearestPlayer = p;
            emit focusedPlayerChanged();
        }
    }
    else if (lastNearestPlayer) {
        scene->setFocusItem(lastNearestPlayer);
        lastNearestPlayer->setHumanControlled(true);
    }
}

Pitch::~Pitch()
{
    lastNearestPlayer = NULL;
    delete ball_;
    delete awayTeam_;
    delete homeTeam_;
}

void Pitch::removePlayers()
{
    foreach( Player *p, players_)
        scene->removeItem(p);

    players_.clear();
}

void Pitch::kickOff(Game state)
{
    motionTimer_->stop();
    gameTimer_->stop();

    qDebug() << "kickOff remaining" << remainingTimeInHalfMs_ / 1000;

    bool needRestartTimers(false);

    switch (state) {
    case Paused:
        break;
    case NotStarted:
        homeTeam_->setHasBall(true);
        homeTeam_->setDirection(Team::NorthToSouth);
        setPlayerStartPositions(homeTeam_);
        setPlayerAttackPositions(homeTeam_);
        setPlayerDefendPositions(homeTeam_);

        awayTeam_->setHasBall(false);
        awayTeam_->setDirection(Team::SouthToNorth);
        setPlayerStartPositions(awayTeam_);
        setPlayerAttackPositions(awayTeam_);
        setPlayerDefendPositions(awayTeam_);

        needRestartTimers = true;

        scene->addItem(ball_);
        scene->addItem(referee_);

        replaySnapShotTimer_->start(KReplayFrameRate);

        nextGameState_ = FirstHalfOver;
        break;
    case FirstHalfOver:
        homeTeam_->setHasBall(false);
        awayTeam_->setHasBall(true);

        awayTeam_->setDirection(Team::NorthToSouth);
        setPlayerStartPositions(awayTeam_);
        setPlayerAttackPositions(awayTeam_);
        setPlayerDefendPositions(awayTeam_);
        homeTeam_->setDirection(Team::SouthToNorth);
        setPlayerStartPositions(homeTeam_);
        setPlayerAttackPositions(homeTeam_);
        setPlayerDefendPositions(homeTeam_);

        ball_->setStartingPosition();

        needRestartTimers = true;
        remainingTimeInHalfMs_ = KGameLength;

        nextGameState_ = SecondHalfOver;
        break;
    case SecondHalfOver:
        nextGameState_ = Finished;
        break;
    case GoalScored:

        setPlayerStartPositions(homeTeam_);
        homeTeam_->setHasBall(true);
        setPlayerStartPositions(awayTeam_);
        awayTeam_->setHasBall(false);

        needRestartTimers = true;
        break;
    case Finished:
        scene->clearFocus();
        // which team won?
        scene->removeItem(ball_);
        scene->removeItem(referee_);
        removePlayers();
        updateDisplayTime();
        break;
    }

    if (needRestartTimers) {
        gameTimer_->start();
        motionTimer_->start();
    }
}

void Pitch::decrementGameTime()
{
    remainingTimeInHalfMs_ = remainingTimeInHalfMs_ - 1000;
    if ( !remainingTimeInHalfMs_ )
        kickOff(nextGameState_);
}

void Pitch::kickOff()
{
    kickOff(nextGameState_);
}

void Pitch::updateDisplayTime()
{
    switch(nextGameState_) {
    case Finished:
        scoreText_->setText(QString("GAME OVER!!!"));
        break;
    case NotStarted:
        break;
    case GoalScored:
    case FirstHalfOver:
    case SecondHalfOver:
        QTime tmp(0,0,0,0);
        tmp = tmp.addMSecs(remainingTimeInHalfMs_);

        QString str(tmp.toString(QString("mm:ss")));
        str.append(" ");
        str.append(homeTeam_->name());
        str.append(" ");
        str.append(QString::number(homeTeam_->goals_));
        str.append(" - ");

        str.append(awayTeam_->name());
        str.append(" ");
        str.append(QString::number(awayTeam_->goals_));
        scoreText_->setText(str);
        break;
    }
}

void Pitch::hasBallCheck()
{
    view->centerOn(ball_->pos());
    scoreText_->setPos(view->mapToScene(view->rect().topLeft()));

    // which team has the ball?
    foreach (Player *p, players_) {
        if ( p->hasBall_ ) {
            homeTeam_->setHasBall(p->team_== this->homeTeam_);
            awayTeam_->setHasBall(p->team_== this->awayTeam_);
            break; // can break as now known
        }
    }
}

void Pitch::newGame()
{
    remainingTimeInHalfMs_ = KGameLength;

    ball_ = new Ball(this);

    connect(ball_, SIGNAL(goalScored(bool)), this, SLOT(goalScored(bool)));

    homeTeam_ = new Team(QString("HOME"), Qt::red);
    awayTeam_ = new Team(QString("AWAY"), Qt::blue);

    createTeamPlayers(homeTeam_);
    connect(ball_, SIGNAL(goalScored(bool)), homeTeam_, SLOT(goalScored(bool)));
    createTeamPlayers(awayTeam_);
    connect(ball_, SIGNAL(goalScored(bool)), awayTeam_, SLOT(goalScored(bool)));

    referee_ = new Referee(this);

    QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
    anim->setItem(ball_);
    anim->setTimeLine(replayTimeLine_);
    animationItems.append(anim);

    foreach (Player *p, players_) {
        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        anim->setTimeLine(replayTimeLine_);
        animationItems.append(anim);
    }

    updateDisplayTime();
    kickOff();
}

void Pitch::pausedGame()
{
    qDebug() << "pausedGame remaining" << remainingTimeInHalfMs_/1000;

    if (gameTimer_->isActive())
        gameTimer_->stop();
    if (motionTimer_->isActive())
        motionTimer_->stop();
}

void Pitch::continueGame()
{
    if (!gameTimer_->isActive())
        gameTimer_->start();
    if (!motionTimer_->isActive())
        motionTimer_->start();
}

void Pitch::action(MWindow::Action action)
{
    // actions only processed if game is ongoing
    if ( nextGameState_ == Finished )
        return;

    // action is only applicabled to the human controlled player
    foreach (Player *p, players_) {
        if ( p->humanControlled() && ( p->team_== homeTeam_ ) ) {
            p->move(action);
            return;
        }
    }
}

void Pitch::createTeamPlayers(Team *team)
{
    bool isHomeTeam(false);

    if (team->name() == QString("HOME"))
        isHomeTeam = true;

    for (int i = Player::GoalKeeper; i < Player::LastDummy; i++ ) {
         Player *pl(NULL);
        if ( i ==  Player::GoalKeeper) {
           pl = new GoalKeeper(
                    this,
                    team);

        } else {
            pl = new Player(
                    !isHomeTeam,
                    this,
                    team,
                    (Player::Role)i);
        }
        pl->createPixmaps();
        players_.append(pl);
        scene->addItem(pl);
    }
}

void Pitch::setPlayerStartPositions(Team *team)
{
    bool nToS(false);
    if ( team->getDirection() == Team::NorthToSouth )
        nToS = true;

    QMap<int,QRectF> startPositions;
    startPositions.insert(Player::GoalKeeper,
                         pitchArea[nToS ? 0 : 7][1]);
    startPositions.insert(Player::LeftDefence,
                         pitchArea[nToS ? 1 : 6][0]);
    startPositions.insert(Player::RightDefence,
                         pitchArea[nToS ? 1 : 6][2]);
    startPositions.insert(Player::LeftMidfield,
                         pitchArea[nToS ? 2 : 5][0]);
    startPositions.insert(Player::CentralMidfield,
                         pitchArea[nToS ? 2 : 5][1]);
    startPositions.insert(Player::RightMidfield,
                         pitchArea[nToS ? 2 : 5][2]);
    startPositions.insert(Player::LeftAttack,
                         pitchArea[nToS ? 3 : 4][1]);
    startPositions.insert(Player::CentralAttack,
                         pitchArea[nToS ? 3 : 4][0]);
    startPositions.insert(Player::RightAttack,
                         pitchArea[nToS ? 3 : 4][2]);

    // action is only applicabled to the human controlled player
    foreach (Player *p, players_) {
        if (p->team_ == team) {
            p->hasBall_ = false;
            p->startPosition_ = startPositions[p->role_];
            p->setPos(p->startPosition_.center());
        }
    }
}

void Pitch::setPlayerDefendPositions(Team *team)
{
    bool nToS(false);
    if ( team->getDirection() == Team::NorthToSouth )
        nToS = true;

    QMap<int,QRectF> defendPositions;
    defendPositions.insert(Player::GoalKeeper,
                         pitchArea[nToS ? 0 : 7][1]);
    defendPositions.insert(Player::LeftDefence,
                         pitchArea[nToS ? 1 : 6][0]);
    defendPositions.insert(Player::RightDefence,
                         pitchArea[nToS ? 1 : 6][2]);
    defendPositions.insert(Player::LeftMidfield,
                         pitchArea[nToS ? 2 : 5][0]);
    defendPositions.insert(Player::CentralMidfield,
                         pitchArea[nToS ? 1 : 6][1]);
    defendPositions.insert(Player::RightMidfield,
                         pitchArea[nToS ? 2 : 5][2]);
    defendPositions.insert(Player::LeftAttack,
                         pitchArea[nToS ? 3 : 4][0]);
    defendPositions.insert(Player::CentralAttack,
                         pitchArea[nToS ? 3 : 4][1]);
    defendPositions.insert(Player::RightAttack,
                         pitchArea[nToS ? 3 : 4][2]);

    // action is only applicabled to the human controlled player
    foreach (Player *p, players_) {
        if (p->team_ == team)
            p->defencePosition_ = defendPositions[p->role_];
    }
}

void Pitch::setPlayerAttackPositions(Team *team)
{
    bool nToS(false);
    if ( team->getDirection() == Team::NorthToSouth )
        nToS = true;

    QMap<int,QRectF> attackPositions;
    attackPositions.insert(Player::GoalKeeper,
                         pitchArea[nToS ? 0 : 7][1]);
    attackPositions.insert(Player::LeftDefence,
                         pitchArea[nToS ? 2 : 6][0]);
    attackPositions.insert(Player::RightDefence,
                         pitchArea[nToS ? 2 : 6][2]);
    attackPositions.insert(Player::LeftMidfield,
                         pitchArea[nToS ? 4 : 2][0]);
    attackPositions.insert(Player::CentralMidfield,
                         pitchArea[nToS ? 5 : 3][1]);
    attackPositions.insert(Player::RightMidfield,
                         pitchArea[nToS ? 4 : 2][2]);
    attackPositions.insert(Player::LeftAttack,
                         pitchArea[nToS ? 6 : 1][0]);
    attackPositions.insert(Player::CentralAttack,
                         pitchArea[nToS ? 6 : 1][1]);
    attackPositions.insert(Player::RightAttack,
                         pitchArea[nToS ? 6 : 1][2]);

    // action is only applicabled to the human controlled player
    foreach (Player *p, players_) {
        if (p->team_ == team)
            p->attackPosition_ = attackPositions[p->role_];
    }
}

void Pitch::goalScored(bool isNorthGoal)
{
    qDebug() << "Pitch::goalScored start";

    kickOff(Pitch::GoalScored);
    qDebug() << "Pitch::goalScored end";
}


void Pitch::replayStart()
{
    qDebug() << "replay secondCounter_" << frameCounter_;
    motionTimer_->stop();
    gameTimer_->stop();

    // dont make snapshots while replaying
    replaySnapShotTimer_->stop();

    // stop if previously started
    replayTimeLine_->stop();

    // only display last 5 seconds
    int startFrame = frameCounter_ - 5*(1000/KReplayFrameRate);
    qDebug() << "replay startFrame" << startFrame;
    // replayTimeLine_->setStartFrame(startFrame);
    //replayTimeLine_->setEndFrame(frameCounter_);
    replayTimeLine_->setFrameRange(startFrame, frameCounter_);
    replayTimeLine_->setDuration(5*1000);
    replayTimeLine_->start();
}

void Pitch::replayStop()
{
    qDebug() << "replayStop";
    // all done, stop the time line now
    replayTimeLine_->stop();
    motionTimer_->start();
    gameTimer_->start();
    replaySnapShotTimer_->start();
}

void Pitch::makeReplaySnapshot()
{
 //   qDebug() << "makeReplaySnapshot";
    // for each graphics item on the scene,
    // make an animation object
    // set the timeline value for the animation objects
    qreal f = frameCounter_ / (KGameLength/KReplayFrameRate);
    animationItems[0]->setPosAt(f, ball_->pos());

    int cnt = 1;
    foreach (Player *p, players_) {

        f = frameCounter_ / (KGameLength/KReplayFrameRate);
        animationItems[cnt++]->setPosAt(f, p->pos());
        }
    frameCounter_++;
}

void Pitch::replayFrame(int frame)
{
    view->centerOn(ball_->pos());
    scoreText_->setPos(view->mapToScene(view->rect().topLeft()));

    if (frame == frameCounter_)
        replayStop();

    qDebug() << "replayFrame frame" << frame;
    foreach (QGraphicsItemAnimation *anim, animationItems) {
        qreal f = frame/ (KGameLength/40.00);
        QPointF before = anim->item()->pos();

        anim->item()->setPos(anim->posAt(f));
    }
    scene->update();
}
