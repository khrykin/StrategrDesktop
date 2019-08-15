//
// Created by Dmitry Khrykin on 2019-08-15.
//

#include <QPainter>

#include "overviewwidget.h"
#include "colorutils.h"
#include "applicationsettings.h"
#include "overview.h"

OverviewWidget::OverviewWidget(Strategy *strategy, QWidget *parent) :
        strategy(strategy), QWidget(parent) {
    setFixedHeight(ApplicationSettings::overviewHeight);
}

void OverviewWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    update();
}

void OverviewWidget::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);
    painter.fillRect(0, 0, width(), height(), windowColor());

    auto overview = Overview(*strategy);
    auto overviewElements = overview.elementsForWidth(width());

    for (auto &item : overviewElements) {
        auto color = ColorUtils::QColorFromStdString(item.color);
        painter.fillRect(item.originX, 0, item.width, height(), color);

    }

    auto borderColor = QColor(Qt::black);
    borderColor.setAlphaF(0.1);
    painter.fillRect(0, 0, width(), 1, borderColor);
}