#include <algorithm>

#include <QTimer>
#include <QApplication>

#include "utils.h"

int QTimeToMinutes(const QTime &time) {
    return time.msecsSinceStartOfDay() / 60 / 1000;
}

QTime QTimeFromMinutes(int minutes) {
    return QTime(0, 0, 0).addSecs(minutes * 60);
}

QString QStringForMinutes(int minutes) {
    auto timeFormat = QLocale().timeFormat(QLocale::ShortFormat);
    return QTimeFromMinutes(minutes).toString(timeFormat);
}

QString QStringForCalendarTime(time_t time) {
    auto timeFormat = QLocale().timeFormat(QLocale::ShortFormat);
    return QDateTime::fromSecsSinceEpoch(time).toString(timeFormat);
}

double devicePixelRatio() {
    auto screenNumber = QApplication::desktop()->screenNumber();
    return QGuiApplication::screens()[screenNumber]->devicePixelRatio();
}

QString StringUtils::toSentenceCase(QString string) {
    if (string.isEmpty()) {
        return string;
    }

    string[0] = string[0].toUpper();
    return string;
}
