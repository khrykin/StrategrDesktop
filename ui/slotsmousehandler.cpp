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

    using namespace std::placeholders;

    handler.on_cursor_change = std::bind(&SlotsMouseHandler::updateCursor, this, _1);
    handler.on_set_selected = std::bind(&SelectionWidget::setIsClicked, slotsWidget->selectionWidget, _1);
}

void SlotsMouseHandler::mousePressEvent(QMouseEvent *event) {
    handler.mouse_press(event);
//    mousePressHappened = true;
//
//    auto leftButtonPressed = mouse_event->buttons() == Qt::LeftButton;
//    auto rightButtonPressed = mouse_event->buttons() == Qt::RightButton;
    auto ctrlPressed = event->modifiers() == Qt::CTRL;
//
//    currentSlotIndex = slotIndexForEvent(mouse_event);
//    clickedSessionIndex = sessionIndexForSlotIndex(currentSlotIndex);
//
//    if (leftButtonPressed && ctrlPressed) {
//        initDragSelectedOperation();
//    } else if (leftButtonPressed) {
//        handleLeftButtonPress(mouse_event);
//    }
}

void SlotsMouseHandler::handleLeftButtonPress(const QMouseEvent *event) {
    if (isSlotIndexSelected(currentSlotIndex)) {
        operation = Operation::OpenActivities;
        slotsWidget->selectionWidget->setIsClicked(true);
        return;
    }

    if (strategy().time_slots()[currentSlotIndex].activity == stg::strategy::no_activity) {
        initDragSelectedOperation();
        return;
    }

    mousePressHappened = true;

    slotsWidget->deselectAllSlots();

    currentMouseZone = determineMouseZone(event, clickedSessionIndex);
    operation = determineOperationForMouseZone(*currentMouseZone);

    previousMouseTop = event->pos().y();

    handleSlotIndex = currentSlotIndex;

    if (operation == Operation::Drag) {
        selectSessionAtSlotIndex(currentSlotIndex);
        strategy().begin_dragging(sessionIndexForSlotIndex(currentSlotIndex));
    } else if (operation == Operation::StretchTop ||
               operation == Operation::StretchBottom) {
        selectStetchingSessions(handleSlotIndex);
        strategy().begin_resizing();
    }

//    updateCursor(pointer);
}

void SlotsMouseHandler::initDragSelectedOperation() {
    operation = Operation::DragSelect;
    selectSlotAtIndex(currentSlotIndex);
    handleSlotIndex = currentSlotIndex;
}

void SlotsMouseHandler::mouseMoveEvent(QMouseEvent *event) {
    handler.mouse_move(event);
//    currentSlotIndex = slotIndexForEvent(event);
//
//    if (!mousePressHappened) {
//        handleMouseHover(event);
//        return;
//    }
//
//    checkForDirectionChange(event);
//
//    auto currentMouseTop = event->pos().y();
//    auto delta = getMouseTopDelta(currentMouseTop, previousMouseTop);
//
//    if (delta < slotHeight() / 2 && operation_type != Operation::Drag) {
//        return;
//    }
//
//    previousMouseTop = currentMouseTop;
//
//    handleAutoScroll(event);
//
//    if (operation_type == Operation::DragSelect) {
//        if (!isSlotIndexSelected(currentSlotIndex))
//            fillSelection(handleSlotIndex, currentSlotIndex);
//        return;
//    }
//
//    if (operation_type == Operation::Drag) {
//        handleDragOperation(event);
//    }
//
//    if (operation_type == Operation::StretchTop ||
//        operation_type == Operation::StretchBottom) {
//        handleStretchOperation(event);
//    }
}

