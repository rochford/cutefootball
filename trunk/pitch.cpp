#include <QtGui>
#include <QtGlobal>
#include <QDebug>

#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>


#include <QGraphicsItemAnimation>

#include "pitch.h"
#include "ball.h"
#include "Player.h"
#include "team.h"
#include "referee.h"
#include "goalkeeper.h"
#include "replay.h"
#include "screengraphics.h"

const QPen KWhitePaintPen(QBrush(Qt::white),3);
const int KOneSecondMs(1000);

Pitch::Pitch(const QRectF& footballGroundRect)
  : QObject(),
    scene(new QGraphicsScene(footballGroundRect)),
    view(new QGraphicsView(scene)),
    motionTimer_(NULL),
    gameTimer_(NULL),
    nextGameState_(NotStarted),
    lastNearestPlayer_(NULL),
    bottomGoal(NULL),
    topGoal(NULL),
    remainingTimeInHalfMs_(KGameLength),
    scoreText_(NULL),
    centerLine_(NULL),
    centerCircle_(NULL)
{
    replay_ = new Replay(this, this);

    timeLine_ = new QTimeLine(KOneSecondMs*6, this);
    timeLine_->setCurveShape(QTimeLine::LinearCurve);
    timeLine_->setFrameRange(0, 100);

    scene->setBackgroundBrush(QPixmap(QString(":/images/pitch2.GIF")));

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setAutoFillBackground(false);

    motionTimer_ = new QTimer(this);
    motionTimer_->setInterval(KGameRefreshRate);
    gameTimer_ = new QTimer(this);
    gameTimer_->setInterval(KOneSecondMs);

    layoutPitch();

    connect(timeLine_, SIGNAL(frameChanged(int)), this, SLOT(playFrame(int)));

    connect(motionTimer_, SIGNAL(timeout()), scene, SLOT(advance()));
    connect(motionTimer_, SIGNAL(timeout()), scene, SLOT(update()));
    connect(motionTimer_, SIGNAL(timeout()), this, SLOT(hasBallCheck()));
    connect(motionTimer_, SIGNAL(timeout()), this, SLOT(selectNearestPlayer()));

    connect(gameTimer_, SIGNAL(timeout()), this, SLOT(updateDisplayTime()));
    connect(gameTimer_, SIGNAL(timeout()), this, SLOT(decrementGameTime()));
}

Player* Pitch::selectNearestPlayer(Team* team)
{
    qreal nearest = 0xffff;
    Player *nearestPlayer(NULL);
    // nobody has ball, select the closest hometeam
    foreach (Player *p, players_) {
        if (p->team_== team) {

            // dont select the goal keeper
            if (p->role_ == Player::GoalKeeper)
                continue;

            if (!nearestPlayer)
                nearestPlayer = p;

            if (p->hasBall_) {
                nearestPlayer = p;
                break;
            }

            p->setHumanControlled(false);

            const int dx = p->pos().x() - ball_->pos().x();
            const int dy = p->pos().y() - ball_->pos().y();
            if ( (qAbs(dx*dx)+qAbs(dy*dy)) < nearest) {
                nearestPlayer = p;
                nearest = qAbs(qAbs(dx*dx)+qAbs(dy*dy));
            }
        }
    }
    return nearestPlayer;
}

void Pitch::setPiece(Team* t, SetPiece s)
{
    switch(s) {
    case Pitch::ThrowIn:
        {
            if (t == awayTeam_) {
                awayTeam_->setHasBall(true);
                homeTeam_->setHasBall(false);
            } else {
                awayTeam_->setHasBall(false);
                homeTeam_->setHasBall(true);
            }

            // find the nearest player to the ball and move them to the throw in position
            Player* throwInTaker = selectNearestPlayer(t);
            if (throwInTaker) {
                // change the player graphic to be throw in graphic
                throwInTaker->setPos(ball_->pos());
                throwInTaker->hasBall_ = true;
                ball_->setControlledBy(throwInTaker);
                throwInTaker->move(MWindow::ThrownIn);
                // hide the ball
               ball_->setVisible(false);
            }
        }
        break;
    default:
        break;
    }
}

