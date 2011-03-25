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

        QList<QByteArray> values = line.split(',');
        int ranking = values.at(Ranking).simplified().toInt();
        QString briefName = values.at(BriefName).simplified();
        QString name = values.at(FullName).simplified();

        QString shirtColorString = values.at(ShirtColor).simplified();
        QString shortColorString = values.at(ShortColor).simplified();

        const int playerSpeed = values.at(PlayerSpeed).simplified().toInt();

        QColor shirtColor(shirtColorString);
        QColor shortColor(shortColorString);

        Team* t = new Team(ranking,
                           briefName,
                           name,
                           shirtColor,
                           shortColor,
                           playerSpeed);
        m_teams.append(t);
    }
    file.close();
}


Team::Team(int ranking, QString briefName, QString teamName, QColor shirtColor, QColor shortColor, int playerSpeed)
    :
    m_briefName(briefName),
    m_name(teamName),
    m_shirtColor(shirtColor),
    m_shortColor(shortColor),
    m_teamHasBall(false),
    m_goals(0),
    m_scoredLastGoal(false),
    m_speed(playerSpeed),
    m_rank(ranking)
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
