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