void Pitch::selectNearestPlayer()
{
    Player *p = selectNearestPlayer(homeTeam_);
    if (p) {
        scene->setFocusItem(p);
        p->setHumanControlled(true);
        if (lastNearestPlayer_ != p) {
            lastNearestPlayer_ = p;
            emit focusedPlayerChanged();
        }
    }
    else if (lastNearestPlayer_) {
        scene->setFocusItem(lastNearestPlayer_);
        lastNearestPlayer_->setHumanControlled(true);
    }
}

Pitch::~Pitch()
{
    lastNearestPlayer_ = NULL;
    delete ball_;
    delete awayTeam_;
    delete homeTeam_;
}

void Pitch::removePlayers()
{
    foreach(Player *p, players_)
        scene->removeItem(p);

    players_.clear();
}

void Pitch::layoutPitch()
{
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

    const qreal w = footballPitch_->rect().width();
    const qreal h = footballPitch_->rect().height();

    for (int row = 0; row < KRow; row++) {
        for (int col = 0; col < KColumn; col++) {
            qreal tlx = (w / KColumn) * col;
            qreal tly = (h / KRow ) * row;
            QPointF tl(tlTopHalf + QPointF(tlx,tly));
            QPointF br(tl + QPointF(w/KColumn,h/KRow));
            pitchArea[row][col] = QRectF(tl, br);
        }
    }

    // half way line
    centerLine_ = scene->addLine(scene->sceneRect().left(),scene->sceneRect().height()/2.0,
                         scene->sceneRect().right(),scene->sceneRect().height()/2.0, KWhitePaintPen);

    // center circle
    centerCircle_ = scene->addEllipse((scene->sceneRect().width()/2.0) -60,(scene->sceneRect().height()/2.0)-60,
                      120.0, 120.0, KWhitePaintPen);
    // simple text
    scoreText_ = new ScreenGraphics(this);

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
}

