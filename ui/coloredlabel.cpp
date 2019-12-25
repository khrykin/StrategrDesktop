//
// Created by Dmitry Khrykin on 2019-07-26.
//

#include <QPainter>
#include <QFontMetrics>
#include <utility>

#include "coloredlabel.h"
#include "fontutils.h"

ColoredLabel::ColoredLabel(QString text, QWidget *parent)
        : _text(std::move(text)),
          QWidget(parent) {
}

const QColor &ColoredLabel::color() const {
    return _color;
}

void ColoredLabel::setColor(const QColor &color) {
    _color = color;
    update();
}

const QString &ColoredLabel::text() const {
    return _text;
}

void ColoredLabel::setText(const QString &text) {
    _text = text;
    updateGeometry();
    update();
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
        auto textRect = QRect(0, 0, width(), height());
        FontUtils::drawTruncatedText(_text, painter, textRect, alignment());
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
    auto metrics = QFontMetrics(font());

    auto lines = _text.split("\n");
    auto maxLineLength = 0;
    auto totalHeight = 0;

    for (auto &line : lines) {
        auto length = metrics.horizontalAdvance(line);
        if (length > maxLineLength) {
            maxLineLength = length;
        }

        totalHeight += metrics.boundingRect(line).height();
    }

    return QSize(maxLineLength + 1,
                 totalHeight + (lines.count() - 1) * metrics.lineSpacing());
}


