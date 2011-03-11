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
                        int number,
                        Pitch* pitch,
                        Team* team,
                        QColor hairColor);

    // goal keepers are players too
    enum { Type = UserType + 2 };
    int type() const
        { return Type;}

public slots:
    void goalAttempt(Team* t, QPointF dest);

public:
    void createPixmaps();
    void advance(int phase);
    void createMoves();
    void pixmapInsert(MWindow::Action a, QString s1, QString s2, QString s3, QRgb goalKeeperShirtColor, QRgb goalKeeperShortColor);

private:
    void gkAdvanceWithBall();
    void gkAdvanceWithoutBall();
};

#endif // GOALKEEPER_H
