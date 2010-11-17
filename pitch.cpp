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
#include "game.h"

const QPen KWhitePaintPen(QBrush(Qt::white),3);

Pitch::Pitch(const QRectF& footballGroundRect)
  : QObject(),
    scene(new QGraphicsScene(footballGroundRect)),
    view(new QGraphicsView(scene)),
    motionTimer_(NULL),
    lastNearestPlayer_(NULL),
    bottomGoal(NULL),
    topGoal(NULL),
    scoreText_(NULL),
    centerLine_(NULL),
    centerCircle_(NULL)
{
    motionTimer_ = new QTimer(this);
    motionTimer_->setInterval(KGameRefreshRate);

    game = new QStateMachine(this);
    firstHalfState = new Game(this, "first half", true);
    secondHalfState = new Game(this, "second half", false);
    allDone = new QFinalState();

    game->addState(firstHalfState);
    game->addState(secondHalfState);
    game->addState(allDone);
    game->setInitialState(firstHalfState);

    firstHalfState->addTransition(firstHalfState, SIGNAL(finished()),secondHalfState);
    secondHalfState->addTransition(secondHalfState, SIGNAL(finished()),allDone);

    replay_ = new Replay(this, this);

    scene->setBackgroundBrush(QPixmap(QString(":/images/pitch2.GIF")));

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setAutoFillBackground(false);

    layoutPitch();

    connect(motionTimer_, SIGNAL(timeout()), scene, SLOT(advance()));
    connect(motionTimer_, SIGNAL(timeout()), scene, SLOT(update()));
    connect(motionTimer_, SIGNAL(timeout()), this, SLOT(hasBallCheck()));
    connect(motionTimer_, SIGNAL(timeout()), this, SLOT(selectNearestPlayer()));
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
    m_footballPitch = scene->addRect(30, 30, scene->width()-60, scene->height() -60,
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );

    // divide the pitch into areas
    // makes it easier for computer based movement
    QPointF tlTopHalf = m_footballPitch->rect().topLeft();
    QPointF brBottomHalf = m_footballPitch->rect().bottomRight();
    QPointF brTopHalf = brBottomHalf - QPointF(m_footballPitch->rect().height()/2,0);
    QPointF tlBottomHalf = tlTopHalf + QPointF(m_footballPitch->rect().height()/2,0);

    const qreal w = m_footballPitch->rect().width();
    const qreal h = m_footballPitch->rect().height();

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
    centerLine_ = scene->addLine(scene->sceneRect().left()+30,scene->sceneRect().height()/2.0,
                         scene->sceneRect().right()-30,scene->sceneRect().height()/2.0, KWhitePaintPen);

    // center circle
    centerCircle_ = scene->addEllipse((scene->sceneRect().width()/2.0) -80,(scene->sceneRect().height()/2.0)-80,
                      160.0, 160.0, KWhitePaintPen);
    // simple text
    scoreText_ = new ScreenGraphics(this);

    // create the goals
    bottomGoal = scene->addRect((scene->width() / 2)-60, scene->height()-30,120,25,
                   QPen(Qt::black),
                   QBrush(Qt::black,Qt::CrossPattern) );
    topGoal = scene->addRect((scene->width() / 2)-60,5,120,25,
                   QPen(Qt::black),
                   QBrush(Qt::black,Qt::CrossPattern) );

    // penalty areas
    topPenaltyArea = scene->addRect((scene->width() / 2)-90, 30,
                                    180, 100,
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );
    bottomPenaltyArea = scene->addRect((scene->width() / 2)-90, scene->height()-130,
                                       180, 100,
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );
}

void Pitch::updateDisplayTime(int timeLeftMs)
{
    if ( game->isRunning() ) {
        QTime tmp(0,0,0,0);
        tmp = tmp.addMSecs(timeLeftMs);

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
    ball_ = new Ball(this);

    homeTeam_ = new Team(QString("HOME"), Qt::magenta);
    awayTeam_ = new Team(QString("AWAY"), Qt::yellow);

    createTeamPlayers(homeTeam_);
    connect(ball_, SIGNAL(goalScored(bool)), homeTeam_, SLOT(goalScored(bool)));
    createTeamPlayers(awayTeam_);
    connect(ball_, SIGNAL(goalScored(bool)), awayTeam_, SLOT(goalScored(bool)));

    referee_ = new Referee(this);

    replay_->createAnimationItems();

    game->start();

    motionTimer_->start();
}

void Pitch::action(MWindow::Action action)
{
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

    QPointF startPos(0,scene->sceneRect().height()/2);
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
        pl->createMoves();
        pl->setPos(startPos);
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

void Pitch::replayStart()
{
    motionTimer_->stop();

    scoreText_->setMode(ScreenGraphics::ReplayMode);
    scoreText_->setText("REPLAY");

    replay_->replayStart();
}

void Pitch::replayStop()
{
    scoreText_->setMode(ScreenGraphics::NormalMode);
    motionTimer_->start();
}
