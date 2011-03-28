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
#ifndef SOUNDEFFECTS_H
#define SOUNDEFFECTS_H

#include <QObject>
#include <QSound>

class SoundEffects : public QObject
{
    Q_OBJECT
public:
    enum GameSound {
        GameThemeTune,
        CrowdNoise,
        BallKick,
        Whistle,
        FinalWhistle,
        Goal,
        Foul,
        BallRebound
    };

    explicit SoundEffects(QObject *parent = 0);
    ~SoundEffects();
    void soundEvent(QString fileName);

public slots:
    inline void soundEnabled(bool enabled) { m_soundEnabled = enabled; }
    void soundEvent(SoundEffects::GameSound e);
    void startSound(SoundEffects::GameSound e);
    void stopSound(SoundEffects::GameSound e);

private:
    bool m_soundEnabled;

    QSound* m_gameThemeTune;
    QSound* m_ballKick;
    QSound* m_whistle;
    QSound* m_finalwhistle;
    QSound* m_crowd;
    QSound* m_goal;
    QSound* m_rebound;
};

#endif // SOUNDEFFECTS_H
