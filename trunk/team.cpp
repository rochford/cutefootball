#include "team.h"
#include <QDebug>

Team::Team(QString teamName, QColor teamColor)
    : name_(teamName),
    color(teamColor),
    teamHasBall_(false),
    goals_(0),
    m_scoredLastGoal(false)
{
}

void Team::setHasBall(bool hasBall)
{
    teamHasBall_ = hasBall;
}

void Team::goalScored(bool isNorthGoal)
{
    if ( (isNorthGoal && dir_ == Team::SouthToNorth)
        || (!isNorthGoal && dir_ == Team::NorthToSouth) ) {
        goals_++;
        m_scoredLastGoal = true;
    } else {
        m_scoredLastGoal = false;
    }
}
