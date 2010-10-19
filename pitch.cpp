#include <QtGui>
#include <QtGlobal>
#include <QDebug>

#include "pitch.h"
#include "ball.h"
#include "Player.h"
#include "team.h"

Pitch::Pitch(const QRectF& footballGroundRect,
             const QRectF& footballPitchRect,
             QLabel* scoreLabel)
  : QObject(),
    scene(new QGraphicsScene(footballGroundRect)),
    view(new QGraphicsView(scene)),
    footballPitch_(footballPitchRect),
    motionTimer_(new QTimer),
    gameTimer_(new QTimer),
    nextGameState_(NotStarted),
    lastNearestPlayer(NULL),
    bottomGoal(NULL),
    topGoal(NULL),
    scoreLabel_(scoreLabel)
{
    scene->setBackgroundBrush(QPixmap(QString(":/images/pitch2.GIF")));

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setAutoFillBackground(false);

    // divide the pitch into areas
    // makes it easier for computer based movement
    QPointF tlTopHalf = scene->sceneRect().topLeft();
    QPointF brBottomHalf = scene->sceneRect().bottomRight();
    QPointF brTopHalf = brBottomHalf - QPointF(scene->sceneRect().height()/2,0);
    QPointF tlBottomHalf = tlTopHalf + QPointF(scene->sceneRect().height()/2,0);

    const int w = scene->sceneRect().width();
    const int h = scene->sceneRect().height();


    for (int row = 0; row < KRow; row++) {
        for (int col = 0; col < KColumn; col++) {
            qreal tlx = (w / KColumn) * col;
            qreal tly = (h / KRow ) * row;
            QPointF tl(tlTopHalf + QPointF(tlx,tly));
            QPointF br(tl + QPointF(w/KColumn,h/KRow));
            pitchArea[row][col] = QRectF(tl, br);
        }
    }

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
                                    QPen(Qt::white),
                                    QBrush(Qt::white,Qt::NoBrush) );
    bottomPenaltyArea = scene->addRect((scene->width() / 2)-70, scene->height()-130,
                                       140, 100,
                                    QPen(Qt::white),
                                    QBrush(Qt::white,Qt::NoBrush) );
    connect(motionTimer_, SIGNAL(timeout()), scene, SLOT(advance()));
    connect(motionTimer_, SIGNAL(timeout()), scene, SLOT(update()));
    connect(motionTimer_, SIGNAL(timeout()), this, SLOT(hasBallCheck()));
    connect(motionTimer_, SIGNAL(timeout()), this, SLOT(selectNearestPlayer()));
    connect(motionTimer_, SIGNAL(timeout()), this, SLOT(updateDisplayTime()));

    connect(gameTimer_, SIGNAL(timeout()), this, SLOT(kickOff()));
}

Player* Pitch::selectNearestPlayer(Team* team)
{
    qreal nearest = 0xffff;
    Player *nearestPlayer(NULL);
    Ball *ball = getBall();
    // nobody has ball, select the closest hometeam
    foreach (Player *p, players) {
        if ( p->team_== team ) {

            if (!nearestPlayer)
                nearestPlayer = p;

            if ( p->hasBall_ ) {
                nearestPlayer = p;
                break;
            }
            const int dx = p->pos().x() - ball->pos().x();
            const int dy = p->pos().y() - ball->pos().y();

            p->setHumanControlled(false);

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
    if (motionTimer_) {
        motionTimer_->stop();
        delete motionTimer_;
    }
    delete awayTeam_;
    delete homeTeam_;
}

void Pitch::removePlayers()
{
    foreach( Player *p, this->players)
        scene->removeItem(p);

    players.clear();
}

void Pitch::kickOff(Game state)
{
    motionTimer_->stop();
    gameTimer_->stop();

    QTime now = QTime::currentTime();
    remainingGameTime_ = (KGameLength/1000) - startTimeStamp_.secsTo(now);

    qDebug() << "kickOff remaining" << remainingGameTime_;

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
        removePlayers();
        updateDisplayTime();
        break;
    }

    if (needRestartTimers) {
        gameTimer_->start(KGameLength);
        motionTimer_->start(KGameRefreshRate);
    }
}

void Pitch::kickOff()
{
    kickOff(nextGameState_);
}

void Pitch::updateDisplayTime()
{
    switch(nextGameState_) {
    case Finished:
        scoreLabel_->setText(QString("GAME OVER!!!"));
        break;
    case NotStarted:
        break;
    case GoalScored:
    case FirstHalfOver:
    case SecondHalfOver:
        QTime now = QTime::currentTime();
        QString str =  QString::number(startTimeStamp_.secsTo(now));
        str.append(" ");
        str.append(homeTeam_->name());
        str.append(" ");
        str.append(QString::number(homeTeam_->goals_));
        str.append(" vs ");

        str.append(awayTeam_->name());
        str.append(" ");
        str.append(QString::number(awayTeam_->goals_));
        scoreLabel_->setText(str);
        break;
    }
}

void Pitch::hasBallCheck()
{
    view->centerOn(this->ball_->pos());
    // which team has the ball?
    foreach (Player *p, players) {
        if ( p->hasBall_ ) {
            homeTeam_->setHasBall(p->team_== this->homeTeam_);
            awayTeam_->setHasBall(p->team_== this->awayTeam_);
            break; // can break as now known
        }
    }
}

void Pitch::newGame()
{
    startTimeStamp_ = QTime::currentTime();

    ball_ = new Ball(this);

    connect(ball_, SIGNAL(goalScored(bool)), this, SLOT(goalScored(bool)));

    homeTeam_ = new Team(QString("HOME"), Qt::red);
    awayTeam_ = new Team(QString("AWAY"), Qt::blue);

    createTeamPlayers(homeTeam_);
    connect(ball_, SIGNAL(goalScored(bool)), homeTeam_, SLOT(goalScored(bool)));
//    createTeamPlayers(awayTeam_);
//    connect(ball_, SIGNAL(goalScored(bool)), awayTeam_, SLOT(goalScored(bool)));
    kickOff();
}

void Pitch::pausedGame()
{
    // figure out how long the game has left
    QTime now = QTime::currentTime();
    remainingGameTime_ = (KGameLength/1000) - startTimeStamp_.secsTo(now);

    qDebug() << "pausedGame remaining" << remainingGameTime_;

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
    foreach (Player *p, players) {
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
        Player *pl = new Player(
                !isHomeTeam,
                this,
                team,
                (Player::Role)i);
        players.append(pl);
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
    foreach (Player *p, players) {
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
    foreach (Player *p, players) {
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
                         pitchArea[nToS ? 0 : 6][1]);
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
    foreach (Player *p, players) {
        if (p->team_ == team)
            p->attackPosition_ = attackPositions[p->role_];
    }
}

void Pitch::goalScored(bool isNorthGoal)
{
    qDebug() << "GOAL !!!";

    kickOff(Pitch::GoalScored);
}
