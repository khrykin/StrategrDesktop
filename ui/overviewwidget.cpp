//
// Created by Dmitry Khrykin on 2019-08-15.
//

#include <QPainter>
#include <QScrollBar>
#include <QMouseEvent>

#include "overviewwidget.h"
#include "colorutils.h"
#include "applicationsettings.h"
#include "overview.h"
#include "slotboardscrollarea.h"
#include "slotboardwidget.h"

OverviewWidget::OverviewWidget(stg::strategy &strategy,
                               SlotboardScrollArea *scrollArea,
                               QWidget *parent) :
        strategy(strategy), QWidget(parent), scrollArea(scrollArea) {
    setFixedHeight(ApplicationSettings::overviewHeight);

    connect(scrollArea->verticalScrollBar(),
            &QScrollBar::valueChanged, [this](int value) {
                update();
            });
}

void OverviewWidget::reloadStrategy() {
    update();
}

void OverviewWidget::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);

    painter.fillRect(0, 0, width(), height(), windowColor());

    auto overview = stg::overview(strategy, width());

    drawElements(painter, overview);
    drawViewportMarker(painter, overview);
    drawCurrentTimeMarker(painter, overview);

    drawBorders(painter);
}

void OverviewWidget::drawBorders(QPainter &painter) const {
    auto borderColor = QColor(Qt::black);
    borderColor.setAlphaF(0.1);

    painter.fillRect(0, 0, width(), 1, borderColor);
    painter.fillRect(0, height() - 1, width(), 1, borderColor);
}

void OverviewWidget::drawCurrentTimeMarker(QPainter &painter, stg::overview &overview) const {
    painter.setBrush(QBrush(ApplicationSettings::currentTimeMarkerColor));
    painter.setPen(Qt::NoPen);

    auto borderColor = QColor(Qt::black);
    borderColor.setAlphaF(0.1);

    painter.drawRect(overview.current_time_position(), 0, 2, height());
    painter.fillRect(overview.current_time_position() - 1, 0, 1, height(), borderColor);
    painter.fillRect(overview.current_time_position() + 2, 0, 1, height(), borderColor);
}

void OverviewWidget::drawViewportMarker(QPainter &painter, stg::overview &overview) const {
    auto viewportMarker = this->makeViewportMarker();

    auto blackColor = QColor(Qt::black);
    blackColor.setAlphaF(0.5);

    auto viewportMarkerPadding = 2;
    if (viewportMarker.origin_x < viewportMarkerPadding) {
        viewportMarker.origin_x = viewportMarkerPadding;
    } else if (viewportMarker.origin_x > width() - viewportMarkerPadding - viewportMarker.width) {
        viewportMarker.origin_x = width() - viewportMarkerPadding - viewportMarker.width;
    }

    auto viewPortMarkerHeight = height() - 2 * viewportMarkerPadding;
    auto viewPortMarkerRadius = viewPortMarkerHeight / 2;

    QPainterPath viewportMarkerPath;
    viewportMarkerPath.addRect(0, 0, width(), height());
    viewportMarkerPath.addRoundedRect(viewportMarker.origin_x,
                                      viewportMarkerPadding,
                                      viewportMarker.width,
                                      viewPortMarkerHeight,
                                      viewPortMarkerRadius,
                                      viewPortMarkerRadius);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillPath(viewportMarkerPath, blackColor);
}

void OverviewWidget::drawElements(QPainter &painter, stg::overview &overview) const {
    for (auto &item : overview.elements()) {
        auto color = ColorUtils::QColorFromStdString(item.color);
        if (color.valueF() < 0.2)
            color.setHsvF(color.hueF(), color.saturationF(), 0.2);

        painter.fillRect(item.origin_x, 0, item.width, height(), color);
    }
}

void OverviewWidget::mouseReleaseEvent(QMouseEvent *event) {
    auto percentage = (float) event->pos().x() / (float) width();
    auto viewportRect = scrollArea->viewportRect();
    auto relativeDistance = percentage * slotboardWidget()->slotsWidget()->height();
    auto topOffset = slotboardWidget()->slotsLayout()->contentsMargins().top();

    int newScrollOffset = relativeDistance - viewportRect.height() / 2 + topOffset;

    scrollArea->setScrollOffset(newScrollOffset);
}

void OverviewWidget::mouseMoveEvent(QMouseEvent *event) {
    mouseReleaseEvent(event);
}

stg::overview::viewport_marker OverviewWidget::makeViewportMarker() const {
    auto overview = stg::overview(strategy, width());
    return overview.viewport_marker_for(slotboardWidget()->slotsWidget()->viewportRect(),
                                        slotboardWidget()->slotsWidget()->contentsRect());
}

SlotBoardWidget *OverviewWidget::slotboardWidget() const {
    return qobject_cast<SlotBoardWidget *>(scrollArea->widget());
}
