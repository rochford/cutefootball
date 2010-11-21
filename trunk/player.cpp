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

    QString n1(s), n2(s), n3(s);
    QPixmap p1, p2, p3;

    teamColorTransform(p1, n1.append(s1), qRgb(255, 0, 0), teamColor);
    teamColorTransform(p2, n2.append(s2), qRgb(255, 0, 0), teamColor);
    teamColorTransform(p3, n3.append(s3), qRgb(255, 0, 0), teamColor);

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
    m_humanControlled(false),
    m_speed(computerControlled ? KPlayerDefaultSpeed - 1 : KPlayerDefaultSpeed),
    m_step(0),
    m_outOfAction(NULL)
{
    setToolTip(m_name);
    m_outOfAction = new QTimer(this);
    m_outOfAction->setSingleShot(true);

#ifdef REFEREE_USED
    // referees dont celebrate goals
    if (role != Player::LastDummy)
#endif //
        connect(m_pitch->ball(), SIGNAL(goalScored(bool)), this, SLOT(goalScored(bool)));
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
    if (m_humanControlled
        && !m_pitch->replay()->isReplay()
        && m_pitch->gameInProgress()) {
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
    const int KShotPower = this->m_pitch->m_scene->sceneRect().width() / 2;
    QPointF dest(p);
    switch(m_lastAction)
    {
    case MWindow::North:
        dest.setY(p.y() - KShotPower);
        break;
    case MWindow::NorthEast:
        dest.setX(p.x() + KShotPower);
        dest.setY(p.y() - KShotPower);
        break;
    case MWindow::East:
        dest.setX(p.x() + KShotPower);
        break;
    case MWindow::SouthEast:
        dest.setX(p.x() + KShotPower);
        dest.setY(p.y() + KShotPower);
        break;
    case MWindow::South:
        dest.setY(p.y() + KShotPower);
        break;
    case MWindow::SouthWest:
        dest.setX(p.x() - KShotPower);
        dest.setY(p.y() + KShotPower);
        break;
    case MWindow::West:
        dest.setX(p.x() - KShotPower);
        break;
    case MWindow::NorthWest:
        dest.setX(p.x() - KShotPower);
        dest.setY(p.y() - KShotPower);
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
            QPointF goal;
            // prefer the player closest to the opposition goal
            if (team_->getDirection() == Team::NorthToSouth)
                goal = m_pitch->m_bottomGoal->rect().center();
            else
                goal = m_pitch->m_topGoal->rect().center();
             m_pitch->ball()->kickBall(MWindow::Shot, dest);
             hasBall_ = false;
             m_pitch->ball()->setControlledBy(NULL);
        } else {
            Player *p = findAvailableTeamMate();
            if (p) {
                m_pitch->ball()->kickBall(MWindow::Pass, p->pos());
                hasBall_ = false;
                m_pitch->ball()->setControlledBy(NULL);
            }
        }
    } else {
        // perform tackle here...

        switch(m_lastAction) {
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
Player* Player::findAvailableTeamMate() const
{
    // 1 find an attacker to pass to
    Player *bestPlayer = NULL;
    int nearest = 0xffff;
    foreach (Player *p, m_pitch->m_players) {
        if (p->team_!= team_)
            continue;
        // not self
        if (p == this)
            continue;        
        // continue if they are man marked
//        if ( p->isManMarked() )
//            continue;

        QPointF goal;
        // prefer the player closest to the opposition goal
        if (team_->getDirection() == Team::NorthToSouth)
            goal = m_pitch->m_bottomGoal->rect().center();
        else
            goal = m_pitch->m_topGoal->rect().center();

        const int dx = p->pos().x() - goal.x();
        const int dy = p->pos().y() - goal.y();

        if ((qAbs(dx*dx)+qAbs(dy*dy)) < nearest) {
            bestPlayer = p;
            nearest = qAbs(qAbs(dx*dx)+qAbs(dy*dy));
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
        if (nearestPlayer == this) {
            // if close to the ball then tackle

            MWindow::Action action;
            int dx = abs(pos().x() - m_pitch->ball()->pos().x());
            int dy = abs(pos().y() - m_pitch->ball()->pos().y());
            if ( m_pitch->ball()->controlledBy() && ( dx < 10) && (dy < 10) )
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
    if (m_humanControlled || hasBall_)
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

