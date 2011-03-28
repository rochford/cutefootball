#
# Copyright 2010,2011 Timothy Rochford
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

QT       += core gui

TARGET = soccer
TEMPLATE = app

TRANSLATIONS = \
    translations/soccer_fi.ts

symbian {
    symbian:TARGET.UID3 = 0xEED6D15E

    # Add files and directories to ship with the application
    # by adapting the examples below.
#    soundFiles.sources = audio\\*.wav
    #file1.source = audio\\Crowd.wav
    #file2.source = audio\\whistle.wav
    #file3.source = audio\\theme.wav
    #file4.source = audio\\finalwhistle.wav
    # dir1.source = mydir
#    DEPLOYMENT += soundFiles # file1 dir1
#    DEPLOYMENT += file1 file2 file3 file4

    # qt ovi products
    # S60 3rd Edition: E72, E71, E66, E63, and E52
    supported_platforms = \
        "; This application only supports certain phones" \
        "[0x20024100], 0, 0, 0, {\"Nokia E5-00\"}" \ #E5
        "[0x20014DD0], 0, 0, 0, {\"Nokia E72\"}" \ #E72
        "[0x2000249B], 0, 0, 0, {\"Nokia E71\"}" \ #E71
        "[0x2000249C], 0, 0, 0, {\"Nokia E66\"}" \ #E66
        "[0x200025C3], 0, 0, 0, {\"Nokia E63\"}" \ #E63
        "[0x20014DCC], 0, 0, 0, {\"Nokia E52\"}"  #E52

    default_deployment.pkg_prerules -= pkg_platform_dependencies
    my_deployment.pkg_prerules += supported_platforms
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
    mainMenuFrame.cpp \
    goalscoredstate.cpp \
    foulstate.cpp \
    teamSelectionFrame.cpp \
    helpFrame.cpp \
    inputsettingsframe.cpp \
    halfstatisticsframe.cpp \
    soccerutils.cpp \
    cameraview.cpp \
    ingamemenuframe.cpp
#    aboutFrame.cpp \

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
    mainMenuFrame.h \
    goalscoredstate.h \
    foulstate.h \
    teamSelectionFrame.h \
    helpFrame.h \
    inputsettingsframe.h \
    halfstatisticsframe.h \
    version.h \
    soccerutils.h \
    cameraview.h \
    ingamemenuframe.h
#    aboutFrame.h \

RESOURCES += \
    soccer.qrc \
    teams.qrc \
    languages.qrc \
    menugraphics.qrc \
    stylesheets.qrc

FORMS += \
    mainwindow.ui \
    settingsFrame.ui \
    mainMenuFrame.ui \
    teamSelectionFrame.ui \
    helpFrame.ui \
    inputSettingsFrame.ui \
    halfstatisticsframe.ui \
    screengraphicsframe.ui \
    ingamemenuframe.ui
#    aboutFrame.ui \

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
    teams/Belgium.txt \
    teams/Serbia.txt \
    teams/Malta.txt \
    soccer_package.pkg

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()
