#include "soundeffects.h"
#include "pitch.h"
#include "ball.h"

SoundEffects::SoundEffects(QObject *parent) :
    QObject(parent)
{
    m_ballKick = new QSound("kick.wav", this);
    m_whistle = new QSound("whistle.wav", this);
}

SoundEffects::~SoundEffects()
{
    delete m_ballKick;
    delete m_whistle;
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
    case BallKick:
        m_ballKick->play();
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
