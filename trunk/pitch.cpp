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
#include "goalkeeper.h"
#ifdef REPLAY_FEATURE
#include "replay.h"
#endif // REPLAY_FEATURE
#include "screengraphics.h"
#include "game.h"
#include "soundEffects.h"
#include "settingsdialog.h"


Pitch::Pitch(const QRectF& footballGroundRect,
             QGraphicsView* view,
             SoundEffects* se,
             QWidget* frame,
             settingsDialog* settingsDlg)
  : QObject(),
    m_scene(new QGraphicsScene(footballGroundRect)),
    m_view(view),
    m_motionTimer(NULL),
    m_bottomGoal(NULL),
    m_topGoal(NULL),
    m_scoreText(NULL),
    m_centerLine(NULL),
    m_centerCircle(NULL),
    m_menuFrame(frame),
    m_settingsDlg(settingsDlg),
    m_soundEffects(se)
{
    m_view->setScene(m_scene);
    m_scene->setBackgroundBrush(QPixmap(QString(":/images/pitch2.GIF")));

    m_motionTimer = new QTimer(this);
    m_motionTimer->setInterval(KGameRefreshRate);

    m_game = new QStateMachine(this);
    m_firstHalfState = new Game(this, "first half", true, false);
    m_secondHalfState = new Game(this, "second half", false, false);
    m_extraTimeFirstHalfState = new Game(this, "extra time first half", true, true);
    m_extraTimeSecondHalfState = new Game(this, "extra time second half", false, true);
    m_allDone = new QFinalState();

    m_game->addState(m_firstHalfState);
    m_game->addState(m_secondHalfState);
    m_game->addState(m_allDone);
    m_game->setInitialState(m_firstHalfState);

    m_firstHalfState->addTransition(m_firstHalfState, SIGNAL(finished()),m_secondHalfState);
    m_secondHalfState->addTransition(m_secondHalfState, SIGNAL(finished()), m_allDone);
    connect(m_allDone, SIGNAL(entered()), this, SLOT(removePlayers()));
    connect(m_allDone, SIGNAL(entered()), this, SLOT(gameStopped()));

#ifdef REPLAY_FEATURE
    m_replay = new Replay(this, this);
#endif // REPLAY_FEATURE
    m_proxyMenuFrame = m_scene->addWidget(m_menuFrame);
    m_proxyMenuFrame->setZValue(20);

    m_proxySettingsDlg = m_scene->addWidget((QWidget*)m_settingsDlg);
    m_proxySettingsDlg->setZValue(20);

    layoutPitch();

    m_soundEffects->soundEnabled(m_settingsDlg->soundEnabled());

    createTeams();

    connect(m_motionTimer, SIGNAL(timeout()), m_scene, SLOT(advance()));
    connect(m_motionTimer, SIGNAL(timeout()), m_scene, SLOT(update()));
    connect(m_motionTimer, SIGNAL(timeout()), this, SLOT(hasBallCheck()));
    connect(m_motionTimer, SIGNAL(timeout()), this, SLOT(selectNearestPlayer()));
    connect(m_settingsDlg, SIGNAL(soundChanged(bool)), m_soundEffects, SLOT(soundEnabled(bool)));
}


Player* Pitch::selectNearestPlayer(Team* team)
{
    qreal nearest = 0xffff;
    Player *nearestPlayer(NULL);
    // nobody has ball, select the closest hometeam
    foreach (Player *p, m_players) {
        if (p->team_== team) {
            // dont select the goal keeper
            if (p->role_ == Player::GoalKeeper)
                continue;

            if (!nearestPlayer)
                nearestPlayer = p;

            const int dx = p->pos().x() - m_ball->pos().x();
            const int dy = p->pos().y() - m_ball->pos().y();
            if ( (qAbs(dx*dx)+qAbs(dy*dy)) < nearest) {
                nearestPlayer = p;
                nearest = qAbs(qAbs(dx*dx)+qAbs(dy*dy));
            }
        }
    }
    return nearestPlayer;
}

