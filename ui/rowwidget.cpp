//
// Created by Dmitry Khrykin on 2019-07-26.
//

#include <QApplication>
#include <QPainter>

#include "applicationsettings.h"
#include "colorutils.h"
#include "rowwidget.h"

void RowWidget::paintEvent(QPaintEvent *event) {
    using namespace ApplicationSettings;

    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);

    painter.setBrush(QApplication::palette().color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width(), height()));

    painter.setBrush(_borderColor());
    auto borderRect = QRect(defaultPadding, height() - 1, width() - defaultPadding, 1);
    painter.drawRect(borderRect);
}

bool RowWidget::isHardBorder() const {
    return _isHardBorder;
}

void RowWidget::setIsHardBorder(bool isHardBorder) {
    _isHardBorder = isHardBorder;
    update();
}

QColor RowWidget::borderColor() const {
    return _borderColor();
}

void RowWidget::setBorderColor(const std::function<QColor(void)> &borderColor) {
    _borderColor = borderColor;
    update();
}
