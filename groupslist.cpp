#include "groupslist.h"
#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyleOption>
#include <QVector>

GroupsList::GroupsList(QWidget *parent) : QWidget(parent) {
  setLayout(new QVBoxLayout());
  layout()->setSpacing(0);
  layout()->setContentsMargins(0, 1, 0, 0);

  setActivityAction = new QAction(tr("Set Activity"), this);
  connect(setActivityAction, &QAction::triggered, this,
          &GroupsList::openActivitiesWindow);
  setActivityAction->setShortcut(QKeySequence(Qt::Key_Return));
  addAction(setActivityAction);

  deleteActivityAction = new QAction(tr("Make Empty"), this);
  QList<QKeySequence> deleteShortcuts;
  deleteShortcuts << QKeySequence(Qt::Key_Delete)
                  << QKeySequence(Qt::Key_Backspace);
  deleteActivityAction->setShortcuts(deleteShortcuts);
  addAction(deleteActivityAction);
  connect(deleteActivityAction, &QAction::triggered, this,
          &GroupsList::deleteActivityInSelection);

  clearSelectionAction = new QAction(tr("Clear Selection"), this);
  clearSelectionAction->setShortcut(QKeySequence(Qt::Key_Escape));
  addAction(clearSelectionAction);
  connect(clearSelectionAction, &QAction::triggered, this,
          &GroupsList::clearCurrentSelection);

  undoAction = new QAction(tr("Undo"), this);
  undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
  addAction(undoAction);
  connect(undoAction, &QAction::triggered, this, &GroupsList::undo);

  redoAction = new QAction(tr("Redo"), this);
  redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
  addAction(redoAction);
  connect(redoAction, &QAction::triggered, this, &GroupsList::redo);

  setStyleSheet("GroupsList {"
                "border-top: 1px solid #ccc;"
                "}");
}

void GroupsList::updateUI() {
  auto state = strategy()->group();
  for (unsigned int i = 0; i < state.size(); i++) {
    ActivityGroupWidget *groupWidget;
    auto *laytoutItem = layout()->itemAt(static_cast<int>(i));
    if (laytoutItem != nullptr) {
      groupWidget = qobject_cast<ActivityGroupWidget *>(laytoutItem->widget());

      if (!groupWidget) {
        layout()->removeItem(laytoutItem);
        return;
      }

      if (groupWidget->isHidden()) {
        groupWidget->show();
      }
    } else {
      groupWidget = new ActivityGroupWidget();
      groupWidget->setSlotHeight(slotHeight());
      layout()->addWidget(groupWidget);
    }

    auto activityGroup = state[i];

    if (groupWidget->slotDuration() != strategy()->slotDuration()) {
      groupWidget->setSlotDuration(strategy()->slotDuration());
    }

    auto groupHeight = _slotHeight * static_cast<int>(activityGroup.length);
    if (groupWidget->minimumHeight() != groupHeight) {
      groupWidget->setHeight(groupHeight);
      groupWidget->setLength(activityGroup.length);
    }

    if (activityGroup.activity) {
      if (groupWidget->activity() != activityGroup.activity) {
        groupWidget->setActivity(activityGroup.activity);
      }
    } else {
      if (groupWidget->activity()) {
        groupWidget->setActivity(std::nullopt);
      }
    }

    auto groupNumber = static_cast<int>(i);
    if (groupWidget->number() != groupNumber) {
      groupWidget->setNumber(groupNumber);
    }
  }

  int stateSize = static_cast<int>(state.size());
  int hideAtIndex = stateSize;

  while (layout()->itemAt(hideAtIndex) != nullptr) {
    auto itemToHide = layout()->itemAt(hideAtIndex);
    if (itemToHide->widget() != nullptr) {
      itemToHide->widget()->hide();
    }

    hideAtIndex++;
  }
}

void GroupsList::mousePressEvent(QMouseEvent *event) {
  _pulledFrom = slotIndexForEvent(event);
  if ((event->buttons() == Qt::LeftButton && event->modifiers() == Qt::CTRL) ||
      (event->buttons() == Qt::RightButton && !hasSelection())) {
    // Slot selection event
    selectSlotAtIndex(_pulledFrom);
  } else if (event->buttons() == Qt::LeftButton) {
    // Slot pulling event
    _isPulling = true;
    selectGroupAtSlotIndex(_pulledFrom);
    historyEntry = Strategy::HistoryEntry(strategy());
    deselectAllSlots();
  }
}

void GroupsList::mouseReleaseEvent(QMouseEvent *event) {
  _isPulling = false;
  _pulledTo = slotIndexForEvent(event);
  deselectAllGroups();

  if (historyEntry) {
    strategy()->commitToHistory(historyEntry.value());
    historyEntry = std::nullopt;
    emit slotsStateChanged();
  }
}

void GroupsList::mouseMoveEvent(QMouseEvent *event) {
  if (_isPulling) {
    _pulledTo = slotIndexForEvent(event);
    fillSlots(_pulledFrom, _pulledTo);
    selectGroupAtSlotIndex(_pulledTo);
  }
}

