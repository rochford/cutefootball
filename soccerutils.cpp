
#include <QtGui>
#include "soccerutils.h"
#include "compileTimeSettings.h"



void teamColorTransform(QPixmap &pixmap, QString pix, QRgb colorFrom, QRgb shirtColor, QRgb shortColor)
{
    QImage img(pix);
    QRect rect = img.rect();
    for (int w = 0; w < img.width(); w++) {
        for (int h = 0; h < img.height(); h++) {
            QRgb rgb = img.pixel(w, h);
            if (qRed(rgb) > 250 && qBlue(rgb) < 8 && qGreen(rgb) < 8)
                img.setPixel(QPoint(w,h), shirtColor);
            else if (qRed(rgb) < 8 && qBlue(rgb) < 8 && qGreen(rgb) > 250)
                img.setPixel(QPoint(w,h), shortColor);
        }
    }
    pixmap = QPixmap::fromImage(img);

    QBitmap bitmap = pixmap.createMaskFromColor(KCuteFootballMaskColor);
    pixmap.setMask(bitmap);
}



MWindow::Action calculateAction(QPointF source,
                                QPointF destination)

{
    QPointF diff(source - destination);

    const qreal dx = diff.x();
    const qreal dy = diff.y();

    if (dx > 0 && dy == 0)
        return MWindow::West;
    else if (dx <= 0 && dy == 0)
        return MWindow::East;
    else if (dx >= 0 && dy < 0)
        return MWindow::SouthWest;
    else if (dx > 0 && dy > 0)
        return MWindow::NorthWest;
    else if (dx < 0 && dy > 0)
        return MWindow::NorthEast;
    else if (dx < 0 && dy < 0)
        return MWindow::SouthEast;
    else if (dx == 0 && dy >= 0)
        return MWindow::North;
    else if (dx == 0 && dy <= 0)
        return MWindow::South;
}

int calculateTackleRotationFromLastAction(MWindow::Action lastAction)
{
    int action;
    switch(lastAction) {
    case MWindow::North:
        action = 0;
        break;
    case MWindow::NorthEast:
        action = 45;
        break;
    case MWindow::East:
        action = 90;
        break;
    case MWindow::SouthEast:
        action = 135;
        break;
    case MWindow::South:
        action = 180;
        break;
    case MWindow::SouthWest:
        action = 225;
        break;
    case MWindow::West:
        action = 270;
        break;
    case MWindow::NorthWest:
        action = 315;
        break;
    default:
        break;
    }
    return action;
}