void Pitch::gameStarted()
{
    m_menuFrame->setVisible(false);
    m_settingsDlg->setVisible(false);
    m_motionTimer->start();
    emit gameInProgress(true);
}

void Pitch::gameStopped()
{
    m_motionTimer->stop();
    m_menuFrame->setVisible(true);
//    m_settingsDlg->setVisible(true);
    emit gameInProgress(false);
}

void Pitch::setPiece(Team* t, SetPiece s)
{
    m_soundEffects->soundEvent(SoundEffects::Whistle);
    switch(s) {
    case Pitch::KickOff:
        foreach (Player *p, m_players) {
                p->m_hasBall = false;
                p->setPos(p->startPosition_.center());
            }
        if (t == m_awayTeam) {
            m_awayTeam->setHasBall(true);
            m_homeTeam->setHasBall(false);
        } else {
            m_awayTeam->setHasBall(false);
            m_homeTeam->setHasBall(true);
        }
        m_scene->addItem(m_ball);
        ball()->setVisible(true);
        m_ball->setStartingPosition();
        break;
#ifndef INDOOR
    case Pitch::ThrowIn:
        {
            if (t == m_awayTeam) {
                m_awayTeam->setHasBall(true);
                m_homeTeam->setHasBall(false);
            } else {
                m_awayTeam->setHasBall(false);
                m_homeTeam->setHasBall(true);
            }

            // find the nearest player to the ball and move them to the throw in position
            Player* throwInTaker = selectNearestPlayer(t);
            if (throwInTaker) {
                // change the player graphic to be throw in graphic
                throwInTaker->setPos(m_ball->pos());
                throwInTaker->hasBall_ = true;
                m_ball->setControlledBy(throwInTaker);
                throwInTaker->move(MWindow::ThrownIn);
                // hide the ball
               m_ball->setVisible(false);
            }
        }
        break;

    case Pitch::GoalKick:
        {
            if (t == m_awayTeam) {
                m_awayTeam->setHasBall(true);
                m_homeTeam->setHasBall(false);
            } else {
                m_awayTeam->setHasBall(false);
                m_homeTeam->setHasBall(true);
            }

            Player *taker = NULL;
            foreach (Player *p, m_players) {
                if (p->team_== t) {

                    // dont select the goal keeper
                    if (p->role_ == Player::GoalKeeper)
                        taker = p;
                        break;
                }
            }

            if (taker) {
                taker->setPos(m_ball->pos());
                taker->hasBall_ = true;
                m_ball->setControlledBy(taker);
                taker->move(MWindow::Shot);
                // hide the ball
            //   m_ball->setVisible(false);
            }
        }
        break;
#endif // INDOOR
    default:
        break;
    }
}

void Pitch::selectNearestPlayer()
{
    Player *p = selectNearestPlayer(m_homeTeam);
    if (p)
        m_scene->setFocusItem(p);
}

Pitch::~Pitch()
{
#ifdef REPLAY_FEATURE
    delete m_replay;
#endif // REPLAY_FEATURE
    if (m_motionTimer->isActive())
        m_motionTimer->stop();
    delete m_motionTimer;
    delete m_game;
}

void Pitch::removePlayers()
{
    foreach(Player *p, m_players)
        m_scene->removeItem(p);

    m_players.clear();
}

