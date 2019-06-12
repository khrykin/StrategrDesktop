#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include "groupslist.h"
#include "slotruler.h"
#include "strategy.h"
#include <QWidget>

class SlotBoard : public QWidget {
  Q_OBJECT
public:
  explicit SlotBoard(QWidget *parent = nullptr);

  Strategy *strategy() const;
  void setStrategy(Strategy *strategy);

  GroupsList *groupsList() const;
  SlotRuler *slotRuler() const;

signals:

public slots:
private:
  Strategy *_strategy;
  GroupsList *_groupsList;
  SlotRuler *_slotRuler;
};

#endif // SLOTBOARD_H
