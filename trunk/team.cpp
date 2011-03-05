#include "team.h"
#include <QtGui>
#include <QtGlobal>
// #include "compileTimeSettings.h" // KPlayerDefaultSpeed
#include <QDebug>

void TeamManager::createTeams()
{
    QStringList colorNames(QColor::colorNames());

    // for each team in the directory
    QFile file(":/teams/teams.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (line.contains('#'))
            continue;

        QList<QByteArray> nameAndColor = line.split(',');
        QString briefName = nameAndColor.at(0).simplified();
        QString name = nameAndColor.at(1).simplified();

        QString shirtColorString = nameAndColor.at(2).simplified();
        QString shortColorString = nameAndColor.at(3).simplified();

        const int playerSpeed = nameAndColor.at(4).simplified().toInt();

        QColor shirtColor(shirtColorString);
        QColor shortColor(shortColorString);

        Team* t = new Team(briefName, name, shirtColor, shortColor, playerSpeed);
        m_teams.append(t);
    }
    file.close();
}


Team::Team(QString briefName, QString teamName, QColor shirtColor, QColor shortColor, int playerSpeed)
    :
    m_briefName(briefName),
    m_name(teamName),
    m_shirtColor(shirtColor),
    m_shortColor(shortColor),
    m_teamHasBall(false),
    m_goals(0),
    m_scoredLastGoal(false),
    m_speed(playerSpeed)
{
}

void Team::setHasBall(bool hasBall)
{
    m_teamHasBall = hasBall;
}

void Team::goalScored(bool isNorthGoal)
{
    if ( (isNorthGoal && m_direction == Team::SouthToNorth)
        || (!isNorthGoal && m_direction == Team::NorthToSouth) ) {
        m_goals++;
        m_scoredLastGoal = true;
    } else {
        m_scoredLastGoal = false;
    }
}
