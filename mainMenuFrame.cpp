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

    // TODO disable these until ready
    ui->m_settingsBtn->setVisible(false);
    ui->m_settingsBtn->setEnabled(false);
    ui->m_playCupGameBtn->setVisible(false);
    ui->m_playCupGameBtn->setEnabled(false);

    animateButtons(size().width());

    connect(parent, SIGNAL(setFrame(MWindow::Frame)),
            this, SLOT(showFrame(MWindow::Frame)));

    connect(ui->actionGameSingle, SIGNAL(triggered()),
            parent, SLOT(showSingleGameTeamSelection()));
    connect(ui->actionWorld_Cup, SIGNAL(triggered()),
            parent, SLOT(showCupTeamSelection()));
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

void mainMenuFrame::resizeEvent(QResizeEvent *e)
{
    const int offset = ui->m_newGameBtn->size().width()/2;
    const int xPos = e->size().width()/2 - offset;
    ui->m_newGameBtn->setProperty("pos", QPoint(xPos,ui->m_newGameBtn->property("pos").toPoint().y()));
    if (ui->m_playCupGameBtn->isEnabled())
        ui->m_playCupGameBtn->setProperty("pos", QPoint(xPos,ui->m_playCupGameBtn->property("pos").toPoint().y()));
    if (ui->m_settingsBtn->isEnabled())
        ui->m_settingsBtn->setProperty("pos", QPoint(xPos,ui->m_settingsBtn->property("pos").toPoint().y()));
    ui->m_inputBtn->setProperty("pos", QPoint(xPos,ui->m_inputBtn->property("pos").toPoint().y()));
    ui->m_informationBtn->setProperty("pos", QPoint(xPos,ui->m_informationBtn->property("pos").toPoint().y()));
    ui->m_aboutBtn->setProperty("pos", QPoint(xPos,ui->m_aboutBtn->property("pos").toPoint().y()));
    ui->m_quitBtn->setProperty("pos", QPoint(xPos,ui->m_quitBtn->property("pos").toPoint().y()));

    animateButtons(e->size().width());
}

void mainMenuFrame::animateButtons(const int width)
{
    delete m_animationGrp;
    m_animationGrp = NULL;
    m_animationGrp = new QParallelAnimationGroup(this);
    const int offset = ui->m_newGameBtn->size().width()/2;
    const int xPoint = width/2 - offset;
    int yPoint(10);
    m_animationGrp->addAnimation(createAnimation(ui->m_newGameBtn, QPoint(xPoint,yPoint+=30)));
    if (ui->m_playCupGameBtn->isEnabled())
        m_animationGrp->addAnimation(createAnimation(ui->m_playCupGameBtn, QPoint(xPoint,yPoint+=30)));
    if (ui->m_settingsBtn->isEnabled())
        m_animationGrp->addAnimation(createAnimation(ui->m_settingsBtn, QPoint(xPoint,yPoint+=30)));
    m_animationGrp->addAnimation(createAnimation(ui->m_inputBtn, QPoint(xPoint,yPoint+=30)));
    m_animationGrp->addAnimation(createAnimation(ui->m_informationBtn, QPoint(xPoint,yPoint+=30)));
    m_animationGrp->addAnimation(createAnimation(ui->m_aboutBtn, QPoint(xPoint,yPoint+=30)));
    m_animationGrp->addAnimation(createAnimation(ui->m_quitBtn, QPoint(xPoint,yPoint+=30)));
}
