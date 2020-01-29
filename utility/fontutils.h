//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_FONTUTILS_H
#define STRATEGR_FONTUTILS_H

#include <QPainter>
#include <QStringList>
#include <QFontMetrics>
#include "session.h"
#include "applicationsettings.h"
#include "utils.h"
#include "colorutils.h"

namespace FontUtils {
    inline void drawSessionTitle(const stg::session &session,
                                 QPainter &painter,
                                 const QRect &rect) {

        auto name = QString::fromStdString(session.activity->name());
        auto duration = humanTimeForMinutes(session.duration());

        auto textComponents = QStringList{duration, name};
        auto head = textComponents.first() + " ";
        auto tail = textComponents.last();
        auto text = textComponents.join(" ");

        auto fontMetrics = QFontMetrics(painter.font());
        auto wholeSize = fontMetrics.size(Qt::TextSingleLine, text);
        auto headSize = fontMetrics.size(Qt::TextSingleLine, head);
        auto tailSize = fontMetrics.size(Qt::TextSingleLine, tail);

        auto maximumWidth = rect.width();

        if (wholeSize.width() >= maximumWidth) {
            tail = fontMetrics.elidedText(tail,
                                          Qt::ElideMiddle,
                                          maximumWidth - headSize.width());

            text = head + tail;
            wholeSize = fontMetrics.size(Qt::TextSingleLine, text);
        }

        auto origin = QPoint(rect.x() + (rect.width() - wholeSize.width()) / 2,
                             rect.y() + (rect.height() - wholeSize.height()) / 2);

        auto headRect = QRect(origin.x(),
                              origin.y(),
                              headSize.width(),
                              headSize.height());

        auto tailRect = QRect(origin.x() + headSize.width(),
                              origin.y(),
                              tailSize.width(),
                              tailSize.height());

        auto headColor = ColorUtils::overlayWithAlpha(
                QApplication::palette().color(QPalette::WindowText),
                0.5);

        auto tailColor = ColorUtils::safeForegroundColor(
                ColorUtils::QColorFromStdString(session.activity->color())
        );

        painter.setPen(headColor);
        painter.drawText(headRect, head);
        painter.setPen(tailColor);
        painter.drawText(tailRect, tail);

        painter.setPen(Qt::NoPen);
    }

    inline void drawTruncatedText(const QString &text,
                                  QPainter &painter,
                                  const QRect &rect,
                                  Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter) {
        auto fontMetrics = QFontMetrics(painter.font());
        auto maximumWidth = rect.width();

        auto printedText = fontMetrics.elidedText(text,
                                                  Qt::ElideMiddle,
                                                  maximumWidth);

        painter.drawText(rect, alignment, printedText);
    }

    inline int defaultFontPointSize() {
        return QFont().pointSize();
    }

    inline int defaultFontPixelSize() {
        return QFont().pixelSize();
    }
}

#endif //STRATEGR_FONTUTILS_H
