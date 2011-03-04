#ifndef SOCCERUTILS_H
#define SOCCERUTILS_H

#include "mainwindow.h"

int calculateTackleRotationFromLastAction(MWindow::Action lastAction);
void teamColorTransform(QPixmap &pixmap, QString pix, QRgb colorFrom, QRgb shirtColor, QRgb shortColor);
MWindow::Action calculateAction(QPointF source,
                                QPointF destination);
#endif // SOCCERUTILS_H
