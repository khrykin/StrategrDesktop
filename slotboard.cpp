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
        auto laytoutItem = layout()->itemAt(static_cast<int>(i));
        if (laytoutItem != nullptr) {
            slot = static_cast<SlotWidget *>(laytoutItem->widget());
        } else {
            slot = new SlotWidget();
            layout()->addWidget(slot);
        }

        auto activityGroup = state[i];
        slot->setMinimumHeight(SLOT_HEIGHT * activityGroup.length);
        slot->setNumber(static_cast<int>(i));

        if (activityGroup.activity.has_value()) {
            slot->setTitle(QString::fromStdString(activityGroup.activity.value()->name));
        } else {
            slot->setTitle("");
        }
    }

    strategy()->debug();
}

void SlotBoard::mousePressEvent(QMouseEvent *event)
{
    isPulling = true;
    pulledFrom = slotIndexForEvent(event);
//    qDebug() << "mousePressEvent" << "pulledFrom" << pulledFrom;
}

void SlotBoard::mouseReleaseEvent(QMouseEvent *event)
{
    isPulling = false;
    pulledTo = slotIndexForEvent(event);
//    fillSlots(pulledFrom, pulledTo);
//    qDebug() << "mouseReleaseEvent" << "pulledFrom" << pulledFrom << "to" << pulledTo;
}

void SlotBoard::mouseMoveEvent(QMouseEvent *event)
{
    pulledTo = slotIndexForEvent(event);
    copySlot(pulledFrom, pulledTo);

    qDebug() << "mouseMoveEvent" << slotIndexForEvent(event);
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

    qDebug() << "copySlot" << fromIndex << toIndex;

    strategy()->copySlot(fromIndex, toIndex);
    updateUI();
}

void SlotBoard::fillSlots(int fromIndex, int toIndex)
{
    auto sourceIndex = fromIndex;

    if (toIndex < fromIndex) {
        auto tempIndex = fromIndex;
        fromIndex = toIndex;
        toIndex = tempIndex;
    }

    for (int i = fromIndex + 1; i <= toIndex; i++) {
        copySlot(sourceIndex, i);
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
