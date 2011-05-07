/*
 * Copyright 2010,2011 Timothy Rochford
 *
 *    This file is part of CuteFootball.
 *
 *    CuteFootball is free software: you can redistribute it and/or modify
 *    it under the terms of the Lesser GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    CuteFootball is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Lesser GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "team.h"
#include <QtGui>
#include <QtGlobal>
// #include "compileTimeSettings.h" // KPlayerDefaultSpeed
#include <QDebug>
#include "soccerutils.h"
#include "teamNameTranslations.h"

TeamManager::TeamManager()
{
}

TeamManager::~TeamManager()
{
}

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


QString TeamManager::translateTeamName(QString untranslated)
{
    if (untranslated == "Andorra") return QObject::tr("Andorra");
    else if (untranslated == "Argentina") return QObject::tr("Argentina");
    else if (untranslated == "Australia") return QObject::tr("Australia");
    else if (untranslated == "Austria") return QObject::tr("Austria");
    else if (untranslated == "Belgium") return QObject::tr("Belgium");
    else if (untranslated == "Brazil") return QObject::tr("Brazil");
    else if (untranslated == "Bulgaria") return QObject::tr("Bulgaria");
    else if (untranslated == "China") return QObject::tr("China");
    else if (untranslated == "Croatia") return QObject::tr("Croatia");
    else if (untranslated == "Czech") return QObject::tr("Czech");
    else if (untranslated == "Denmark") return QObject::tr("Denmark");
    else if (untranslated == "Egypt") return QObject::tr("Egypt");
    else if (untranslated == "England") return QObject::tr("England");
    else if (untranslated == "Estonia") return QObject::tr("Estonia");
    else if (untranslated == "Finland") return QObject::tr("Finland");
    else if (untranslated == "France") return QObject::tr("France");
    else if (untranslated == "Germany") return QObject::tr("Germany");
    else if (untranslated == "Greece") return QObject::tr("Greece");
    else if (untranslated == "Hungary") return QObject::tr("Hungary");
    else if (untranslated == "Iceland") return QObject::tr("Iceland");
    else if (untranslated == "India") return QObject::tr("India");
    else if (untranslated == "Ireland") return QObject::tr("Ireland");
    else if (untranslated == "Italy") return QObject::tr("Italy");
    else if (untranslated == "Japan") return QObject::tr("Japan");
    else if (untranslated == "Malaysia") return QObject::tr("Malaysia");
    else if (untranslated == "Malta") return QObject::tr("Malta");
    else if (untranslated == "Mexico") return QObject::tr("Mexico");
    else if (untranslated == "Netherlands") return QObject::tr("Netherlands");
    else if (untranslated == "NorthernIreland") return QObject::tr("NorthernIreland");
    else if (untranslated == "Norway") return QObject::tr("Norway");
    else if (untranslated == "Peru") return QObject::tr("Peru");
    else if (untranslated == "Philippines") return QObject::tr("Philippines");
    else if (untranslated == "Poland") return QObject::tr("Poland");
    else if (untranslated == "Portugal") return QObject::tr("Portugal");
    else if (untranslated == "Romania") return QObject::tr("Romania");
    else if (untranslated == "Russia") return QObject::tr("Russia");
    else if (untranslated == "SanMarino") return QObject::tr("SanMarino");
    else if (untranslated == "Scotland") return QObject::tr("Scotland");
    else if (untranslated == "Serbia") return QObject::tr("Serbia");
    else if (untranslated == "Slovakia") return QObject::tr("Slovakia");
    else if (untranslated == "SouthAfrica") return QObject::tr("SouthAfrica");
    else if (untranslated == "Spain") return QObject::tr("Spain");
    else if (untranslated == "Sweden") return QObject::tr("Sweden");
    else if (untranslated == "Switzerland") return QObject::tr("Switzerland");
    else if (untranslated == "Turkey") return QObject::tr("Turkey");
    else if (untranslated == "Ukraine") return QObject::tr("Ukraine");
    else if (untranslated == "UnitedStates") return QObject::tr("UnitedStates");
    else if (untranslated == "Uruguay") return QObject::tr("Uruguay");
    else if (untranslated == "Vietnam") return QObject::tr("Vietnam");
    else if (untranslated == "Wales") return QObject::tr("Wales");
    else return untranslated;
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
    m_rank(ranking),
    m_humanControlled(false)
{
}

void Team::setHasBall(bool hasBall)
{
    m_teamHasBall = hasBall;
}

void Team::goalScored(bool isNorthGoal)
{
    if ( (isNorthGoal && (m_direction == Team::SouthToNorth))
        || (!isNorthGoal && (m_direction == Team::NorthToSouth)) ) {
        m_goals++;
        m_scoredLastGoal = true;
    } else {
        m_scoredLastGoal = false;
    }
}