void SlotsMouseHandler::handleAutoScroll(const QMouseEvent *event) {
    if (isPerformingAutoscroll) {
        return;
    }

    auto topOffsetInViewPort = mapTo(slotBoardScrollArea()->viewport(), event->pos()).y();

    auto topAutoScrollBreakpoint = slotsWidget->slotHeight();
    auto bottomAutoScrollBreakpoint = slotBoardScrollArea()->height() - slotsWidget->slotHeight();

    auto needsAutoScrollBottom = direction == Direction::Down &&
                                 topOffsetInViewPort > bottomAutoScrollBreakpoint &&
                                 topOffsetInViewPort <= slotBoardScrollArea()->height();
    auto needsAutoScrollTop = direction == Direction::Up &&
                              topOffsetInViewPort < topAutoScrollBreakpoint &&
                              topOffsetInViewPort >= 0;

    auto needsAutoScroll = !isPerformingAutoscroll && (needsAutoScrollTop || needsAutoScrollBottom);

    const auto scrollSpeed = 5;

    int scrollLength = 0;
    int endValue = verticalScrollBar()->value();

    if (needsAutoScrollBottom) {
        scrollLength = slotsWidget->height() - relativeTop(event);
        endValue = verticalScrollBar()->maximum();
    } else if (needsAutoScrollTop) {
        scrollLength = relativeTop(event);
        endValue = verticalScrollBar()->minimum();
    }

    if (needsAutoScroll) {
        if (!autoscrollAnimation)
            autoscrollAnimation = new QPropertyAnimation(verticalScrollBar(),
                                                         "value",
                                                         this);
        autoscrollAnimation->disconnect();

        connect(autoscrollAnimation,
                &QAbstractAnimation::finished,
                [this]() {
                    isPerformingAutoscroll = false;
                });

        auto fakeEvent = *event;
        connect(autoscrollAnimation,
                &QVariantAnimation::valueChanged,
                [this,
                        &fakeEvent,
                        topOffsetInViewPort](const QVariant &value) {

                    auto directionMargin = 1;
                    if (direction == Direction::Up) {
                        directionMargin *= -1;
                    }

                    auto topInSlotBoard = value.toInt() + topOffsetInViewPort + directionMargin;
                    fakeEvent.setLocalPos(QPoint(fakeEvent.pos().x(), topInSlotBoard));

                    mouseMoveEvent(&fakeEvent);
                });

        if (scrollLength > 0) {
            autoscrollAnimation->setDuration(scrollSpeed * scrollLength);
            autoscrollAnimation->setStartValue(verticalScrollBar()->value());
            autoscrollAnimation->setEndValue(endValue);

            isPerformingAutoscroll = true;
            autoscrollAnimation->start();
        } else {
            resetAutoscrollAnimation();
        }
    }
}

QScrollBar *
SlotsMouseHandler::verticalScrollBar() const {
    return slotBoardScrollArea()->verticalScrollBar();
}

QScrollArea *SlotsMouseHandler::slotBoardScrollArea() const {
    return qobject_cast<SessionsMainWidget *>
            (slotsWidget->parent()->parent()
                     ->parent()->parent())
            ->slotBoardScrollArea();
}

bool SlotsMouseHandler::checkForDirectionChange(const QMouseEvent *event) {
    auto previousDirection = direction;
    auto newDirection = determineDirection(event);

    if (newDirection.has_value()) {
        direction = newDirection;
    }

    if (previousDirection.has_value() &&
        direction != previousDirection) {

        if (operation != Operation::Drag)
            handleResizeDirectionChange(previousDirection);

        if (isPerformingAutoscroll) {
            auto currentMouseTop = event->pos().y();
            auto delta = getMouseTopDelta(currentMouseTop, previousMouseTop);
            if (delta > 10) {
                resetAutoscrollAnimation();
                handleAutoScroll(event);
            }
        }

        return true;
    }

    return false;
}

int SlotsMouseHandler::getMouseTopDelta(int currentMouseTop, int previousMouseTop) {
    auto totalHeight = static_cast<double>(previousMouseTop - firstSlotTopOffset());
    auto closestSlotBoundary = round(totalHeight / slotHeight()) * slotHeight();

    return std::abs(static_cast<int>(currentMouseTop - closestSlotBoundary));;
}


std::optional<SlotsMouseHandler::Direction>
SlotsMouseHandler::determineDirection(const QMouseEvent *event) {
    auto currentMouseTop = event->pos().y();

    std::optional<Direction> direction;
    if (currentMouseTop < previousMouseTop) {
        direction = Direction::Up;
    } else if (currentMouseTop > previousMouseTop) {
        direction = Direction::Down;
    } else {
        direction = std::nullopt;
    }

    return direction;
}


