#include "team.h"
// #include "compileTimeSettings.h" // KPlayerDefaultSpeed
#include <QDebug>

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
