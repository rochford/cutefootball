#include "Player.h"
#include "pitch.h"
#include "ball.h"
#include "replay.h"

#include <QtGui>
#include <QDebug>
#include <QGraphicsItem>


MWindow::Action calculateAction(QPointF source,
                                QPointF destination)
{
    const int dx = source.x() - destination.x();
    const int dy = source.y() - destination.y();
    if ( dx > 0 && dy == 0 )
        return MWindow::West;
    else if ( dx >= 0 && dy < 0)
        return MWindow::SouthWest;
    else if ( dx > 0 && dy > 0)
        return MWindow::NorthWest;
    else if ( dx <= 0 && dy == 0)
        return MWindow::East;
    else if ( dx < 0 && dy > 0)
        return MWindow::NorthEast;
    else if ( dx < 0 && dy < 0)
        return MWindow::SouthEast;
    else if ( dx == 0 && dy >= 0 )
        return MWindow::North;
    else if ( dx == 0 && dy <= 0 )
        return MWindow::South;
}

void Player::createMoves()
{
    moveDistance_.insert(MWindow::North, QPointF(0,-speed_));
    moveDistance_.insert(MWindow::NorthEast, QPointF(speed_,-speed_));
    moveDistance_.insert(MWindow::East, QPointF(speed_,0));
    moveDistance_.insert(MWindow::SouthEast, QPointF(speed_,speed_));
    moveDistance_.insert(MWindow::South, QPointF(0,speed_));
    moveDistance_.insert(MWindow::SouthWest, QPointF(-speed_,speed_));
    moveDistance_.insert(MWindow::West, QPointF(-speed_,0));
    moveDistance_.insert(MWindow::NorthWest, QPointF(-speed_,-speed_));
}

void Player::pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3)
{
    QString s(":/images/");
    if (team_ == pitch_->homeTeam_)
        s.append("red/");
    else
        s.append("blue/");

    QString n1(s), n2(s), n3(s);
    QStringList list;
    list <<   n1.append(s1) << n2.append(s2) << n3.append(s3);
    images_.insert(a, list);
}

