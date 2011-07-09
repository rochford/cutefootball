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
#include "Player.h"
#include "pitch.h"
#include "pitchscene.h"
#include "ball.h"
#include "soundeffects.h"
#include "soccerutils.h"

#include <QtGui>
#include <QDebug>
#include <QGraphicsItem>
#include <QToolTip>
#include <QFocusEvent>

Player::Player(QString name,
               int number,
               bool computerControlled,
               Pitch *pitch,
               Team* team,
               qreal speed,
               Role role,
               QColor hairColor,
               QColor skinColor)
    : QObject(),
    QGraphicsPixmapItem(NULL,NULL),
    m_name(name),
    m_number(number),
    m_hasBall(false),
    m_team(team),
    m_role(role),
    m_pitch(pitch),
    m_speed(speed),
    m_step(0),
    m_allowedOffPitch(true),
    m_hairColor(hairColor),
    m_skinColor(skinColor),
    m_positionLocked(false),
    m_requiredNextAction(MWindow::NoAction),
    m_destination(QPointF(0.0,0.0))
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    m_keyEventTimer = new QTimer(this);
    m_keyEventTimer->setInterval(KKeyPressRefreshRate);

    if (!computerControlled)
        setFlag(QGraphicsItem::ItemIsFocusable);

    setToolTip(m_name);

    m_outOfAction = new QTimer(this);
    m_outOfAction->setSingleShot(true);

    connect(m_outOfAction, SIGNAL(timeout()), this, SLOT(standupPlayer()));
    connect(m_keyEventTimer, SIGNAL(timeout()), this, SLOT(repeatKeyEvent()));
    connect(m_pitch, SIGNAL(foul(Team*,QPointF)), this, SLOT(foulEventStart(Team*,QPointF)));

    createKeyboardActions();
    setTransformOriginPoint(boundingRect().center());

    m_toolTipText = "";
    m_toolTipPen = KPlayerNameFocused;
    m_toolTipTextPos = QPointF(boundingRect().topLeft().x()-2,
                               boundingRect().y()-5);
}

Player::~Player()
{
    m_images.clear();
    m_actions.clear();

    if (m_keyEventTimer->isActive())
        m_keyEventTimer->stop();
    delete m_keyEventTimer;
    if (m_outOfAction->isActive())
        m_outOfAction->stop();
    delete m_outOfAction;
}

void Player::foulEventStart(Team* t, QPointF foulLocation)
{
    qDebug() << "Player::foulEvent";
    if (t && t == this->m_team) {
        // not allowed within certain distance of foulLocation
        QPainterPath path;
        path.addEllipse(foulLocation, KFoulDistance, KFoulDistance);
        QList<QGraphicsItem *> list = m_pitch->scene()->items ( path,
                                                             Qt::IntersectsItemShape,
                                                             Qt::AscendingOrder);
        foreach (QGraphicsItem *item, list) {
            if (item == this)
                qDebug() << this->name() << " needs to move away from ball";
        }
    }
}

void Player::pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3, QRgb shirtColor, QRgb shortColor)
{
    QString s(":/images/red/");

    QString key1(m_team->briefName()+m_hairColor.name()+m_skinColor.name()),
            key2(m_team->briefName()+m_hairColor.name()+m_skinColor.name()),
            key3(m_team->briefName()+m_hairColor.name()+m_skinColor.name());
    key1.append(s1);
    key2.append(s2);
    key3.append(s3);

    QString n1(s), n2(s), n3(s);
    QPixmap p1, p2, p3;

    if (!QPixmapCache::find(key1, &p1)) {
        teamColorTransform(p1, n1.append(s1), qRgb(255, 0, 0), shirtColor, shortColor,
                           m_hairColor.rgb(), m_skinColor.rgb());
        QPixmapCache::insert(key1, p1);
    }

    if (!QPixmapCache::find(key2, &p2)) {
        teamColorTransform(p2, n2.append(s2), qRgb(255, 0, 0),
                           shirtColor, shortColor,
                           m_hairColor.rgb(), m_skinColor.rgb());
        QPixmapCache::insert(key2, p2);
    }
    if (!QPixmapCache::find(key3, &p3)) {
        teamColorTransform(p3, n3.append(s3), qRgb(255, 0, 0),
                           shirtColor, shortColor,
                           m_hairColor.rgb(), m_skinColor.rgb());
        QPixmapCache::insert(key3, p3);
    }

    QList<QPixmap> list;
    list << p1 <<  p2 << p3;
    m_images.insert(a, list);
}

