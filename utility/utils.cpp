#include "utils.h"

QString humanTimeForMinutes(int mins) {
    if (mins < 1) {
        return "Less than 1 min";
    }

    int hours = mins / 60;
    int minsRemainder = mins - hours * 60;

    QString result;

    if (hours > 0) {
        result += QString::number(hours);
    }

    if (hours > 0 && minsRemainder == 30) {
        result += ".5 h";
    } else if (hours > 0) {
        result += " h";
    }

    if (minsRemainder != 0) {
        if (hours > 0) {
            if (minsRemainder == 30) {
                return result;
            }

            result += " ";
        }

        result += QString::number(minsRemainder) + " min";
    }

    return result;
}

int minutesFromQTime(const QTime &time) {
    return time.msecsSinceStartOfDay() / 60 / 1000;
}

QTime qTimeFromMinutes(int minutes) {
    return QTime(0, 0, 0).addSecs(minutes * 60);
}

QString qStringForMinutes(int minutes) {
    auto timeFormat = QLocale().timeFormat(QLocale::ShortFormat);
    return qTimeFromMinutes(minutes).toString(timeFormat);
}

int currentMinutes() {
    return currentSeconds() / 60;
}

int currentSeconds() {
    return QTime::currentTime().msecsSinceStartOfDay() / 1000;
}

int devicePixelRatio() {
    auto screenNumber = QApplication::desktop()->screenNumber();
    return QGuiApplication::screens()[screenNumber]->devicePixelRatio();
}
