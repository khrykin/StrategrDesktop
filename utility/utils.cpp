#include "utils.h"

#include <QTimer>
#include <QApplication>

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

QTime QTimeFromMinutes(int minutes) {
    return QTime(0, 0, 0).addSecs(minutes * 60);
}

QString QStringForMinutes(int minutes) {
    auto timeFormat = QLocale().timeFormat(QLocale::ShortFormat);
    return QTimeFromMinutes(minutes).toString(timeFormat);
}

int currentMinutes() {
    return currentSeconds() / 60;
}

int currentSeconds() {
    return QTime::currentTime().msecsSinceStartOfDay() / 1000;
}

double devicePixelRatio() {
    auto screenNumber = QApplication::desktop()->screenNumber();
    return QGuiApplication::screens()[screenNumber]->devicePixelRatio();
}

stg::Rect RectFromQRect(const QRect &rect) {
    return stg::Rect{rect.left(), rect.top(), rect.width(), rect.height()};
}

void dispatchToMainThread(const std::function<void()> &callback) {
    auto *timer = new QTimer();
    timer->moveToThread(QApplication::instance()->thread());
    timer->setSingleShot(true);

    QObject::connect(timer, &QTimer::timeout, [=]() {
        // main thread
        callback();
        timer->deleteLater();
    });

    QMetaObject::invokeMethod(timer,
                              "start",
                              Qt::QueuedConnection,
                              Q_ARG(int, 0));
}

QString StringUtils::toSentenceCase(QString string) {
    if (string.isEmpty()) {
        return string;
    }

    string[0] = string[0].toUpper();
    return string;
}