void Pitch::layoutPitch()
{
    // create the pitch
    m_footballPitch = m_scene->addRect(30, 30, m_scene->width()-60, m_scene->height() -60,
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
            m_pitchArea[row][col] = QRectF(tl, br);
        }
    }

    // half way line
    m_centerLine = m_scene->addLine(m_scene->sceneRect().left()+30,m_scene->sceneRect().height()/2.0,
                         m_scene->sceneRect().right()-30,m_scene->sceneRect().height()/2.0, KWhitePaintPen);

    // center circle
    m_centerCircle = m_scene->addEllipse((m_scene->sceneRect().width()/2.0) -80,(m_scene->sceneRect().height()/2.0)-80,
                      160.0, 160.0, KWhitePaintPen);
    // simple text
    m_scoreText = new ScreenGraphics(this);

    // create the goals
    m_bottomGoal = m_scene->addRect((m_scene->width() / 2)-60, m_scene->height()-30,120,25,
                   QPen(Qt::black),
                   QBrush(Qt::black,Qt::CrossPattern) );
    m_topGoal = m_scene->addRect((m_scene->width() / 2)-60,5,120,25,
                   QPen(Qt::black),
                   QBrush(Qt::black,Qt::CrossPattern) );

    // penalty areas
    m_topPenaltyArea = m_scene->addRect((m_scene->width() / 2)-90, 30,
                                    180, 100,
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );
    m_bottomPenaltyArea = m_scene->addRect((m_scene->width() / 2)-90, m_scene->height()-130,
                                       180, 100,
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );

    m_entrancePoint = QPointF(0, m_scene->sceneRect().height()/2);
}

void Pitch::updateDisplayTime(int timeLeftMs)
{
    if ( m_game->isRunning() ) {
        QTime tmp(0,0,0,0);
        tmp = tmp.addMSecs(timeLeftMs);

        QString str(tmp.toString(QString(tr("mm:ss"))));
        str.append(" ");
        str.append(m_homeTeam->name());
        str.append(" ");
        str.append(QString::number(m_homeTeam->goals_));
        str.append(" - ");

        str.append(m_awayTeam->name());
        str.append(" ");
        str.append(QString::number(m_awayTeam->goals_));
        m_scoreText->setText(str);
    }
}

void Pitch::hasBallCheck()
{
    m_view->centerOn(m_ball->pos());
    m_scoreText->updatePosition();

    // which team has the ball?
    Player* p = m_ball->lastPlayerToTouchBall();
    if ( p ) {
        m_homeTeam->setHasBall(p->team_== m_homeTeam);
        m_awayTeam->setHasBall(p->team_== m_awayTeam);
    }
}

void Pitch::createTeams()
{
    QStringList teamNames;
    teamNames << "United" << "City" << "Dynamo" << "Galaxy"
              << "Town" << "Athletic" << "Real" << "Sporting";
    QList<Qt::GlobalColor> colours;
    colours << Qt::darkBlue << Qt::magenta << Qt::darkRed << Qt::yellow
            << Qt::black << Qt::gray << Qt::white << Qt::darkYellow;
    for (int i = 0; i < 8; ++i) {
        Team* t = new Team(teamNames.at(i), colours.at(i));
        m_teams.append(t);
    }
}

void Pitch::newGame()
{
    m_firstHalfState->setGameLength(m_settingsDlg->gameLengthMinutes());
    m_secondHalfState->setGameLength(m_settingsDlg->gameLengthMinutes());
    m_menuFrame->setVisible(false);
    m_ball = new Ball(this);

    m_homeTeam = m_teams.at(0);
    m_awayTeam = m_teams.at(1);

    createTeamPlayers(m_homeTeam);
    createTeamPlayers(m_awayTeam);
    connect(m_ball, SIGNAL(goalScored(bool)), m_awayTeam, SLOT(goalScored(bool)));
    connect(m_ball, SIGNAL(goalScored(bool)), m_homeTeam, SLOT(goalScored(bool)));
    connect(m_ball, SIGNAL(soundEvent(SoundEffects::GameSound)),
            m_soundEffects, SLOT(soundEvent(SoundEffects::GameSound)));
#ifdef REPLAY_FEATURE
    m_replay->createAnimationItems();
#endif // REPLAY_FEATURE
    m_game->start();
}

