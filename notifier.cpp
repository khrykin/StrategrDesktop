#include "notifier.h"
#include <QTime>
#include <QDebug>
#include "utils.h"

Notifier::Notifier(Strategy *strategy, QObject *parent) : QObject(parent), strategy(strategy)
{

    contextMenu = new QMenu();
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(contextMenu);
    trayIcon->setIcon(QIcon(":/icons/icon.png"));
    trayIcon->show();

    timer = new QTimer(this);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Notifier::timerTick);
    timer->start(1000);
    timerTick();

}

Notifier::~Notifier()
{
    delete contextMenu;
}

void Notifier::timerTick()
{
    auto currentTime = QTime::currentTime().msecsSinceStartOfDay() / 1000;
    if (currentTime / 60 == currentMinute) {
//       return;
    }

    currentMinute = currentTime / 60;
    auto currentSlotIndex = findSlotIndexForTime(currentMinute);
    if (currentSlotIndex < 0) {
        return;
    }

    auto currentGroupIndex = strategy->groupIndexForSlotIndex(currentSlotIndex);
    if (!currentGroupIndex.has_value()) {
        return;
    }


    auto groups = strategy->group();
    auto currentGroup = groups[static_cast<unsigned int>(currentGroupIndex.value())];

    auto nextGroupIndex = static_cast<unsigned int>(currentGroupIndex.value()) + 1;

    if (nextGroupIndex >= groups.size()) {
        // TODO: Go to bed!
        return;
    }

    auto nextGroup = groups[nextGroupIndex];

    if (currentGroup.activity == nextGroup.activity || !nextGroup.activity.has_value()) {
        return;
    }

    auto nextGroupStartSlotIndex = strategy->startSlotIndexForGroupIndex(static_cast<int>(nextGroupIndex));
    auto nextGroupStartTime = strategy->startTimeForSlotIndex(nextGroupStartSlotIndex.value());

    if (nextGroup.activity != targetActivity) {
        getReadySent = false;
        startSent = false;
        targetActivity = nextGroup.activity;
    }

    qDebug() << "currentGroup" << QString::fromStdString(currentGroup.activity.value_or(Activity("None")).name);
    qDebug() << "nextGroup" << QString::fromStdString(nextGroup.activity.value_or(Activity("None")).name);

    auto countdown = nextGroupStartTime * 60 - currentTime;

    qDebug() << "countdown" << countdown;
    auto nextActivityName = QString::fromStdString(nextGroup.activity.value().name);
    if (countdown < 5 * 60 && !getReadySent) {
        if (QSystemTrayIcon::supportsMessages()) {
            qDebug() << "showMessage" <<"Coming up in 5 minutes";
            trayIcon->showMessage(titleForGroup(nextGroup),
                                  "Coming up in 5 minutes",
                                  QIcon(),
                                  10000);
        }

        getReadySent = true;
        return;
    }

    if (countdown < 10 && !startSent) {
        if (QSystemTrayIcon::supportsMessages()) {
            qDebug() << "showMessage" <<"Starts right now";
            trayIcon->showMessage(titleForGroup(nextGroup),
                                  "Starts right now",
                                  QIcon(),
                                  10000);
        }

        startSent = true;
        return;
    }


}

void Notifier::setStrategy(Strategy *value)
{
    strategy = value;
}

int Notifier::findSlotIndexForTime(int mins)
{
    auto startTimes = strategy->startTimes();
    for (unsigned int i = 0; i < strategy->numberOfSlots(); i++) {
        auto startTime = startTimes[i];
        auto endTime = startTime + strategy->slotDuration();

        if (mins >= startTime && mins < endTime) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

QString Notifier::titleForGroup(ActivityGroup &activityGroup)
{
    return QString::fromStdString(activityGroup.activity->name)
            + " ("
            + timeStringForMins(static_cast<int>(activityGroup.length) * strategy->slotDuration())
            + ")";
}
