//
// Created by Dmitry Khrykin on 25.05.2020.
//

#include "slotboardscrollarea.h"

#include <iostream>

#include <QDebug>
#include <QResizeEvent>
#include <QScrollBar>

void SlotboardScrollArea::setViewportTopOffset(int top) {
    viewportTopOffset = top;

    if (widget())
        widget()->setContentsMargins(0, top, 0, 0);

    fixScrollBar();
}

void SlotboardScrollArea::resizeEvent(QResizeEvent *event) {
    QScrollArea::resizeEvent(event);

    fixScrollBar();
}

void SlotboardScrollArea::wheelEvent(QWheelEvent *event) {
    QScrollArea::wheelEvent(event);

    fixScrollBar();
}

void SlotboardScrollArea::fixScrollBar() {
    auto oldGeometry = verticalScrollBar()->geometry();

    verticalScrollBar()->setGeometry(oldGeometry.x(),
                                     viewportTopOffset,
                                     oldGeometry.width(),
                                     viewport()->height() - viewportTopOffset);
}

QRect SlotboardScrollArea::viewportRectRelativeToContent() {
    return QRect(viewport()->geometry().x(),
                 verticalScrollBar()->value(),
                 viewport()->width(),
                 viewport()->height() - viewportTopOffset);
}

QRect SlotboardScrollArea::viewportRect() {
    return QRect(viewport()->geometry().x(),
                 viewportTopOffset,
                 viewport()->width(),
                 viewport()->height() - viewportTopOffset);
}

void SlotboardScrollArea::setScrollOffset(int topOffset) {
    verticalScrollBar()->setValue(topOffset);

    fixScrollBar();
}
