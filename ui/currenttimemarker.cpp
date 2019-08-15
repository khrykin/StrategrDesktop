#include "currenttimemarker.h"
#include "slotruler.h"
#include <QPaintEvent>
#include <QPainter>

CurrentTimeMarker::CurrentTimeMarker(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void CurrentTimeMarker::paintEvent(QPaintEvent *event) {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto background = QBrush(ApplicationSettings::currentTimeMarkerColor);
    auto rulerRect = QRect(0, markerSize, geometry().width(), 1);

    painter.fillRect(rulerRect, background);
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(markerSize, markerSize),
                        markerSize,
                        markerSize);
    painter.end();
}

