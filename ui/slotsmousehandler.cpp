//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include <cmath>

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
    auto leftButtonPressed = event->buttons() == Qt::LeftButton;
    auto rightButtonPressed = event->buttons() == Qt::RightButton;
    auto ctrlPressed = event->modifiers() == Qt::CTRL;

    if (leftButtonPressed) {
        handleLeftButtonPress(event);
    }


//
//    if ((leftButtonPressed && ctrlPressed)
//        || (rightButtonPressed && !hasSelection())) {
//        // Slot selection event
//        selectSlotAtIndex(pulledFrom);
//    } else if (leftButtonPressed) {
//        // Slot pulling event
//        isPulling = true;
//        selectSessionAtSlotIndex(pulledFrom);
//        slotsWidget->deselectAllSlots();
//    }
}

void SlotsMouseHandler::handleLeftButtonPress(const QMouseEvent *event) {
    currentSlotIndex = slotIndexForEvent(event);

    mousePressHappened = true;

    updateDraggedSession(currentSlotIndex);

    currentMouseZone = determineMouseZone(event, currentSessionIndex);

    previousMouseTop = event->pos().y();

    operation = determineOperationForMouseZone(*currentMouseZone);

    if (operation == Operation::Drag) {
        dragHandleSlotIndex = currentSlotIndex;
        selectSessionAtSlotIndex(currentSlotIndex);
        strategy()->beginDragging(sessionIndexForSlotIndex(currentSlotIndex));
    }

    updateCursor();
}

void SlotsMouseHandler::updateDraggedSession(int slotIndex) {
    currentSessionIndex = sessionIndexForSlotIndex(slotIndex);
    draggedSession = strategy()->activitySessions()[currentSessionIndex];
}


