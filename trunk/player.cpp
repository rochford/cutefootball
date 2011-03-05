#include "Player.h"
#include "pitch.h"
#include "ball.h"
#include "soundeffects.h"
#include "soccerutils.h"

#include <QtGui>
#include <QDebug>
#include <QGraphicsItem>
#include <QToolTip>

Player::Player(QString name,
               bool computerControlled,
               Pitch *pitch,
               Team* team,
               qreal speed,
               Role role)
    : QObject(),
    QGraphicsPixmapItem(NULL,NULL),
    m_name(name),
    m_hasBall(false),
    m_team(team),
    m_role(role),
    m_pitch(pitch),
    m_speed(speed),
    m_step(0),
    m_allowedOffPitch(true)
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
    setRotation(0);
    setTransformOriginPoint(boundingRect().center());
}

Player::~Player()
{
    m_moveDistance.clear();
    m_images.clear();
    m_actions.clear();

    delete m_keyEventTimer;
    delete m_outOfAction;
}

void Player::foulEventStart(Team* t, QPointF foulLocation)
{
    qDebug() << "Player::foulEvent";
    if (t && t == this->m_team) {
        // not allowed within certain distance of foulLocation
        QPainterPath path;
        path.addEllipse(foulLocation, KFoulDistance, KFoulDistance);
        QList<QGraphicsItem *> list = m_pitch->m_scene->items ( path,
                                                             Qt::IntersectsItemShape,
                                                             Qt::AscendingOrder);
        foreach (QGraphicsItem *item, list) {
            if (item == this) {
                qDebug() << this->name() << " needs to move away from ball";
            }
        }
    }
}

void Player::standupPlayer()
{
    setRotation(0);
    setPixmap(m_images[m_lastAction].at(0));
}

void Player::createMoves()
{
    m_moveDistance.insert(MWindow::North, QPointF(0,-m_speed));
    m_moveDistance.insert(MWindow::NorthEast, QPointF(m_speed,-m_speed));
    m_moveDistance.insert(MWindow::East, QPointF(m_speed,0));
    m_moveDistance.insert(MWindow::SouthEast, QPointF(m_speed,m_speed));
    m_moveDistance.insert(MWindow::South, QPointF(0,m_speed));
    m_moveDistance.insert(MWindow::SouthWest, QPointF(-m_speed,m_speed));
    m_moveDistance.insert(MWindow::West, QPointF(-m_speed,0));
    m_moveDistance.insert(MWindow::NorthWest, QPointF(-m_speed,-m_speed));
}

