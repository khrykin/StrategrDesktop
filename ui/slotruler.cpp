#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

#include "applicationsettings.h"
#include "utils.h"
#include "slotruler.h"
#include "slotboardwidget.h"
#include "slotswidget.h"

SlotRuler::SlotRuler(const QVector<TimeLabel> &labels,
                     int cellHeight,
                     QWidget *parent)
        : _labels(labels),
          _cellHeight(cellHeight),
          QWidget(parent) {
    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    auto fontWidth = calculateLabelWidth() + 2 * ApplicationSettings::defaultPadding;
    auto width = std::max(fontWidth, 40);
    setFixedWidth(width);

    updateList();

    selection().add_on_change_callback(this, &SlotRuler::updateList);
}

int SlotRuler::calculateLabelWidth() const {
    auto font = QFont();
    font.setPixelSize(bigFontHeight);
    font.setBold(true);

    return QFontMetrics(font)
            .horizontalAdvance(QStringForMinutes(0));
}

const QVector<TimeLabel> &SlotRuler::labels() const {
    return _labels;
}

void SlotRuler::setLabels(const QVector<TimeLabel> &labels) {
    if (_labels != labels) {
        _labels = labels;
        updateList();
    }
}

void SlotRuler::paintEvent(QPaintEvent *) {
    QPainter p(this);

    auto handleBorderColor = highlightColor();
    handleBorderColor.setAlphaF(0.25);

    auto handleBgColor = highlightColor();
    handleBgColor.setAlphaF(0.1);

    p.setBrush(handleBgColor);
    p.setPen(handleBorderColor);

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

int SlotRuler::cellHeight() const { return _cellHeight; }

void SlotRuler::setCellHeight(int cellHeight) {
    _cellHeight = cellHeight;
    updateList();
}

int SlotRuler::numberOfItems() {
    return labels().count();
}

QVBoxLayout *SlotRuler::listLayout() {
    return dynamic_cast<QVBoxLayout *>(layout());
}

void SlotRuler::reuseItemAtIndex(int index, ColoredLabel *itemWidget) {
    itemWidget->setText(labels()[index].label);
    itemWidget->setFixedHeight(cellHeight());
    itemWidget->setFontHeight(isIntegerHourAtIndex(index) ? bigFontHeight : smallFontHeight);
    itemWidget->setDynamicColor(labelColorGetterAtIndex(index));
}

SlotRuler::ColorGetter SlotRuler::labelColorGetterAtIndex(int index) {
    auto colorGetter = this->isIntegerHourAtIndex(index)
                       ? &SlotRuler::secondaryTextColor
                       : &SlotRuler::tertiaryTextColor;

    if (selection().is_boundary(index) ||
        slotsWidget()->slotBeforeBoundaryIndex() == index - 1)
        colorGetter = &SlotRuler::controlColor;


    return colorGetter;
}

ColoredLabel *SlotRuler::makeNewItemAtIndex(int index) {
    auto label = new ColoredLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setFixedHeight(cellHeight());
    label->setBold(true);
    label->setFontHeight(isIntegerHourAtIndex(index) ? bigFontHeight : smallFontHeight);
    label->setDynamicColor(labelColorGetterAtIndex(index));
    label->setText(labels()[index].label);

    return label;
}

bool SlotRuler::isIntegerHourAtIndex(int index) const {
    auto &timeLabel = labels()[index];
    auto isIntegerHour = timeLabel.time % 60 == 0;

    return isIntegerHour;
}

const stg::selection &SlotRuler::selection() const {
    return slotsWidget()->selection();
}

SlotsWidget *SlotRuler::slotsWidget() const {
    auto slotBoardWidget = qobject_cast<SlotBoardWidget *>(parentWidget());
    return slotBoardWidget->slotsWidget();
}

