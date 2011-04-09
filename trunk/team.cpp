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
    m_teamNameTranslations.insert("Andorra", KAndorra);
    m_teamNameTranslations.insert("Argentina", KArgentina);
    m_teamNameTranslations.insert("Australia", KAustralia);
    m_teamNameTranslations.insert("Austria", KAustria);
    m_teamNameTranslations.insert("Belgium", KBelgium);
    m_teamNameTranslations.insert("Brazil", KBrazil);
    m_teamNameTranslations.insert("Bulgaria", KBulgaria);
    m_teamNameTranslations.insert("China", KChina);
    m_teamNameTranslations.insert("Croatia", KCroatia);
    m_teamNameTranslations.insert("Czech", KCzech);
    m_teamNameTranslations.insert("Denmark", KDenmark);
    m_teamNameTranslations.insert("Egypt", KEgypt);
    m_teamNameTranslations.insert("England", KEngland);
    m_teamNameTranslations.insert("Estonia", KEstonia);
    m_teamNameTranslations.insert("Finland", KFinland);
    m_teamNameTranslations.insert("France", KFrance);
    m_teamNameTranslations.insert("Germany", KGermany);
    m_teamNameTranslations.insert("Greece", KGreece);
    m_teamNameTranslations.insert("Hungary", KHungary);
    m_teamNameTranslations.insert("Iceland", KIceland);
    m_teamNameTranslations.insert("India", KIndia);
    m_teamNameTranslations.insert("Ireland", KIreland);
    m_teamNameTranslations.insert("Italy", KItaly);
    m_teamNameTranslations.insert("Japan", KJapan);
    m_teamNameTranslations.insert("Malaysia", KMalaysia);
    m_teamNameTranslations.insert("Malta", KMalta);
    m_teamNameTranslations.insert("Mexico", KMexico);
    m_teamNameTranslations.insert("Netherlands", KNetherlands);
    m_teamNameTranslations.insert("NorthernIreland", KNorthernIreland);
    m_teamNameTranslations.insert("Norway", KNorway);
    m_teamNameTranslations.insert("Peru", KPeru);
    m_teamNameTranslations.insert("Philippines", KPhilippines);
    m_teamNameTranslations.insert("Poland", KPoland);
    m_teamNameTranslations.insert("Portugal", KPortugal);
    m_teamNameTranslations.insert("Romania", KRomania);
    m_teamNameTranslations.insert("Russia", KRussia);
    m_teamNameTranslations.insert("SanMarino", KSanMarino);
    m_teamNameTranslations.insert("Scotland", KScotland);
    m_teamNameTranslations.insert("Serbia", KSerbia);
    m_teamNameTranslations.insert("Slovakia", KSlovakia);
    m_teamNameTranslations.insert("SouthAfrica", KSouthAfrica);
    m_teamNameTranslations.insert("Spain", KSpain);
    m_teamNameTranslations.insert("Sweden", KSweden);
    m_teamNameTranslations.insert("Switzerland", KSwitzerland);
    m_teamNameTranslations.insert("Turkey", KTurkey);
    m_teamNameTranslations.insert("Ukraine", KUkraine);
    m_teamNameTranslations.insert("UnitedStates", KUnitedStates);
    m_teamNameTranslations.insert("Uruguay", KUruguay);
    m_teamNameTranslations.insert("Vietnam", KVietnam);
    m_teamNameTranslations.insert("Wales", KWales);
}

TeamManager::~TeamManager()
{
    m_teamNameTranslations.clear();
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
        QString name = translateTeamName(values.at(FullName).simplified());

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
    return m_teamNameTranslations[untranslated];
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
