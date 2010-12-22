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

public:
    void createPixmaps();
    void advance(int phase);
//    void createMoves();
    void pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3, QRgb goalKeeperColor);

private:
    void gkAdvanceWithBall();
    void gkAdvanceWithoutBall();
};

#endif // GOALKEEPER_H
