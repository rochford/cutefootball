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
    Team(QString briefName, QString teamName, QColor shirtColor, QColor shortColor, int playerSpeed);
    QString fullName() const { return m_name; }
    void setDirection(Direction dir) { m_direction = dir; }
    Direction getDirection() const { return m_direction; }
    void setHasBall(bool hasBall);
    bool scoredLastGoal() const { return m_scoredLastGoal; }
    void setShots(int newValue) { m_shotCount = newValue; }
    inline int shots() const { return m_shotCount; }
    void newGame() { m_goals = 0; m_scoredLastGoal = false; m_teamHasBall = false; m_shotCount = 0; }
    inline bool teamHasBall() const { return m_teamHasBall; }
    inline int speed() const { return m_speed; }
    inline QString briefName() const { return m_briefName; }

public slots:
    void goalScored(bool isNorthGoal);

public:
    QColor m_shirtColor;
    QColor m_shortColor;
    Direction m_direction;
    int m_goals;

private:
    QString m_briefName;
    QString m_name;
    bool m_teamHasBall;
    // this team scored the last goal
    bool m_scoredLastGoal;
    int m_shotCount;
    int m_speed;
};


#endif // TEAM_H