void SlotsMouseHandler::handleResizeDirectionChange(const std::optional<Direction> &previousDirection) {
    if (!resizeBoundarySlotIndex.has_value()) {
        return;
    }

    if (operation == Operation::StretchTop &&
        previousDirection == Direction::Down) {
        if (handleSlotIndex != *resizeBoundarySlotIndex + 1)
            handleSlotIndex = *resizeBoundarySlotIndex + 1;
    }

    if (operation == Operation::StretchBottom &&
        previousDirection == Direction::Up) {
        if (handleSlotIndex != *resizeBoundarySlotIndex)
            handleSlotIndex = *resizeBoundarySlotIndex;
    }
}


void SlotsMouseHandler::handleStretchOperation(QMouseEvent *event) {
    if (!resizeBoundarySlotIndex.has_value()) {
        return;
    }

    if (operation == Operation::StretchTop &&
        direction == Direction::Down) {
        if (handleSlotIndex != *resizeBoundarySlotIndex)
            handleSlotIndex = *resizeBoundarySlotIndex;
    }

    if (operation == Operation::StretchBottom &&
        direction == Direction::Up) {
        if (handleSlotIndex != *resizeBoundarySlotIndex + 1)
            handleSlotIndex = *resizeBoundarySlotIndex + 1;
    }

    strategy().fill_time_slots(handleSlotIndex, currentSlotIndex);

    handleSlotIndex = currentSlotIndex;

    selectStetchingSessions(handleSlotIndex);
}


void SlotsMouseHandler::selectStetchingSessions(int sourceIndex) {
    auto firstSelectionIndex = static_cast<int>(
            strategy().sessions()
                    .session_index_for_time_slot_index(sourceIndex)
    );

    if (operation == Operation::StretchTop &&
        direction == Direction::Down) {
        firstSelectionIndex++;
    }

    if (operation == Operation::StretchBottom &&
        direction == Direction::Up) {
        firstSelectionIndex--;
    }

    int secondSelectionIndex = firstSelectionIndex + 1;

    if (operation == Operation::StretchTop) {
        secondSelectionIndex = firstSelectionIndex - 1;
    }

    if (firstSelectionIndex < 0 ||
        firstSelectionIndex >= strategy().sessions().size() ||
        secondSelectionIndex < 0 ||
        secondSelectionIndex >= strategy().sessions().size() ||
        (strategy().sessions()[firstSelectionIndex].activity == stg::strategy::no_activity &&
         strategy().sessions()[secondSelectionIndex].activity == stg::strategy::no_activity)) {
        deselectAllSessions();
        resizeBoundarySlotIndex = std::nullopt;
        update();
        reset();
        return;
    }

    auto selectBorderIndex = std::min(firstSelectionIndex, secondSelectionIndex);

    for (int i = 0; i < strategy().sessions().size(); i++) {
        sessionWidgetAtIndex(i)->setSelectBorder(i == selectBorderIndex);
    }

    selectSessionsAtIndices({firstSelectionIndex, secondSelectionIndex});

    auto borderSlot = strategy().sessions()[selectBorderIndex].time_slots.back();
    resizeBoundarySlotIndex = strategy().time_slots().index_of(borderSlot);

    update();
}

void SlotsMouseHandler::handleMouseHover(const QMouseEvent *event) {
    clickedSessionIndex = sessionIndexForSlotIndex(currentSlotIndex);

    if (clickedSessionIndex < 0) {
        return;
    }

    auto mouseZone = determineMouseZone(event, clickedSessionIndex);

    if (mouseZone != currentMouseZone) {
        currentMouseZone = mouseZone;
//        updateCursor(pointer);
    }
}

void SlotsMouseHandler::handleDragOperation(QMouseEvent *event) {
    auto distance = currentSlotIndex - handleSlotIndex;

    strategy().drag_activity_session(clickedSessionIndex, distance);

    handleSlotIndex = currentSlotIndex;
    clickedSessionIndex = sessionIndexForSlotIndex(currentSlotIndex);

    selectSessionAtSlotIndex(currentSlotIndex);
}


void SlotsMouseHandler::mouseReleaseEvent(QMouseEvent *event) {
    handler.mouse_release(event);
//    if (operation_type == Operation::OpenActivities) {
//        slotsWidget->openActivitiesWindow();
//    }
//
//    if (mousePressHappened) {
//        teardown();
//    }
}


