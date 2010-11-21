#ifndef SOUNDEFFECTS_H
#define SOUNDEFFECTS_H

#include <QObject>
#include <QSound>

class SoundEffects : public QObject
{
    Q_OBJECT
public:
    enum GameSound {
        BallKick,
        Whistle,
        Goal,
        Foul
    };

    explicit SoundEffects(QObject *parent = 0);
    ~SoundEffects();
    static void soundEvent(QString fileName);

public slots:
    void soundEvent(SoundEffects::GameSound e);

private:
    QSound* m_ballKick;
    QSound* m_whistle;
};

#endif // SOUNDEFFECTS_H
