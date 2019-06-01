#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QObject>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>

#include "strategy.h"

class Notifier : public QObject
{
    Q_OBJECT
public:
    explicit Notifier(Strategy *strategy, QObject *parent = nullptr);

    void setStrategy(Strategy *value);

signals:

public slots:

private:
    QTimer *timer;
    int nextTime;
    int currentMinute;
    std::optional<Activity> targetActivity;

    void timerTick();
    Strategy *strategy;

    bool getReadySent = false;
    bool startSent = false;

    int findSlotIndexForTime(int mins);

    QSystemTrayIcon *trayIcon;
    QMenu *contextMenu;

    QString titleForGroup(ActivityGroup &activityGroup);
};

#endif // NOTIFIER_H