int SlotsMouseHandler::sessionIndexForSlotIndex(int slotIndex) {
    return strategy().sessions()
            .session_index_for_time_slot_index(slotIndex);
}

std::optional<SlotsMouseHandler::Operation>
SlotsMouseHandler::determineOperationForMouseZone(MouseZone mouseZone) {
    auto currentSlotIsEmpty
            = strategy().time_slots()[currentSlotIndex]
                      .activity == stg::strategy::no_activity;

    switch (mouseZone) {
        case MouseZone::DragZone:
            if (currentSlotIsEmpty) {
                return std::nullopt;
            }

            return Operation::Drag;
        case MouseZone::TopStretchZone:
            return Operation::StretchTop;
        case MouseZone::BottomStretchZone:
            return Operation::StretchBottom;
    }
}


SlotsMouseHandler::MouseZone
SlotsMouseHandler::determineMouseZone(const QMouseEvent *event, int sessionIndex) {
    auto *currentSessionWidget = sessionWidgetAtIndex(sessionIndex);

    auto topStretchZone = StretchZonePosition{currentSessionWidget->geometry().top()};
    auto bottomStretchZone = StretchZonePosition{currentSessionWidget->geometry().top()
                                                 + currentSessionWidget->height()
                                                 - stretchZoneHeight};

    if (event->pos().y() >= topStretchZone.top() &&
        event->pos().y() <= topStretchZone.bottom()) {
        return MouseZone::TopStretchZone;
    } else if (event->pos().y() >= bottomStretchZone.top() &&
               event->pos().y() <= bottomStretchZone.bottom()) {
        return MouseZone::BottomStretchZone;
    } else {
        return MouseZone::DragZone;
    }
}

void SlotsMouseHandler::updateCursor(stg::mousehandler::cursor new_cursor) {
    using namespace stg;

    switch (new_cursor) {
        case mousehandler::cursor::pointer:
            unsetCursor();
            break;
        case mousehandler::cursor::resize:
            setCursor(resizeVerticalCursor());
            break;
        case mousehandler::cursor::closed_hand:
            setCursor(closedHandCursor());
            break;
        case mousehandler::cursor::open_hand:
            setCursor(openHandCursor());
            break;
    }
}


void SlotsMouseHandler::contextMenuEvent(QContextMenuEvent *event) {
    auto currentSlotIndex = slotIndexForEvent(event);
    auto &currentSlot = strategy().time_slots()[currentSlotIndex];

    QMenu menu(slotsWidget);
    menu.addAction(slotsWidget->setActivityAction);
    slotsWidget->setActivityAction->setEnabled(hasSelection());

    menu.addSeparator();

    if (currentSlot.activity && hasSelection()) {
        menu.addAction(slotsWidget->deleteActivityAction);
    }

    menu.addAction(slotsWidget->shiftSlotsBelowAction);
    menu.addSeparator();

    menu.addAction(slotsWidget->clearSelectionAction);
    slotsWidget->clearSelectionAction->setEnabled(hasSelection());


    menu.exec(event->globalPos());
}

bool SlotsMouseHandler::hasSelection() {
    return !slotsWidget->selectionWidget->selection().empty();
}

void SlotsMouseHandler::selectSlotAtIndex(int slotIndex) {
    if (slotIndex < 0 ||
        slotIndex >= strategy().number_of_time_slots()) {
        return;
    }

    slotsWidget->selectionWidget->selectAtIndex(slotIndex);
    slotsWidget->clearSelectionAction->setEnabled(true);
}

void SlotsMouseHandler::selectSessionAtSlotIndex(int slotIndex) {
    auto sessionIndex = strategy().sessions()
            .session_index_for_time_slot_index(slotIndex);

    if (sessionIndex) {
        selectSessionsAtIndices({static_cast<int>(sessionIndex)});
    }
}

void SlotsMouseHandler::selectSessionsAtIndices(const std::vector<int> &sessionIndices) {
    for (int i = 0; i < slotsWidget->slotsLayout->layout()->count(); i++) {
        auto isSelected = std::find(sessionIndices.begin(),
                                    sessionIndices.end(),
                                    i) != sessionIndices.end();
        setSelectedForSessionIndex(i, isSelected);
    }
}


