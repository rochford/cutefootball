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
