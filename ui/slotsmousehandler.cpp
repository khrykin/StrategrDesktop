//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include <cmath>
#include <algorithm>

#include <QMenu>
#include <QScreen>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScrollBar>

#include "slotsmousehandler.h"
#include "macoswindow.h"
#include "sessionsmainwidget.h"

SlotsMouseHandler::SlotsMouseHandler(SlotsWidget *slotsWidget)
        : QFrame(nullptr),
          slotsWidget(slotsWidget) {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    using namespace std::placeholders;

    handler.on_cursor_change = std::bind(&SlotsMouseHandler::updateCursor, this, _1);
    handler.on_select_sessions = std::bind(&SlotsMouseHandler::selectSessionsAtIndices, this, _1);
    handler.on_resize_boundary_change = std::bind(&SlotsMouseHandler::updateResizeBoundary, this, _1, _2);
    handler.on_open_activities = std::bind(&SlotsWidget::openActivitiesWindow, slotsWidget);

}

int SlotsMouseHandler::slotHeight() {
    return slotsWidget->_slotHeight;
}

stg::selection &SlotsMouseHandler::selection() {
    return slotsWidget->selectionWidget->selection;
}


QScrollBar *SlotsMouseHandler::verticalScrollBar() const {
    return slotBoardScrollArea()->verticalScrollBar();
}

QScrollArea *SlotsMouseHandler::slotBoardScrollArea() const {
    return qobject_cast<SessionsMainWidget *>
            (slotsWidget->parent()->parent()
                     ->parent()->parent())
            ->slotBoardScrollArea();
}

void SlotsMouseHandler::mousePressEvent(QMouseEvent *event) {
    handler.mouse_press(event);
}

void SlotsMouseHandler::mouseMoveEvent(QMouseEvent *event) {
    handler.mouse_move(event);
}

void SlotsMouseHandler::mouseReleaseEvent(QMouseEvent *event) {
    handler.mouse_release(event);
}

void SlotsMouseHandler::leaveEvent(QEvent *) {

}

void SlotsMouseHandler::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
    handler.key_down(event);
}

void SlotsMouseHandler::keyReleaseEvent(QKeyEvent *event) {
    QWidget::keyReleaseEvent(event);
    handler.key_up(event);
}

void SlotsMouseHandler::handleAutoScroll(const QMouseEvent *event) {
//    if (isPerformingAutoscroll) {
//        return;
//    }
//
//    auto topOffsetInViewPort = mapTo(slotBoardScrollArea()->viewport(), event->pos()).y();
//
//    auto topAutoScrollBreakpoint = slotsWidget->slotHeight();
//    auto bottomAutoScrollBreakpoint = slotBoardScrollArea()->height() - slotsWidget->slotHeight();
//
//    auto needsAutoScrollBottom = direction == Direction::Down &&
//                                 topOffsetInViewPort > bottomAutoScrollBreakpoint &&
//                                 topOffsetInViewPort <= slotBoardScrollArea()->height();
//    auto needsAutoScrollTop = direction == Direction::Up &&
//                              topOffsetInViewPort < topAutoScrollBreakpoint &&
//                              topOffsetInViewPort >= 0;
//
//    auto needsAutoScroll = !isPerformingAutoscroll && (needsAutoScrollTop || needsAutoScrollBottom);
//
//    const auto scrollSpeed = 5;
//
//    int scrollLength = 0;
//    int endValue = verticalScrollBar()->value();
//
//    if (needsAutoScrollBottom) {
//        scrollLength = slotsWidget->height() - relativeTop(event);
//        endValue = verticalScrollBar()->maximum();
//    } else if (needsAutoScrollTop) {
//        scrollLength = relativeTop(event);
//        endValue = verticalScrollBar()->minimum();
//    }
//
//    if (needsAutoScroll) {
//        if (!autoscrollAnimation)
//            autoscrollAnimation = new QPropertyAnimation(verticalScrollBar(),
//                                                         "value",
//                                                         this);
//        autoscrollAnimation->disconnect();
//
//        connect(autoscrollAnimation,
//                &QAbstractAnimation::finished,
//                [this]() {
//                    isPerformingAutoscroll = false;
//                });
//
//        auto fakeEvent = *event;
//        connect(autoscrollAnimation,
//                &QVariantAnimation::valueChanged,
//                [this,
//                        &fakeEvent,
//                        topOffsetInViewPort](const QVariant &value) {
//
//                    auto directionMargin = 1;
//                    if (direction == Direction::Up) {
//                        directionMargin *= -1;
//                    }
//
//                    auto topInSlotBoard = value.toInt() + topOffsetInViewPort + directionMargin;
//                    fakeEvent.setLocalPos(QPoint(fakeEvent.pos().x(), topInSlotBoard));
//
//                    mouseMoveEvent(&fakeEvent);
//                });
//
//        if (scrollLength > 0) {
//            autoscrollAnimation->setDuration(scrollSpeed * scrollLength);
//            autoscrollAnimation->setStartValue(verticalScrollBar()->value());
//            autoscrollAnimation->setEndValue(endValue);
//
//            isPerformingAutoscroll = true;
//            autoscrollAnimation->start();
//        } else {
//            resetAutoscrollAnimation();
//        }
//    }
}

