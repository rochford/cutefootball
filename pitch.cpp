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


Pitch::Pitch(const QRectF& footballGroundRect,
             QGraphicsView* view,
             SoundEffects* se,
             settingsFrame* settingsFrame)
  : QObject(),
    m_scene(new QGraphicsScene(footballGroundRect)),
    m_view(view),
    m_motionTimer(NULL),
    m_bottomGoal(NULL),
    m_topGoal(NULL),
    m_scoreText(NULL),
    m_centerLine(NULL),
    m_centerCircle(NULL),
    m_centerMark(NULL),
    m_settingsFrame(settingsFrame),
    m_soundEffects(se)
{
    m_view->scale(1.5,1.5);
    m_view->setScene(m_scene);
    m_scene->setBackgroundBrush(QBrush(Qt::blue));

    m_motionTimer = new QTimer(this);
    m_motionTimer->setInterval(KGameRefreshRate);

    m_game = new QStateMachine(this);
    m_firstHalfState = new Game(this, tr("First half"), true, false);
    m_secondHalfState = new Game(this, tr("Second half"), false, false);
    m_extraTimeFirstHalfState = new Game(this, tr("extra time first half"), true, true);
    m_extraTimeSecondHalfState = new Game(this, tr("extra time second half"), false, true);
    m_allDone = new QFinalState();

    m_game->addState(m_firstHalfState);
    m_game->addState(m_secondHalfState);
    m_game->addState(m_extraTimeFirstHalfState);
    m_game->addState(m_extraTimeSecondHalfState);
    m_game->addState(m_allDone);
    m_game->setInitialState(m_firstHalfState);

    m_firstHalfState->addTransition(m_firstHalfState, SIGNAL(finished()),m_secondHalfState);
//    m_secondHalfState->addTransition(m_secondHalfState, SIGNAL(finished()), m_allDone);
    m_secondHalfState->addTransition(m_secondHalfState, SIGNAL(finished()), m_extraTimeFirstHalfState);
    m_extraTimeFirstHalfState->addTransition(m_extraTimeFirstHalfState, SIGNAL(finished()), m_extraTimeSecondHalfState);
    m_extraTimeSecondHalfState->addTransition(m_extraTimeSecondHalfState, SIGNAL(finished()), m_allDone);

    connect(m_allDone, SIGNAL(entered()), this, SLOT(removePlayers()));
    connect(m_allDone, SIGNAL(entered()), this, SLOT(gameStopped()));

    layoutPitch();

    m_soundEffects->soundEnabled(m_settingsFrame->soundEnabled());

    createTeams();

    connect(m_motionTimer, SIGNAL(timeout()), m_scene, SLOT(advance()));
    connect(m_motionTimer, SIGNAL(timeout()), m_scene, SLOT(update()));
    connect(m_motionTimer, SIGNAL(timeout()), this, SLOT(hasBallCheck()));
    connect(m_motionTimer, SIGNAL(timeout()), this, SLOT(selectNearestPlayer()));
    connect(m_settingsFrame, SIGNAL(soundChanged(bool)), m_soundEffects, SLOT(soundEnabled(bool)));
}

