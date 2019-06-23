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

  std::optional<ActivityGroup> currentGroup() const;
  std::optional<int> currentGroupIndex() const;
  void timerTick();

signals:

public slots:

private:
  QTimer *timer;
  int nextTime;
  int currentMinute;
  std::optional<Activity> targetActivity;

  Strategy *strategy;

  bool getReadySent = false;
  bool startSent = false;

  QSystemTrayIcon *trayIcon;
  QMenu *contextMenu;
  NotifierBackend *backend;
  std::optional<ActivityGroup> _currentGroup;
  std::optional<int> _currentGroupIndex;

  QString titleForGroup(ActivityGroup &activityGroup);
};

#endif // NOTIFIER_H
