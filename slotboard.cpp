#include "slotboard.h"
#include <QDebug>
#include <QLayout>
#include <QPainter>
#include <QStyleOption>
#include <QScrollArea>
#include <QScrollBar>

SlotBoard::SlotBoard(QWidget *parent) : QWidget(parent)
{
    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);
    layout()->setMargin(0);
}

void SlotBoard::updateUI()
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
            auto title = QString::fromStdString(activityGroup.activity.value()->name);
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

void SlotBoard::mousePressEvent(QMouseEvent *event)
{
    _pulledFrom = slotIndexForEvent(event);
    if (event->modifiers() == Qt::CTRL) {
        selectSlotAtIndex(_pulledFrom);
    } else if (event->buttons() == Qt::LeftButton) {
        _isPulling = true;
        selectGroupAtIndex(_pulledFrom);
        deselectAllSlots();
    }
}

void SlotBoard::mouseReleaseEvent(QMouseEvent *event)
{
    _isPulling = false;
    _pulledTo = slotIndexForEvent(event);
    deselectAllGroups();

    qDebug() << "mouseReleaseEvent";
}

void SlotBoard::mouseMoveEvent(QMouseEvent *event)
{
    if (_isPulling) {
        _pulledTo = slotIndexForEvent(event);
        fillSlots(_pulledFrom, _pulledTo);
        selectGroupAtIndex(_pulledTo);
    }
}

int SlotBoard::slotIndexForEvent(QMouseEvent *event)
{
    return event->pos().y() / SLOT_HEIGHT;
}

void SlotBoard::copySlot(int fromIndex, int toIndex)
{
    auto fromSlot = strategy()->slotAtIndex(fromIndex);
    auto toSlot = strategy()->slotAtIndex(toIndex);

    if (fromSlot == toSlot) {
        return;
    }

    strategy()->copySlot(fromIndex, toIndex);
    updateUI();
}

void SlotBoard::fillSlots(int fromIndex, int toIndex)
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

void SlotBoard::selectGroupAtIndex(int selectedGroupIndex)
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

void SlotBoard::deselectAllGroups()
{
    _selectedGroupIndex = nullopt;
    for (int i = 0; i < layout()->count(); i++) {
        auto *slot = groupWidgetAtIndex(i);
        if (slot->isSelected() != false) {
            slot->setIsSelected(false);
        }
    }
}

void SlotBoard::selectSlotAtIndex(int slotIndex)
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

void SlotBoard::deselectAllSlots()
{
    for (auto i = 0; i < layout()->count(); i++) {
        auto *groupWidget = groupWidgetAtIndex(i);
        if (groupWidget->hasSelection()) {
            groupWidget->deselectAllSlots();
        }
    }
}

ActivityGroupWidget *SlotBoard::groupWidgetAtIndex(int index)
{
    return static_cast<ActivityGroupWidget *>(layout()->itemAt(index)->widget());
}

Strategy *SlotBoard::strategy() const
{
    return _strategy;
}

void SlotBoard::setStrategy(Strategy *strategy)
{
    _strategy = strategy;
}

void SlotBoard::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
