#-------------------------------------------------
#
# Project created by QtCreator 2019-05-01T13:08:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Strategr
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        activitieslistitem.cpp \
        activitieslistwidget.cpp \
        activity.cpp \
        activityeditor.cpp \
        activitygroup.cpp \
        activitygroupwidget.cpp \
        currenttimemarker.cpp \
        filesystemiomanager.cpp \
        groupslist.cpp \
        jsonserializer.cpp \
        main.cpp \
        mainwindow.cpp \
        navbar.cpp \
        notifier.cpp \
        slotboard.cpp \
        slotruler.cpp \
        stacklayout.cpp \
        strategy.cpp \
        utils.cpp

HEADERS += \
        activitieslistitem.h \
        activitieslistwidget.h \
        activity.h \
        activityeditor.h \
        activitygroup.h \
        activitygroupwidget.h \
        currenttimemarker.h \
        filesystemiomanager.h \
        groupslist.h \
        jsonserializer.h \
        mainwindow.h \
        navbar.h \
        notifier.h \
        slotboard.h \
        slotruler.h \
        stacklayout.h \
        strategy.h \
        utils.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
QMAKE_EXTRA_TARGETS += before_build makefilehook

makefilehook.target = $(MAKEFILE)
makefilehook.depends = .beforebuild

PRE_TARGETDEPS += .beforebuild

before_build.target = .beforebuild
before_build.depends = FORCE
before_build.commands = chcp 1251
}

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14

RESOURCES += \
    icons.qrc
