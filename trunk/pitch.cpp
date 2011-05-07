/*
 * Copyright 2010,2011 Timothy Rochford
 *
 *    This file is part of CuteFootball.
 *
 *    CuteFootball is free software: you can redistribute it and/or modify
 *    it under the terms of the Lesser GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    CuteFootball is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Lesser GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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
#include "screengraphics.h"
#include "game.h"
#include "soundEffects.h"
#include "settingsFrame.h"
#include "cameraview.h"
#include "soccerutils.h"
#include "pitchscene.h"

Pitch::Pitch(const QRectF& footballGroundRect,
             QGraphicsView* view,
             SoundEffects* se,
             settingsFrame* settingsFrame)
  : QObject(),
    m_scene(new PitchScene(footballGroundRect)),
    m_view(view),
    m_motionTimer(NULL),
    m_bottomGoal(NULL),
    m_topGoal(NULL),
    m_centerLine(NULL),
    m_centerCircle(NULL),
    m_centerMark(NULL),
    m_settingsFrame(settingsFrame),
    m_soundEffects(se),
    m_teamMgr(new TeamManager)
{
    m_view->scale(1.4,1.4);

    m_view->setScene(m_scene);
    m_cameraView = new CameraView(*m_view, this);


    m_ball = new Ball(this);

    m_motionTimer = new QTimer(this);
    m_motionTimer->setInterval(KGameRefreshRate);

    m_gameFSM = new QStateMachine(this);
    m_game = new Game(*m_gameFSM, *this);
    connect(m_gameFSM, SIGNAL(started()), this, SLOT(gameStarted()));
    connect(m_gameFSM, SIGNAL(finished()), this, SLOT(gameStopped()));
    connect(m_gameFSM, SIGNAL(stopped()), this, SLOT(gameStopped()));

    layoutPitch();

    m_soundEffects->soundEnabled(m_settingsFrame->soundEnabled());

    m_teamMgr->createTeams();

    connect(m_motionTimer, SIGNAL(timeout()), m_cameraView, SLOT(update()));
    connect(m_motionTimer, SIGNAL(timeout()), m_scene, SLOT(advance()));
    connect(m_motionTimer, SIGNAL(timeout()), m_scene, SLOT(update()));
    connect(m_motionTimer, SIGNAL(timeout()), this, SLOT(hasBallCheck()));

    connect(m_motionTimer, SIGNAL(timeout()), this, SLOT(selectNearestPlayer()));
    connect(m_settingsFrame, SIGNAL(soundChanged(bool)), m_soundEffects, SLOT(soundEnabled(bool)));
}

Pitch::~Pitch()
{
    // some items are dependent on the scene...
    while (!m_adverts.isEmpty())
        delete m_adverts.takeFirst();

    m_scene->removeItem(m_screenGraphicsFrameProxy);
    delete m_screenGraphicsLabel;

    m_scene->removeItem(m_grass);
    delete m_grass;

    delete m_scene;

    if (m_motionTimer->isActive())
        m_motionTimer->stop();
    delete m_motionTimer;
    delete m_gameFSM;

    delete m_ball;
    delete m_teamMgr;
    delete m_cameraView;

}

void Pitch::centerOn(QPointF point)
{
    m_cameraView->centerOn(point);
}


void Pitch::centerOn(QGraphicsItem *item)
{
    m_cameraView->centerOn(item);
}

void Pitch::selectNearestPlayer()
{
    Player *p = selectNearestPlayer(m_homeTeam);
    if (p)
        m_scene->setFocusItem(p);
}

Player* Pitch::selectNearestPlayer(Team* team)
{
    qreal nearest = 0xffff;
    Player *nearestPlayer(NULL);
    // nobody has ball, select the closest hometeam
    foreach (Player *p, m_players) {
        if (p->team()== team) {
            // dont select the goal keeper
            if (p->m_role == Player::GoalKeeper)
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

void Pitch::gameStop()
{
    qDebug() << "Pitch::gameStop";
    if (m_gameFSM->isRunning())
        m_gameFSM->stop();
}

void Pitch::gameStarted()
{
    m_motionTimer->start();
    emit gameInProgress(true);
}

void Pitch::gameStopped()
{
    qDebug() << "Pitch::gameStopped";
    const QString initialStateName = static_cast<GameHalf*>(m_gameFSM->initialState())->objectName();
    if ( initialStateName == m_game->firstHalfState()->objectName()
            && extraTimeAllowed() && extraTime() ) {
        m_gameFSM->setInitialState(m_game->extraTimeFirstHalfState());
        m_gameFSM->start();
    } else if (initialStateName == m_game->extraTimeFirstHalfState()->objectName() && extraTime() ) {
        m_gameFSM->setInitialState(m_game->penaltiesState());
        m_gameFSM->start();
    } else {
        m_scene->removeItem(m_ball);

        foreach(Player *p, m_players)
            m_scene->removeItem(p);

        m_players.clear();

        m_motionTimer->stop();
        emit gameInProgress(false);
    }
}

void Pitch::setPiece(Team* originatingTeam, SetPiece s, QPointF foulLocation)
{
    m_soundEffects->soundEvent(SoundEffects::Whistle);
    switch(s) {
    case Pitch::Foul: // TODO foul logic
        //emit foul(originatingTeam, foulLocation);
        break;
    case Pitch::KickOff:
        emit kickOff(homeTeam());
        break;
    default:
        break;
    }
}
void Pitch::layoutPitchBorder()
{
    QPixmap advertVertical(m_footballPitch->rect().height(),10);
    advertVertical.fill(Qt::yellow);
    QPixmap advertHorizontal(m_footballPitch->rect().width(),10);
    advertHorizontal.fill(Qt::yellow);

    qreal rotation(90.0);
    // the right adverts
    QGraphicsPixmapItem* advert = m_scene->addPixmap(advertVertical);
    advert->setPos(m_footballPitch->rect().right()+10, m_footballPitch->rect().top());
    advert->setRotation(90.0);
    m_adverts.append(advert);

    advert = NULL;
    // the top adverts
    advert = m_scene->addPixmap(advertHorizontal);
    advert->setPos(m_footballPitch->rect().left(), m_footballPitch->rect().top()-10);
    m_adverts.append(advert);

    advert = NULL;
    // the left adverts
    advert = m_scene->addPixmap(advertVertical);
    advert->setRotation(-90.0);
    advert->setPos(m_footballPitch->rect().left()-10, m_footballPitch->rect().bottom());
    m_adverts.append(advert);
    advert = NULL;
    // the bottom adverts
    advert = m_scene->addPixmap(advertHorizontal);
    advert->setPos(m_footballPitch->rect().left(), m_footballPitch->rect().bottom());
    m_adverts.append(advert);
}

void Pitch::layoutPitch()
{
    const int KPitchBoundaryWidth = 40;
    QPixmap pitchUnscaled(QString(":/images/pitch3.png"));
    m_grass = new QGraphicsPixmapItem(pitchUnscaled /* pitchScaled */);
    m_scene->addItem(m_grass);

    // create the pitch
    m_footballPitch = m_scene->addRect(KPitchBoundaryWidth, KPitchBoundaryWidth,
                                       m_scene->width()-(KPitchBoundaryWidth*2), m_scene->height()-(KPitchBoundaryWidth*2),
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );

    layoutPitchBorder();

    // half way line
    m_centerLine = m_scene->addLine(m_footballPitch->rect().left(), (m_footballPitch->rect().height()/2.0)+KPitchBoundaryWidth,
                                    m_footballPitch->rect().right(),(m_footballPitch->rect().height()/2.0)+KPitchBoundaryWidth,
                                    KWhitePaintPen);

    // center circle
    m_centerCircle = m_scene->addEllipse((m_scene->width()/2.0)-40,(m_scene->height()/2.0)-40,
                      80.0, 80.0, KWhitePaintPen);
    // center mark
    m_centerMark = m_scene->addEllipse((m_scene->width()/2.0)-4,(m_scene->height()/2.0)-4,
                      4.0, 4.0, KWhitePaintPen);

    // simple text

    // half statistics frame
    m_screenGraphicsLabel = new ScreenGraphics(this);
    m_screenGraphicsFrameProxy = m_scene->addWidget(m_screenGraphicsLabel);
    m_screenGraphicsFrameProxy->setZValue(ZScoreText);
    m_cameraView->appendProxyWidget(m_screenGraphicsFrameProxy, CameraView::TopLeft );

