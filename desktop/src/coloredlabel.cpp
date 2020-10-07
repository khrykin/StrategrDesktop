//
// Created by Dmitry Khrykin on 2019-07-26.
//

#include <QFontMetrics>
#include <QPainter>
#include <utility>

#include "coloredlabel.h"
#include "fontutils.h"

ColoredLabel::ColoredLabel(QString text, QWidget *parent)
    : _text(std::move(text)),
      QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

const QColor &ColoredLabel::color() const {
    return _color;
}

const QString &ColoredLabel::text() const {
    return _text;
}

void ColoredLabel::setText(const QString &text) {
    _text = text;
    updateGeometry();
}

const Qt::Alignment &ColoredLabel::alignment() const {
    return _alignment;
}

void ColoredLabel::setAlignment(const Qt::Alignment &alignment) {
    _alignment = alignment;
    update();
}

void ColoredLabel::paintEvent(QPaintEvent *event) {
    auto painter = QPainter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(dynamicColor());
    painter.setFont(font());

    if (customRenderer) {
        customRenderer(&painter, _text);
    } else {
        auto textRect = contentsRect();
        auto printedText = _elideMode == Qt::ElideNone
                               ? _text
                               : fontMetrics().elidedText(_text, Qt::ElideMiddle, textRect.width());

        painter.drawText(textRect, _alignment, printedText);
    }
}

void ColoredLabel::setBold(bool isBold) {
    auto newFont = QFont(font());
    newFont.setBold(isBold);

    setFont(newFont);
    updateGeometry();
}

void ColoredLabel::setFontHeight(int fontHeight) {
    auto newFont = QFont(font());
    newFont.setPixelSize(fontHeight);

    setFont(newFont);
    updateGeometry();
}


void ColoredLabel::setDynamicColor(const std::function<QColor()> &newColorGetter) {
    colorGetter = newColorGetter;
    update();
}

QColor ColoredLabel::dynamicColor() {
    if (colorGetter) {
        return colorGetter();
    }

    return color();
}

QSize ColoredLabel::sizeHint() const {
    auto lines = _text.split("\n");

    auto maxLineLength = 0;
    auto totalHeight = 0;

    for (auto &line : lines) {
        auto length = fontMetrics().horizontalAdvance(line);
        if (length > maxLineLength) {
            maxLineLength = length;
        }

        totalHeight += fontMetrics().boundingRect(line).height();
    }

    return QSize(maxLineLength,
                 totalHeight + (lines.count() - 1) * fontMetrics().lineSpacing());
}

Qt::TextElideMode ColoredLabel::elideMode() const {
    return _elideMode;
}

void ColoredLabel::setElideMode(Qt::TextElideMode elideMode) {
    _elideMode = elideMode;

    update();
}
