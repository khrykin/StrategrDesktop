//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_FONTUTILS_H
#define STRATEGR_FONTUTILS_H

#include <QFontMetrics>
#include <QPainter>
#include <QStringList>

#include <strategr/activity.h>
#include <strategr/session.h>
#include <strategr/time_utils.h>

#include "applicationsettings.h"
#include "colorutils.h"
#include "utils.h"

namespace FontUtils {
    inline void
    drawSessionTitle(const stg::session &session,
                     QPainter &painter,
                     const QRect &rect,
                     QColor durationColor = QColor(255, 255, 255, 0),
                     QColor titleColor = QColor(255, 255, 255, 0)) {

        auto name = QString::fromStdString(session.activity->name());
        auto duration = QString::fromStdString(stg::time_utils::human_string_from_minutes(session.duration()));

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

        auto durationRect = QRect(origin.x(),
                                  origin.y(),
                                  headSize.width(),
                                  headSize.height());

        auto titleRect = QRect(origin.x() + headSize.width(),
                               origin.y(),
                               tailSize.width(),
                               tailSize.height());


        if (durationColor == QColor(255, 255, 255, 0))
            durationColor = ColorUtils::overlayWithAlpha(
                QApplication::palette().color(QPalette::WindowText),
                0.5);

        if (titleColor == QColor(255, 255, 255, 0))
            titleColor = ColorUtils::safeForegroundColor(
                ColorUtils::QColorFromStdString(session.activity->color()));

        auto thickFont = painter.font();
        auto thinFont = painter.font();
        thinFont.setBold(false);

        //        painter.setFont(thinFont);
        painter.setPen(durationColor);
        painter.drawText(durationRect, head);

        //        painter.setFont(thickFont);
        painter.setPen(titleColor);
        painter.drawText(titleRect, tail);

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

#endif//STRATEGR_FONTUTILS_H
