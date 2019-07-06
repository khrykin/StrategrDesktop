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
    filesystemiomanager.cpp \
    jsonserializer.cpp \
    main.cpp \
    models/activity.cpp \
    models/activitygroup.cpp \
    models/strategy.cpp \
    notifier.cpp \
    third-party/slidingstackedwidget.cpp \
    third-party/stacklayout.cpp \
    ui/abstractspinboxdecorator.cpp \
    ui/activitieslistitem.cpp \
    ui/activitieslistwidget.cpp \
    ui/activitygroupwidget.cpp \
    ui/colorpicker.cpp \
    ui/colorpickeritem.cpp \
    ui/currentactivitywidget.cpp \
    ui/currenttimemarker.cpp \
    ui/enterpresseater.cpp \
    ui/groupslist.cpp \
    ui/mainwindow.cpp \
    ui/navbar.cpp \
    ui/newactivitymenu.cpp \
    ui/slidinganimator.cpp \
    ui/slotboard.cpp \
    ui/slotruler.cpp \
    ui/steppedtimeedit.cpp \
    ui/strategysettings.cpp \
    utils.cpp

HEADERS += \
    colorutils.h \
    filesystemiomanager.h \
    jsonserializer.h \
    models/activity.h \
    models/activitygroup.h \
    models/strategy.h \
    notifier.h \
    notifierbackend.h \
    third-party/slidingstackedwidget.h \
    third-party/stacklayout.h \
    ui/abstractspinboxdecorator.h \
    ui/activitieslistitem.h \
    ui/activitieslistwidget.h \
    ui/activitygroupwidget.h \
    ui/colorpicker.h \
    ui/colorpickeritem.h \
    ui/currentactivitywidget.h \
    ui/currenttimemarker.h \
    ui/enterpresseater.h \
    ui/groupslist.h \
    ui/mainwindow.h \
    ui/navbar.h \
    ui/newactivitymenu.h \
    ui/slidinganimator.h \
    ui/slotboard.h \
    ui/slotruler.h \
    ui/steppedtimeedit.h \
    ui/strategysettings.h \
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
    fonts/fonts.qrc
