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
#include "soundeffects.h"
#include "pitch.h"
#include "ball.h"

SoundEffects::SoundEffects(QObject *parent) :
    QObject(parent)
{
    m_gameThemeTune = new QSound("theme.wav", this);
    m_ballKick = new QSound("kick.wav", this);
    m_whistle = new QSound("whistle.wav", this);
    m_finalwhistle = new QSound("finalwhistle.wav", this);
    m_crowd = new QSound("Crowd.wav", this);
    m_goal = new QSound("goalCelebration.wav", this);
    m_rebound = new QSound("rebound.wav", this);

//    m_gameThemeTune->setLoops(1000);
    m_crowd->setLoops(1000);
}

SoundEffects::~SoundEffects()
{
    delete m_gameThemeTune;
    delete m_ballKick;
    delete m_whistle;
    delete m_finalwhistle;
    delete m_crowd;
    delete m_goal;
    delete m_rebound;
}

void SoundEffects::soundEvent(QString fileName)
{
    if (m_soundEnabled)
        QSound::play(fileName);
}
void SoundEffects::startSound(SoundEffects::GameSound e)
{
    if (!m_soundEnabled)
        return;

    switch(e) {
    case GameThemeTune:
        m_gameThemeTune->play();
        break;
    case CrowdNoise:
        m_crowd->play();
        break;
    default:
        break;
    }
}

void SoundEffects::stopSound(SoundEffects::GameSound e)
{
    switch(e) {
    case GameThemeTune:
        m_gameThemeTune->stop();
        break;
    case CrowdNoise:
        m_crowd->stop();
        break;
    default:
        break;
    }
}

void SoundEffects::soundEvent(GameSound e)
{
    if (!m_soundEnabled)
        return;

    switch(e) {
    case GameThemeTune:
        m_gameThemeTune->play();
        break;
    case BallKick:
        m_ballKick->play();
        break;
    case CrowdNoise:
        m_crowd->play();
        break;
    case FinalWhistle:
        m_finalwhistle->play();
        break;
    case Whistle:
        m_whistle->play();
        break;
    case BallRebound:
        m_rebound->play();
        break;
    case Goal:
        m_goal->play();
        break;
    case Foul:
    default:
        break;
    }
}
