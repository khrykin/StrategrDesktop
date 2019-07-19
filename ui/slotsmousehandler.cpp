//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include "slotsmousehandler.h"
#include "slotswidget.h"
#include <QMenu>

SlotsMouseHandler::SlotsMouseHandler(SlotsWidget *slotsWidget)
        : slotsWidget(slotsWidget) {
}

void SlotsMouseHandler::mousePressEvent(QMouseEvent *event) {
    pulledFrom = slotIndexForEvent(event);

    auto leftButtonPressed = event->buttons() == Qt::LeftButton;
    auto rightButtonPressed = event->buttons() == Qt::RightButton;
    auto ctrlPressed = event->modifiers() == Qt::CTRL;

    if ((leftButtonPressed && ctrlPressed)
        || (rightButtonPressed && !hasSelection())) {
        // Slot selection event
        selectSlotAtIndex(pulledFrom);
    } else if (leftButtonPressed) {
        // Slot pulling event
        isPulling = true;
        slotsWidget->setCursor(QCursor(Qt::SizeVerCursor));
        selectSessionAtSlotIndex(pulledFrom);
        slotsWidget->deselectAllSlots();
    }
}

void SlotsMouseHandler::mouseMoveEvent(QMouseEvent *event) {
    if (isPulling) {
        auto slotIndex = slotIndexForEvent(event);
        if (pulledTo != slotIndex) {
            pulledTo = slotIndex;
            fillSlots(pulledFrom, pulledTo);
            selectSessionAtSlotIndex(pulledTo);
        }
    }
}


void SlotsMouseHandler::mouseReleaseEvent(QMouseEvent *event) {
    if (isPulling) {
        reset();
        strategy()->commitToHistory();
        slotsWidget->unsetCursor();
    }
}

void SlotsMouseHandler::contextMenuEvent(QContextMenuEvent *event) {
    auto currentIndex = slotIndexForEvent(event);
    auto &currentSlot = strategy()->timeSlots()[currentIndex];

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
    auto groupIndex = strategy()
            ->activitySessions()
            .sessionIndexForTimeSlotIndex(slotIndex);

    if (groupIndex && _selectedGroupIndex != groupIndex) {
        _selectedGroupIndex = groupIndex;

        for (int i = 0; i < slotsWidget->slotsLayout->count(); i++) {
            setSelectedForSessionIndex(i, i == groupIndex);
        }
    }
}

void SlotsMouseHandler::deselectAllSessions() {
    _selectedGroupIndex = std::nullopt;
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

void SlotsMouseHandler::fillSlots(int fromIndex, int toIndex) {
    strategy()->fillTimeSlots(fromIndex, toIndex);
}

Strategy *SlotsMouseHandler::strategy() {
    return slotsWidget->strategy;
}

void SlotsMouseHandler::reset() {
    isPulling = false;
    pulledFrom = -1;
    pulledTo = -1;
    _selectedGroupIndex = std::nullopt;
    slotsWidget->selectionWidget->deselectAll();

    deselectAllSessions();
}

