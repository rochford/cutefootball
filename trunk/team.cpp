#include "team.h"

Team::Team(QString teamName, QColor teamColor)
    : name_(teamName),
    color(teamColor),
    state_(Attacking),
    teamHasBall_(false),
    goals_(0)
{
}

void Team::setHasBall(bool hasBall)
{
    teamHasBall_ = hasBall;
    hasBall ? state_ = Attacking : state_ = Defending;
}

void Team::goalScored(bool isNorthGoal)
{
    if ( (isNorthGoal && dir_ == Team::SouthToNorth)
        || (!isNorthGoal && dir_ == Team::NorthToSouth) )
        goals_++;
}