void SlotsMouseHandler::mouseMoveEvent(QMouseEvent *event) {
    currentSlotIndex = slotIndexForEvent(event);

    if (!mousePressHappened) {
        handleMouseHover(event);
        return;
    }

    auto previousDirection = direction;

    auto currentMouseTop = event->pos().y();

    if (std::abs(currentMouseTop - previousMouseTop) < slotHeight() / 2 &&
        operation != Operation::Drag) {
        return;
    }

    auto newDirection = determineDirection(event);

    previousMouseTop = currentMouseTop;

    if (!newDirection) {
        return;
    }

    direction = newDirection;

    if (previousDirection.has_value() &&
        direction != previousDirection &&
        operation != Operation::Drag) {
        // TODO: Direction change!
        qDebug() << "\nDirection change!";
        qDebug() << "------------------------------------\n";

        qDebug() << "previousDirection" << static_cast<int>(*previousDirection);
        qDebug() << "direction" << static_cast<int>(*direction);

//        if (operation == Operation::StretchTop) {
//            operation = Operation::StretchBottom;
//        } else if (operation == Operation::StretchBottom) {
//            operation = Operation::StretchTop;
//        }
    }

    previousDirection = direction;

    if (operation == Operation::Drag) {
        handleDragOperation(event);
    }

    if (operation == Operation::StretchTop ||
        operation == Operation::StretchBottom) {
        handleResizeOperation();
    }




//    if (isPulling) {
//        auto slotIndex = slotIndexForEvent(event);
//        if (pulledTo != slotIndex) {
//            pulledTo = slotIndex;
//            fillSlots(pulledFrom, pulledTo);
//            selectSessionAtSlotIndex(pulledTo);
//        }
//    }
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

void SlotsMouseHandler::handleResizeOperation() {
    int sourceIndex;

//    qDebug() << "\n------------------------------------\n";
//    qDebug() << "operation" << static_cast<int>(*operation);
//    qDebug() << "direction" << (static_cast<int>(*direction) ? "↓" : "↑");

    if (operation == Operation::StretchTop) {
        auto firstSlotIndex = strategy()->timeSlots().indexOf(draggedSession.value().timeSlots.front());
        switch (*direction) {
            case Direction::Up:
                sourceIndex = firstSlotIndex;
                break;
            case Direction::Down:
                sourceIndex = firstSlotIndex - 1;
                break;
        }
    } else {
        auto lastSlotIndex = strategy()->timeSlots().indexOf(draggedSession.value().timeSlots.back());
        switch (*direction) {
            case Direction::Up:
                sourceIndex = lastSlotIndex + 1;
                break;
            case Direction::Down:
                sourceIndex = lastSlotIndex;
                break;
        }
    }

//    qDebug() << "sourceIndex" << sourceIndex;
//    qDebug() << "currentSlotIndex" << currentSlotIndex;

    strategy()->fillTimeSlots(sourceIndex, currentSlotIndex);
//    selectSessionAtSlotIndex(currentSlotIndex);
}

void SlotsMouseHandler::handleMouseHover(const QMouseEvent *event) {
    currentSessionIndex = sessionIndexForSlotIndex(currentSlotIndex);

    auto mouseZone = determineMouseZone(event, currentSessionIndex);

    if (mouseZone != currentMouseZone) {
        currentMouseZone = mouseZone;
        updateCursor();
    }
}

void SlotsMouseHandler::handleDragOperation(QMouseEvent *event) {
    auto distance = currentSlotIndex - dragHandleSlotIndex;
    auto positionInSlot = relativeTopPositionInSlotForEvent(event);

    strategy()->dragActivitySession(currentSessionIndex, distance);

    dragHandleSlotIndex = currentSlotIndex;
    currentSessionIndex = sessionIndexForSlotIndex(currentSlotIndex);

    selectSessionAtSlotIndex(currentSlotIndex);
}


void SlotsMouseHandler::mouseReleaseEvent(QMouseEvent *event) {
    if (mousePressHappened) {
        reset();
        strategy()->commitToHistory();
    }
}


int SlotsMouseHandler::sessionIndexForSlotIndex(int slotIndex) {
    return strategy()->activitySessions()
            .sessionIndexForTimeSlotIndex(slotIndex).value_or(-1);
}

std::optional<SlotsMouseHandler::Operation>
SlotsMouseHandler::determineOperationForMouseZone(MouseZone mouseZone) {
    bool currentSlotIsEmpty = strategy()->timeSlots()[currentSlotIndex].activity
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

const ActivitySession &
SlotsMouseHandler::currentSession() {
    return strategy()->activitySessions()[currentSessionIndex];
}


SlotsMouseHandler::MouseZone
SlotsMouseHandler::determineMouseZone(const QMouseEvent *event, int sessionIndex) {
    auto *currentSessionWidget = sessionWidgetAtIndex(sessionIndex);

    auto topStretchZone = ZoneSize{currentSessionWidget->geometry().top()};

    auto bottomStretchZone = ZoneSize{currentSessionWidget->geometry().top()
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
    auto currentSession = strategy()->activitySessions()[currentSessionIndex];
    if (currentSession.activity == Strategy::NoActivity) {
        unsetCursor();
        return;
    }

    if (currentMouseZone == MouseZone::DragZone) {
        if (operation == Operation::Drag) {
            setCursor(QCursor(Qt::ClosedHandCursor));
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
        setCursor(QCursor(Qt::SizeVerCursor));
    }

#ifdef Q_OS_MAC
//    auto pixmap = MacOSWindow::resizeCursor();
//    auto screenNumber = QApplication::desktop()->screenNumber();
//    auto devicePixelRatio = QGuiApplication::screens()[screenNumber]->devicePixelRatio();
//    pixmap.setDevicePixelRatio(devicePixelRatio);
//    slotsWidget->setCursor(QCursor(pixmap));
#else
    slotsWidgete->setCursor(QCursor(Qt::SizeVerCursor));
#endif
}


void SlotsMouseHandler::contextMenuEvent(QContextMenuEvent *event) {
    auto currentSlotIndex = slotIndexForEvent(event);
    auto &currentSlot = strategy()->timeSlots()[currentSlotIndex];

    QMenu menu(slotsWidget);
    menu.addAction(slotsWidget->setActivityAction);
    menu.addSeparator();

    if (currentSlot.activity) {
        menu.addAction(slotsWidget->deleteActivityAction);
    }

    menu.addAction(slotsWidget->shiftSlotsBelowAction);
    menu.addSeparator();

    menu.addAction(slotsWidget->clearSelectionAction);

    menu.exec(event->globalPos());
}

bool SlotsMouseHandler::hasSelection() {
    return !slotsWidget->selectionWidget->selection().empty();
}

void SlotsMouseHandler::selectSlotAtIndex(int slotIndex) {
    slotsWidget->selectionWidget->selectAtIndex(slotIndex);
    slotsWidget->clearSelectionAction->setEnabled(true);
}

void SlotsMouseHandler::selectSessionAtSlotIndex(int slotIndex) {
    auto sessionIndex = strategy()
            ->activitySessions()
            .sessionIndexForTimeSlotIndex(slotIndex);

    for (int i = 0; i < slotsWidget->slotsLayout->count(); i++) {
        setSelectedForSessionIndex(i, i == sessionIndex);
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

void SlotsMouseHandler::fillSlots(int fromIndex, int toIndex) {
    strategy()->fillTimeSlots(fromIndex, toIndex);
}

Strategy *SlotsMouseHandler::strategy() {
    return slotsWidget->strategy;
}

void SlotsMouseHandler::reset() {
    mousePressHappened = false;
    currentMouseZone = std::nullopt;
    operation = std::nullopt;
    direction = std::nullopt;
    draggedSession = std::nullopt;

    strategy()->endDragging();

    unsetCursor();

    currentSlotIndex = -1;

    currentSessionIndex = -1;
    dragHandleSlotIndex = -1;

    previousMouseTop = 0;

//    isPulling = false;
//    pulledFrom = -1;
//    pulledTo = -1;
//    _selectedGroupIndex = std::nullopt;
//    slotsWidget->selectionWidget->deselectAll();
//
    deselectAllSessions();
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

double SlotsMouseHandler::relativeTopPositionInSlotForEvent(QMouseEvent *event) {
    auto relativeY = static_cast<double>(event->pos().y() - firstSlotTopOffset()) / slotHeight();
    float _ = 0;

    return std::modf(relativeY, &_);
}

