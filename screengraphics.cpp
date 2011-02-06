#include "screengraphics.h"

#include <QGraphicsSimpleTextItem>
#include <QTimer>

#include "pitch.h"

const QFont KSmallFont("Times", 6, QFont::Bold);
const QFont KLargeFont("Times", 8, QFont::Bold);

ScreenGraphics::ScreenGraphics(Pitch *p)
    : QObject(p),
    m_pitch(p),
    m_scoreText(NULL),
    m_timer(NULL)
{
    m_scoreText = m_pitch->m_scene->addSimpleText(QString("XXX"),KSmallFont);
    m_scoreText->setVisible(false);
    m_scoreText->setZValue(Pitch::ZScoreText);

    m_timer = new QTimer(this);
    m_timer->setInterval(500);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(changeFont()));
    updatePosition();
}

ScreenGraphics::~ScreenGraphics()
{
    m_timer->stop();
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

void ScreenGraphics::changeFont()
{
    if ( m_scoreText->font() == KSmallFont )
        m_scoreText->setFont(KLargeFont);
    else
        m_scoreText->setFont(KSmallFont);
    m_pitch->m_scene->update();
}

