//
// Created by Dmitry Khrykin on 2019-08-15.
//

#include <QPainter>
#include <QScrollBar>
#include <QMouseEvent>

#include "overviewwidget.h"
#include "colorutils.h"
#include "applicationsettings.h"
#include "Overview.h"

OverviewWidget::OverviewWidget(Strategy &strategy,
                               QScrollArea *scrollArea,
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

    auto overview = Overview(strategy, width());

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

void OverviewWidget::drawCurrentTimeMarker(QPainter &painter, Overview &overview) const {
    auto currentTimeMarkerPadding = 1;
    auto currentTimeMarkerRadius = height() - 2 * currentTimeMarkerPadding;

    painter.setBrush(QBrush(ApplicationSettings::currentTimeMarkerColor));
    painter.setPen(Qt::NoPen);

    auto borderColor = QColor(Qt::black);
    borderColor.setAlphaF(0.1);

    painter.drawRect(overview.currentTimePosition(), 0, 2, height());
    painter.fillRect(overview.currentTimePosition() - 1, 0, 1, height(), borderColor);
    painter.fillRect(overview.currentTimePosition() + 2, 0, 1, height(), borderColor);
}

void OverviewWidget::drawViewportMarker(QPainter &painter, Overview &overview) const {
    auto viewportMarker = overview.viewportMarkerFor(scrollArea->viewport()->height(),
                                                     scrollArea->widget()->height(),
                                                     scrollArea->verticalScrollBar()->value());

    auto blackColor = QColor(Qt::black);
    blackColor.setAlphaF(0.5);

    auto viewportMarkerPadding = 2;
    if (viewportMarker.originX < viewportMarkerPadding) {
        viewportMarker.originX = viewportMarkerPadding;
    } else if (viewportMarker.originX > width() - viewportMarkerPadding - viewportMarker.width) {
        viewportMarker.originX = width() - viewportMarkerPadding - viewportMarker.width;
    }

    auto viewPortMarkerHeight = height() - 2 * viewportMarkerPadding;
    auto viewPortMarkerRadius = viewPortMarkerHeight / 2;

    QPainterPath viewportMarkerPath;
    viewportMarkerPath.addRect(0, 0, width(), height());
    viewportMarkerPath.addRoundedRect(viewportMarker.originX,
                                      viewportMarkerPadding,
                                      viewportMarker.width,
                                      viewPortMarkerHeight,
                                      viewPortMarkerRadius,
                                      viewPortMarkerRadius);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillPath(viewportMarkerPath, blackColor);
}

void OverviewWidget::drawElements(QPainter &painter, Overview &overview) const {
    for (auto &item : overview.elements()) {
        auto color = ColorUtils::QColorFromStdString(item.color);
        painter.fillRect(item.originX, 0, item.width, height(), color);
    }
}

void OverviewWidget::mouseReleaseEvent(QMouseEvent *event) {
    auto position = static_cast<float>(event->pos().x()) / width();
    scrollArea->verticalScrollBar()->setValue(
            position * scrollArea->widget()->height()
            - scrollArea->viewport()->height() / 2);
}

void OverviewWidget::mouseMoveEvent(QMouseEvent *event) {
    mouseReleaseEvent(event);
}
