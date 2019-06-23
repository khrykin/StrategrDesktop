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
        activitygroup.cpp \
        activitygroupwidget.cpp \
        animatablelayout.cpp \
        colorpicker.cpp \
        colorpickeritem.cpp \
        currentactivitywidget.cpp \
        currenttimemarker.cpp \
        enterpresseater.cpp \
        filesystemiomanager.cpp \
        groupslist.cpp \
        jsonserializer.cpp \
        main.cpp \
        mainwindow.cpp \
        navbar.cpp \
        newactivitymenu.cpp \
        notifier.cpp \
        slidingstackedwidget.cpp \
        slotboard.cpp \
        slotruler.cpp \
        stacklayout.cpp \
        steppedtimeedit.cpp \
        strategy.cpp \
        strategysettings.cpp \
        utils.cpp

HEADERS += \
        activitieslistitem.h \
        activitieslistwidget.h \
        activity.h \
        activitygroup.h \
        activitygroupwidget.h \
        animatablelayout.h \
        colorpicker.h \
        colorpickeritem.h \
        colorutils.h \
        currentactivitywidget.h \
        currenttimemarker.h \
        enterpresseater.h \
        filesystemiomanager.h \
        groupslist.h \
        jsonserializer.h \
        mainwindow.h \
        navbar.h \
        newactivitymenu.h \
        notifier.h \
        notifierbackend.h \
        slidingstackedwidget.h \
        slotboard.h \
        slotruler.h \
        stacklayout.h \
        steppedtimeedit.h \
        strategy.h \
        strategysettings.h \
        utils.h

FORMS +=
macx {
  LIBS += -framework Foundation
  OBJECTIVE_SOURCES += notifierbackend.mm
} else {
  SOURCES += notifierbackend.mm
}

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
