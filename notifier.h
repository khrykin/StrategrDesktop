#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

#include "notifierbackend.h"
#include "strategy.h"

class Notifier : public QObject {
  Q_OBJECT
public:
  explicit Notifier(Strategy *strategy, QObject *parent = nullptr);
  ~Notifier();
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
  NotifierBackend *backend;

  QString titleForGroup(ActivityGroup &activityGroup);
};

#endif // NOTIFIER_H
