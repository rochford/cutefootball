#
# Copyright 2010,2011 Timothy Rochford
#
#    CuteFootball is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    CuteFootball is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with CuteFootball.  If not, see <http://www.gnu.org/licenses/>.
#

QT       += core gui

TARGET = "5soccer"
TEMPLATE = app

TRANSLATIONS = \
    translations/soccer_fi.ts

# Avoid auto screen rotation
DEFINES += ORIENTATIONLOCK

# Symbian^3, touch ui and VGA resolution support
# DEFINES += SOCCER_VGA

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

     vendorinfo = \
         "%{\"Timothy Rochford\"}" \
         ":\"Timothy Rochford\""

    supported_platforms = \
        ";Supports S60 v3.1" \
        "[0x102032BE], 0, 0, 0, {\"SymbianProductID\"}" \
        ";Supports S60 v3.2" \
        "[0x102752AE], 0, 0, 0, {\"SymbianProductID\"}"
#
#       qt ovi products
#       S60 3rd Edition: E72, E71, E66, E63, and E52
#        "; This application only supports certain phones" \
#        "[0x20024100], 0, 0, 0, {\"Nokia E5-00\"}" \ #E5
#        "[0x20014DD0], 0, 0, 0, {\"Nokia E72\"}" \ #E72
#        "[0x2000249B], 0, 0, 0, {\"Nokia E71\"}" \ #E71
#        "[0x2000249C], 0, 0, 0, {\"Nokia E66\"}" \ #E66
#        "[0x200025C3], 0, 0, 0, {\"Nokia E63\"}" \ #E63
#        "[0x20014DCC], 0, 0, 0, {\"Nokia E52\"}"  #E52
#
    license_file = \
        "IF  (LANGUAGE=1) ; If the language is English" \
        "   \"lgpl.txt\"-\"\", FILETEXT, TEXTABORT" \
        "ELSE" \
        "   \"lgpl.txt\"-\"\", FILETEXT, TEXTABORT" \
        "ENDIF"

     embedded_deployments = \
         "; Embed Qt dependencies" \
         "IF NOT(0x2001E62D)" \
         "@\"c:/QtSDK/Symbian/sis/Symbian^1/Qt/4.7.1/qt_installer.sis\",(0x2001E62D)" \
         "ENDIF"

    default_deployment.pkg_prerules -= pkg_platform_dependencies
    my_deployment.pkg_prerules += vendorinfo
    my_deployment.pkg_prerules += supported_platforms
#    my_deployment.pkg_prerules += license_file
# not needed
#    my_deployment.pkg_prerules += embedded_deployments
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
    aboutFrame.cpp \
    ingamemenuframe.cpp \
    pitchscene.cpp \
    kickoffstate.cpp \
    onscreenbuttonsframe.cpp

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
    aboutFrame.h \
    ingamemenuframe.h \
    pitchscene.h \
    kickoffstate.h \
    onscreenbuttonsframe.h
#    teamNameTranslations.h

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
    aboutFrame.ui \
    ingamemenuframe.ui \
    onscreenbuttonsframe.ui

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
    teams/Ireland.txt \
    teams/Belgium.txt \
    teams/Serbia.txt \
    teams/Malta.txt \
    teams/Vietnam.txt \
    teams/Uruguay.txt \
    teams/UnitedStates.txt \
    teams/SouthAfrica.txt \
    teams/Philippines.txt \
    teams/Peru.txt \
    teams/Mexico.txt \
    teams/Malaysia.txt \
    teams/Japan.txt \
    teams/India.txt \
    teams/Egypt.txt \
    teams/China.txt \
    teams/Brazil.txt \
    teams/Australia.txt \
    teams/Argentina.txt \
    teams/Algeria.txt \
    teams/Cameroon.txt \
    teams/Canada.txt \
    teams/Colombia.txt \
    teams/Ghana.txt \
    teams/Israel.txt \
    teams/Jamaica.txt \
    teams/Jordan.txt \
    teams/KoreaRepublic.txt \
    teams/Morocco.txt \
    teams/NewZealand.txt \
    teams/Niger.txt \
    teams/Nigeria.txt \
    teams/Pakistan.txt \
    teams/Thailand.txt \
    teams/Tunisia.txt \
    mobileQVGA.qss \
    mobileVGA.qss \
    info.txt \
    5soccer_package.pkg

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()
