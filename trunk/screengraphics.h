#ifndef SCREENGRAPHICS_H
#define SCREENGRAPHICS_H

#include <QFrame>

#include "ui_screengraphicsframe.h"

namespace Ui {
    class ScreenGraphicsFrame;
}


class Team;
class Pitch;

class ScreenGraphics : public QFrame
{
public:
    ScreenGraphics(Pitch *p);
    virtual ~ScreenGraphics();
    void update(QString s);
    void setTeams(Team* home, Team* away);

private:
    Ui::ScreenGraphicsFrame *ui;
    Pitch *m_pitch;
};

#endif // SCREENGRAPHICS_H
