#ifndef GOALKEEPER_H
#define GOALKEEPER_H

#include <QtGui>

#include "Player.h"

class Pitch;
class Team;

class GoalKeeper : public Player
{
    Q_OBJECT
public:
    explicit GoalKeeper(QString name,
                        Pitch* pitch,
                        Team* team);

    enum { Type = UserType + 3 };
    int type() const
        { return Type;}

public slots:
    void shotAttempted(QPointF dest);

public:
    void createPixmaps();
    void advance(int phase);
    void createMoves();

private:
    void gkAdvanceWithBall();
    void gkAdvanceWithoutBall();
private:
    QMap<MWindow::Action,QPointF> moveDistance_;
};

#endif // GOALKEEPER_H
