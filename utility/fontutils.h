//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_FONTUTILS_H
#define STRATEGR_FONTUTILS_H

#include <QPainter>
#include <QStringList>
#include <QFontMetrics>

namespace FontUtils {
    inline void drawTruncatedTitleFromComponents(const QStringList &textComponents,
                                                 QPainter &painter,
                                                 QRect rect,
                                                 const QColor &headColor,
                                                 const QColor &tailColor) {
        auto head = textComponents.first() + " ";
        auto tail = textComponents.last();
        auto text = textComponents.join(" ");

        auto fontMetrics = QFontMetrics(painter.font());
        auto wholeSize = fontMetrics.size(Qt::TextSingleLine, text);
        auto headSize = fontMetrics.size(Qt::TextSingleLine, head);

        auto maximumWidth = rect.width() - 8;

        if (wholeSize.width() >= maximumWidth) {
            tail = fontMetrics.elidedText(tail,
                                          Qt::ElideRight,
                                          maximumWidth - headSize.width());

            text = head + tail;
            wholeSize = fontMetrics.size(Qt::TextSingleLine, text);
        }


        auto origin = QPoint((rect.width() - wholeSize.width()) / 2,
                             rect.height() / 2 + static_cast<int>(wholeSize.height() * 0.3));

        painter.setPen(headColor);
        painter.drawText(origin.x(), origin.y(), head);
        painter.setPen(tailColor);
        painter.drawText(origin.x() + headSize.width(), origin.y(), tail);

        painter.setPen(Qt::NoPen);
    }
}

#endif //STRATEGR_FONTUTILS_H
