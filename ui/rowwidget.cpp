//
// Created by Dmitry Khrykin on 2019-07-26.
//

#include <QPainter>
#include <QApplication>

#include "rowwidget.h"
#include "colorutils.h"
#include "applicationsettings.h"

void RowWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);

    painter.setBrush(QApplication::palette().color(QPalette::Base));
    painter.drawRect(QRect(0, 0, width(), height()));

    const auto borderColor = isHardBorder()
                             ? hardBorderColor()
                             : RowWidget::borderColor();

    painter.setBrush(borderColor);
    auto borderRect = QRect(0, height() - 1, width(), 1);
    painter.drawRect(borderRect);
}

bool RowWidget::isHardBorder() const {
    return _isHardBorder;
}

void RowWidget::setIsHardBorder(bool isHardBorder) {
    _isHardBorder = isHardBorder;
    update();
}
