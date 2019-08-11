//
// Created by Dmitry Khrykin on 2019-08-05.
//

#include <QPainter>

#include "separator.h"
#include "colorprovider.h"

Separator::Separator(QWidget *parent) : QWidget(parent) {
    setFixedHeight(1);
}

void Separator::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);
    auto color = ColorProvider::textColor();
    color.setAlphaF(0.1);
    painter.fillRect(QRect(0, 0, width(), height()), color);
}
