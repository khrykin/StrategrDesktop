#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include "currenttimemarker.h"
#include "groupslist.h"
#include "slotruler.h"
#include "strategy.h"
#include <QTimer>
#include <QWidget>

class SlotBoard : public QWidget {
  Q_OBJECT
public:
  explicit SlotBoard(QWidget *parent = nullptr);

  Strategy *strategy() const;
  void setStrategy(Strategy *strategy);

  GroupsList *groupsList() const;
  SlotRuler *slotRuler() const;

  CurrentTimeMarker *currentTimeMarker() const;

signals:

public slots:
private:
  Strategy *_strategy;
  GroupsList *_groupsList;
  SlotRuler *_slotRuler;
  CurrentTimeMarker *_currentTimeMarker;
  QTimer *currentTimeTimer;

  void resizeEvent(QResizeEvent *event);
  void updateCurrentTimeMarker();
};

#endif // SLOTBOARD_H
