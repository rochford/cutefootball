#-------------------------------------------------
#
# Project created by QtCreator 2010-09-04T15:53:22
#
#-------------------------------------------------

QT       += core gui

TARGET = soccer
TEMPLATE = app

TRANSLATIONS = translations/soccer_fi.ts \
    translations/soccer_en.ts

symbian:TARGET.UID3 = 0xEED6D15E

# Add files and directories to ship with the application
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
DEPLOYMENTFOLDERS = # file1 dir1

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
    teamSelectionFrame.cpp \
    helpFrame.cpp \
    inputsettingsframe.cpp

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
    teamSelectionFrame.h \
    helpFrame.h \
    inputsettingsframe.h

RESOURCES += \
    soccer.qrc

FORMS += \
    mainwindow.ui \
    settingsFrame.ui \
    aboutFrame.ui \
    mainMenuFrame.ui \
    teamSelectionFrame.ui \
    helpFrame.ui \
    inputSettingsFrame.ui

OTHER_FILES += \
    stylesheet.qss \
    teams/teams.txt \
    teams/England.txt \
    teams/Spain.txt

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()