void Player::createPixmaps()
{
    pixmapInsert(MWindow::North, "pN.PNG", "pN1.PNG", "pN2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::NorthEast, "pNE.PNG", "pNE1.PNG", "pNE2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::East, "pE.PNG", "pE1.PNG", "pE2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::SouthEast, "pSE.PNG", "pSE1.PNG", "pSE2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::South, "pS.PNG", "pS1.PNG", "pS2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::SouthWest, "pSW.PNG", "pSW1.PNG", "pSW2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::West, "pW.PNG", "pW1.PNG", "pW2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::NorthWest, "pNW.PNG", "pNW1.PNG", "pNW2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());

    pixmapInsert(MWindow::TackleN, "tackleN.PNG", "tackleN.PNG", "tackleN.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::TackleNE, "tackleNE.PNG", "tackleNE.PNG", "tackleNE.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::TackleNW, "tackleNW.PNG", "tackleNW.PNG", "tackleNW.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::TackleE, "tackleE.PNG", "tackleE.PNG", "tackleE.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::TackleSE, "tackleSE.PNG", "tackleSE.PNG", "tackleSE.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::TackleS, "tackleS.PNG", "tackleS.PNG", "tackleS.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::TackleSW, "tackleSW.PNG", "tackleSW.PNG", "tackleSW.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::TackleW, "tackleW.PNG", "tackleW.PNG", "tackleW.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());

    pixmapInsert(MWindow::FallenOver, "pTackled.PNG", "pTackled.PNG", "pTackled.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb()); // TODO XXX TIM
    pixmapInsert(MWindow::NoAction, "pS.PNG", "pS1.PNG", "pS2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());

    // set default pixmap
    m_lastAction = MWindow::North;
    setPixmap(m_images[m_lastAction].at(0));
}

QRectF Player::boundingRect() const
{
    return QRectF(-18, -18,
                  18, 18);
}

void Player::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    if (m_toolTipText != "") {
        painter->setPen(m_toolTipPen);
        painter->setFont(KPlayerToolTipFont);
        painter->drawText(m_toolTipTextPos, m_toolTipText);
    }

    // Draw QGraphicsPixmapItem face
    if (!pixmap())
        setPixmap(m_images[m_lastAction].at(m_step % 3));

//    Q_ASSERT(pixmap());
    painter->drawPixmap(boundingRect().toRect(), pixmap());
}

void Player::focusInEvent(QFocusEvent * event)
{
    //qDebug() << " Player::focusInEvent " << m_name;
    m_toolTipText = m_name;
}
void Player::focusOutEvent(QFocusEvent * event)
{
    //qDebug() << " Player::focusOutEvent " << m_name;
    m_toolTipText = "";
    // stop any key events
    stopKeyEvent();
}


QPainterPath Player::shape() const
{
    QPainterPath path;
    path.addRect(-18, -18,
                 18, 18);
    return path;
}

void Player::movePlayer(MWindow::Action action)
{
    // if the ball is not owned then take ownership
    if (ballCollisionCheck() && !m_pitch->ball()->ballOwner()) {
        m_hasBall = true;
        m_pitch->ball()->setBallOwner(this);
    } else if (!ballCollisionCheck())
        m_hasBall = false;

    if (m_hasBall)
        m_pitch->ball()->moveBall(action, m_speed);

    m_step++;
    // make the move
    switch(action)
    {
    case MWindow::North:
    case MWindow::NorthEast:
    case MWindow::East:
    case MWindow::SouthEast:
    case MWindow::South:
    case MWindow::SouthWest:
    case MWindow::West:
    case MWindow::NorthWest:
    {
        QLineF tragectory;
        tragectory.setP1(pos());
        tragectory.setAngle((qreal)action);
        tragectory.setLength(m_speed);

//        qDebug() << "Player::movePlayer " << (qreal)action << " deg, speed=" << m_speed;
//        qDebug() << "Player::movePlayer start " << tragectory.p1() << " end " << tragectory.p2();
//        qDebug() << "Player::movePlayer dx " << tragectory.dx() << " dy " << tragectory.dy();

        setPixmap(m_images[action].at(m_step % 3));
        // if the destination is less than move distance, only move that much
        QLineF diff(pos(), m_destination);
        if (diff.length() < tragectory.length())
            tragectory.setLength(diff.length());
        moveBy(tragectory.dx(), tragectory.dy());
        m_lastAction = action;
    }
        break;
    case MWindow::NoAction:
        setPixmap(m_images[action].at(0));
        m_lastAction = action;
        break;
    default:
        Q_ASSERT(0);
        break;
    }
}

bool Player::withinShootingDistance() const
{
    QPointF diff;

    if (m_team->getDirection() == Team::NorthToSouth)
        // TODO XXX TIM is center() correct ???
        diff = m_pitch->m_bottomGoal->center() - m_pitch->ball()->pos();
    else
        // TODO XXX TIM is center() correct ???
        diff = m_pitch->ball()->pos() - m_pitch->m_topGoal->center();

    if ( ( (m_pitch->footballPitch().height() / 5.0 )*2.0) > diff.manhattanLength())
        return true;
    else
        return false;
}

QPointF Player::calculateBallDestination(MWindow::Action act)
{
    const QPointF p(pos());
    const int KShotPower = m_pitch->scene()->sceneRect().height() / 4;
    const int KPassPower = m_pitch->scene()->sceneRect().height() / 5;
    QPointF shotDest(p);
    QPointF passDest(p);
    switch(m_lastAction)
    {
    case MWindow::North:
        shotDest.setY(p.y() - KShotPower);
        passDest.setY(p.y() - KPassPower);
        break;
    case MWindow::NorthEast:
        shotDest.setX(p.x() + KShotPower);
        shotDest.setY(p.y() - KShotPower);
        passDest.setX(p.x() + KPassPower);
        passDest.setY(p.y() - KPassPower);
        break;
    case MWindow::East:
        shotDest.setX(p.x() + KShotPower);
        passDest.setX(p.x() + KPassPower);
        break;
    case MWindow::SouthEast:
        shotDest.setX(p.x() + KShotPower);
        shotDest.setY(p.y() + KShotPower);
        passDest.setX(p.x() + KPassPower);
        passDest.setY(p.y() + KPassPower);
        break;
    case MWindow::South:
        shotDest.setY(p.y() + KShotPower);
        passDest.setY(p.y() + KPassPower);
        break;
    case MWindow::SouthWest:
        shotDest.setX(p.x() - KShotPower);
        passDest.setY(p.y() + KPassPower);
        break;
    case MWindow::West:
        shotDest.setX(p.x() - KShotPower);
        passDest.setX(p.x() - KPassPower);
        break;
    case MWindow::NorthWest:
        shotDest.setX(p.x() - KShotPower);
        shotDest.setY(p.y() - KShotPower);
        passDest.setX(p.x() - KPassPower);
        passDest.setY(p.y() - KPassPower);
        break;
    default:
        break;
    }
    if ( act == MWindow::Shot)
        return shotDest;
    else
        return passDest;
}

void Player::specialAction(MWindow::Action action)
{
    switch (action) {
        case MWindow::DiveEast:
        case MWindow::DiveWest:
            setPixmap(m_images[action].at(0));
            m_outOfAction->stop();
            m_outOfAction->start(750);
            return;

        case MWindow::Tackle:
            {
            // perform tackle here...
            MWindow::Action tackleAction = calculateTackleActionFromLastAction(m_lastAction);

            setPixmap(m_images[tackleAction].at(0));
            QLineF tragectory;
            tragectory.setP1(pos());
            tragectory.setAngle((qreal)m_lastAction);
            tragectory.setLength(m_speed);

            moveBy(tragectory.dx(), tragectory.dy());
            m_outOfAction->stop();
            m_outOfAction->start(500);

            const bool ballCollision = ballCollisionCheck();
            const bool playerCollision = playerCollisionCheck();

            if ( playerCollision && !ballCollision ) {
                qDebug() << "Player::specialAction foul";
                Player *p = m_pitch->ball()->ballOwner();
                if (p)
                    p->setTackled();
                // TODO foul logic here...
                m_pitch->setPiece( team(), Pitch::Foul, pos());
            }
            // if tackle causes contact with the ball then transfer the ownership
            else if ( playerCollision && ballCollision  ) {
                Player *p = m_pitch->ball()->ballOwner();
                if (p)
                    p->setTackled();
                QPointF ballDest = calculateBallDestination(MWindow::Pass);
                m_pitch->ball()->kickBall(MWindow::Pass, ballDest);
                m_hasBall = false;
            }
            else if ( !playerCollision && ballCollision ) {
                QPointF ballDest = calculateBallDestination(MWindow::Pass);
                m_pitch->ball()->kickBall(MWindow::Pass, ballDest);
                m_hasBall = false;
            }

            return;
            }
    case MWindow::FallenOver:
            {
            setPixmap(m_images[action].at(0));
            // start an out of action timer
            m_outOfAction->stop();
            m_outOfAction->start(1500);
            return;
            }
        default:
            break;
    }

    QPointF dest = calculateBallDestination(action);

    // if not have ball then must be tackle
    // if have ball then either shot or pass
    if (m_hasBall) {
        if (action == MWindow::Shot
            || withinShootingDistance()) {
             m_pitch->ball()->kickBall(MWindow::Shot, dest);
             m_hasBall = false;
        } else {
            Player *p = findAvailableTeamMate(pos());
            if (p)
                m_pitch->ball()->kickBall(MWindow::Pass, p->pos());
            else
                // pass short in direction travelling
                m_pitch->ball()->kickBall(MWindow::Pass, dest);
            m_hasBall = false;
        }
    }
}

void Player::move(MWindow::Action action)
{
    if (m_outOfAction->isActive())
        return;

    if ( m_hasBall ) {
        if ( action == MWindow::ButtonShortPress )
            action = MWindow::Pass;
        else if ( m_hasBall && action == MWindow::ButtonLongPress)
            action = MWindow::Shot;
    } else {
        if ( action == MWindow::ButtonShortPress
            || action == MWindow::ButtonLongPress )
            action = MWindow::Tackle;
    }

    if (m_requiredNextAction != MWindow::NoAction ) {
        if (action ==  m_requiredNextAction) {
            setRequiredNextAction(MWindow::NoAction);
        } else {
            return;
        }
    }

    if ( action == MWindow::Shot
        || action == MWindow::Tackle
        || action == MWindow::FallenOver
        || action == MWindow::Pass
        || action == MWindow::DiveEast
        || action == MWindow::DiveWest )
        specialAction(action);
    else
        movePlayer(action);
}

// same team
// Not man marked
// must have direct line of sight
// furthest forward player is preferred
Player* Player::findAvailableTeamMate(QPointF myPos) const
{
    Player *bestPlayer = NULL;
//    int nearest = 0xffff;
    foreach (Player *p, m_pitch->m_players) {
        if (p->m_team!= m_team)
            continue;
        // not self
        if (p == this)
            continue;        

        QPointF diff(p->pos() - myPos);
//        const int dx = p->pos().x() - myPos.x();
//        const int dy = p->pos().y() - myPos.y();

//        if (qAbs(dx) < 50 && qAbs(dy) < 50)
        if (diff.manhattanLength() < (m_pitch->footballPitch().height() / 3.0))
            bestPlayer = p;
    }
    return bestPlayer;
}

bool Player::ballCollisionCheck() const
{
    // collision check before move...
    QList<QGraphicsItem*> list = collidingItems(Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem *item, list) {
        if (item == this)
            continue;
        Ball *b = qgraphicsitem_cast<Ball *>(item);
        if ( b )
            return true;
    }
    return false;
}

bool Player::playerCollisionCheck() const
{
    // collision check before move...
    QList<QGraphicsItem*> list = collidingItems(Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem *item, list) {
        if (item == this)
            continue;
        Player *p = qgraphicsitem_cast<Player *>(item);
        if ( p && ( team() != p->team() ) )
            return true;
    }
    return false;
}

void Player::computerAdvance(int phase)
{
    if ( m_requiredNextAction != MWindow::NoAction ) {
        qDebug() << "Player::computerAdvance " << m_name << " requires Next Action";
        move(m_requiredNextAction);
        m_requiredNextAction = MWindow::NoAction;
        m_hasBall = false;
        return;
    }

    if (m_hasBall)
         computerAdvanceWithBall();
    else
        computerAdvanceWithoutBall();
}

void Player::computerAdvanceWithoutBall()
{
    setZValue(Pitch::ZComputerControlledPlayer);
    Player *nearestPlayer = m_pitch->selectNearestPlayer(m_pitch->awayTeam());

    if (nearestPlayer == this ) {
        // if close to the ball then tackle

        MWindow::Action action;
        const QPointF ballPos(m_pitch->ball()->pos());

        //QPointF diff(pos() - ballPos);
        int dx = abs(pos().x() - m_pitch->ball()->pos().x());
        int dy = abs(pos().y() - m_pitch->ball()->pos().y());
        if ( m_pitch->ball()->ballOwner() && ( (dx < 10) && (dy < 10)) ) {
            action = MWindow::Tackle;
            move(action);
        } else {
            action = calculateAction(pos(), ballPos);
            m_destination = ballPos;
            move(action);
        }
    }
    else
       automove();
}

void Player::computerAdvanceWithBall()
{
    setZValue(Pitch::ZFocusedPlayer);

    if (m_team->getDirection() == Team::SouthToNorth )
        m_destination = QPointF(m_pitch->m_topGoal->center().x(), m_pitch->m_topGoal->bottom());
    else
        m_destination = QPointF(m_pitch->m_bottomGoal->center().x(), m_pitch->m_bottomGoal->top());

    MWindow::Action act = calculateAction(pos(), m_destination);

    switch(m_role) {
    case Player::GoalKeeper:
        // should not happen
        break;
    default:
        if (withinShootingDistance()) {
            move(MWindow::Shot);
            m_hasBall = false;
        } else {
            // is there an opposition player in the way?
            Team* oppTeam = m_pitch->homeTeam();
            if (team() == m_pitch->homeTeam())
                oppTeam = m_pitch->awayTeam();

            Player* opposition = m_pitch->selectNearestPlayer(oppTeam);
            const int KProximity = 30;
            QRectF area(QPointF(pos().x() - KProximity,pos().y() - KProximity),QSize(KProximity,KProximity));
            if ( area.contains(opposition->pos())) {
                qDebug() << "computerAdvanceWithBall need to avoid player";
                act = MWindow::Pass;
                m_hasBall = false;
            }
            move(act);
        }
        break;
    }
}

void Player::automove()
{
    // automove not applicable to human players or ball holders
    if (hasFocus() || m_hasBall)
        return;

    if (!m_team->teamHasBall()) {
        MWindow::Action act;
        // If the ball enters m_defendZone then move towards it
        if ( m_defendZone.contains( m_pitch->ball()->pos() ) ) {
            act = calculateAction( pos(), m_pitch->ball()->pos() );
            m_destination = m_pitch->ball()->pos();
            move(act);
        } else {
            act = calculateAction( pos(), m_startPositionRectF.center() );
            m_destination = m_startPositionRectF.center();
            move(act);
        }
    }
}

void Player::humanAdvance(int phase)
{
    automove();
}

void Player::advance(int phase)
{
    if (!phase)
        return;
    if (m_outOfAction->isActive())
        return;
    if (m_team == m_pitch->awayTeam())
        computerAdvance(phase);
    else
        humanAdvance(phase);
}

void Player::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat() || !m_actions.contains(event->key())) {
        event->ignore();
        return;
    }

    //qDebug() << "keyPressEvent";
    MWindow::Action a = m_actions[ event->key() ];

    if (m_lastKeyEvent == event->key()) {
        //qDebug() << "halt";
        m_lastAction = MWindow::NoAction;
        m_lastKeyEvent = -1; // some invalid value
        move(MWindow::NoAction);
        event->accept();
        m_keyEventTimer->stop();
        return;
    }

    switch ( a )
    {
    case MWindow::Button:
        m_elapsedTime.restart();
        break;
    case MWindow::North:
    case MWindow::NorthEast:
    case MWindow::East:
    case MWindow::SouthEast:
    case MWindow::South:
    case MWindow::SouthWest:
    case MWindow::West:
    case MWindow::NorthWest:
        calculatePlayerDestination(a);
        m_lastAction = a;
        m_lastKeyEvent = event->key();
        move(a);
        // start a timer
        m_keyEventTimer->start();
        break;
    case MWindow::Pause:
        m_pitch->pause();
        break;
    default:
        break;
    }

    event->accept();
}