#if 0
    m_goalTextLabel = new QLabel();
    m_goalTextLabel->setText(tr("GOAL"));
    m_goalTextLabelProxy = m_scene->addWidget(m_goalTextLabel);
    m_goalTextLabelProxy->setZValue(ZScoreText);
    m_cameraView->appendProxyWidget(m_goalTextLabelProxy, CameraView::Center );
#endif // 0
    // create the goals
    m_bottomGoal = m_scene->addRect((m_scene->width() / 2)-60, m_scene->height()-KPitchBoundaryWidth,120,KPitchBoundaryWidth/2,
                   KWhitePaintPen,
                   QBrush(Qt::white,Qt::Dense5Pattern) );
    m_bottomGoal->setZValue(ZGoalObject);
    m_topGoal = m_scene->addRect((m_scene->width() / 2)-60,KPitchBoundaryWidth/2,120,KPitchBoundaryWidth/2,
                   KWhitePaintPen,
                   QBrush(Qt::white,Qt::Dense5Pattern) );
    m_topGoal->setZValue(ZGoalObject);

    // penalty areas
    QPainterPath path;
    QRectF penaltyRectF((m_scene->width() / 2)-80, 0, 160, KPitchBoundaryWidth*2 );
    path.moveTo(penaltyRectF.center());
    path.arcTo(penaltyRectF,180.0,180.0);
    path.closeSubpath();

    m_topPenaltyArea = m_scene->addPath(path,
                                        KWhitePaintPen,
                                        QBrush(Qt::white,Qt::NoBrush) );

    QPainterPath path2;
    QRectF penaltyRectF2((m_scene->width() / 2)-80, m_scene->height()-(KPitchBoundaryWidth*2), 160, KPitchBoundaryWidth*2);
    path2.moveTo(penaltyRectF2.center());
    path2.arcTo(penaltyRectF2,180.0,-180.0);
    path2.closeSubpath();
    m_bottomPenaltyArea = m_scene->addPath(path2,
                                        KWhitePaintPen,
                                        QBrush(Qt::white,Qt::NoBrush) );

    m_entrancePoint = QPointF(0, (m_scene->sceneRect().height())/2);
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
}

