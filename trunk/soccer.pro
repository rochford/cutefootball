#-------------------------------------------------
#
# Project created by QtCreator 2010-09-04T15:53:22
#
#-------------------------------------------------

QT       += core gui

TARGET = soccer
TEMPLATE = app

TRANSLATIONS = \
    translations/soccer_en.ts \
    translations/soccer_fi.ts \
    translations/soccer_de.ts \
    translations/soccer_es.ts \
    translations/soccer_pt.ts \
    translations/soccer_se.ts \
    translations/soccer_fr.ts

symbian {
    symbian:TARGET.UID3 = 0xEED6D15E

    # Add files and directories to ship with the application
    # by adapting the examples below.
    soundFiles.sources = audio\\*.wav
    # file1.source = myfile
    # dir1.source = mydir
    DEPLOYMENT += soundFiles # file1 dir1

    vendorinfo = "%{\"Tim Rochford-EN\"}" ":\"Tim Rochford\""
    my_deployment.pkg_prerules = vendorinfo
    DEPLOYMENT += my_deployment
}

SOURCES += \
    main.cpp\
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
    inputsettingsframe.cpp \
    halfstatisticsframe.cpp

HEADERS  += \
    mainwindow.h \
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
    inputsettingsframe.h \
    halfstatisticsframe.h \
    version.h

RESOURCES += \
    soccer.qrc \
    teams.qrc \
    languages.qrc \
    menugraphics.qrc \
    stylesheets.qrc

FORMS += \
    mainwindow.ui \
    settingsFrame.ui \
    aboutFrame.ui \
    mainMenuFrame.ui \
    teamSelectionFrame.ui \
    helpFrame.ui \
    inputSettingsFrame.ui \
    halfstatisticsframe.ui \
    exitconfirmationdialog.ui

OTHER_FILES += \
    teams/teams.txt \
    teams/England.txt \
    teams/Spain.txt \
    teams/Italy.txt \
    teams/Finland.txt \
    teams/Switzerland.txt \
    teams/Germany.txt \
    teams/France.txt \
    teams/Portugal.txt \
    teams/Sweden.txt \
    teams/SanMarino.txt \
    teams/Turkey.txt \
    teams/Russia.txt \
    teams/Netherlands.txt \
    teams/Greece.txt \
    teams/Estonia.txt \
    teams/Andorra.txt \
    teams/Wales.txt \
    teams/Ukraine.txt \
    teams/Slovakia.txt \
    teams/Scotland.txt \
    teams/Romania.txt \
    teams/Poland.txt \
    teams/Norway.txt \
    teams/NorthernIreland.txt \
    teams/Iceland.txt \
    teams/Hungary.txt \
    teams/Denmark.txt \
    teams/Czech.txt \
    teams/Croatia.txt \
    teams/Austria.txt \
    teams/Bulgaria.txt \
    desktop.qss \
    mobile.qss \
    teams/Ireland.txt \
    teams/Belgium.txt

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()