void SlotsMouseHandler::updateResizeBoundary(int sessionBeforeBoundaryIndex, int slotBeforeBoundaryIndex) {
    for (int i = 0; i < strategy().sessions().size(); i++) {
        sessionWidgetAtIndex(i)->setSelectBorder(i == sessionBeforeBoundaryIndex);
    }

    emit resizeBoundaryChanged(sessionBeforeBoundaryIndex, slotBeforeBoundaryIndex);
}

void SlotsMouseHandler::updateCursor(stg::mouse_handler::cursor new_cursor) {
    using namespace stg;

    switch (new_cursor) {
        case mouse_handler::cursor::pointer:
            unsetCursor();
            break;
        case mouse_handler::cursor::resize:
            setCursor(resizeVerticalCursor());
            break;
        case mouse_handler::cursor::closed_hand:
            setCursor(closedHandCursor());
            break;
        case mouse_handler::cursor::open_hand:
            setCursor(openHandCursor());
            break;
    }
}


void SlotsMouseHandler::contextMenuEvent(QContextMenuEvent *event) {
//    auto currentSlotIndex = slotIndexForEvent(event);
//    auto &currentSlot = strategy().time_slots()[currentSlotIndex];
//
//    QMenu menu(slotsWidget);
//    menu.addAction(slotsWidget->setActivityAction);
//    slotsWidget->setActivityAction->setEnabled(handler.selection.empty());
//
//    menu.addSeparator();
//
//    if (currentSlot.activity && hasSelection()) {
//        menu.addAction(slotsWidget->deleteActivityAction);
//    }
//
//    menu.addAction(slotsWidget->shiftSlotsBelowAction);
//    menu.addSeparator();
//
//    menu.addAction(slotsWidget->clearSelectionAction);
//    slotsWidget->clearSelectionAction->setEnabled(hasSelection());
//
//
//    menu.exec(event->globalPos());
}

void SlotsMouseHandler::selectSessionsAtIndices(const std::vector<int> &sessionIndices) {
    for (int i = 0; i < slotsWidget->slotsLayout->layout()->count(); i++) {
        auto isSelected = std::find(sessionIndices.begin(),
                                    sessionIndices.end(),
                                    i) != sessionIndices.end();
        setSelectedForSessionIndex(i, isSelected);
    }
}

void SlotsMouseHandler::setSelectedForSessionIndex(int sessionIndex,
                                                   bool isSelected) {
    auto *sessionWidget = sessionWidgetAtIndex(sessionIndex);

    if (sessionWidget && sessionWidget->isSelected() != isSelected) {
        sessionWidget->setIsSelected(isSelected);
    }
}


SessionWidget
*SlotsMouseHandler::sessionWidgetAtIndex(int sessionIndex) {
    auto sessionWidget = slotsWidget
            ->slotsLayout
            ->itemAt(sessionIndex)
            ->widget();

    return qobject_cast<SessionWidget *>(sessionWidget);
}

stg::strategy &SlotsMouseHandler::strategy() {
    return slotsWidget->strategy;
}

void SlotsMouseHandler::resetAutoscrollAnimation() {
    if (isPerformingAutoscroll) {
        isPerformingAutoscroll = false;
        if (autoscrollAnimation) {
            autoscrollAnimation->stop();
        }
    }
}

//SessionWidget *SlotsMouseHandler::sessionWidgetAtSlotIndex(int slotIndex) {
//    auto sessionIndex = strategy().sessions()
//            .session_index_for_time_slot_index(slotIndex);
//
//    if (!sessionIndex) {
//        return nullptr;
//    }
//
//
//    return sessionWidgetAtIndex(sessionIndex);
//}

void SlotsMouseHandler::paintEvent(QPaintEvent *event) {
//    using namespace ApplicationSettings;
//    auto painter = QPainter(this);
//
//    if (slotBeforeResizeBoundaryIndex >= -1) {
//        painter.setRenderHint(QPainter::Antialiasing);
//
//        painter.setPen(QPen(highlightColor(), 2));
//        painter.setBrush(baseColor());
//
//        auto radius = 6;
//        auto topOffset = firstSlotTopOffset()
//                         + (slotBeforeResizeBoundaryIndex + 1) * slotHeight()
//                         - 1 - radius / 2;
//        auto circleLeftRect = QRect(defaultPadding / 2, topOffset, radius, radius);
//        auto circleRightRect = QRect(width() - radius - defaultPadding + defaultPadding / 2,
//                                     topOffset,
//                                     radius,
//                                     radius);
//
//        painter.drawEllipse(circleLeftRect);
//        painter.drawEllipse(circleRightRect);
//    }
}