void Pitch::pause()
{
    qDebug() << "pause";
    m_motionTimer->stop();
    emit pauseGameClock();
    if ( m_game->currentState() )
        m_game->currentState()->pauseGameClock();
}

void Pitch::continueGame()
{
    qDebug() << "continueGame";
    m_motionTimer->start();
    emit continueGameClock();
    if ( m_game->currentState() )
        m_game->currentState()->continueGameClock();
}


void Pitch::updateDisplayTime(int timeLeftMs)
{
    if ( m_gameFSM->isRunning() ) {
        QTime tmp(0,0,0,0);
        tmp = tmp.addMSecs(timeLeftMs);
        m_screenGraphicsLabel->update(tmp.toString(QString("m:ss")));
    } else {
        qDebug() << "updateDisplayTime FSM not running";
    }
}

void Pitch::hasBallCheck()
{
    // which team has the ball?
    Player* p = m_ball->lastPlayerToTouchBall();
    if ( p ) {
        m_homeTeam->setHasBall(p->team()== m_homeTeam);
        m_awayTeam->setHasBall(p->team()== m_awayTeam);
    }
}


void Pitch::countShots(Team* team, QPointF /* dest */)
{
    if (team)
        team->setShots(team->shots() + 1);
}

void Pitch::newGame(int homeTeam, int awayTeam)
{
    m_game->setHalfLength(m_settingsFrame->gameLengthMinutes());

    m_homeTeam = m_teamMgr->at(homeTeam);
    m_homeTeam->newGame();
    m_awayTeam = m_teamMgr->at(awayTeam);
    m_awayTeam->newGame();

    m_screenGraphicsLabel->setTeams(m_homeTeam, m_awayTeam);

    createTeamPlayers(m_homeTeam);
    createTeamPlayers(m_awayTeam);
    connect(m_ball, SIGNAL(shot(Team*,QPointF)), this, SLOT(countShots(Team*,QPointF)));
    connect(m_ball, SIGNAL(goalScored(bool)), m_awayTeam, SLOT(goalScored(bool)));
    connect(m_ball, SIGNAL(goalScored(bool)), m_homeTeam, SLOT(goalScored(bool)));
    connect(m_ball, SIGNAL(soundEvent(SoundEffects::GameSound)),
            m_soundEffects, SLOT(soundEvent(SoundEffects::GameSound)));
    m_soundEffects->soundEvent(SoundEffects::CrowdNoise);
    m_game->setTeamToKickOff(m_homeTeam);
    emit
    m_gameFSM->start();

}