Pitch::~Pitch()
{
    delete m_scene;

    if (m_motionTimer->isActive())
        m_motionTimer->stop();
    delete m_motionTimer;
    delete m_game;
    delete m_firstHalfState;
    delete m_secondHalfState;
    delete m_extraTimeFirstHalfState;
    delete m_extraTimeSecondHalfState;
    delete m_allDone;
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

void Pitch::gameStarted()
{
    m_motionTimer->start();
    emit gameInProgress(true);
}

void Pitch::gameStopped()
{
    m_motionTimer->stop();
    emit gameInProgress(false);
}

void Pitch::setPiece(Team* t, SetPiece s)
{
    m_soundEffects->soundEvent(SoundEffects::Whistle);
    switch(s) {
    case Pitch::Foul: // TODO foul logic
        // emit foul();
        break;
    case Pitch::KickOff:
        foreach (Player *p, m_players) {
                p->setHasBall(false);
                p->setPos(p->m_startPositionRectF.center());
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


void Pitch::removePlayers()
{
    foreach(Player *p, m_players)
        m_scene->removeItem(p);

    m_players.clear();
}

void Pitch::layoutPitch()
{
    QPixmap advertTop(":/images/advertTop.png");
    QPixmap advertRight(":/images/advertRight.png");
    QPixmap advertLeft(":/images/advertLeft.png");
    QPixmap advertBottom(":/images/advertBottom.png");

    const int KPitchBoundaryWidth = 40;
    QPixmap pitchUnscaled(QString(":/images/pitch3.png"));
    QPixmap pitchScaled = pitchUnscaled.scaled(QSize(m_scene->width(),m_scene->height()));
    m_grass = new QGraphicsPixmapItem(pitchScaled);
    m_scene->addItem(m_grass);

    // create the pitch
    m_footballPitch = m_scene->addRect(KPitchBoundaryWidth, KPitchBoundaryWidth,
                                       m_scene->width()-(KPitchBoundaryWidth*2), m_scene->height()-(KPitchBoundaryWidth*2),
                                    KWhitePaintPen,
                                    QBrush(Qt::white,Qt::NoBrush) );
    // top line adverts
    QPen advertPen(QBrush(advertTop),10);
    m_scene->addLine(m_footballPitch->rect().left(), (m_footballPitch->rect().top()-advertPen.width()),
                     (m_scene->width() / 2)-60-KWhitePaintPen.width()*4,(m_footballPitch->rect().top()-advertPen.width()),
                     advertPen);
    m_scene->addLine((m_scene->width() / 2)+60+KWhitePaintPen.width()*4, (m_footballPitch->rect().top()-advertPen.width()),
                     m_footballPitch->rect().right(),(m_footballPitch->rect().top()-advertPen.width()),
                     advertPen);
    // bottom line adverts
    advertPen = QPen(QBrush(advertBottom),10);
    m_scene->addLine(m_footballPitch->rect().left(), (m_footballPitch->rect().bottom()+advertPen.width()),
                     (m_scene->width() / 2)-60-KWhitePaintPen.width()*4,(m_footballPitch->rect().bottom()+advertPen.width()),
                     advertPen);
    m_scene->addLine((m_scene->width() / 2)+60+KWhitePaintPen.width()*4, (m_footballPitch->rect().bottom()+advertPen.width()),
                     m_footballPitch->rect().right(),(m_footballPitch->rect().bottom()+advertPen.width()),
                     advertPen);

    // right line adverts
    advertPen = QPen(QBrush(advertRight),10);
    m_scene->addLine(m_footballPitch->rect().right()+advertPen.width(), (m_footballPitch->rect().top()-advertPen.width()),
                     m_footballPitch->rect().right()+advertPen.width(), (m_footballPitch->rect().bottom()+advertPen.width()),
                     advertPen);
    // left line adverts
    advertPen = QPen(QBrush(advertLeft),10);
    m_scene->addLine(m_footballPitch->rect().left()-advertPen.width(), (m_footballPitch->rect().top()-advertPen.width()),
                     m_footballPitch->rect().left()-advertPen.width(), (m_footballPitch->rect().bottom()+advertPen.width()),
                     advertPen);


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
    m_scoreText = new ScreenGraphics(this);

    // create the goals
    m_bottomGoal = m_scene->addRect((m_scene->width() / 2)-60, m_scene->height()-KPitchBoundaryWidth,120,KPitchBoundaryWidth/2,
                   KWhitePaintPen,
                   QBrush(Qt::white,Qt::Dense5Pattern) );
    m_topGoal = m_scene->addRect((m_scene->width() / 2)-60,KPitchBoundaryWidth/2,120,KPitchBoundaryWidth/2,
                   KWhitePaintPen,
                   QBrush(Qt::white,Qt::Dense5Pattern) );

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

void Pitch::updateDisplayTime(int timeLeftMs)
{
    if ( m_game->isRunning() ) {
        QString homeTeam(m_homeTeam->name());
        homeTeam.truncate(3);

        QString awayTeam(m_awayTeam->name());
        awayTeam.truncate(3);
        QTime tmp(0,0,0,0);
        tmp = tmp.addMSecs(timeLeftMs);

        QString str(tmp.toString(QString("mm:ss")));
        str.append(" ");

        str.append(homeTeam);
        str.append(" ");
        str.append(QString::number(m_homeTeam->m_goals));
        str.append(" - ");

        str.append(awayTeam);
        str.append(" ");
        str.append(QString::number(m_awayTeam->m_goals));
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
        m_homeTeam->setHasBall(p->team()== m_homeTeam);
        m_awayTeam->setHasBall(p->team()== m_awayTeam);
    }
}

void Pitch::parseTeamList()
{
    QStringList colorNames(QColor::colorNames());

    // for each team in the directory
    QFile file(":/teams/teams.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QList<QByteArray> nameAndColor = line.split(',');
        QString name = nameAndColor.at(0).simplified();

        QString shirtColorString = nameAndColor.at(1).simplified();
        QString shortColorString = nameAndColor.at(2).simplified();

        QColor shirtColor(shirtColorString);
        QColor shortColor(shortColorString);

        Team* t = new Team(name, shirtColor, shortColor);
        m_teams.append(t);
    }
    file.close();
}

void Pitch::createTeams()
{
    parseTeamList();
}

void Pitch::newGame(int homeTeam, int awayTeam)
{
    m_firstHalfState->setGameLength(m_settingsFrame->gameLengthMinutes());
    m_secondHalfState->setGameLength(m_settingsFrame->gameLengthMinutes());

    m_extraTimeFirstHalfState->setGameLength(1);
    m_extraTimeSecondHalfState->setGameLength(1);

    m_ball = new Ball(this);

    m_homeTeam = m_teams.at(homeTeam);
    m_homeTeam->newGame();
    m_awayTeam = m_teams.at(awayTeam);
    m_awayTeam->newGame();

    createTeamPlayers(m_homeTeam);
    createTeamPlayers(m_awayTeam);
    connect(m_ball, SIGNAL(goalScored(bool)), m_awayTeam, SLOT(goalScored(bool)));
    connect(m_ball, SIGNAL(goalScored(bool)), m_homeTeam, SLOT(goalScored(bool)));
    connect(m_ball, SIGNAL(soundEvent(SoundEffects::GameSound)),
            m_soundEffects, SLOT(soundEvent(SoundEffects::GameSound)));

    m_game->start();
}

QStringList Pitch::parsePlayers(QString teamName)
{
    QStringList names;

    // for each team in the directory
    QString f(":/teams/");
    f.append(teamName);
    f.append(".txt");

    QFile file(f);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return names;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();

        QString name(line.simplified());
        qDebug() << name;
        names.append(name);
    }
    file.close();
    return names;
}

void Pitch::createTeamPlayers(Team *team)
{
    QStringList names = parsePlayers(team->name());
    bool isHomeTeam(false);

    if (team == m_homeTeam)
        isHomeTeam = true;

    QList<Player::Role> formation;
    formation << Player::GoalKeeper
              << Player::LeftCentralDefence
              << Player::RightCentralDefence
              << Player::LeftMidfield
              << Player::RightMidfield
              << Player::CentralAttack;

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

void Pitch::setPlayerDefendZone(Player *p)
{
    bool nToS(false);
    if (p->team()->getDirection() == Team::NorthToSouth)
        nToS = true;

    QPointF tlTopHalf = m_footballPitch->rect().topLeft();
    QPointF brBottomHalf = m_footballPitch->rect().bottomRight();
    QPointF brTopHalf = brBottomHalf - QPointF(m_footballPitch->rect().height()/2,0);
    QPointF tlBottomHalf = tlTopHalf + QPointF(m_footballPitch->rect().height()/2,0);
    QPointF midTop(m_footballPitch->rect().width()/2,m_footballPitch->rect().top());
    QPointF midBottom(m_footballPitch->rect().width()/2,m_footballPitch->rect().height());

    QRectF topHalf(tlTopHalf, brTopHalf);
    QRectF bottomHalf(tlBottomHalf, brBottomHalf);
    QRectF leftHalf(tlTopHalf,midBottom);
    QRectF rightHalf(midTop,brBottomHalf);

    QRectF zone;
    switch (p->m_role)
    {
#if 0
    case Player::RightAttack:
    case Player::CentralAttack:
    case Player::LeftAttack:
        nToS ? zone = bottomHalf : zone = topHalf;
        break;
    case Player::LeftMidfield:
        zone = leftHalf;
        break;
    case Player::RightMidfield:
        zone = rightHalf;
        break;
    case Player::LeftCentralDefence:
    case Player::RightCentralDefence:
        nToS ? zone = topHalf : zone = bottomHalf;
        break;
#endif //
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

    foreach (Player *p, m_players) {
        if (p->team() == team) {
            p->setHasBall(false);
            p->m_startPositionRectF = startPositions[p->m_role];
            setPlayerDefendZone(p);
        }
    }
}

void Pitch::playGameSound(SoundEffects::GameSound s)
{
    m_soundEffects->soundEvent(s);
}