void Player::calculatePlayerDestination(MWindow::Action act)
{
    // the idea is that player wishes to travel in a direction
    // for an infinite amount of time
    const int KSomeBigDistance = 500;
    QPointF dst = pos();
    switch ( act )
    {
    case MWindow::North:
        dst.setY(0);
        break;
    case MWindow::NorthEast:
        dst.setY(0);
        dst.setX(KSomeBigDistance);
        break;
    case MWindow::East:
        dst.setX(KSomeBigDistance);
        break;
    case MWindow::SouthEast:
        dst.setY(KSomeBigDistance);
        dst.setX(KSomeBigDistance);
        break;
    case MWindow::South:
        dst.setY(KSomeBigDistance);
        break;
    case MWindow::SouthWest:
        dst.setY(KSomeBigDistance);
        dst.setX(0);
        break;
    case MWindow::West:
        dst.setX(0);
        break;
    case MWindow::NorthWest:
        dst.setY(0);
        dst.setX(0);
        break;
    default:
        Q_ASSERT(0);
        break;
    }
    m_destination = dst;
}

void Player::keyReleaseEvent(QKeyEvent *event)
{
    if ( event->isAutoRepeat() || !m_actions.contains( event->key() ) ) {
        event->ignore();
        return;
    }
    //qDebug() << "keyReleaseEvent";
    MWindow::Action a = m_actions[ event->key() ];

    if ( a == MWindow::Button ) {
        int elapsed = m_elapsedTime.elapsed();
        if ( elapsed > KLongPressValue )
            move(MWindow::ButtonLongPress);
        else
            move(MWindow::ButtonShortPress);
    }
    event->accept();
}