void SlotsMouseHandler::deselectAllSessions() {
    for (int i = 0; i < slotsWidget->slotsLayout->count(); i++) {
        setSelectedForSessionIndex(i, false);
    }
}

void SlotsMouseHandler::setSelectedForSessionIndex(int sessionIndex,
                                                   bool isSelected) {
    auto *sessionWidget = sessionWidgetAtIndex(sessionIndex);

    if (sessionWidget && sessionWidget->isSelected() != isSelected) {
        sessionWidget->setIsSelected(isSelected);
    }
}

int SlotsMouseHandler::slotHeight() {
    return slotsWidget->_slotHeight;
}

SessionWidget
*SlotsMouseHandler::sessionWidgetAtIndex(int sessionIndex) {
    auto sessionWidget = slotsWidget
            ->slotsLayout
            ->itemAt(sessionIndex)
            ->widget();

    return qobject_cast<SessionWidget *>(sessionWidget);
}

int SlotsMouseHandler::firstSlotTopOffset() {
    auto firstItem = slotsWidget->slotsLayout->itemAt(0);
    if (!firstItem || !firstItem->widget()) {
        return 0;
    }

    return firstItem->geometry().top();
}

stg::strategy &SlotsMouseHandler::strategy() {
    return slotsWidget->strategy;
}

void SlotsMouseHandler::reset() {
    if (operation == Operation::Drag) {
        strategy().end_dragging();
    }

    if (operation == Operation::StretchBottom ||
        operation == Operation::StretchTop) {
        strategy().end_resizing();
    }

    mousePressHappened = false;
    currentMouseZone = std::nullopt;
    operation = std::nullopt;
    direction = std::nullopt;
    resizeBoundarySlotIndex = std::nullopt;

    currentSlotIndex = -1;
    clickedSessionIndex = -1;
    handleSlotIndex = -1;

    previousMouseTop = 0;

    resetAutoscrollAnimation();

    unsetCursor();
    deselectAllSessions();

    update();
}

void SlotsMouseHandler::resetAutoscrollAnimation() {
    if (isPerformingAutoscroll) {
        isPerformingAutoscroll = false;
        if (autoscrollAnimation) {
            autoscrollAnimation->stop();
        }
    }
}

SessionWidget *SlotsMouseHandler::sessionWidgetAtSlotIndex(int slotIndex) {
    auto sessionIndex = strategy().sessions()
            .session_index_for_time_slot_index(slotIndex);

    if (!sessionIndex) {
        return nullptr;
    }


    return sessionWidgetAtIndex(sessionIndex);
}

void SlotsMouseHandler::paintEvent(QPaintEvent *event) {
    using namespace ApplicationSettings;
    auto painter = QPainter(this);

    if (resizeBoundarySlotIndex.has_value()) {
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(highlightColor(), 2));
        painter.setBrush(baseColor());

        auto radius = 6;
        auto topOffset = firstSlotTopOffset()
                         + (*resizeBoundarySlotIndex + 1) * slotHeight()
                         - 1 - radius / 2;
        auto circleLeftRect = QRect(defaultPadding / 2, topOffset, radius, radius);
        auto circleRightRect = QRect(width() - radius - defaultPadding + defaultPadding / 2,
                                     topOffset,
                                     radius,
                                     radius);

        painter.drawEllipse(circleLeftRect);
        painter.drawEllipse(circleRightRect);
    }
}

bool SlotsMouseHandler::isSlotIndexSelected(int slotIndex) {
    return slotsWidget->selectionWidget->isSlotIndexSelected(slotIndex);
}

void SlotsMouseHandler::fillSelection(int fromIndex, int toIndex) {
    slotsWidget->selectionWidget->fillSelected(fromIndex, toIndex);
}

void SlotsMouseHandler::leaveEvent(QEvent *) {

}

std::ostream &operator<<(std::ostream &os, SlotsMouseHandler::Direction direction) {
    switch (direction) {
        case SlotsMouseHandler::Direction::Up:
            os << "\u2191";
            break;
        case SlotsMouseHandler::Direction::Down:
            os << "\u2193";
            break;
    }

    return os;
}

