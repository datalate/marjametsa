#-------------------------------------------------
#
# Project created by QtCreator 2016-09-22T20:03:34
#
#-------------------------------------------------

QT      += core gui qml quick quickwidgets

greaterThan(QT_MAJOR_VERSION, 4):
QT += \
    widgets \
    network \
    xml

TARGET = syndikaatti
TEMPLATE = app

CONFIG += c++11
QMAKE_CXXFLAGS += -Wall -Wextra -Werror

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    levelparser.cpp \
    parkingdata.cpp \
    tampere.cpp \
    tile.cpp \
    parkinghall.cpp \
    entity.cpp \
    player.cpp \
    mission.cpp \
    missionparser.cpp \
    weapon.cpp \
    weaponparser.cpp \
    enemy.cpp \
    tilehighlight.cpp \
    character.cpp \
    resource.cpp \
    npc.cpp \
    shopping.cpp


HEADERS += \
    mainwindow.h \
    levelparser.h \
    parkingdata.h \
    tampere.h \
    tile.h \
    parkinghall.h \
    entity.h \
    player.h \
    mission.h \
    missionparser.h \
    weapon.h \
    weaponparser.h \
    enemy.h \
    tilehighlight.h \
    character.h \
    common.h \
    resource.h \
    npc.h \
    shopping.h

DISTFILES += \
    sidebar.qml

RESOURCES += \
    syndikaatti.qrc