void Player::repeatKeyEvent()
{
    // repeating the last Key Event works ok with keyboard, but not with mouse.
    // so need to calculate next action based upon m_destination.
    MWindow::Action a = calculateAction(pos(), m_destination);
    m_lastAction = a;
    move(a);
   m_keyEventTimer->start();
}

void Player::stopKeyEvent()
{
    if (m_keyEventTimer->isActive())
        m_keyEventTimer->stop();
}

void Player::createKeyboardActions()
{
    m_actions.insert( Qt::Key_0, MWindow::Pause );

    m_actions.insert( Qt::Key_Up, MWindow::North );
    m_actions.insert( Qt::Key_Down, MWindow::South );
    m_actions.insert( Qt::Key_Left, MWindow::West );
    m_actions.insert( Qt::Key_Right, MWindow::East );
#if defined(Q_OS_SYMBIAN)

    m_actions.insert( Qt::Key_2, MWindow::North );
    m_actions.insert( Qt::Key_3, MWindow::NorthEast );
    m_actions.insert( Qt::Key_9, MWindow::SouthEast );
    m_actions.insert( Qt::Key_8, MWindow::South );
    m_actions.insert( Qt::Key_7, MWindow::SouthWest );
    m_actions.insert( Qt::Key_1, MWindow::NorthWest );
#else
    m_actions.insert( Qt::Key_8, MWindow::North );
    m_actions.insert( Qt::Key_9, MWindow::NorthEast );
    m_actions.insert( Qt::Key_3, MWindow::SouthEast );
    m_actions.insert( Qt::Key_2, MWindow::South );
    m_actions.insert( Qt::Key_1, MWindow::SouthWest );
    m_actions.insert( Qt::Key_7, MWindow::NorthWest );
#endif
    m_actions.insert( Qt::Key_4, MWindow::West );
    m_actions.insert( Qt::Key_6, MWindow::East );

    m_actions.insert( Qt::Key_5, MWindow::Button );
    m_actions.insert( Qt::Key_Space, MWindow::Button );
}

