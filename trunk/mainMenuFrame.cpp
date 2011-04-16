/*
 * Copyright 2010,2011 Timothy Rochford
 *
 *    This file is part of CuteFootball.
 *
 *    CuteFootball is free software: you can redistribute it and/or modify
 *    it under the terms of the Lesser GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    CuteFootball is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Lesser GNU General Public License for more details.
 *
 *    You should have received a copy of the Lesser GNU General Public License
 *    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "mainMenuFrame.h"
#include "ui_mainMenuFrame.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

mainMenuFrame::mainMenuFrame(MWindow *parent) :
    QFrame(parent),
    ui(new Ui::mainMenuFrame)
{
    ui->setupUi(this);
    ui->m_settingsBtn->setVisible(false);

    m_animationGrp = new QParallelAnimationGroup(this);
    const int xPoint(120);
    int yPoint(10);
    m_animationGrp->addAnimation(createAnimation(ui->m_newGameBtn, QPoint(xPoint,yPoint+=30)));
    if (ui->m_settingsBtn->isVisible())
        m_animationGrp->addAnimation(createAnimation(ui->m_settingsBtn, QPoint(xPoint,yPoint+=30)));
    m_animationGrp->addAnimation(createAnimation(ui->m_inputBtn, QPoint(xPoint,yPoint+=30)));
    m_animationGrp->addAnimation(createAnimation(ui->m_informationBtn, QPoint(xPoint,yPoint+=30)));
    m_animationGrp->addAnimation(createAnimation(ui->m_aboutBtn, QPoint(xPoint,yPoint+=30)));
    m_animationGrp->addAnimation(createAnimation(ui->m_quitBtn, QPoint(xPoint,yPoint+=30)));

    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(ui->actionNewGame, SIGNAL(triggered()),
            parent, SLOT(showTeamSelectionFrame()));
    connect(ui->actionSettings, SIGNAL(triggered()),
            parent, SLOT(showSettingsFrame()));
    connect(ui->actionInputSettings, SIGNAL(triggered()),
            parent, SLOT(showInputSettingsFrame()));
    connect(ui->actionHelp, SIGNAL(triggered()),
            parent,SLOT(showHelpFrame()));
    connect(ui->actionQuit, SIGNAL(triggered()),
            parent, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()),
            parent, SLOT(showAboutFrame()));
}

QPropertyAnimation* mainMenuFrame::createAnimation(QWidget* widget, QPoint finalPosition)
{
    QPropertyAnimation* anim = new QPropertyAnimation(widget, "pos");
    anim->setDuration(1200);
    anim->setEasingCurve(QEasingCurve::OutElastic);
    anim->setStartValue(QPoint(0,finalPosition.y()));
    anim->setEndValue(finalPosition);
    return anim;

}

mainMenuFrame::~mainMenuFrame()
{
    delete ui;
    delete m_animationGrp;
}

void mainMenuFrame::showFrame(MWindow::Frame f)
{
    qDebug() << "mainMenuFrame::showFrame" << f;
    if ( f == MWindow::MainMenu ) {
        ui->m_newGameBtn->setFocus();
        showMaximized();
        m_animationGrp->start();
    } else {
        setVisible(false);
        m_animationGrp->stop();
    }
}

