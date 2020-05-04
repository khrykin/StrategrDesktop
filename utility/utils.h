#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

#include <QString>
#include <QTime>
#include <QWidget>
#include <QLocale>
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

#include "geometry.h"

/* time_utils */

QString humanTimeForMinutes(int mins);

int QTimeToMinutes(const QTime &time);

QTime QTimeFromMinutes(int minutes);
QString QStringForMinutes(int minutes);

int currentSeconds();

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

std::vector<std::string> QStringListToStdVector(const QStringList &list);
QStringList QStringListFromStdVector(const std::vector<std::string> &vector);

/* Multithreading */

void dispatchToMainThread(const std::function<void()> &callback);

#endif // UTILS_H
