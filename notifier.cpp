#include "notifier.h"
#include "utils.h"
#include <QDebug>
#include <QTime>

Notifier::Notifier(Strategy *strategy, QObject *parent)
    : QObject(parent), strategy(strategy) {

  contextMenu = new QMenu();
  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(contextMenu);
  trayIcon->show();

  backend = new NotifierBackend(trayIcon, this);

  timer = new QTimer(this);
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &Notifier::timerTick);
  timer->start(1000);
  timerTick();
}

Notifier::~Notifier() {
  delete contextMenu;
  delete trayIcon;
}

void Notifier::timerTick() {
  auto currentTime = QTime::currentTime().msecsSinceStartOfDay() / 1000;
  if (currentTime / 60 == currentMinute) {
    //       return;
  }

  currentMinute = currentTime / 60;
  auto currentSlotIndex = strategy->findSlotIndexForTime(currentMinute);
  if (!currentSlotIndex) {
    return;
  }

  _currentGroupIndex =
      strategy->groupIndexForSlotIndex(currentSlotIndex.value());
  if (!_currentGroupIndex) {
    return;
  }

  auto groups = strategy->calculateGroups();
  _currentGroup = groups[static_cast<unsigned int>(_currentGroupIndex.value())];

  auto nextGroupIndex =
      static_cast<unsigned int>(_currentGroupIndex.value()) + 1;

  std::optional<ActivityGroup> nextGroup;
  int nextGroupStartTime;
  QString messageTitle;
  const char *prepareText;
  const char *startsText;

  if (nextGroupIndex < groups.size()) {

    nextGroup = groups[nextGroupIndex];

    if (!nextGroup->activity ||
        _currentGroup->activity->name == nextGroup->activity->name) {
      return;
    }

    auto nextGroupStartSlotIndex =
        strategy->startSlotIndexForGroupIndex(static_cast<int>(nextGroupIndex));
    nextGroupStartTime =
        strategy->startTimeForSlotIndex(nextGroupStartSlotIndex.value());

    if (nextGroup->activity->name != targetActivity->name) {
      getReadySent = false;
      startSent = false;
      targetActivity = nextGroup->activity;
    }

    auto nextActivityName = QString::fromStdString(nextGroup->activity->name);
    messageTitle = titleForGroup(nextGroup.value());
    prepareText = "Coming up in 5 minutes";
    startsText = "Starts right now";
  } else {
    nextGroupStartTime = strategy->endTime();
    messageTitle = "End Of Strategy";
    prepareText = "Coming up in 5 minutes";
    startsText = "Strategy ends right now";
  }

  auto countdown = nextGroupStartTime * 60 - currentTime;
  if (countdown < 5 * 60 && !getReadySent) {
    backend->sendMessage(messageTitle, tr(prepareText));
    getReadySent = true;
    return;
  }

  if (countdown < 10 && !startSent) {
    backend->sendMessage(messageTitle, tr(startsText));
    startSent = true;
    return;
  }
}

std::optional<int> Notifier::currentGroupIndex() const {
  return _currentGroupIndex;
}

std::optional<ActivityGroup> Notifier::currentGroup() const {
  return _currentGroup;
}

void Notifier::setStrategy(Strategy *value) { strategy = value; }

QString Notifier::titleForGroup(ActivityGroup &activityGroup) {
  return QString::fromStdString(activityGroup.activity->name) + " (" +
         timeStringForMins(static_cast<int>(activityGroup.length) *
                           strategy->slotDuration()) +
         ")";
}
