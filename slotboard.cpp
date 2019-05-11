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

    for (unsigned int i = 0; i < state.size(); i++) {
        SlotWidget *slot;
        auto *laytoutItem = layout()->itemAt(static_cast<int>(i));
        if (laytoutItem != nullptr) {
            slot = static_cast<SlotWidget *>(laytoutItem->widget());
            if (slot->isHidden()) {
                slot->show();
            }
        } else {
            slot = new SlotWidget();
            layout()->addWidget(slot);
        }

        auto activityGroup = state[i];

        auto groupHeight = SLOT_HEIGHT * static_cast<int>(activityGroup.length);
        if (slot->minimumHeight() != groupHeight) {
            slot->setMinimumHeight(groupHeight);
        }

        auto groupNumber = static_cast<int>(i);
        if (slot->number() != groupNumber) {
            slot->setNumber(groupNumber);
        }

        if (activityGroup.activity.has_value()) {
            auto title = QString::fromStdString(activityGroup.activity.value()->name);
            if (slot->title() != title) {
                slot->setTitle(title);
            }
        } else {
            if (slot->title() != "") {
                slot->setTitle("");
            }
        }
    }

    int stateSize = static_cast<int>(state.size());
    while (layout()->itemAt(stateSize) != nullptr && layout()->itemAt(stateSize)->widget()->isVisible()) {
        layout()->itemAt(stateSize)->widget()->hide();
    }

    qDebug().noquote() << QString::fromStdString(strategy()->debugGroups());
}

void SlotBoard::mousePressEvent(QMouseEvent *event)
{
    _isPulling = true;
    _pulledFrom = slotIndexForEvent(event);
    setSlotSelected(_pulledFrom);
}

void SlotBoard::mouseReleaseEvent(QMouseEvent *event)
{
    _isPulling = false;
    _pulledTo = slotIndexForEvent(event);
    deselectAllSLots();

//    qDebug() << "mouseReleaseEvent" << "pulledFrom" << pulledFrom << "to" << pulledTo;
}

void SlotBoard::mouseMoveEvent(QMouseEvent *event)
{
    _pulledTo = slotIndexForEvent(event);
    fillSlots(_pulledFrom, _pulledTo);
    setSlotSelected(_pulledTo);
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

    if (fromSlot == toSlot) {
        return;
    }

    strategy()->fillSlots(fromIndex, toIndex);
    updateUI();
}

void SlotBoard::setSlotSelected(int selectedIndex)
{
    if (_selectedSlotIndex.has_value() && _selectedSlotIndex.value() == selectedIndex) {
        return;
    }

    _selectedSlotIndex = make_optional(selectedIndex);
    auto groupIndex = strategy()->groupIndexForSlotIndex(selectedIndex);
    if (groupIndex.has_value()) {
        for (int i = 0; i < layout()->count(); i++) {
            auto *slot = slotAtIndex(i);
            auto isSelected = i == groupIndex.value() ? true : false;

            if (slot->isSelected() != isSelected) {
                slot->setIsSelected(isSelected);
            }
        }
    }
}

void SlotBoard::deselectAllSLots()
{
    _selectedSlotIndex = nullopt;
    for (int i = 0; i < layout()->count(); i++) {
        auto *slot = slotAtIndex(i);
        if (slot->isSelected() != false) {
            slot->setIsSelected(false);
        }
    }
}

SlotWidget *SlotBoard::slotAtIndex(int index)
{
    return static_cast<SlotWidget *>(layout()->itemAt(index)->widget());
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
