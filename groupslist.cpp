#include "groupslist.h"
#include <QDebug>
#include <QLayout>
#include <QPainter>
#include <QStyleOption>
#include <QScrollArea>
#include <QScrollBar>
#include <QMenu>
#include <QVector>

GroupsList::GroupsList(QWidget *parent) : QWidget(parent)
{
    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);
    layout()->setMargin(0);

    setActivityAction = new QAction(tr("Set Activity"), this);
    connect(setActivityAction, &QAction::triggered, this, &GroupsList::openActivitiesWindow);
    setActivityAction->setShortcut(QKeySequence(Qt::Key_Return));
    addAction(setActivityAction);

    deleteActivityAction = new QAction(tr("Make Empty"), this);
    deleteActivityAction->setShortcut(QKeySequence(Qt::Key_Delete));
    addAction(deleteActivityAction);
    connect(deleteActivityAction, &QAction::triggered, this, &GroupsList::deleteActivityInSelection);

    clearSelectionAction = new QAction(tr("Clear Selection"), this);
    clearSelectionAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(clearSelectionAction);
    connect(clearSelectionAction, &QAction::triggered, this, &GroupsList::clearCurrentSelection);

    undoAction = new QAction(tr("Undo"), this);
    undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    addAction(undoAction);
    connect(undoAction, &QAction::triggered, this, &GroupsList::undo);

    redoAction = new QAction(tr("Undo"), this);
    redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
    addAction(redoAction);
    connect(redoAction, &QAction::triggered, this, &GroupsList::redo);
}

void GroupsList::updateUI()
{
    qDebug() << "setState";

    auto state = strategy()->group();

    int stateSize = static_cast<int>(state.size());
    int hideAtIndex = stateSize;
    while (layout()->itemAt(hideAtIndex) != nullptr) {
        auto *widgetToHide = layout()->itemAt(hideAtIndex)->widget();
        if (widgetToHide->isVisible()) {
            widgetToHide->hide();
        }

        hideAtIndex++;
    }

    for (unsigned int i = 0; i < state.size(); i++) {
        ActivityGroupWidget *groupWidget;
        auto *laytoutItem = layout()->itemAt(static_cast<int>(i));
        if (laytoutItem != nullptr) {
            groupWidget = static_cast<ActivityGroupWidget *>(laytoutItem->widget());
            if (groupWidget->isHidden()) {
                groupWidget->show();
            }
        } else {
            groupWidget = new ActivityGroupWidget();
            layout()->addWidget(groupWidget);
        }

        auto activityGroup = state[i];

        auto groupHeight = SLOT_HEIGHT * static_cast<int>(activityGroup.length);
        if (groupWidget->minimumHeight() != groupHeight) {
            groupWidget->setSlotHeight(groupHeight);
        }

        auto groupNumber = static_cast<int>(i);
        if (groupWidget->number() != groupNumber) {
            groupWidget->setNumber(groupNumber);
        }

        if (activityGroup.activity.has_value()) {
            auto title = QString::fromStdString(activityGroup.activity.value().name);
            if (groupWidget->title() != title) {
                groupWidget->setTitle(title);
            }
        } else {
            if (groupWidget->title() != "") {
                groupWidget->setTitle("");
            }
        }
    }

    qDebug().noquote() << QString::fromStdString(strategy()->debugGroups());

    for (int i = 0; i < layout()->count(); i++) {
        qDebug() << "layout item" << i << "visible?" << layout()->itemAt(i)->widget()->isVisible();
    }

}

void GroupsList::mousePressEvent(QMouseEvent *event)
{
    _pulledFrom = slotIndexForEvent(event);
    if ((event->buttons() == Qt::LeftButton && event->modifiers() == Qt::CTRL)
            || (event->buttons() == Qt::RightButton && !hasSelection())) {
        // Slot selection event
        selectSlotAtIndex(_pulledFrom);
    } else if (event->buttons() == Qt::LeftButton) {
        // Slot pulling event
        _isPulling = true;
        selectGroupAtIndex(_pulledFrom);
        historyEntry = Strategy::HistoryEntry(strategy());
        deselectAllSlots();
    }
}

void GroupsList::mouseReleaseEvent(QMouseEvent *event)
{
    _isPulling = false;
    _pulledTo = slotIndexForEvent(event);
    deselectAllGroups();

    if (historyEntry.has_value()) {
        strategy()->commitToHistory(historyEntry.value());
        historyEntry = std::nullopt;
    }

    qDebug() << "mouseReleaseEvent";
}

void GroupsList::mouseMoveEvent(QMouseEvent *event)
{
    if (_isPulling) {
        _pulledTo = slotIndexForEvent(event);
        fillSlots(_pulledFrom, _pulledTo);
        selectGroupAtIndex(_pulledTo);
    }
}

