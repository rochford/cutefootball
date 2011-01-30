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
    void newGame() { m_goals = 0; m_scoredLastGoal = false; m_teamHasBall = false; }
    inline bool teamHasBall() { return m_teamHasBall; }

public slots:
    void goalScored(bool isNorthGoal);

public:
    QString name_;
    QColor color;
    Direction dir_;
    int m_goals;
    // this team scored the last goal
    bool m_scoredLastGoal;
private:
    bool m_teamHasBall;
};


#endif // TEAM_H
