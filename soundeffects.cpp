#include "soundeffects.h"
#include "pitch.h"
#include "ball.h"

SoundEffects::SoundEffects(QObject *parent) :
    QObject(parent)
{
 //   m_ballKick = new QSound("kick.wav", this);
    m_whistle = new QSound("Whistle.wav", this);
    m_crowd = new QSound("Crowd.wav", this);
}

SoundEffects::~SoundEffects()
{
//    delete m_ballKick;
    delete m_whistle;
    delete m_crowd;
}

void SoundEffects::soundEvent(QString fileName)
{
    if (m_soundEnabled)
        QSound::play(fileName);
}

void SoundEffects::soundEvent(GameSound e)
{
    if (!m_soundEnabled)
        return;

    switch(e) {
//    case BallKick:
//        m_ballKick->play();
//        break;
    case CrowdNoise:
        m_crowd->play();
        break;
    case Whistle:
        m_whistle->play();
        break;
    case Goal:
    case Foul:
    default:
        break;
    }
}
