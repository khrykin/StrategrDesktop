#include <QGraphicsBlurEffect>
#include <QHBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QStyleOption>
#include <QTimer>

#include "currenttimemarker.h"
#include "mainwindow.h"
#include "slotboardscrollarea.h"
#include "slotboardwidget.h"
#include "slotruler.h"
#include "utils.h"

SlotBoardWidget::SlotBoardWidget(QWidget *parent) : DataProviderWidget(parent) {
    auto *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);

    layoutChildWidgets(mainLayout);

    actionCenter().on_update_current_time_marker = [this]() {
        updateCurrentTimeMarker();
    };

    actionCenter().on_scroll_to_current_time = [this](bool animated) {
        auto slotsRect = slotsWidget->geometry();

        auto topOffset = stg::current_time_marker(strategy(), 5)
                             .scroll_offset(slotsRect, slotboardScrollArea()->viewportRectRelativeToContent());

        topOffset++;

        auto *scrollBar = slotboardScrollArea()->verticalScrollBar();

        if (animated) {
            auto *animation = new QPropertyAnimation(scrollBar, "value");
            animation->setDuration(200);
            animation->setStartValue(scrollBar->value());
            animation->setEndValue(topOffset);
            animation->setEasingCurve(QEasingCurve::OutCubic);

            animation->start();
        } else {
            scrollBar->setValue(topOffset);
        }
    };

    auto child_on_resize_boundary_change = mouseHandler().on_resize_boundary_change;
    mouseHandler().on_resize_boundary_change = [this, child_on_resize_boundary_change]() {
        child_on_resize_boundary_change();
        slotRuler->reloadStrategy();

        update();
    };

    mouseHandler().on_draw_dragged_session = [this](auto session_index, auto first_slot_index) {
        drawDraggedSession(session_index, first_slot_index);
    };
}

void SlotBoardWidget::layoutChildWidgets(QHBoxLayout *mainLayout) {
    slotsWidget = new SlotsWidget(this);
    slotRuler = new SlotRuler(this);

    mainLayout->addWidget(slotRuler);
    mainLayout->addWidget(slotsWidget);

    currentTimeMarkerWidget = new CurrentTimeMarkerWidget(this);
}

void SlotBoardWidget::reloadStrategy() {
    slotsWidget->reloadStrategy();
    slotRuler->reloadStrategy();
}

void SlotBoardWidget::paintEvent(QPaintEvent *event) {
    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());

    painter.drawRect(QRect(0, 0, width(), height()));

    drawResizeBoundaryCircles(painter);
}

void SlotBoardWidget::drawResizeBoundaryCircles(QPainter &painter) {
    auto slotBeforeResizeBoundaryIndex = mouseHandler().resize_boundary().slot_index;

    if (slotBeforeResizeBoundaryIndex >= -1) {
        using namespace ApplicationSettings;

        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(controlColor(), 2));
        painter.setBrush(baseColor());

        auto slotsRect = slotsWidget->geometry();

        const auto diameter = 6;

        auto topOffset = slotsRect.top() + slotHeight() / 2 + (slotBeforeResizeBoundaryIndex + 1) * slotHeight() + 1 - diameter / 2;

        auto circleLeftRect = QRect(slotsRect.left() - defaultPadding / 2 - 1,
                                    topOffset,
                                    diameter,
                                    diameter);
        auto circleRightRect = QRect(slotsRect.right() - diameter - defaultPadding + defaultPadding / 2 + 2,
                                     topOffset,
                                     diameter,
                                     diameter);

        painter.drawEllipse(circleLeftRect);
        painter.drawEllipse(circleRightRect);
    }
}

void SlotBoardWidget::resizeEvent(QResizeEvent *event) {
    updateCurrentTimeMarker();
}

