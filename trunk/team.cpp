#include "team.h"
#include <QDebug>

Team::Team(QString teamName, QColor teamColor)
    : m_name(teamName),
    color(teamColor),
    m_teamHasBall(false),
    m_goals(0),
    m_scoredLastGoal(false)
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