void Pitch::kickOff(Game state)
{
    motionTimer_->stop();
    gameTimer_->stop();

    bool needRestartTimers(false);

    switch (state) {
    case Paused:
        break;
    case NotStarted:
        homeTeam_->setDirection(Team::NorthToSouth);
        setPlayerStartPositions(homeTeam_);
        setPlayerAttackPositions(homeTeam_);
        setPlayerDefendPositions(homeTeam_);

        awayTeam_->setDirection(Team::SouthToNorth);
        setPlayerStartPositions(awayTeam_);
        setPlayerAttackPositions(awayTeam_);
        setPlayerDefendPositions(awayTeam_);

        nextGameState_ = PlayersTakePositions;

        createPlayerAnimationItems(timeLine_, nextGameState_);
        timeLine_->start();
        break;

    case PlayersTakePositions:
        needRestartTimers = true;
        homeTeam_->setHasBall(true);
        awayTeam_->setHasBall(false);

        scene->addItem(ball_);
        scene->addItem(referee_);

        nextGameState_ = FirstHalfOver;
        replay_->replaySnapShotTimer_->start();
        break;
    case FirstHalfOver:

        createPlayerAnimationItems(timeLine_, nextGameState_);
        timeLine_->start();
        ball_->setVisible(false);

        nextGameState_ = HalfTimeBreakOver;
        break;
     case HalfTimeBreakOver:

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

        foreach (Player *p, players_) {
                p->hasBall_ = false;
                p->setPos(p->startPosition_.center());
            }

        ball_->setVisible(true);
        ball_->setStartingPosition();

        needRestartTimers = true;
        remainingTimeInHalfMs_ = KGameLength;

        nextGameState_ = SecondHalfOver;
        break;

    case SecondHalfOver:
        nextGameState_ = Finished;
        break;
    case GoalScored:

        foreach (Player *p, players_) {
                p->hasBall_ = false;
                p->setPos(p->startPosition_.center());
            }

        homeTeam_->setHasBall(true);
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
    remainingTimeInHalfMs_ = remainingTimeInHalfMs_ - KOneSecondMs;
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
    scoreText_->updatePosition();

    // which team has the ball?
    foreach (Player *p, players_) {
        if (p->hasBall_) {
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

    replay_->createAnimationItems();

    updateDisplayTime();
    kickOff();
}

void Pitch::pausedGame()
{
    qDebug() << "pausedGame remaining" << remainingTimeInHalfMs_/KOneSecondMs;

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
        if (p->humanControlled() && (p->team_== homeTeam_)) {
            p->move(action);
            return;
        }
    }
}

void Pitch::createTeamPlayers(Team *team)
{
    QStringList names;
    names << "adam" << "bob" << "charlie" << "dave" << "ed" << "frank" << "george" << "harry" << "ian"
            << "jack" << "kai" << "luke" << "matt" << "nigel" << "oscar" << "pete"<< "roger" << "steve" << "tom" << "walt";
    bool isHomeTeam(false);

    if (team->name() == QString("HOME"))
        isHomeTeam = true;

    for (int i = Player::GoalKeeper; i < Player::LastDummy; i++ ) {
         Player *pl(NULL);
        if (i == Player::GoalKeeper) {
           pl = new GoalKeeper(
                    names.at(i),
                    this,
                    team);

        } else {
            pl = new Player(
                    names.at(i),
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
    if (team->getDirection() == Team::NorthToSouth)
        nToS = true;

    QMap<int,QRectF> startPositions;
    startPositions.insert(Player::GoalKeeper,
                         pitchArea[nToS ? 0 : 7][2]);
    startPositions.insert(Player::LeftDefence,
                         pitchArea[nToS ? 1 : 6][0]);
    startPositions.insert(Player::LeftCentralDefence,
                         pitchArea[nToS ? 1 : 6][1]);
    startPositions.insert(Player::RightCentralDefence,
                         pitchArea[nToS ? 1 : 6][3]);
    startPositions.insert(Player::RightDefence,
                         pitchArea[nToS ? 1 : 6][4]);
    startPositions.insert(Player::LeftMidfield,
                         pitchArea[nToS ? 2 : 5][0]);
    startPositions.insert(Player::CentralMidfield,
                         pitchArea[nToS ? 2 : 5][2]);
    startPositions.insert(Player::RightMidfield,
                         pitchArea[nToS ? 2 : 5][4]);
    startPositions.insert(Player::LeftAttack,
                         pitchArea[nToS ? 3 : 4][0]);
    startPositions.insert(Player::CentralAttack,
                         pitchArea[nToS ? 3 : 4][2]);
    startPositions.insert(Player::RightAttack,
                         pitchArea[nToS ? 3 : 4][4]);

    // action is only applicabled to the human controlled player
    foreach (Player *p, players_) {
        if (p->team_ == team) {
            p->hasBall_ = false;
            p->startPosition_ = startPositions[p->role_];
        }
    }
}

void Pitch::setPlayerDefendPositions(Team *team)
{
    bool nToS(false);
    if (team->getDirection() == Team::NorthToSouth)
        nToS = true;

    QMap<int,QRectF> defendPositions;
    defendPositions.insert(Player::GoalKeeper,
                         pitchArea[nToS ? 0 : 7][2]);
    defendPositions.insert(Player::LeftDefence,
                         pitchArea[nToS ? 1 : 6][0]);
    defendPositions.insert(Player::LeftCentralDefence,
                         pitchArea[nToS ? 1 : 6][1]);
    defendPositions.insert(Player::RightCentralDefence,
                         pitchArea[nToS ? 1 : 6][3]);
    defendPositions.insert(Player::RightDefence,
                         pitchArea[nToS ? 1 : 6][4]);
    defendPositions.insert(Player::LeftMidfield,
                         pitchArea[nToS ? 2 : 5][0]);
    defendPositions.insert(Player::CentralMidfield,
                         pitchArea[nToS ? 1 : 6][2]);
    defendPositions.insert(Player::RightMidfield,
                         pitchArea[nToS ? 2 : 5][4]);
    defendPositions.insert(Player::LeftAttack,
                         pitchArea[nToS ? 3 : 4][0]);
    defendPositions.insert(Player::CentralAttack,
                         pitchArea[nToS ? 3 : 4][2]);
    defendPositions.insert(Player::RightAttack,
                         pitchArea[nToS ? 3 : 4][4]);

    // action is only applicabled to the human controlled player
    foreach (Player *p, players_) {
        if (p->team_ == team)
            p->defencePosition_ = defendPositions[p->role_];
    }
}

void Pitch::setPlayerAttackPositions(Team *team)
{
    bool nToS(false);
    if (team->getDirection() == Team::NorthToSouth)
        nToS = true;

    QMap<int,QRectF> attackPositions;
    attackPositions.insert(Player::GoalKeeper,
                         pitchArea[nToS ? 0 : 7][2]);
    attackPositions.insert(Player::LeftDefence,
                         pitchArea[nToS ? 2 : 6][0]);
    attackPositions.insert(Player::LeftCentralDefence,
                         pitchArea[nToS ? 2 : 6][1]);
    attackPositions.insert(Player::RightCentralDefence,
                         pitchArea[nToS ? 2 : 6][3]);
    attackPositions.insert(Player::RightDefence,
                         pitchArea[nToS ? 2 : 6][4]);
    attackPositions.insert(Player::LeftMidfield,
                         pitchArea[nToS ? 4 : 2][0]);
    attackPositions.insert(Player::CentralMidfield,
                         pitchArea[nToS ? 5 : 3][2]);
    attackPositions.insert(Player::RightMidfield,
                         pitchArea[nToS ? 4 : 2][4]);
    attackPositions.insert(Player::LeftAttack,
                         pitchArea[nToS ? 6 : 1][0]);
    attackPositions.insert(Player::CentralAttack,
                         pitchArea[nToS ? 6 : 1][2]);
    attackPositions.insert(Player::RightAttack,
                         pitchArea[nToS ? 6 : 1][4]);

    // action is only applicabled to the human controlled player
    foreach (Player *p, players_) {
        if (p->team_ == team)
            p->attackPosition_ = attackPositions[p->role_];
    }
}

void Pitch::goalScored(bool isNorthGoal)
{
    kickOff(Pitch::GoalScored);
}

void Pitch::replayStart()
{
    motionTimer_->stop();
    gameTimer_->stop();

    scoreText_->setMode(ScreenGraphics::ReplayMode);
    scoreText_->setText("REPLAY");

    replay_->replayStart();
}

void Pitch::replayStop()
{
    scoreText_->setMode(ScreenGraphics::NormalMode);
    motionTimer_->start();
    gameTimer_->start();
}


// animate from present player position to another point.
void Pitch::createPlayerAnimationItems(QTimeLine *timeLine, Game g)
{
    playerAnimationItems.clear(); // TODO XXX TIM delete all

    foreach (Player *p, players_) {
        QGraphicsItemAnimation* anim = new QGraphicsItemAnimation(this);
        anim->setItem(p);
        anim->setTimeLine(timeLine);
        playerAnimationItems.append(anim);

        QPointF tmp;
        qreal stepX;
        qreal stepY;

        switch(g)
        {
        case Pitch::PlayersTakePositions:
            tmp = p->pos();
            stepX = ( p->startPosition_.center().x() - tmp.x()) / 100.0;
            stepY = ( p->startPosition_.center().y() - tmp.y()) / 100.0;
            break;
        case Pitch::FirstHalfOver:
            tmp = p->pos();
            stepX = ( 0 - tmp.x() ) / 100.0;
            stepY = ( 0 - tmp.y() ) / 100.0;
            break;
        default:
            break;
        }

        for (int i = 0; i < 100; ++i) {
            anim->setPosAt(i / 100.0, QPointF(tmp.x() + stepX,
                                              tmp.y() + stepY));
            tmp.setX(tmp.x() + stepX);
            tmp.setY(tmp.y() + stepY);
        }
    }
}

void Pitch::playFrame(int frame)
{
    qDebug() << "playFrame frame" << frame;
    foreach (QGraphicsItemAnimation *anim, playerAnimationItems) {
        qreal f = frame/ 100.00;

        anim->item()->setPos(anim->posAt(f));
    }
    scene->update();

    if (frame == timeLine_->endFrame())
       kickOff(nextGameState_);
}