void Pitch::createTeamPlayers(Team *team)
{
    QStringList names;
    names << "adam" << "bob" << "charlie" << "dave"
          << "ed" << "frank" << "george" << "harry"
          << "ian" << "jack" << "kai" << "luke"
          << "matt" << "nigel" << "oscar" << "pete"
          << "roger" << "steve" << "tom" << "walt";
    bool isHomeTeam(false);

    if (team == m_homeTeam)
        isHomeTeam = true;

    QList<Player::Role> formation;
#ifdef INDOOR
    formation << Player::GoalKeeper
              << Player::LeftCentralDefence
              << Player::RightCentralDefence
              << Player::LeftMidfield
              << Player::RightMidfield
              << Player::CentralAttack;
#else
    formation << Player::GoalKeeper
              << Player::LeftDefence
              << Player::LeftCentralDefence
              << Player::RightCentralDefence
              << Player::RightDefence
              << Player::LeftMidfield
              << Player::CentralMidfield
              << Player::RightMidfield
              << Player::LeftAttack
              << Player::CentralAttack
              << Player::RightAttack;
#endif // INDOOR

    QPointF startPos(0, m_scene->sceneRect().height()/2);
    int i = 0;
    foreach( Player::Role r, formation ) {
        Player *pl(NULL);
        if (r == Player::GoalKeeper) {
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
                    r);
        }
        pl->createPixmaps();
        pl->createMoves();
        pl->setPos(startPos);
        m_players.append(pl);
        m_scene->addItem(pl);
        i++;
    }
}

void Pitch::setPlayerStartPositions(Team *team)
{
    bool nToS(false);
    if (team->getDirection() == Team::NorthToSouth)
        nToS = true;

    QMap<int,QRectF> startPositions;
    startPositions.insert(Player::GoalKeeper,
                         m_pitchArea[nToS ? 0 : 7][2]);
    startPositions.insert(Player::LeftDefence,
                         m_pitchArea[nToS ? 1 : 6][0]);
    startPositions.insert(Player::LeftCentralDefence,
                         m_pitchArea[nToS ? 1 : 6][1]);
    startPositions.insert(Player::RightCentralDefence,
                         m_pitchArea[nToS ? 1 : 6][3]);
    startPositions.insert(Player::RightDefence,
                         m_pitchArea[nToS ? 1 : 6][4]);
    startPositions.insert(Player::LeftMidfield,
                         m_pitchArea[nToS ? 2 : 5][0]);
    startPositions.insert(Player::CentralMidfield,
                         m_pitchArea[nToS ? 2 : 5][2]);
    startPositions.insert(Player::RightMidfield,
                         m_pitchArea[nToS ? 2 : 5][4]);
    startPositions.insert(Player::LeftAttack,
                         m_pitchArea[nToS ? 3 : 4][0]);
    startPositions.insert(Player::CentralAttack,
                         m_pitchArea[nToS ? 3 : 4][2]);
    startPositions.insert(Player::RightAttack,
                         m_pitchArea[nToS ? 3 : 4][4]);

    // action is only applicabled to the human controlled player
    foreach (Player *p, m_players) {
        if (p->team_ == team) {
            p->m_hasBall = false;
            p->startPosition_ = startPositions[p->role_];
        }
    }
}
#ifdef REPLAY_FEATURE
void Pitch::replayStart()
{
    m_scene->setBackgroundBrush(QBrush(QColor(Qt::gray)));
    m_motionTimer->stop();

    m_scoreText->setMode(ScreenGraphics::ReplayMode);
    m_scoreText->setText(tr("REPLAY"));

    m_replay->replayStart();
}

void Pitch::replayStop()
{
    m_scene->setBackgroundBrush(QPixmap(QString(":/images/pitch2.GIF")));



    m_scoreText->setMode(ScreenGraphics::NormalMode);
    m_motionTimer->start();
}
#endif // REPLAY_FEATURE
void Pitch::playGameSound(SoundEffects::GameSound s)
{
    m_soundEffects->soundEvent(s);
}
