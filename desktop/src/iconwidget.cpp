//
// Created by Dmitry Khrykin on 2020-02-08.
//

#include <QPainter>
#include <QStyleOption>
#include <utility>

#include "iconwidget.h"

IconWidget::IconWidget(QString code, QWidget *parent)
    : QWidget(parent), _code(std::move(code)) {}

void IconWidget::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);
    auto iconRect = QRect(0, 0, width(), height());

    auto font = QFont("FontAwesome");
    font.setPixelSize(14);

    auto color = _color;
    if (isPressed)
        color.setAlphaF(0.6);

    painter.setFont(font);
    painter.setPen(color);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(iconRect, Qt::AlignCenter, _code);
}

const QString &IconWidget::code() const {
    return _code;
}

void IconWidget::setCode(const QString &code) {
    _code = code;
    update();
}

void IconWidget::setColor(const QColor &color) {
    _color = color;
    update();
}

void IconWidget::mousePressEvent(QMouseEvent *) {
    isPressed = true;
    update();
}

void IconWidget::mouseReleaseEvent(QMouseEvent *) {
    isPressed = false;
    update();

    emit clicked();
}

QString IconWidget::backCode() {
    return QStringLiteral(u"\uf053");
}

QString IconWidget::addCode() {
    return QStringLiteral(u"\uf067");
}
