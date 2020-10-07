#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

#include <QApplication>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QLocale>
#include <QScreen>
#include <QString>
#include <QTime>
#include <QWidget>

#include <strategr/geometry.h>

int QTimeToMinutes(const QTime &time);
QTime QTimeFromMinutes(int minutes);
QString QStringForMinutes(int minutes);
QString QStringForCalendarTime(time_t time);

/* Widgets */
template<class T>
T *findParentWidget(QWidget *childWidget) {
    auto *widget = childWidget->parentWidget();
    while (widget) {
        auto *searchedWideget = qobject_cast<T *>(widget);
        if (searchedWideget) {
            return searchedWideget;
        } else {
            widget = widget->parentWidget();
        }
    }

    return nullptr;
}

double devicePixelRatio();

/* Strings */
namespace StringUtils {
    QString toSentenceCase(QString string);
}

#endif// UTILS_H
