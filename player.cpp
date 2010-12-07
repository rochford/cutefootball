#include "Player.h"
#include "pitch.h"
#include "ball.h"
#include "replay.h"
#include "soundeffects.h"

#include <QtGui>
#include <QDebug>
#include <QGraphicsItem>
#include <QToolTip>

void teamColorTransform(QPixmap &pixmap, QString pix, QRgb colorFrom, QRgb colorTo)
{
    QImage img(pix);
    QRect rect = img.rect();
    for (int w = 0; w < img.width(); w++) {
        for (int h = 0; h < img.height(); h++) {
            QRgb rgb = img.pixel(w, h);
            if (qRed(rgb) > 250 && qBlue(rgb) < 8 && qGreen(rgb) < 8)
                img.setPixel(QPoint(w,h), colorTo);
        }
    }
    pixmap = QPixmap::fromImage(img);

    QBitmap bitmap = pixmap.createMaskFromColor(KCuteFootballMaskColor);
    pixmap.setMask(bitmap);
}

MWindow::Action calculateAction(QPointF source,
                                QPointF destination)

{
    const int dx = source.x() - destination.x();
    const int dy = source.y() - destination.y();
    if (dx > 0 && dy == 0)
        return MWindow::West;
    else if (dx >= 0 && dy < 0)
        return MWindow::SouthWest;
    else if (dx > 0 && dy > 0)
        return MWindow::NorthWest;
    else if (dx <= 0 && dy == 0)
        return MWindow::East;
    else if (dx < 0 && dy > 0)
        return MWindow::NorthEast;
    else if (dx < 0 && dy < 0)
        return MWindow::SouthEast;
    else if (dx == 0 && dy >= 0)
        return MWindow::North;
    else if (dx == 0 && dy <= 0)
        return MWindow::South;
}

MWindow::Action calculateTackleFromLastAction(MWindow::Action lastAction)
{
    MWindow::Action action;
    switch(lastAction) {
    case MWindow::North:
        action = MWindow::TackleNorth;
        break;
    case MWindow::NorthEast:
        action = MWindow::TackleNorthEast;
        break;
    case MWindow::East:
        action = MWindow::TackleEast;
        break;
    case MWindow::SouthEast:
        action = MWindow::TackleSouthEast;
        break;
    case MWindow::South:
        action = MWindow::TackleSouth;
        break;
    case MWindow::SouthWest:
        action = MWindow::TackleSouthWest;
        break;
    case MWindow::West:
        action = MWindow::TackleWest;
        break;
    case MWindow::NorthWest:
        action = MWindow::TackleNorthWest;
        break;
    default:
        break;
    }
    return action;
}


Player::Player(QString name,
               bool computerControlled,
               Pitch *pitch,
               Team* team,
               Role role)
    : QObject(),
    QGraphicsPixmapItem(NULL,NULL),
    m_name(name),
    m_soundFile(QString(m_name + ".wav")),
    hasBall_(false),
    team_(team),
    role_(role),
    m_pitch(pitch),
    m_speed(computerControlled ? KPlayerDefaultSpeed - 1 : KPlayerDefaultSpeed),
    m_step(0),
    m_outOfAction(NULL)
{

    m_keyEventTimer = new QTimer(this);
    m_keyEventTimer->setInterval(KGameRefreshRate);

    if (!computerControlled)
        setFlag(QGraphicsItem::ItemIsFocusable);

    setToolTip(m_name);
    m_outOfAction = new QTimer(this);
    m_outOfAction->setSingleShot(true);

#ifdef REFEREE_USED
    // referees dont celebrate goals
    if (role != Player::LastDummy)
#endif //
    connect(m_pitch->ball(), SIGNAL(goalScored(bool)), this, SLOT(goalScored(bool)));
    connect(m_keyEventTimer, SIGNAL(timeout()), this, SLOT(repeatKeyEvent()));

    createKeyboardActions();
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

void Player::pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3, QRgb teamColor)
{
    QString s(":/images/red/");

    QString key1(team_->name_), key2(team_->name_), key3(team_->name_);
    key1.append(s1);
    key2.append(s2);
    key3.append(s3);

    QString n1(s), n2(s), n3(s);
    QPixmap p1, p2, p3;

    if (!QPixmapCache::find(key1, &p1)) {
        teamColorTransform(p1, n1.append(s1), qRgb(255, 0, 0), teamColor);
        QPixmapCache::insert(key1, p1);
    }

    if (!QPixmapCache::find(key2, &p2)) {
        teamColorTransform(p2, n2.append(s2), qRgb(255, 0, 0), teamColor);
        QPixmapCache::insert(key2, p2);
    }
    if (!QPixmapCache::find(key3, &p3)) {
        teamColorTransform(p3, n3.append(s3), qRgb(255, 0, 0), teamColor);
        QPixmapCache::insert(key3, p3);
    }

    QList<QPixmap> list;
    list << p1 <<  p2 << p3;
    m_images.insert(a, list);
}

