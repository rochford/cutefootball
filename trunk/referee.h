#ifndef REFEREE_H
#define REFEREE_H

#include <QtGui>

#include "Player.h"

class Pitch;

class Referee : public Player
{
public:
    explicit Referee(Pitch* pitch, QObject *parent = 0);

    enum { Type = UserType + 4 };
    int type() const
        { return Type;}

    void advance(int phase);
public:
    virtual void createPixmaps();
};

#endif // REFEREE_H
