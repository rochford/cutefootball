#-------------------------------------------------
#
# Project created by QtCreator 2010-09-04T15:53:22
#
#-------------------------------------------------

QT       += core gui

TARGET = soccer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pitch.cpp \
    player.cpp \
    ball.cpp \
    team.cpp \
    goalkeeper.cpp \
    screengraphics.cpp \
    game.cpp \
    soundeffects.cpp \
    settingsFrame.cpp \
    aboutFrame.cpp \
    mainMenuFrame.cpp \
    goalscoredstate.cpp \
    foulstate.cpp \
    teamSelectionFrame.cpp

HEADERS  += mainwindow.h \
    pitch.h \
    Player.h \
    ball.h \
    team.h \
    goalkeeper.h \
    screengraphics.h \
    game.h \
    compileTimeSettings.h \
    soundeffects.h \
    settingsFrame.h \
    aboutFrame.h \
    mainMenuFrame.h \
    goalscoredstate.h \
    foulstate.h \
    teamSelectionFrame.h

RESOURCES += \
    soccer.qrc

FORMS += \
    mainwindow.ui \
    settingsFrame.ui \
    aboutFrame.ui \
    mainMenuFrame.ui \
    teamSelectionFrame.ui

OTHER_FILES += \
    stylesheet.qss