void Player::createPixmaps()
{
    pixmapInsert(MWindow::North, "playerNorth.PNG", "playerNorth1.PNG", "playerNorth2.PNG", team_->color.rgb());
    pixmapInsert(MWindow::NorthEast, "playerNorthEast.PNG", "playerNorthEast1.PNG", "playerNorthEast2.PNG", team_->color.rgb());
    pixmapInsert(MWindow::East, "playerEast.PNG", "playerEast1.PNG", "playerEast2.PNG", team_->color.rgb());
    pixmapInsert(MWindow::SouthEast, "playerSouthEast.PNG", "playerSouthEast1.PNG", "playerSouthEast2.PNG", team_->color.rgb());
    pixmapInsert(MWindow::South, "playerSouth.PNG", "playerSouth1.PNG", "playerSouth2.PNG", team_->color.rgb());
    pixmapInsert(MWindow::SouthWest, "playerSouthWest.PNG", "playerSouthWest1.PNG", "playerSouthWest2.PNG", team_->color.rgb());
    pixmapInsert(MWindow::West, "playerWest.PNG", "playerWest1.PNG", "playerWest2.PNG", team_->color.rgb());
    pixmapInsert(MWindow::NorthWest, "playerNorthWest.PNG", "playerNorthWest1.PNG", "playerNorthWest2.PNG", team_->color.rgb());

    pixmapInsert(MWindow::ThrownIn, "playerNorthWest.PNG", "playerNorthWest1.PNG", "playerNorthWest2.PNG", team_->color.rgb()); // TODO XXX TIM

    pixmapInsert(MWindow::TackleNorth, "tackleNorth.PNG", "tackleNorth.PNG", "tackleNorth.PNG", team_->color.rgb()); // TODO XXX TIM
    pixmapInsert(MWindow::TackleNorthEast, "tackleNorthEast.PNG", "tackleNorthEast.PNG", "tackleNorthEast.PNG", team_->color.rgb()); // TODO XXX TIM
    pixmapInsert(MWindow::TackleEast, "tackleEast.PNG", "tackleEast.PNG", "tackleEast.PNG", team_->color.rgb()); // TODO XXX TIM
    pixmapInsert(MWindow::TackleSouthEast, "tackleSouthEast.PNG", "tackleSouthEast.PNG", "tackleSouthEast.PNG", team_->color.rgb()); // TODO XXX TIM
    pixmapInsert(MWindow::TackleSouth, "tackleSouth.PNG", "tackleSouth.PNG", "tackleSouth.PNG", team_->color.rgb()); // TODO XXX TIM
    pixmapInsert(MWindow::TackleSouthWest, "tackleSouthWest.PNG", "tackleSouthWest.PNG", "tackleSouthWest.PNG", team_->color.rgb()); // TODO XXX TIM
    pixmapInsert(MWindow::TackleWest, "tackleWest.PNG", "tackleWest.PNG", "tackleWest.PNG", team_->color.rgb()); // TODO XXX TIM
    pixmapInsert(MWindow::TackleNorthWest, "tackleNorthWest.PNG", "tackleNorthWest.PNG", "tackleNorthWest.PNG", team_->color.rgb()); // TODO XXX TIM

    pixmapInsert(MWindow::GoalCelebration, "playerNorth.PNG", "playerNorth1.PNG", "playerNorth2.PNG", team_->color.rgb()); // TODO XXX TIM

    // set default pixmap
    setPixmap(m_images[MWindow::North].at(0));
}