void GroupsList::contextMenuEvent(QContextMenuEvent *event)
{
    auto currentIndex = slotIndexForEvent(event);
    auto currentSlot = strategy()->slotAtIndex(currentIndex);

    QMenu menu(this);
    menu.addAction(setActivityAction);
    if (currentSlot.has_value()) {
        menu.addAction(deleteActivityAction);
    }
    menu.addAction(clearSelectionAction);
    menu.exec(event->globalPos());

    qDebug() << "context menu event" << selectionSlots();
}

int GroupsList::slotIndexForEvent(QContextMenuEvent *event)
{
    return event->pos().y() / SLOT_HEIGHT;
}

int GroupsList::slotIndexForEvent(QMouseEvent *event)
{
    return event->pos().y() / SLOT_HEIGHT;
}

void GroupsList::copySlot(int fromIndex, int toIndex)
{
    auto fromSlot = strategy()->slotAtIndex(fromIndex);
    auto toSlot = strategy()->slotAtIndex(toIndex);

    if (fromSlot == toSlot) {
        return;
    }

    strategy()->copySlot(fromIndex, toIndex);
    updateUI();
}

void GroupsList::fillSlots(int fromIndex, int toIndex)
{
    auto fromSlot = strategy()->slotAtIndex(fromIndex);
    auto toSlot = strategy()->slotAtIndex(toIndex);

    deselectAllSlots();

    if (fromSlot == toSlot) {
        return;
    }

    strategy()->fillSlots(fromIndex, toIndex);
    updateUI();
}

void GroupsList::selectGroupAtIndex(int selectedGroupIndex)
{
    if (_selectedGroupIndex.has_value() && _selectedGroupIndex.value() == selectedGroupIndex) {
        return;
    }

    _selectedGroupIndex = make_optional(selectedGroupIndex);
    auto groupIndex = strategy()->groupIndexForSlotIndex(selectedGroupIndex);
    if (groupIndex.has_value()) {
        for (int i = 0; i < layout()->count(); i++) {
            auto *group = groupWidgetAtIndex(i);
            auto isSelected = i == groupIndex.value() ? true : false;

            if (group->isSelected() != isSelected) {
                group->setIsSelected(isSelected);
            }
        }
    }
}

void GroupsList::deselectAllGroups()
{
    _selectedGroupIndex = nullopt;
    for (int i = 0; i < layout()->count(); i++) {
        auto *slot = groupWidgetAtIndex(i);
        if (slot->isSelected() != false) {
            slot->setIsSelected(false);
        }
    }
}

void GroupsList::selectSlotAtIndex(int slotIndex)
{
    auto groupIndex = strategy()->groupIndexForSlotIndex(slotIndex);

    if (!groupIndex.has_value()) {
        return;
    }

    auto *selectedGroupWidget = groupWidgetAtIndex(groupIndex.value());
    auto startSlotIndex = strategy()->startSlotIndexForGroupIndex(groupIndex.value());

    if (!startSlotIndex.has_value()) {
        return;
    }

    auto relativeIndex = slotIndex - startSlotIndex.value();

    selectedGroupWidget->selectSlotAtIndex(relativeIndex);

    qDebug() << "startSlotIndex" << startSlotIndex.value() << "relativeIndex" << relativeIndex;
}

void GroupsList::deselectAllSlots()
{
    for (auto i = 0; i < layout()->count(); i++) {
        auto *groupWidget = groupWidgetAtIndex(i);
        if (groupWidget->hasSelection()) {
            groupWidget->deselectAllSlots();
        }
    }
}

ActivityGroupWidget *GroupsList::groupWidgetAtIndex(int index)
{
    return static_cast<ActivityGroupWidget *>(layout()->itemAt(index)->widget());
}

Strategy *GroupsList::strategy() const
{
    return _strategy;
}

void GroupsList::setStrategy(Strategy *strategy)
{
    _strategy = strategy;
    updateUI();
}

void GroupsList::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QVector<int> GroupsList::selectionSlots()
{
    QVector<int> result;
    for (int i = 0; i < layout()->count(); i++) {
        auto *groupWidget = groupWidgetAtIndex(i);
        auto selectionSlotsRelativeIndices = groupWidget->selectionSlots().keys();
        for (auto relativeIndex : selectionSlotsRelativeIndices) {
            auto startSlotIndex = strategy()->startSlotIndexForGroupIndex(i);
            if (startSlotIndex.has_value()) {
                result.append(startSlotIndex.value() + relativeIndex);
            }
        }

    }

    return result;
}

bool GroupsList::hasSelection()
{
    return selectionSlots().length() > 0;
}

void GroupsList::openActivitiesWindow()
{
    emit wantToSetActivtyForSelection(selectionSlots());
    qDebug() << "open activities window";
}

void GroupsList::deleteActivityInSelection()
{
    if (hasSelection()) {
        strategy()->setSlotAtIndices(selectionSlots().toStdVector(), nullopt);
        updateUI();
        deselectAllSlots();
        qDebug() << "Delete activity";
    }
}

void GroupsList::clearCurrentSelection()
{
    deselectAllSlots();
}

void GroupsList::undo()
{
    strategy()->undo();
    updateUI();
}

void GroupsList::redo()
{
    strategy()->redo();
    updateUI();
}