void GroupsList::contextMenuEvent(QContextMenuEvent *event) {
  auto currentIndex = slotIndexForEvent(event);
  auto currentSlot = strategy()->slotAtIndex(currentIndex);

  QMenu menu(this);
  menu.addAction(setActivityAction);
  if (currentSlot) {
    menu.addAction(deleteActivityAction);
  }
  menu.addAction(clearSelectionAction);
  menu.exec(event->globalPos());
}

int GroupsList::slotIndexForEvent(QContextMenuEvent *event) {
  return event->pos().y() / _slotHeight;
}

int GroupsList::slotIndexForEvent(QMouseEvent *event) {
  return event->pos().y() / _slotHeight;
}

void GroupsList::copySlot(int fromIndex, int toIndex) {
  auto fromSlot = strategy()->slotAtIndex(fromIndex);
  auto toSlot = strategy()->slotAtIndex(toIndex);

  if (fromSlot == toSlot) {
    return;
  }

  strategy()->copySlot(fromIndex, toIndex);
  updateUI();
}

void GroupsList::fillSlots(int fromIndex, int toIndex) {
  auto fromSlot = strategy()->slotAtIndex(fromIndex);
  auto toSlot = strategy()->slotAtIndex(toIndex);

  deselectAllSlots();

  if (fromSlot == toSlot) {
    return;
  }

  strategy()->fillSlots(fromIndex, toIndex);
  updateUI();
}

void GroupsList::selectGroupAtSlotIndex(int selectedSlotIndex) {
  if (_selectedGroupIndex && _selectedGroupIndex.value() == selectedSlotIndex) {
    return;
  }

  _selectedGroupIndex = selectedSlotIndex;
  auto groupIndex = strategy()->groupIndexForSlotIndex(selectedSlotIndex);

  if (groupIndex) {
    for (int i = 0; i < layout()->count(); i++) {
      auto *group = groupWidgetAtIndex(i);
      auto isSelected = i == groupIndex.value() ? true : false;

      if (group->isSelected() != isSelected) {
        group->setIsSelected(isSelected);
      }
    }
  }
}

void GroupsList::deselectAllGroups() {
  _selectedGroupIndex = nullopt;
  for (int i = 0; i < layout()->count(); i++) {
    auto *slot = groupWidgetAtIndex(i);
    if (slot->isSelected() != false) {
      slot->setIsSelected(false);
    }
  }
}

void GroupsList::selectSlotAtIndex(int slotIndex) {
  auto groupIndex = strategy()->groupIndexForSlotIndex(slotIndex);

  if (!groupIndex) {
    return;
  }

  auto *selectedGroupWidget = groupWidgetAtIndex(groupIndex.value());
  auto startSlotIndex =
      strategy()->startSlotIndexForGroupIndex(groupIndex.value());

  if (!startSlotIndex) {
    return;
  }

  auto relativeIndex = slotIndex - startSlotIndex.value();
  selectedGroupWidget->selectSlotAtIndex(relativeIndex);
}

void GroupsList::deselectAllSlots() {
  for (auto i = 0; i < layout()->count(); i++) {
    auto *groupWidget = groupWidgetAtIndex(i);
    if (groupWidget->hasSelection()) {
      groupWidget->deselectAllSlots();
    }
  }
}

ActivityGroupWidget *GroupsList::groupWidgetAtIndex(int index) {
  return static_cast<ActivityGroupWidget *>(layout()->itemAt(index)->widget());
}

int GroupsList::slotHeight() const { return _slotHeight; }

Strategy *GroupsList::strategy() const { return _strategy; }

void GroupsList::setStrategy(Strategy *strategy) {
  _strategy = strategy;
  updateUI();
  deselectAllSlots();
  deselectAllGroups();
}

void GroupsList::paintEvent(QPaintEvent *) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QAction *GroupsList::getUndoAction() const { return undoAction; }

QAction *GroupsList::getRedoAction() const { return redoAction; }

QVector<int> GroupsList::selectionSlots() {
  QVector<int> result;
  for (int i = 0; i < layout()->count(); i++) {
    auto *groupWidget = groupWidgetAtIndex(i);
    auto selectionSlotsRelativeIndices = groupWidget->selectionSlots().keys();
    for (auto relativeIndex : selectionSlotsRelativeIndices) {
      auto startSlotIndex = strategy()->startSlotIndexForGroupIndex(i);
      if (startSlotIndex) {
        result.append(startSlotIndex.value() + relativeIndex);
      }
    }
  }

  return result;
}

bool GroupsList::hasSelection() { return selectionSlots().length() > 0; }

void GroupsList::openActivitiesWindow() {
  emit wantToSetActivtyForSelection(selectionSlots());
}

void GroupsList::deleteActivityInSelection() {
  if (hasSelection()) {
    strategy()->setSlotAtIndices(selectionSlots().toStdVector(), nullopt);
    updateUI();
    deselectAllSlots();
  }
}

void GroupsList::clearCurrentSelection() { deselectAllSlots(); }

void GroupsList::undo() {
  strategy()->undo();
  updateUI();
  emit slotsStateChanged();
  emit wantToUpdateActivitiesList();
}

void GroupsList::redo() {
  strategy()->redo();
  updateUI();
  emit slotsStateChanged();
  emit wantToUpdateActivitiesList();
}