QRectF Player::boundingRect() const
{
    return QRectF(-18*KScaleFactor, -18*KScaleFactor,
                  18*KScaleFactor, 18*KScaleFactor);
}

void Player::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    // the player that is focused get red circle around them
    if ( hasFocus()
        && !m_pitch->replay()->isReplay()) {
        QBrush brush(Qt::white, Qt::Dense3Pattern);
        painter->setBrush(brush);
        painter->drawEllipse(QPointF(0,0), 8*KScaleFactor, 8*KScaleFactor);
    }
#ifdef SHOW_PLAYER_NAMES_AS_TOOLTIPS
    if ( ( hasBall_ || humanControlled_ ) && !pitch_->replay()->isReplay() )
        painter->drawText(QPointF(12,12), toolTip());
#endif //

    QSize pixmapSize = pixmap().size();
    pixmapSize.scale(QSizeF(36*KScaleFactor,36*KScaleFactor).toSize(), Qt::KeepAspectRatio);

    // Draw QGraphicsPixmapItem face
    painter->drawPixmap(boundingRect().toRect(), pixmap());
}

void Player::goalScored(bool isTopGoal)
{
    if (isTopGoal && (team_->getDirection() == Team::SouthToNorth))
        // celebrate goal
        ;
    else if (!isTopGoal && (team_->getDirection() == Team::NorthToSouth))
        // celebrate goal
        ;
}

QPainterPath Player::shape() const
{
    QPainterPath path;
    path.addRect(-18*KScaleFactor, -18*KScaleFactor,
                 18*KScaleFactor, 18*KScaleFactor);
    return path;
}

void Player::movePlayer(MWindow::Action action)
{
    // if the ball is not owned then take ownership
    if (ballCollisionCheck() && !m_pitch->ball()->controlledBy()) {
        SoundEffects::soundEvent(m_soundFile);
        hasBall_ = true;
        m_pitch->ball()->setControlledBy(this);
    } else if (!ballCollisionCheck())
        hasBall_ = false;

    if (hasBall_)
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
        setPixmap(m_images[action].at(m_step % 3));
        moveBy(m_moveDistance[action].x(), m_moveDistance[action].y());
        m_lastAction = action;
        break;
    default:
        break;
    }
}

bool Player::withinShootingDistance() const
{
    if (team_->getDirection() == Team::NorthToSouth
        && m_pitch->m_bottomPenaltyArea->contains(m_pitch->ball()->pos()))
        return true;
    else if (team_->getDirection() == Team::SouthToNorth
             && m_pitch->m_topPenaltyArea->contains(m_pitch->ball()->pos()))
        return true;
    else {
        return false;
    }
}

