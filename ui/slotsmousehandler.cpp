//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include <cmath>
#include <algorithm>

#include <QMenu>
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>
#include <QPainter>

#include "slotsmousehandler.h"
#include "slotswidget.h"
#include "macoswindow.h"

SlotsMouseHandler::SlotsMouseHandler(SlotsWidget *slotsWidget)
        : QFrame(nullptr),
          slotsWidget(slotsWidget) {
    setMouseTracking(true);
}

void SlotsMouseHandler::mousePressEvent(QMouseEvent *event) {
    mousePressHappened = true;

    auto leftButtonPressed = event->buttons() == Qt::LeftButton;
    auto rightButtonPressed = event->buttons() == Qt::RightButton;
    auto ctrlPressed = event->modifiers() == Qt::CTRL;

    currentSlotIndex = slotIndexForEvent(event);
    clickedSessionIndex = sessionIndexForSlotIndex(currentSlotIndex);

    if (leftButtonPressed && ctrlPressed) {
        initDragSelectedOperation();
    } else if (leftButtonPressed) {
        handleLeftButtonPress(event);
    }
}

void SlotsMouseHandler::handleLeftButtonPress(const QMouseEvent *event) {
    if (isSlotIndexSelected(currentSlotIndex)) {
        operation = Operation::OpenActivities;
        slotsWidget->selectionWidget->setIsClicked(true);
        return;
    }

    if (strategy()->timeSlots()[currentSlotIndex].activity == Strategy::NoActivity) {
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
        strategy()->beginDragging(sessionIndexForSlotIndex(currentSlotIndex));
    } else if (operation == Operation::StretchTop ||
               operation == Operation::StretchBottom) {
        selectStetchingSessions(handleSlotIndex);
        strategy()->beginResizing();
    }

    updateCursor();
}

void SlotsMouseHandler::initDragSelectedOperation() {
    operation = Operation::DragSelect;
    selectSlotAtIndex(currentSlotIndex);
    handleSlotIndex = currentSlotIndex;
}


void SlotsMouseHandler::mouseMoveEvent(QMouseEvent *event) {
    currentSlotIndex = slotIndexForEvent(event);

    if (!mousePressHappened) {
        handleMouseHover(event);
        return;
    }

    if (operation == Operation::DragSelect) {
        if (!isSlotIndexSelected(currentSlotIndex))
            fillSelection(handleSlotIndex, currentSlotIndex);
        return;
    }

    checkForDirectionChange(event);

    auto currentMouseTop = event->pos().y();
    auto delta = getMouseTopDelta(currentMouseTop, previousMouseTop);

    if (delta < slotHeight() / 2 && operation != Operation::Drag) {
        return;
    }

    previousMouseTop = currentMouseTop;

    if (operation == Operation::Drag) {
        handleDragOperation(event);
    }

    if (operation == Operation::StretchTop ||
        operation == Operation::StretchBottom) {
        handleStretchOperation(event);
    }
}

void SlotsMouseHandler::checkForDirectionChange(const QMouseEvent *event) {
    auto previousDirection = direction;
    auto newDirection = determineDirection(event);

    if (newDirection.has_value()) {
        direction = newDirection;
    }

    if (previousDirection.has_value() &&
        direction != previousDirection &&
        operation != Operation::Drag) {
        handleDirectionChange(previousDirection);
    }
}

int SlotsMouseHandler::getMouseTopDelta(int currentMouseTop, int previousMouseTop) {
    auto totalHeight = static_cast<double>(previousMouseTop - firstSlotTopOffset());
    auto closestSlotBoundary = round(totalHeight / slotHeight()) * slotHeight();

    return abs(static_cast<int>(currentMouseTop - closestSlotBoundary));;
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


void SlotsMouseHandler::handleDirectionChange(const std::optional<Direction> &previousDirection) {
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

    strategy()->fillTimeSlots(handleSlotIndex, currentSlotIndex);

    handleSlotIndex = currentSlotIndex;

    selectStetchingSessions(handleSlotIndex);
}


void SlotsMouseHandler::selectStetchingSessions(int sourceIndex) {
    auto firstSelectionIndex = static_cast<int>(*strategy()
            ->activitySessions()
            .sessionIndexForTimeSlotIndex(sourceIndex));

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
        firstSelectionIndex >= strategy()->activitySessions().size() ||
        secondSelectionIndex < 0 ||
        secondSelectionIndex >= strategy()->activitySessions().size() ||
        (strategy()->activitySessions()[firstSelectionIndex].activity == Strategy::NoActivity &&
         strategy()->activitySessions()[secondSelectionIndex].activity == Strategy::NoActivity)) {
        deselectAllSessions();
        resizeBoundarySlotIndex = std::nullopt;
        update();
        reset();
        return;
    }

    auto selectBorderIndex = std::min(firstSelectionIndex, secondSelectionIndex);

    for (int i = 0; i < strategy()->activitySessions().size(); i++) {
        sessionWidgetAtIndex(i)->setSelectBorder(i == selectBorderIndex);
    }

    selectSessionsAtIndices({firstSelectionIndex, secondSelectionIndex});

    auto borderSlot = strategy()->activitySessions()[selectBorderIndex].timeSlots.back();
    resizeBoundarySlotIndex = strategy()->timeSlots().indexOf(borderSlot);

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
        updateCursor();
    }
}

