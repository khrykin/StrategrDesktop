#ifndef SLOT_H
#define SLOT_H

#include "activity.h"
#include <QLabel>
#include <QMap>
#include <QWidget>

class ActivityGroupWidget : public QWidget {
  Q_OBJECT

public:
  explicit ActivityGroupWidget(QWidget *parent = nullptr);
  int number() const;
  void setNumber(int number);

  std::optional<Activity> activity() const;
  void setActivity(std::optional<Activity> activity);

  int length() const;
  void setLength(unsigned int length);

  bool isSelected() const;
  void setIsSelected(bool isSelected);

  void setHeight(int height);
  void updateUI();

  void selectSlotAtIndex(int slotIndex);
  void deselectSlotAtIndex(int slotIndex);
  void deselectAllSlots();

  bool hasSelection();

  QMap<int, QWidget *> selectionSlots() const;

  int slotDuration() const;
  void setSlotDuration(int slotDuration);

  void setSlotHeight(int value);

private:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void updateStyleSheet();
  int _number;
  int _length = 1;
  int _slotDuration;
  int slotHeight;

  bool _isSelected = false;

  std::optional<Activity> _activity;

  QLabel *label;
  QLabel *titleLabel;
  QWidget *selectionWidget;
  QMap<int, QWidget *> _selectionSlots;

  QWidget *mainWidget;
};

#endif // SLOT_H