void Player::specialAction(MWindow::Action action)
{
    switch (action) {
    case MWindow::ThrownIn:
        {
            setPixmap(m_images[action].at(0));
            m_outOfAction->start(1500);
            m_lastAction = action;
        }
        return;
    case MWindow::GoalCelebration:
        setPixmap(m_images[action].at(0));
        return;
    case MWindow::DiveLeft:
    case MWindow::DiveRight:
        setPixmap(m_images[action].at(0));
        moveBy(m_moveDistance[action].x(), m_moveDistance[action].y());
        // if dive causes contact with the ball then transfer the ownership
        if (ballCollisionCheck()) {
            m_pitch->ball()->setControlledBy(this);
            hasBall_ = true;
        }
        return;
    default:
        break;
    }

    const QPointF p(pos());
    const int KShotPower = m_pitch->m_scene->sceneRect().width() / 2;
    const int KPassPower = m_pitch->m_scene->sceneRect().width() / 5;
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

    // if not have ball then must be tackle
    // if have ball then either shot or pass
    if (hasBall_) {
        if (action == MWindow::ButtonLongPress
            || action == MWindow::Shot
            || withinShootingDistance()) {
             m_pitch->ball()->kickBall(MWindow::Shot, shotDest);
             hasBall_ = false;
             m_pitch->ball()->setControlledBy(NULL);
        } else {
            Player *p = findAvailableTeamMate(pos());
            if (p) {
                m_pitch->ball()->kickBall(MWindow::Pass, p->pos());
            } else {
                // pass short in direction travelling
                m_pitch->ball()->kickBall(MWindow::Pass, passDest);
            }
            hasBall_ = false;
            m_pitch->ball()->setControlledBy(NULL);
        }
    } else {
        // perform tackle here...

        action = calculateTackleFromLastAction(m_lastAction);

        setPixmap(m_images[action].at(0));
        moveBy(m_moveDistance[m_lastAction].x(), m_moveDistance[m_lastAction].y());
        m_outOfAction->stop();
        m_outOfAction->start(500);

        // if tackle causes contact with the ball then transfer the ownership
        if (ballCollisionCheck()) {
            Player *p = m_pitch->ball()->controlledBy();
            if (p)
                p->isTackled(true);
            m_pitch->ball()->setControlledBy(this);
            hasBall_ = true;
        }
    }
}

void Player::isTackled(bool defeated)
{
    if (defeated) {
        // TODO use a different pixmap
        // start an out of action timer
        m_outOfAction->stop();
        m_outOfAction->start(1500);
    }
}

void Player::move(MWindow::Action action)
{
    if (m_outOfAction->isActive())
        return;
    if (action == MWindow::ButtonShortPress
        || action == MWindow::ButtonLongPress
        || action == MWindow::Shot
        || action == MWindow::Tackle
        || action == MWindow::Pass
        || action == MWindow::ThrownIn
        || action == MWindow::DiveLeft
        || action == MWindow::DiveRight
        || action == MWindow::GoalCelebration)
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
        if (p->team_!= team_)
            continue;
        // not self
        if (p == this)
            continue;        
        // continue if they are man marked
//        if ( p->isManMarked() )
//            continue;

        const int dx = p->pos().x() - myPos.x();
        const int dy = p->pos().y() - myPos.y();

//        if ((qAbs(dx*dx)+qAbs(dy*dy)) < nearest) {
        if (qAbs(dx) < 50 && qAbs(dy) < 50) {
            bestPlayer = p;
//            nearest = qAbs(qAbs(dx*dx)+qAbs(dy*dy));
        }
    }
    return bestPlayer;
}

bool Player::ballCollisionCheck()
{
    // collision check before move...
    QList<QGraphicsItem*> list = collidingItems(Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem *item, list) {
        if (item == this)
            continue;
        Ball *b = qgraphicsitem_cast<Ball *>(item);
        if ( b ) {
            return true;
        }
    }
    return false;
}

// player is being man marked
bool Player::isManMarked() const
{
    // find out where this player is
    QPointF myPos(this->pos());
    // if there is a player of the opposite team within
    // certain radius then this player is man-marked
    QList<QGraphicsItem*> list = scene()->items(myPos.x()-5,
                   myPos.y()-5,
                   myPos.x()+5,
                   myPos.y()+5,
                   Qt::ContainsItemShape,
                   Qt::AscendingOrder);
    foreach (QGraphicsItem *item, list) {
        Player *p = qgraphicsitem_cast<Player*>(item);
        if ( p && ( p != this ) &&  ( p->team_ != this->team_ ) )
            return true;
    }
    return false;
}

void Player::computerAdvance(int phase)
{
    if (hasBall_)
         computerAdvanceWithBall();
    else
        computerAdvanceWithoutBall();
}

