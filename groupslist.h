#ifndef GROUPSLIST_H
#define GROUPSLIST_H

#include <QAction>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "activitygroupwidget.h"
#include "slotruler.h"
#include "strategy.h"

class GroupsList : public QWidget {
  Q_OBJECT
public:
  explicit GroupsList(QWidget *parent = nullptr);

  void updateUI();
  Strategy *strategy() const;
  void setStrategy(Strategy *strategy);

  QVector<int> selectionSlots();
  bool hasSelection();
  void selectSlotAtIndex(int slotIndex);
  void deselectAllSlots();
  void undo();
  void redo();

  QAction *getUndoAction() const;
  QAction *getRedoAction() const;

  int slotHeight() const;

signals:
  void wantToSetActivtyForSelection(QVector<int> selection);
  void wantToUpdateActivitiesList();
  void slotsStateChanged();

private:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;

  int slotIndexForEvent(QContextMenuEvent *event);
  int slotIndexForEvent(QMouseEvent *event);

  void copySlot(int fromIndex, int toIndex);
  void fillSlots(int fromIndex, int toIndex);
  void selectGroupAtSlotIndex(int selectedSlotIndex);
  void deselectAllGroups();

  ActivityGroupWidget *groupWidgetAtIndex(int index);

  int _slotHeight = 35;

  void paintEvent(QPaintEvent *) override;

  Strategy *_strategy;

  std::optional<int> _selectedGroupIndex;

  int _pulledFrom;
  int _pulledTo;

  bool _isPulling = false;

  QTimer *_longPressTimer;

  QWidget *selectionWidget;

  QAction *setActivityAction;
  QAction *deleteActivityAction;
  QAction *clearSelectionAction;

  QAction *undoAction;
  QAction *redoAction;

  std::optional<Strategy::HistoryEntry> historyEntry;

  void openActivitiesWindow();
  void deleteActivityInSelection();
  void clearCurrentSelection();
};

#endif // GROUPSLIST_H
