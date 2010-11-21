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
    referee.cpp \
    goalkeeper.cpp \
    replay.cpp \
    screengraphics.cpp \
    game.cpp \
    soundeffects.cpp

HEADERS  += mainwindow.h \
    pitch.h \
    Player.h \
    ball.h \
    team.h \
    referee.h \
    goalkeeper.h \
    replay.h \
    screengraphics.h \
    game.h \
    compileTimeSettings.h \
    soundeffects.h

RESOURCES += \
    soccer.qrc
