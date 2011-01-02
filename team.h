#ifndef TEAM_H
#define TEAM_H

#include <QObject>
#include <QColor>
#include <QString>

class Team : public QObject {
    Q_OBJECT;
public:
    // the attacking direction
    enum Direction { SouthToNorth, NorthToSouth };
    Team(QString teamName, QColor teamColor);
    QString name() { return name_; }
    void setDirection(Direction dir) { dir_ = dir; }
    Direction getDirection() { return dir_; }
    void setHasBall(bool hasBall);
    bool scoredLastGoal() { return m_scoredLastGoal; }

public slots:
    void goalScored(bool isNorthGoal);

public:
    QString name_;
    QColor color;
    Direction dir_;
    bool teamHasBall_;
    int goals_;
    // this team scored the last goal
    bool m_scoredLastGoal;
};


#endif // TEAM_H