void Player::createPixmaps()
{
    QString s(":/images/");
    if (team_ == pitch_->homeTeam_)
        s.append("red/");
    else
        s.append("blue/");

    pixmapInsert(MWindow::North, "playerNorth.PNG", "playerNorth1.PNG", "playerNorth2.PNG");
    pixmapInsert(MWindow::NorthEast, "playerNorthEast.PNG", "playerNorthEast1.PNG", "playerNorthEast2.PNG");
    pixmapInsert(MWindow::East, "playerEast.PNG", "playerEast1.PNG", "playerEast2.PNG");
    pixmapInsert(MWindow::SouthEast, "playerSouthEast.PNG", "playerSouthEast1.PNG", "playerSouthEast2.PNG");
    pixmapInsert(MWindow::South, "playerSouth.PNG", "playerSouth1.PNG", "playerSouth2.PNG");
    pixmapInsert(MWindow::SouthWest, "playerSouthWest.PNG", "playerSouthWest1.PNG", "playerSouthWest2.PNG");
    pixmapInsert(MWindow::West, "playerWest.PNG", "playerWest1.PNG", "playerWest2.PNG");
    pixmapInsert(MWindow::NorthWest, "playerNorthWest.PNG", "playerNorthWest1.PNG", "playerNorthWest2.PNG");

    pixmapInsert(MWindow::ThrownIn, "playerNorthWest.PNG", "playerNorthWest1.PNG", "playerNorthWest2.PNG"); // TODO XXX TIM

    QStringList list;
    QString n1(s), n2(s), n3(s);

    n1 = s; n2 = s; n3 = s;
    list << QString(":/images/red/tackleNorth.PNG");
    images_.insert(MWindow::TackleNorth, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << QString(":/images/red/tackleNorthEast.PNG");
    images_.insert(MWindow::TackleNorthEast, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << QString(":/images/red/tackleEast.PNG");
    images_.insert(MWindow::TackleEast, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << QString(":/images/red/tackleSouthEast.PNG");
    images_.insert(MWindow::TackleSouthEast, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << QString(":/images/red/tackleSouth.PNG");
    images_.insert(MWindow::TackleSouth, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << QString(":/images/red/tackleSouthWest.PNG");
    images_.insert(MWindow::TackleSouthWest, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << QString(":/images/red/tackleWest.PNG");
    images_.insert(MWindow::TackleWest, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << QString(":/images/red/tackleNorthWest.PNG");
    images_.insert(MWindow::TackleNorthWest, list);
}

Player::Player(bool computerControlled,
               Pitch *pitch,
               Team* team,
               Role role)
    : QObject(),
    QGraphicsPixmapItem(NULL,NULL),
    hasBall_(false),
    team_(team),
    role_(role),
    pitch_(pitch),
    humanControlled_(false),
    computerControlled(computerControlled),
    buttonDown_(false),
    speed_(computerControlled ? KPlayerDefaultSpeed - 1 : KPlayerDefaultSpeed),
    step_(0),
    outOfAction_(NULL)
{
    outOfAction_ = new QTimer(this);
    outOfAction_->setSingleShot(true);

    createMoves();

    connect(pitch_->getBall(), SIGNAL(goalScored(bool)),this,SLOT(goalScored(bool)));
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
    if ( humanControlled_ && !pitch_->replay_->isReplay()) {
        QBrush brush(Qt::white, Qt::Dense3Pattern);
        painter->setBrush(brush);
        painter->drawEllipse(QPointF(0,0), 8*KScaleFactor, 8*KScaleFactor);
    }

    QSize pixmapSize = pixmap().size();
    pixmapSize.scale(QSizeF(36*KScaleFactor,36*KScaleFactor).toSize(), Qt::KeepAspectRatio);

    // Draw QGraphicsPixmapItem face
    painter->drawPixmap(boundingRect().toRect(), pixmap());
}

void Player::goalScored(bool isLeftGoal)
{
    qDebug() << "Player::goalScored start";
 //   setPixmap(QPixmap(images_[MWindow::North].at(step_ % 3)));
    qDebug() << "Player::goalScored end";
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
    if ( ballCollisionCheck() && !pitch_->getBall()->controlledBy() ) {
        hasBall_ = true;
        pitch_->getBall()->setControlledBy(this);
    }
    else if ( !ballCollisionCheck())
        hasBall_ = false;

    if ( hasBall_ )
        pitch_->getBall()->moveBall(action, speed_);

    step_++;
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
        setPixmap(QPixmap(images_[action].at(step_ % 3)));
        moveBy(moveDistance_[action].x(), moveDistance_[action].y());
        lastAction_ = action;
        break;
    }
}

bool Player::withinShootingDistance() const
{
    if (team_->getDirection() == Team::NorthToSouth
        && pitch_->bottomPenaltyArea->contains(pitch_->getBall()->pos()))
        return true;
    else if (team_->getDirection() == Team::SouthToNorth
             && pitch_->topPenaltyArea->contains(pitch_->getBall()->pos()))
        return true;
    else {
//        qDebug() << "withinShootingDistance return false";
        return false;
    }
}

void Player::specialAction(MWindow::Action action)
{
    switch ( action ) {
    case MWindow::ThrownIn:
        {
            setPixmap(QPixmap(images_[action].at(0)));
            outOfAction_->start(1500);
        }
        return;
    default:
        break;
    }

    qDebug() << "specialAction start";
    // if not have ball then must be tackle
    // if have ball then either shot or pass
    if ( hasBall_ ) {
        qDebug() << "specialAction has ball";
        if ( withinShootingDistance() ) {
            qDebug() << "specialAction within shooting distance";
            QPointF goal;
            // prefer the player closest to the opposition goal
            if (team_->getDirection() == Team::NorthToSouth)
                goal = pitch_->bottomGoal->rect().center();
            else
                goal = pitch_->topGoal->rect().center();
             pitch_->getBall()->moveBall(MWindow::Shoot, goal);
             hasBall_ = false;
             pitch_->getBall()->setControlledBy(NULL);
        }
        else
        {
            qDebug() << "specialAction pass Ball";

            Player *p = findAvailableTeamMate();
            if (p) {
                qDebug() << "specialAction found team mate";
                pitch_->getBall()->moveBall(MWindow::Pass, p->pos());
                hasBall_ = false;
                pitch_->getBall()->setControlledBy(NULL);
                qDebug() << "specialAction found team mate end";
            }
        }
    }
    else {
        qDebug() << "specialAction tackle";
        // perform tackle here...

        switch(lastAction_) {
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
        }
        setPixmap(QPixmap(images_[action].at(0)));
        moveBy(moveDistance_[lastAction_].x(), moveDistance_[lastAction_].y());
        //outOfAction_->stop();
        //outOfAction_->start(500);

        // if tackle causes contact with the ball then transfer the ownership
        if ( ballCollisionCheck() ) {
            Player *p = pitch_->getBall()->controlledBy();
            qDebug() << "specialAction tackle";
            if (p)
                p->isTackled(true);
            pitch_->getBall()->setControlledBy(this);
            hasBall_ = true;
        }
    }
    qDebug() << "specialAction end";
}

void Player::isTackled(bool defeated)
{
    if (defeated) {
        // TODO use a different pixmap
        // start an out of action timer
        outOfAction_->stop();
        outOfAction_->start(1500);
    }
}

void Player::move(MWindow::Action action)
{
    if (action == MWindow::Button
        || action == MWindow::Shoot
        || action == MWindow::Tackle
        || action == MWindow::Pass
        || action == MWindow::ThrownIn ) {
        qDebug() << "move specialAction";
        specialAction(action);
    }
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
    foreach (Player *p, pitch_->players_) {
        if ( p->team_!= team_)
            continue;
        // not self
        if ( p == this )
            continue;        
        // continue if they are man marked
//        if ( p->isManMarked() )
//            continue;

        QPointF goal;
        // prefer the player closest to the opposition goal
        if (team_->getDirection() == Team::NorthToSouth)
            goal = pitch_->bottomGoal->rect().center();
        else
            goal = pitch_->topGoal->rect().center();

        const int dx = p->pos().x() - goal.x();
        const int dy = p->pos().y() - goal.y();

        if ( (qAbs(dx*dx)+qAbs(dy*dy)) < nearest) {
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
    if (outOfAction_->isActive())
        return;
    if ( hasBall_ )
         computerAdvanceWithBall();
    else
        computerAdvanceWithoutBall();
}

void Player::computerAdvanceWithoutBall()
{
    setZValue(5);
    if (!team_->teamHasBall_) {
        // qDebug() << "computerAdvanceWithoutBall";
        Player *nearestPlayer = pitch_->selectNearestPlayer(pitch_->awayTeam_);
        if (nearestPlayer == this) {
            // if close to the ball then tackle

            MWindow::Action action;
            int dx = abs(pos().x() - pitch_->getBall()->pos().x());
            int dy = abs(pos().y() - pitch_->getBall()->pos().y());
            if ( pitch_->getBall()->controlledBy() && ( dx < 15) && (dy < 15) )
                action = MWindow::Tackle;
            else
                action = calculateAction(pos(), pitch_->getBall()->pos());

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
    if ( lastAction_ == MWindow::ThrownIn ) {
        pitch_->getBall()->setVisible( true );
        move(MWindow::Pass);
        return;
    }

    //qDebug() << "computerAdvanceWithBall start";
    QPointF destination;
    if (team_->getDirection() == Team::SouthToNorth )
        destination = QPointF(pitch_->topGoal->rect().center().x(), pitch_->topGoal->rect().top());
    else
        destination = QPointF(pitch_->bottomGoal->rect().center().x(), pitch_->bottomGoal->rect().bottom());

    MWindow::Action act = calculateAction(pos(), destination);

    switch(role_) {
    case Player::GoalKeeper:
        // should not happen
        break;
    default:
        if (withinShootingDistance()) {
            move(MWindow::Shoot);
            hasBall_ = false;
            pitch_->getBall()->setControlledBy(NULL);
        }
        else
            move(act);
        break;
    }
}

void Player::automove()
{
    // automove not applicable to human players or ball holders
    if ( humanControlled_ || hasBall_ )
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

    if (team_ == pitch_->awayTeam_)
        computerAdvance(phase);
    else
        humanAdvance(phase);
}

