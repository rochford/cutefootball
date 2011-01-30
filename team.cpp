#include "team.h"
#include <QDebug>

Team::Team(QString teamName, QColor teamColor)
    : name_(teamName),
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
    if ( (isNorthGoal && dir_ == Team::SouthToNorth)
        || (!isNorthGoal && dir_ == Team::NorthToSouth) ) {
        m_goals++;
        m_scoredLastGoal = true;
    } else {
        m_scoredLastGoal = false;
    }
}
