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
    Team(QString teamName, QColor shirtColor, QColor shortColor);
    QString name() { return m_name; }
    void setDirection(Direction dir) { m_direction = dir; }
    Direction getDirection() { return m_direction; }
    void setHasBall(bool hasBall);
    bool scoredLastGoal() { return m_scoredLastGoal; }
    void setShots(int newValue) { m_shotCount = newValue; }
    inline int shots() { return m_shotCount; }
    void newGame() { m_goals = 0; m_scoredLastGoal = false; m_teamHasBall = false; m_shotCount = 0; }
    inline bool teamHasBall() { return m_teamHasBall; }
    inline int speed() { return m_speed; }

public slots:
    void goalScored(bool isNorthGoal);

public:
    QColor m_shirtColor;
    QColor m_shortColor;
    Direction m_direction;
    int m_goals;

private:
    QString m_name;
    bool m_teamHasBall;
    // this team scored the last goal
    bool m_scoredLastGoal;
    int m_shotCount;
    int m_speed;
};


#endif // TEAM_H