void Player::pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3, QRgb shirtColor, QRgb shortColor)
{
    QString s(":/images/red/");

    QString key1(m_team->briefName()), key2(m_team->briefName()), key3(m_team->briefName());
    key1.append(s1);
    key2.append(s2);
    key3.append(s3);

    QString n1(s), n2(s), n3(s);
    QPixmap p1, p2, p3;

    if (!QPixmapCache::find(key1, &p1)) {
        teamColorTransform(p1, n1.append(s1), qRgb(255, 0, 0), shirtColor, shortColor);
        QPixmapCache::insert(key1, p1);
    }

    if (!QPixmapCache::find(key2, &p2)) {
        teamColorTransform(p2, n2.append(s2), qRgb(255, 0, 0), shirtColor, shortColor);
        QPixmapCache::insert(key2, p2);
    }
    if (!QPixmapCache::find(key3, &p3)) {
        teamColorTransform(p3, n3.append(s3), qRgb(255, 0, 0), shirtColor, shortColor);
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
    pixmapInsert(MWindow::SouthWest, "pSWest.PNG", "pSW1.PNG", "pSW2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::West, "pW.PNG", "pW1.PNG", "pW2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::NorthWest, "pNW.PNG", "pNW1.PNG", "pNW2.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());

    pixmapInsert(MWindow::Tackle, "tackleN.PNG", "tackleN.PNG", "tackleN.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb());
    pixmapInsert(MWindow::FallenOver, "pTackled.PNG", "pTackled.PNG", "pTackled.PNG", m_team->m_shirtColor.rgb(), m_team->m_shortColor.rgb()); // TODO XXX TIM
    // set default pixmap
    setPixmap(m_images[MWindow::North].at(0));
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
    // the player that is focused get red circle around them
    if ( hasFocus() ) {
        painter->setBrush(KFocusPlayerBrush);
        painter->drawEllipse(boundingRect().center(), 8, 8);
    }

    // Draw QGraphicsPixmapItem face
    painter->drawPixmap(boundingRect().toRect(), pixmap());
}

QPainterPath Player::shape() const
{
    QPainterPath path;
    path.addRect(-18, -18,
                 18, 18);
    return path;
}

void Player::movePlayer(MWindow::Action action, QPointF destination)
{
#if 0
    if (this->m_role == Player::GoalKeeper) {
        qDebug() << "Player::movePlayer GoalKeeper start";
        if (m_pitch->ball()->ballOwner())
            qDebug() << "Player::movePlayer m_pitch->ball()->ballOwner() name " << m_pitch->ball()->ballOwner()->m_name;
    }
#endif
    // if the ball is not owned then take ownership
    if (ballCollisionCheck() && !m_pitch->ball()->ballOwner()) {
//        qDebug() << "Player::movePlayer taken ball";
        m_hasBall = true;
        m_pitch->ball()->setBallOwner(this);
    } else if (!ballCollisionCheck()) {
        m_hasBall = false;
//        qDebug() << "Player::movePlayer no ball";
    }
    if (m_hasBall) {
//        qDebug() << "Player::movePlayer moving ball";
        m_pitch->ball()->moveBall(action, m_speed);
    }
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
        setPixmap(m_images[action].at(m_step % 3));
        setRotation(0);
        // if the destination is less than move distance, only move that much
        QPointF diff = destination - pos();
    //    qreal dx = destination.x() - pos().x();
    //    qreal dy = destination.y() - pos().y();
        qreal x = m_moveDistance[action].x();
        qreal y = m_moveDistance[action].y();
        if ( qAbs(diff.x()) < qAbs(x))
            x = diff.x();
        if ( qAbs(diff.y()) < qAbs(y))
            y = diff.y();
        moveBy(x, y);
        m_lastAction = action;
    }
        break;
    default:
        break;
    }
}

bool Player::withinShootingDistance() const
{
    QPointF diff;

    if (m_team->getDirection() == Team::NorthToSouth) {
        diff = m_pitch->m_bottomGoal->pos() - m_pitch->ball()->pos();
    } else {
        diff = m_pitch->ball()->pos() - m_pitch->m_topGoal->pos();
    }
    if ( ( (m_pitch->m_footballPitch->rect().height() / 5.0 )*2.0) > diff.manhattanLength())
        return true;
    else
        return false;
}

QPointF Player::calculateDestination(MWindow::Action act)
{
    const QPointF p(pos());
    const int KShotPower = m_pitch->m_scene->sceneRect().height() / 4;
    const int KPassPower = m_pitch->m_scene->sceneRect().height() / 5;
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
        case MWindow::Tackle:
            {
            // perform tackle here...
            int rotation = calculateTackleRotationFromLastAction(m_lastAction);

            setPixmap(m_images[MWindow::Tackle].at(0));
            setRotation(rotation);
            moveBy(m_moveDistance[m_lastAction].x(), m_moveDistance[m_lastAction].y());
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
                QPointF ballDest = calculateDestination(MWindow::Pass);
                m_pitch->ball()->kickBall(MWindow::Pass, ballDest);
                m_hasBall = false;
            }
            else if ( !playerCollision && ballCollision ) {
                QPointF ballDest = calculateDestination(MWindow::Pass);
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

    QPointF dest = calculateDestination(action);

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

void Player::setTackled()
{
    if ( m_hasBall ) {
        m_pitch->ball()->setNoBallOwner();
        m_hasBall = false;
    }
    move(MWindow::FallenOver);
}

void Player::move(MWindow::Action action, QPointF destination)
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

    if ( action == MWindow::Shot
        || action == MWindow::Tackle
        || action == MWindow::FallenOver
        || action == MWindow::Pass)
        specialAction(action);
    else
        movePlayer(action, destination);
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
        if (diff.manhattanLength() < (m_pitch->m_footballPitch->rect().height() / 3.0))
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

        QPointF diff(pos() - ballPos);
        int dx = abs(pos().x() - m_pitch->ball()->pos().x());
        int dy = abs(pos().y() - m_pitch->ball()->pos().y());
        if ( m_pitch->ball()->ballOwner() && ( (dx < 10) && (dy < 10)) ) {
            action = MWindow::Tackle;
            move(action);
        } else {
            action = calculateAction(pos(), ballPos);
            move(action, ballPos);
        }
    }
    else
       automove();
}

void Player::computerAdvanceWithBall()
{
    setZValue(Pitch::ZFocusedPlayer);

    QPointF destination;
    if (m_team->getDirection() == Team::SouthToNorth )
        destination = QPointF(m_pitch->m_topGoal->rect().center().x(), m_pitch->m_topGoal->rect().bottom());
    else
        destination = QPointF(m_pitch->m_bottomGoal->rect().center().x(), m_pitch->m_bottomGoal->rect().top());

    MWindow::Action act = calculateAction(pos(), destination);

    switch(m_role) {
    case Player::GoalKeeper:
        // should not happen
        break;
    default:
        if (withinShootingDistance()) {
            move(MWindow::Shot);
            m_hasBall = false;
        }
        else
            move(act, destination);
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
            move(act, m_pitch->ball()->pos());
        } else {
            act = calculateAction( pos(), m_startPositionRectF.center() );
            move(act, m_startPositionRectF.center());
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

    MWindow::Action a = m_actions[ event->key() ];

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
        m_lastAction = a;
        move(a);
        // start a timer
        m_keyEventTimer->start();
        break;
    default:
        break;
    }

    event->accept();
}

void Player::keyReleaseEvent(QKeyEvent *event)
{
    if ( event->isAutoRepeat() || !m_actions.contains( event->key() ) ) {
        event->ignore();
        return;
    }
    MWindow::Action a = m_actions[ event->key() ];

    if ( a != MWindow::Button ) {
        stopKeyEvent();
    } else {
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
   move(m_lastAction);
   m_keyEventTimer->start();
}

void Player::stopKeyEvent()
{
    if (m_keyEventTimer->isActive())
        m_keyEventTimer->stop();
}

void Player::createKeyboardActions()
{
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
         QRectF pitch = m_pitch->m_footballPitch->rect();

         if ( ( m_role != Player::GoalKeeper )
             && ( m_pitch->m_bottomPenaltyArea->contains(newPos)
                || m_pitch->m_topPenaltyArea->contains(newPos) ) ) {
             if ( m_hasBall ) {
                 qDebug() << "Player::itemChange has ball";
                 m_pitch->ball()->setNoBallOwner();
                 m_hasBall = false;
             }
             if ( hasFocus() ) {
                 qDebug() << "Player::itemChange penalty!";
             }
            return m_lastPos;
            }

         if (!pitch.contains(newPos) && !m_allowedOffPitch)
             return m_lastPos;
         else {
             m_lastPos = newPos;
             return newPos;
         }
     }
     return QGraphicsItem::itemChange(change, value);
 }