void Player::computerAdvanceWithoutBall()
{
    setZValue(5);
    if (!team_->teamHasBall_) {
        Player *nearestPlayer = m_pitch->selectNearestPlayer(m_pitch->awayTeam());

        if (nearestPlayer == this ) {
            // if close to the ball then tackle

            MWindow::Action action;
            int dx = abs(pos().x() - m_pitch->ball()->pos().x());
            int dy = abs(pos().y() - m_pitch->ball()->pos().y());
            if ( m_pitch->ball()->controlledBy() && ( dx < 5) && (dy < 5) )
                action = MWindow::Tackle;
            else
                action = calculateAction(pos(), m_pitch->ball()->pos());

            move(action);
        }
    }
    else
        automove();
}

void Player::computerAdvanceWithBall()
{
    setZValue(6);

    // if the last action was thrownIn, then just pass the ball...
    if (m_lastAction == MWindow::ThrownIn) {
        m_pitch->ball()->setVisible(true);
        move(MWindow::Pass);
        return;
    }

    QPointF destination;
    if (team_->getDirection() == Team::SouthToNorth )
        destination = QPointF(m_pitch->m_topGoal->rect().center().x(), m_pitch->m_topGoal->rect().top());
    else
        destination = QPointF(m_pitch->m_bottomGoal->rect().center().x(), m_pitch->m_bottomGoal->rect().bottom());

    MWindow::Action act = calculateAction(pos(), destination);

    switch(role_) {
    case Player::GoalKeeper:
        // should not happen
        break;
    default:
        if (withinShootingDistance()) {
            move(MWindow::Shot);
            hasBall_ = false;
            m_pitch->ball()->setControlledBy(NULL);
        }
        else
            move(act);
        break;
    }
}

void Player::automove()
{
    // automove not applicable to human players or ball holders
    if (hasFocus() || hasBall_)
        return;

    QPointF desiredPosition(0,0);
    if ( team_->state_ == Team::Attacking )
        desiredPosition = attackPosition_.center();
    else
        desiredPosition = defencePosition_.center();

    MWindow::Action act = calculateAction(pos(), desiredPosition);
    move(act);
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
    if (team_ == m_pitch->awayTeam())
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
    qDebug() << "Player::keyPressEvent";

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
        // start a timer
        m_lastAction = a;
//        m_pitch->action(a);
        move(a);
        m_keyEventTimer->start();
        break;
    case MWindow::Replay:
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
//    qDebug() << "Player::keyReleaseEvent";
    MWindow::Action a = m_actions[ event->key() ];

    if ( a != MWindow::Button ) {
        stopKeyEvent();
    } else {
        int elapsed = m_elapsedTime.elapsed();
        if ( elapsed > KLongPressValue )
            move(MWindow::ButtonLongPress);
//            m_pitch->action(MWindow::ButtonLongPress);
        else
//            m_pitch->action(MWindow::ButtonShortPress);
            move(MWindow::ButtonShortPress);
    }

    event->accept();
}


void Player::repeatKeyEvent()
{
 //   qDebug() << "Player::repeatKeyEvent";
 //   m_pitch->action(m_lastAction);
   move(m_lastAction);
   m_keyEventTimer->start();
}

void Player::stopKeyEvent()
{
    qDebug() << "Player::stopKeyEvent";
    if (m_keyEventTimer->isActive())
        m_keyEventTimer->stop();
}

void Player::createKeyboardActions()
{
//    qDebug() << "Player::createKeyboardActions";
    m_actions.insert( Qt::Key_W, MWindow::North );
    m_actions.insert( Qt::Key_E, MWindow::NorthEast );
    m_actions.insert( Qt::Key_D, MWindow::East );
    m_actions.insert( Qt::Key_C, MWindow::SouthEast );
    m_actions.insert( Qt::Key_X, MWindow::South );
    m_actions.insert( Qt::Key_Z, MWindow::SouthWest );
    m_actions.insert( Qt::Key_A, MWindow::West );
    m_actions.insert( Qt::Key_Q, MWindow::NorthWest );

    m_actions.insert( Qt::Key_S, MWindow::Button );
}
