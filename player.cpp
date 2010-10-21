#include "Player.h"
#include "pitch.h"
#include "ball.h"

#include <QtGui>
#include <QDebug>
#include <QGraphicsItem>

namespace {

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

void Player::createPixmaps()
{
    QString s(":/images/");
    if (team_ == pitch_->homeTeam_)
        s.append("red/");
    else
        s.append("blue/");

    QStringList list;
    QString n1(s), n2(s), n3(s);
    list <<   n1.append("playerNorth.PNG") << n2.append("playerNorth1.PNG") << n3.append("playerNorth2.PNG");
    images_.insert(MWindow::North, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << n1.append("playerNorthEast.PNG") << n2.append("playerNorthEast1.PNG")<< n3.append("playerNorthEast2.PNG");
    images_.insert(MWindow::NorthEast, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << n1.append("playerEast.PNG") << n2.append("playerEast1.PNG")<< n3.append("playerEast2.PNG");
    images_.insert(MWindow::East, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << n1.append("playerSouthEast.PNG") << n2.append("playerSouthEast1.PNG")<< n3.append("playerSouthEast2.PNG");
    images_.insert(MWindow::SouthEast, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << n1.append("playerSouth.PNG") << n2.append("playerSouth1.PNG")<< n3.append("playerSouth2.PNG");
    images_.insert(MWindow::South, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << n1.append("playerSouthWest.PNG") << n2.append("playerSouthWest1.PNG")<< n3.append("playerSouthWest2.PNG");
    images_.insert(MWindow::SouthWest, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << n1.append("playerWest.PNG") << n2.append("playerWest1.PNG")<< n3.append("playerWest2.PNG");
    images_.insert(MWindow::West, list);
    list.clear();

    n1 = s; n2 = s; n3 = s;
    list << n1.append("playerNorthWest.PNG") << n2.append("playerNorthWest1.PNG")<< n3.append("playerNorthWest2.PNG");
    images_.insert(MWindow::NorthWest, list);
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
    color(team->color),
    buttonDown_(false),
    speed_(computerControlled ? KPlayerDefaultSpeed - 1 : KPlayerDefaultSpeed),
    step(0),
    outOfAction_(NULL)
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    outOfAction_ = new QTimer(this);
    outOfAction_->setSingleShot(true);

    createMoves();
    createPixmaps();

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
    if ( humanControlled_ && !pitch_->isReplay()) {
        QBrush brush(Qt::white, Qt::SolidPattern);
        painter->setBrush(brush);
        painter->drawEllipse(QPointF(0,0), 10*KScaleFactor, 10*KScaleFactor);
    }

    QSize pixmapSize = pixmap().size();
    pixmapSize.scale(QSizeF(36*KScaleFactor,36*KScaleFactor).toSize(), Qt::KeepAspectRatio);

    // Draw QGraphicsPixmapItem face
    painter->drawPixmap(boundingRect().toRect(), pixmap());
}

void Player::goalScored(bool isLeftGoal)
{
    qDebug() << "Player::goalScored";
    setPixmap(QPixmap(images_[MWindow::North].at(step % 3)));
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
    if ( ballCollisionCheck() && pitch_->getBall()->controlledBy() == NULL ) {
        hasBall_ = true;
        pitch_->getBall()->setControlledBy(this);
    }
    else if ( !ballCollisionCheck())
        hasBall_ = false;

    if ( hasBall_ )
        pitch_->getBall()->moveBall(action, speed_);

    step++;
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
        setPixmap(QPixmap(images_[action].at(step % 3)));
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
    qDebug() << "specialAction start";
    // if not have ball then must be tackle
    // if have ball then either shot or pass
    if ( hasBall_ ) {
        if ( withinShootingDistance() ) {
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
                pitch_->getBall()->moveBall(MWindow::Pass, p->pos());
                hasBall_ = false;
                pitch_->getBall()->setControlledBy(NULL);
            }
        }
    }
    else {
        qDebug() << "specialAction tackle";
        // perform tackle here...

        switch(lastAction_) {
        case MWindow::North:
            setPixmap(QPixmap(QString(":/images/red/tackleNorth.PNG")));
            moveBy(moveDistance_[lastAction_].x(), moveDistance_[lastAction_].y());
            break;
        case MWindow::NorthEast:
            setPixmap(QPixmap(QString(":/images/red/tackleNorthEast.PNG")));
            moveBy(moveDistance_[lastAction_].x(), moveDistance_[lastAction_].y());
            break;
        case MWindow::East:
            setPixmap(QPixmap(QString(":/images/red/tackleEast.PNG")));
            moveBy(moveDistance_[lastAction_].x(), moveDistance_[lastAction_].y());
            break;
        case MWindow::SouthEast:
            setPixmap(QPixmap(QString(":/images/red/tackleSouthEast.PNG")));
            moveBy(moveDistance_[lastAction_].x(), moveDistance_[lastAction_].y());
            break;
        case MWindow::South:
            setPixmap(QPixmap(QString(":/images/red/tackleSouth.PNG")));
            moveBy(moveDistance_[lastAction_].x(), moveDistance_[lastAction_].y());
            break;
        case MWindow::SouthWest:
            setPixmap(QPixmap(QString(":/images/red/tackleSouthWest.PNG")));
            moveBy(moveDistance_[lastAction_].x(), moveDistance_[lastAction_].y());
            break;
        case MWindow::West:
            setPixmap(QPixmap(QString(":/images/red/tackleWest.PNG")));
            moveBy(moveDistance_[lastAction_].x(), moveDistance_[lastAction_].y());
            break;
        case MWindow::NorthWest:
            setPixmap(QPixmap(QString(":/images/red/tackleNorthWest.PNG")));
            moveBy(moveDistance_[lastAction_].x(), moveDistance_[lastAction_].y());
            break;
        }
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

bool Player::canPass() const
{
    return findAvailableTeamMate();
}

void Player::move(MWindow::Action action)
{
    //qDebug() << "move start";

    if (action == MWindow::Button
        || action == MWindow::Shoot
        || action == MWindow::Tackle
        || action == MWindow::Pass ) {
        qDebug() << "move specialAction";
        specialAction(action);
    }
    else
        movePlayer(action);
    //qDebug() << "move end";
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
    else
        automove();
}

void Player::computerAdvanceWithBall()
{
    //qDebug() << "computerAdvanceWithBall start";
    QPointF destination;
    if (team_->getDirection() == Team::SouthToNorth )
        destination = QPointF(pitch_->topGoal->rect().center().x(), pitch_->topGoal->rect().top());
    else
        destination = QPointF(pitch_->bottomGoal->rect().center().x(), pitch_->bottomGoal->rect().bottom());

    MWindow::Action act = calculateAction(pos(), destination);

    switch(role_) {
        // goal keepers kick the ball
    case Player::GoalKeeper:
        move(MWindow::Pass);
        break;
    // defenders run with it until man marked
    case Player::LeftDefence:
    case Player::RightDefence:
    case Player::LeftMidfield:
    case Player::CentralMidfield:
    case Player::RightMidfield:
    case Player::LeftAttack:
    case Player::RightAttack:
    case Player::CentralAttack:
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

QVariant Player::itemChange(GraphicsItemChange change, const QVariant &value)
 {
     if (change == ItemPositionChange && scene()) {
         // value is the new position.
         QPointF newPos = value.toPointF();
            // TODO penalty scecnarios
            // offside scenarios
         }
     return QGraphicsItem::itemChange(change, value);
 }