void SlotsMouseHandler::handleDragOperation(QMouseEvent *event) {
    auto distance = currentSlotIndex - handleSlotIndex;

    strategy()->dragActivitySession(clickedSessionIndex, distance);

    handleSlotIndex = currentSlotIndex;
    clickedSessionIndex = sessionIndexForSlotIndex(currentSlotIndex);

    selectSessionAtSlotIndex(currentSlotIndex);
}


void SlotsMouseHandler::mouseReleaseEvent(QMouseEvent *event) {
    if (operation == Operation::OpenActivities) {
        slotsWidget->openActivitiesWindow();
    }

    if (mousePressHappened) {
        reset();
    }
}


int SlotsMouseHandler::sessionIndexForSlotIndex(int slotIndex) {
    return strategy()->activitySessions()
            .sessionIndexForTimeSlotIndex(slotIndex).value_or(-1);
}

std::optional<SlotsMouseHandler::Operation>
SlotsMouseHandler::determineOperationForMouseZone(MouseZone mouseZone) {
    auto currentSlotIsEmpty = strategy()->timeSlots()[currentSlotIndex].activity
                              == Strategy::NoActivity;

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
        default:
            return std::nullopt;
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

void SlotsMouseHandler::updateCursor() {
    auto currentSession = strategy()->activitySessions()[clickedSessionIndex];
    if (currentSession.activity == Strategy::NoActivity) {
        unsetCursor();
        return;
    }

    if (currentMouseZone == MouseZone::DragZone) {
        if (operation == Operation::Drag) {
            setCursor(closedHandCursor());
        } else {
            unsetCursor();
        }
    } else if (currentSlotIndex == 0 &&
               currentMouseZone == MouseZone::TopStretchZone) {
        unsetCursor();
    } else if (currentSlotIndex == strategy()->timeSlots().size() - 1 &&
               currentMouseZone == MouseZone::BottomStretchZone) {
        unsetCursor();
    } else {
        setCursor(resizeVerticalCursor());
    }
}


void SlotsMouseHandler::contextMenuEvent(QContextMenuEvent *event) {
    auto currentSlotIndex = slotIndexForEvent(event);
    auto &currentSlot = strategy()->timeSlots()[currentSlotIndex];

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
        slotIndex >= strategy()->numberOfTimeSlots()) {
        return;
    }

    slotsWidget->selectionWidget->selectAtIndex(slotIndex);
    slotsWidget->clearSelectionAction->setEnabled(true);
}

void SlotsMouseHandler::selectSessionAtSlotIndex(int slotIndex) {
    auto sessionIndex = strategy()
            ->activitySessions()
            .sessionIndexForTimeSlotIndex(slotIndex);

    if (sessionIndex) {
        selectSessionsAtIndices({static_cast<int>(*sessionIndex)});
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

ActivitySessionWidget
*SlotsMouseHandler::sessionWidgetAtIndex(int sessionIndex) {
    auto sessionWidget = slotsWidget
            ->slotsLayout
            ->itemAt(sessionIndex)
            ->widget();

    return qobject_cast<ActivitySessionWidget *>(sessionWidget);
}

int SlotsMouseHandler::firstSlotTopOffset() {
    auto firstItem = slotsWidget->slotsLayout->itemAt(0);
    if (!firstItem || !firstItem->widget()) {
        return 0;
    }

    return firstItem->geometry().top();
}

Strategy *SlotsMouseHandler::strategy() {
    return slotsWidget->strategy;
}

void SlotsMouseHandler::reset() {
    if (operation == Operation::Drag) {
        strategy()->endDragging();
    }

    if (operation == Operation::StretchBottom ||
        operation == Operation::StretchTop) {
        strategy()->endResizing();
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

    unsetCursor();
    deselectAllSessions();

    update();
}

ActivitySessionWidget *SlotsMouseHandler::sessionWidgetAtSlotIndex(int slotIndex) {
    auto sessionIndex = strategy()
            ->activitySessions()
            .sessionIndexForTimeSlotIndex(slotIndex);

    if (!sessionIndex) {
        return nullptr;
    }


    return sessionWidgetAtIndex(*sessionIndex);
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

