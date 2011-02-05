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
    QString name() { return m_name; }
    void setDirection(Direction dir) { m_direction = dir; }
    Direction getDirection() { return m_direction; }
    void setHasBall(bool hasBall);
    bool scoredLastGoal() { return m_scoredLastGoal; }
    void newGame() { m_goals = 0; m_scoredLastGoal = false; m_teamHasBall = false; }
    inline bool teamHasBall() { return m_teamHasBall; }

public slots:
    void goalScored(bool isNorthGoal);

public:
    QColor color;
    Direction m_direction;
    int m_goals;
    // this team scored the last goal
    bool m_scoredLastGoal;
private:
    QString m_name;
    bool m_teamHasBall;
};


#endif // TEAM_H
