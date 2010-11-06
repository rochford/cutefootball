#include "screengraphics.h"

#include <QGraphicsSimpleTextItem>
#include <QTimer>

#include "pitch.h"

#ifdef Q_OS_SYMBIAN
const QFont KSmallFont("Times", 9, QFont::Bold);
const QFont KLargeFont("Times", 17, QFont::Bold);
#else
const QFont KSmallFont("Times", 12, QFont::Bold);
const QFont KLargeFont("Times", 20, QFont::Bold);
#endif // Q_OS_SYMBIAN

const int KReallyHighZValue = 10;

ScreenGraphics::ScreenGraphics(Pitch *p)
    : QObject(p),
    m_pitch(p),
    m_scoreText(NULL),
    m_mode(NormalMode),
    m_timer(NULL)
{
    m_scoreText = m_pitch->scene->addSimpleText(QString("XXX"),KSmallFont);
    m_scoreText->setVisible(false);
    m_scoreText->setZValue(KReallyHighZValue);

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
    m_scoreText->setPos(m_pitch->view->mapToScene(
            m_pitch->view->rect().topLeft()));
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
    m_pitch->scene->update();
}

void ScreenGraphics::setMode(ScreenGraphicsMode mode)
{
    if (mode == ScreenGraphics::ReplayMode )
        m_timer->start();
    else {
        m_timer->stop();
        m_scoreText->setFont(KSmallFont);
    }
    m_mode = mode;
}
