#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QTime>
#include <QWidget>
#include <QLocale>

QString humanTimeForMinutes(int mins);

int minutesFromQTime(const QTime &time);

QTime qTimeFromMinutes(int minutes);

QString qStringForMinutes(int minutes);

int currentMinutes();

template<class T>
T *findParentWidget(QWidget *childWidget) {
    auto *widget = childWidget->parentWidget();
    while (widget) {
        auto *mainScene = qobject_cast<T *>(widget);
        if (mainScene) {
            return mainScene;
        } else {
            widget = widget->parentWidget();
        }
    }

    return nullptr;
}

#endif // UTILS_H
