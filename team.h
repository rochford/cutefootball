#ifndef TEAM_H
#define TEAM_H

#include <QObject>
#include <QColor>
#include <QString>

class Team : public QObject {
        Q_OBJECT;
public:
    enum State { Attacking, Defending, None };
    // the attacking direction
    enum Direction { SouthToNorth, NorthToSouth };
    Team(QString teamName, QColor teamColor);
    QString name() { return name_; }
    void setDirection(Direction dir) { dir_ = dir; }
    Direction getDirection() { return dir_; }
    void setHasBall(bool hasBall);
    void setState(State s) {  state_ = s;}

    public slots:
            void goalScored(bool isNorthGoal);

public:
    QString name_;
    QColor color;
    State state_;
    Direction dir_;
    bool teamHasBall_;
    int goals_;
};


#endif // TEAM_H
