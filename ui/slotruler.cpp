#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

#include "applicationsettings.h"
#include "slotboardwidget.h"
#include "slotruler.h"
#include "slotswidget.h"
#include "utils.h"

SlotRuler::SlotRuler(QWidget *parent) : DataProviderWidget(parent) {
    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    auto fontWidth = calculateLabelWidth() + 2 * ApplicationSettings::defaultPadding;
    auto width = std::max(fontWidth, 40);
    setFixedWidth(width);

    reloadStrategy();

    selection().add_on_change_callback(this, &SlotRuler::reloadStrategy);
    strategy().time_slots().add_on_ruler_change_callback([this] {
        reloadStrategy();
    });
}

int SlotRuler::calculateLabelWidth() {
    auto font = QFont();
    font.setPixelSize(bigFontHeight);
    font.setBold(true);

    return QFontMetrics(font)
        .horizontalAdvance(QStringForMinutes(0));
}

void SlotRuler::paintEvent(QPaintEvent *) {
    //    QPainter p(this);
    //
    //    auto handleBorderColor = highlightColor();
    //    handleBorderColor.setAlphaF(0.25);
    //
    //    auto handleBgColor = highlightColor();
    //    handleBgColor.setAlphaF(0.1);
    //
    //    p.setBrush(handleBgColor);
    //    p.setPen(handleBorderColor);

    //    for (auto &selectionSegment : selection().grouped()) {
    //        auto firstIndex = selectionSegment.front();
    //        auto lastIndex = selectionSegment.back() + 1;
    //
    //        auto handleWidth = calculateLabelWidth() + 4;
    //        auto handleHeight = bigFontHeight * 1.5;
    //
    //        auto rect1 = QRect((width() - handleWidth) / 2,
    //                           firstIndex * cellHeight() + (cellHeight() - handleHeight) / 2,
    //                           handleWidth,
    //                           handleHeight);
    //
    //        auto rect2 = QRect((width() - handleWidth) / 2,
    //                           lastIndex * cellHeight() + (cellHeight() - handleHeight) / 2,
    //                           handleWidth,
    //                           handleHeight);
    //
    //
    //        p.drawRoundedRect(rect1, 2, 2);
    //        p.drawRoundedRect(rect2, 2, 2);
    //    }
}

int SlotRuler::numberOfItems() {
    return strategy().time_slots().ruler_times().size();
}

QVBoxLayout *SlotRuler::listLayout() {
    return dynamic_cast<QVBoxLayout *>(layout());
}

void SlotRuler::reuseItemAtIndex(int index, ColoredLabel *itemWidget) {
    if (slotTimeChangedAt(index)) {
        itemWidget->setText(labelAtIndex(index));
    }

    itemWidget->setFixedHeight(slotHeight());
    itemWidget->setFontHeight(isIntegerHourAtIndex(index) ? bigFontHeight : smallFontHeight);
    itemWidget->setDynamicColor(labelColorGetterAtIndex(index));
}

SlotRuler::ColorGetter SlotRuler::labelColorGetterAtIndex(int index) {
    auto colorGetter = this->isIntegerHourAtIndex(index)
                           ? &SlotRuler::secondaryTextColor
                           : &SlotRuler::tertiaryTextColor;

    if (selection().is_boundary(index) || mouseHandler().resize_boundary().slot_index == index - 1) {
        colorGetter = &SlotRuler::controlColor;
    }

    return colorGetter;
}

ColoredLabel *SlotRuler::makeNewItemAtIndex(int index) {
    auto label = new ColoredLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setFixedHeight(slotHeight());
    label->setBold(true);
    label->setFontHeight(isIntegerHourAtIndex(index) ? bigFontHeight : smallFontHeight);
    label->setDynamicColor(labelColorGetterAtIndex(index));
    label->setText(labelAtIndex(index));

    return label;
}

bool SlotRuler::isIntegerHourAtIndex(int index) {
    auto time = strategy().time_slots().ruler_times()[index];
    return time % 3600 == 0;
}

bool SlotRuler::slotTimeChangedAt(int index) {
    auto indexIsSafe = index < prevTimes.size() && index >= 0;
    auto &slotTime = strategy().time_slots().ruler_times()[index];

    return indexIsSafe && prevTimes[index] != slotTime;
}

QString SlotRuler::labelAtIndex(int index) {
    auto time = strategy().time_slots().ruler_times()[index];
    return QStringForCalendarTime(time);
}

void SlotRuler::reloadStrategy() {
    updateList();
    prevTimes = strategy().time_slots().ruler_times();
}