void Pitch::createTeamPlayers(Team *team)
{
    QStringList names;

    bool isHomeTeam(false);
    if (team == m_homeTeam)
        isHomeTeam = true;

    QPointF startPos(0, m_scene->sceneRect().height()/2);

    // for each team in the directory
    QString f(":/teams/");
    f.append(team->fullName());
    f.append(".txt");

    QFile file(f);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    int i = 0;
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (line.contains('#'))
            continue;

        QList<QByteArray> nameAndPosition = line.split(',');
        QString name = nameAndPosition.at(0).simplified();
        Player::Role r = mapPositionStringToRole(nameAndPosition.at(1).simplified());
        QString hairColorString = nameAndPosition.at(2).simplified();
        QColor hairColor(hairColorString);
        QString skinColorStr = nameAndPosition.at(3).simplified();
        QColor skinColor(skinColorMapping(skinColorStr));

        Player *pl(NULL);

        if (r == Player::GoalKeeper) {
           pl = new GoalKeeper(
                    name,
                    i+1,
                    this,
                    team,
                    hairColor,
                    skinColor);

        } else {
            pl = new Player(
                    name,
                    i+1,
                    !isHomeTeam,
                    this,
                    team,
                    team->speed(),
                    r,
                    hairColor,
                    skinColor);
        }
        pl->createPixmaps();
        pl->setPos(startPos);

        m_players.append(pl);
        m_scene->addItem(pl);
        i++;
    }
    file.close();
}

void Pitch::setPlayerDefendZone(Player *p)
{
    bool nToS(false);
    if (p->team()->getDirection() == Team::NorthToSouth)
        nToS = true;

    QRectF zone;
    switch (p->m_role)
    {
    case Player::GoalKeeper:
        // goal keepers dont have a defend zone
        break;
    default:
        zone = p->m_startPositionRectF;
        break;
    }
    p->m_defendZone = zone;
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
    startPositions.insert(Player::CentralDefence,
                         m_pitchArea[nToS ? 1 : 6][2]);
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
                         m_pitchArea[nToS ? 2 : 5][2]);
    startPositions.insert(Player::RightAttack,
                         m_pitchArea[nToS ? 3 : 4][4]);

    foreach (Player *p, m_players) {
        if (p->team() == team) {
            p->setHasBall(false);
            p->m_startPositionRectF = startPositions[p->m_role];
            setPlayerDefendZone(p);
        }
    }
}
