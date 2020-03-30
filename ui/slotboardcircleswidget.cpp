//
// Created by Dmitry Khrykin on 2020-01-31.
//

#include <QPainter>

#include "slotboardcircleswidget.h"
#include "applicationsettings.h"

SlotBoardCirclesWidget::SlotBoardCirclesWidget(const std::function<int()> &getSlotSize,
                                               const std::function<const QRect()> &getSlotsRect,
                                               QWidget *parent)
        : QWidget(parent),
          getSlotHeight(getSlotSize),
          getSlotsRect(getSlotsRect) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void SlotBoardCirclesWidget::paintEvent(QPaintEvent *event) {
    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);

    if (_slotBeforeResizeBoundaryIndex >= -1) {
        using namespace ApplicationSettings;

        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(controlColor(), 2));
        painter.setBrush(baseColor());

        auto slotsRect = getSlotsRect();

        auto radius = 6;
        auto firstSlotTopOffset = slotsRect.top() + 2;

        auto topOffset = firstSlotTopOffset
                         + (_slotBeforeResizeBoundaryIndex + 1) * getSlotHeight()
                         - 1 - radius / 2;

        auto circleLeftRect = QRect(slotsRect.left() + defaultPadding / 2 - 1,
                                    topOffset,
                                    radius,
                                    radius);
        auto circleRightRect = QRect(slotsRect.right() - radius - defaultPadding + defaultPadding / 2 + 2,
                                     topOffset,
                                     radius,
                                     radius);

        painter.drawEllipse(circleLeftRect);
        painter.drawEllipse(circleRightRect);
    }
}

void SlotBoardCirclesWidget::updateResizeBoundary(int, int slotBeforeResizeBoundaryIndex) {
    this->_slotBeforeResizeBoundaryIndex = slotBeforeResizeBoundaryIndex;
    update();
}

int SlotBoardCirclesWidget::slotBeforeResizeBoundaryIndex() const {
    return _slotBeforeResizeBoundaryIndex;
}
