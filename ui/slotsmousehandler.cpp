//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include <algorithm>

#include <QMenu>
#include <QScreen>
#include <QPainter>
#include <QTimer>
#include <QScrollBar>
#include <QGraphicsDropShadowEffect>

#include "slotsmousehandler.h"
#include "macoswindow.h"
#include "sessionsmainwidget.h"
#include "slotboardwidget.h"

SlotsMouseHandler::SlotsMouseHandler(SlotsWidget *slotsWidget)
        : QFrame(nullptr),
          slotsWidget(slotsWidget) {
    setMouseTracking(true);

    using namespace std::placeholders;

    handler.on_cursor_change = std::bind(&SlotsMouseHandler::updateCursor, this, _1);
    handler.on_select_sessions = std::bind(&SlotsMouseHandler::selectSessionsAtIndices, this, _1);
    handler.on_resize_boundary_change = std::bind(&SlotsMouseHandler::updateResizeBoundary, this, _1, _2);
    handler.on_start_auto_scroll = std::bind(&SlotsMouseHandler::startAutoScroll, this, _1);
    handler.on_stop_auto_scroll = std::bind(&SlotsMouseHandler::stopAutoScroll, this);
    handler.on_context_menu_event = std::bind(&SlotsMouseHandler::showContextMenu, this, _1, _2, _3);
    handler.on_open_activities = std::bind(&SlotsWidget::openActivitiesWindow, slotsWidget);
    handler.on_draw_dragged_session = [this](int sessionIndex, int firstSlotIndex) {
        emit drawDraggedSession(sessionIndex, firstSlotIndex);
    };
}

int SlotsMouseHandler::slotHeight() {
    return slotsWidget->_slotHeight;
}

stg::selection &SlotsMouseHandler::selection() {
    return slotsWidget->selectionWidget->selection;
}

SlotBoardWidget *SlotsMouseHandler::slotBoard() {
    return qobject_cast<SlotBoardWidget *>(slotsWidget->parent());
}

QScrollArea *SlotsMouseHandler::slotBoardScrollArea() {
    return qobject_cast<SessionsMainWidget *>(slotsWidget
                                                      ->parent() // layout
                                                      ->parent() // slotboard
                                                      ->parent() // scroll area
                                                      ->parent()) // layout
            ->slotBoardScrollArea();
}

QScrollBar *SlotsMouseHandler::verticalScrollBar() {
    return slotBoardScrollArea()->verticalScrollBar();
}

void SlotsMouseHandler::mousePressEvent(QMouseEvent *event) {
    handler.mouse_press(event);
}

void SlotsMouseHandler::mouseMoveEvent(QMouseEvent *event) {
    if (!hasFocus())
        setFocus();

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

void SlotsMouseHandler::startAutoScroll(stg::mouse_handler::scroll_direction direction) {
    int constexpr frameDuration = 1000 / 30;
    auto constexpr pixelsInSecond = 200;

    if (!autoscrollAnimation)
        autoscrollAnimation = new QTimer(this);

    autoscrollAnimation->disconnect();

    connect(autoscrollAnimation, &QTimer::timeout, [=] {
        auto newValue = direction == stg::mouse_handler::scroll_direction::down
                        ? verticalScrollBar()->value() + pixelsInSecond * frameDuration / 1000
                        : verticalScrollBar()->value() - pixelsInSecond * frameDuration / 1000;

        verticalScrollBar()->setValue(newValue);
        handler.auto_scroll_frame(mapFromGlobal(QCursor::pos()));
    });

    autoscrollAnimation->start(frameDuration);
}

void SlotsMouseHandler::stopAutoScroll() {
    if (autoscrollAnimation) {
        autoscrollAnimation->stop();
    }
}

stg::rect SlotsMouseHandler::viewportRect() {
    return stg::rect{0,
                     slotBoardScrollArea()->verticalScrollBar()->value()
                     - slotBoard()->slotsLayout()->contentsMargins().top(),
                     slotBoardScrollArea()->viewport()->width(),
                     slotBoardScrollArea()->viewport()->height()};
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
        case mouse_handler::cursor::drag_copy:
            setCursor(dragCopyCursor());
            break;
    }
}


void SlotsMouseHandler::showContextMenu(const stg::point &position,
                                        int slotIndex,
                                        int /*sessionIndex*/) {
    auto &currentSlot = strategy().time_slots()[slotIndex];

    QMenu menu(slotsWidget);
    menu.addAction(slotsWidget->setActivityAction);
    slotsWidget->setActivityAction->setEnabled(!selection().empty());

    menu.addSeparator();

    if (currentSlot.activity && !selection().empty()) {
        menu.addAction(slotsWidget->deleteActivityAction);
    }

    menu.addAction(slotsWidget->shiftSlotsBelowAction);
    menu.addSeparator();

    menu.addAction(slotsWidget->clearSelectionAction);
    slotsWidget->clearSelectionAction->setEnabled(!selection().empty());

    menu.exec(slotsWidget->mapToGlobal(position));
}

void SlotsMouseHandler::selectSessionsAtIndices(const std::vector<int> &sessionIndices) {
    auto nothingSelected = sessionIndices.empty();

    for (int i = 0; i < slotsWidget->slotsLayout->layout()->count(); i++) {
        auto isSelected = std::find(sessionIndices.begin(),
                                    sessionIndices.end(),
                                    i) != sessionIndices.end();

        auto *sessionWidget = sessionWidgetAtIndex(i);
        if (!sessionWidget)
            continue;

        if (sessionWidget->isSelected() != isSelected) {
            sessionWidget->setIsSelected(isSelected);
        }

        sessionWidget->setDimmed(!isSelected && !nothingSelected);
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

void SlotsMouseHandler::showEvent(QShowEvent *event) {
    setFocus();
}
