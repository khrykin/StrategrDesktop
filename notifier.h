#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QWidget>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>

#include "strategy.h"

class Notifier : public QWidget
{
    Q_OBJECT
public:
    explicit Notifier(Strategy *strategy, QWidget *parent = nullptr);

signals:

public slots:

private:
    QTimer *timer;
    int nextTime;
    int currentMinute;
    std::optional<Activity> targetActivity;

    void timertTick();
    Strategy *strategy;

    bool getReadySent = false;
    bool startSent = false;

    int findSlotIndexForTime(int mins);

    QSystemTrayIcon *trayIcon;
    QMenu *contextMenu;

    QString titleForGroup(ActivityGroup &activityGroup);
};

#endif // NOTIFIER_H
