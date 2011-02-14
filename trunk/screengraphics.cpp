#include "screengraphics.h"

#include <QGraphicsSimpleTextItem>

#include "pitch.h"

const QFont KFont("Times", 6, QFont::Bold);

ScreenGraphics::ScreenGraphics(Pitch *p)
  :  m_pitch(p),
    m_scoreText(NULL)
{
    m_scoreText = m_pitch->m_scene->addSimpleText(QString("XXX"),KFont);
    m_scoreText->setVisible(false);
    m_scoreText->setZValue(Pitch::ZScoreText);

    updatePosition();
}

ScreenGraphics::~ScreenGraphics()
{
    delete m_scoreText;
}

void ScreenGraphics::updatePosition()
{
    m_scoreText->setPos(m_pitch->m_view->mapToScene(
            m_pitch->m_view->rect().topLeft()));
}

void ScreenGraphics::setText(QString s)
{
    m_scoreText->setVisible(true);
    m_scoreText->setText(s);
}