QVariant Player::itemChange(GraphicsItemChange change, const QVariant &value)
 {
     if (change == ItemPositionChange && scene()) {
         // value is the new position.
         QPointF newPos = value.toPointF();
         QRectF pitch = m_pitch->footballPitch();

         if ( ( m_role != Player::GoalKeeper )
             && ( m_pitch->m_bottomPenaltyArea.contains(newPos)
                || m_pitch->m_topPenaltyArea.contains(newPos) ) ) {
             if ( m_hasBall ) {
                 qDebug() << "Player::itemChange has ball";
                 m_pitch->ball()->setNoBallOwner();
                 m_hasBall = false;
             }
             if ( hasFocus() ) {
                 //qDebug() << "Player::itemChange penalty!";
             }
            return m_lastPos;
            }

         if (m_pitch->m_centerCircle.contains(newPos)
             && !m_allowedInCenterCircle)
             return m_lastPos;


         if (!pitch.contains(newPos) && !m_allowedOffPitch)
             return m_lastPos;
         else {
             m_lastPos = newPos;
             return newPos;
         }
     }
     return QGraphicsItem::itemChange(change, value);
 }

void Player::setTackled()
{
    if ( m_hasBall ) {
        m_pitch->ball()->setNoBallOwner();
        m_hasBall = false;
    }
    move(MWindow::FallenOver);
}

void Player::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    qDebug() << "Player mousePressEvent " << e->buttonDownScenePos(Qt::LeftButton);
    QPointF scenePos = e->buttonDownScenePos(Qt::LeftButton);
    setDestination(scenePos);
    MWindow::Action a = calculateAction(pos(), scenePos);
    m_lastAction = a;
    move(a);
    m_keyEventTimer->start();
}