void SlotBoardWidget::drawDraggedSession(int sessionIndex, int firstSlotIndex) {
    auto getRect = [=]() -> QRect {
        const auto &session = strategy().sessions()[sessionIndex];

        auto lastSlotIndex = firstSlotIndex + session.length() - 1;

        const auto &lastSlot = strategy().time_slots()[lastSlotIndex];
        auto bottomMargin = lastSlot.end_time() % 60 == 0 ? 2 : 4;
        auto horizontalMargin = 8;
        auto top = firstSlotIndex * slotHeight() + slotHeight() / 2 + slotsWidget->geometry().top();

        auto rect = QRect(slotRuler->width(),
                          top,
                          slotsWidget->width() - horizontalMargin,
                          session.length() * slotHeight() - bottomMargin);

        return rect;
    };

    auto makeBigRect = [=](const QRect &smallRect) -> QRect {
        auto bigRect = smallRect;
        bigRect.setX(smallRect.x() - 2);
        bigRect.setWidth(smallRect.width() + 4);
        return bigRect;
    };

    auto makeSmallRect = [=](const QRect &bigRect) -> QRect {
        auto smallRect = bigRect;
        smallRect.setX(bigRect.x() + 2);
        smallRect.setWidth(bigRect.width() - 4);
        return smallRect;
    };

    bool initial = false;

    if (!draggedSessionWidget && sessionIndex >= 0) {
        initial = true;

        draggedSessionWidget = new SessionWidget(strategy().sessions()[sessionIndex], this);
        draggedSessionWidget->setDrawsBorders(false);
        draggedSessionWidget->setIsSelected(true);
        currentTimeMarkerWidget->raise();

        auto shadowColor = textColor();
        shadowColor.setAlphaF(0.2);

        auto *effect = new QGraphicsDropShadowEffect;
        effect->setBlurRadius(25);
        effect->setXOffset(0);
        effect->setYOffset(0);
        effect->setColor(shadowColor);

        draggedSessionWidget->setGraphicsEffect(effect);

        draggedSessionWidget->setVisible(true);

        draggedSessionAnimation = new QPropertyAnimation(draggedSessionWidget, "geometry");
        draggedSessionAnimation->setDuration(50);

        auto smallRect = getRect();
        auto bigRect = makeBigRect(smallRect);

        draggedSessionWidget->setGeometry(smallRect);
        draggedSessionAnimation->setStartValue(smallRect);
        draggedSessionAnimation->setEndValue(bigRect);

        draggedSessionAnimation->start();
    }

    if (draggedSessionWidget && sessionIndex < 0) {
        if (draggedSessionAnimation)
            draggedSessionAnimation->deleteLater();

        draggedSessionAnimation = new QPropertyAnimation(draggedSessionWidget, "geometry");
        draggedSessionAnimation->setDuration(50);
        auto draggedSessionWidgetLocal = draggedSessionWidget;
        draggedSessionWidget = nullptr;

        connect(draggedSessionAnimation, &QPropertyAnimation::finished,
                [this, draggedSessionWidgetLocal]() {
                    draggedSessionAnimation->deleteLater();
                    draggedSessionWidgetLocal->deleteLater();

                    draggedSessionAnimation = nullptr;
                });

        auto smallRect = makeSmallRect(draggedSessionWidgetLocal->geometry());

        draggedSessionAnimation->setStartValue(draggedSessionWidgetLocal->geometry());
        draggedSessionAnimation->setEndValue(smallRect);

        draggedSessionAnimation->start();
    }

    if (draggedSessionWidget && !initial) {
        auto smallRect = getRect();
        auto bigRect = makeBigRect(smallRect);

        draggedSessionAnimation->stop();
        draggedSessionWidget->setGeometry(bigRect);
    }
}

void SlotBoardWidget::updateCurrentTimeMarker() {
    auto currentTimeMarker = stg::current_time_marker(strategy(), CurrentTimeMarkerWidget::markerSize);
    auto slotsRect = slotsWidget->geometry();

    currentTimeMarkerWidget->setGeometry(currentTimeMarker.rect_in_slots_rect(slotsRect));
    currentTimeMarkerWidget->setHidden(currentTimeMarker.is_hidden());
}
