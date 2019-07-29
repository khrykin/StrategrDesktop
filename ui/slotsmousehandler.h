//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_SLOTSMOUSEHANDLER_H
#define STRATEGR_SLOTSMOUSEHANDLER_H

#include <QMouseEvent>
#include <QFrame>
#include "strategy.h"

class SlotsWidget;
class ActivitySessionWidget;

class SlotsMouseHandler : public QFrame {
public:
    explicit SlotsMouseHandler(SlotsWidget *slotsWidget);

    void reset();
private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    static const auto stretchZoneHeight = 3;

    struct ZoneSize;

    enum class Operation {
        Drag,
        StretchTop,
        StretchBottom,
    };

    enum class MouseZone {
        DragZone,
        TopStretchZone,
        BottomStretchZone,
    };

    enum class Direction {
        Up,
        Down
    };

    SlotsWidget *slotsWidget;

    std::optional<Operation> operation;
    std::optional<Direction> direction;

    std::optional<MouseZone> currentMouseZone = std::nullopt;

    int currentSlotIndex = -1;
    int currentSessionIndex = -1;

    int previousMouseTop = 0;
    int mousePressTop = 0;
    int dragHandleSlotIndex = -1;

    bool mousePressHappened = false;
    std::optional<ActivitySession> draggedSession;


    ActivitySessionWidget *sessionWidgetAtIndex(int sessionIndex);
    ActivitySessionWidget *sessionWidgetAtSlotIndex(int timeSlotIndex);

    Strategy *strategy();
    int slotHeight();

    int firstSlotTopOffset();

    template<class Event>
    int slotIndexForEvent(Event *event) {
        return (event->pos().y() - firstSlotTopOffset()) / slotHeight();
    }

    double relativeTopPositionInSlotForEvent(QMouseEvent *event);

    bool hasSelection();
    void selectSlotAtIndex(int slotIndex);
    void selectSessionAtSlotIndex(int slotIndex);
    void deselectAllSessions();
    void setSelectedForSessionIndex(int sessionIndex, bool isSelected);

    void fillSlots(int fromIndex, int toIndex);
    void updateCursor();
    SlotsMouseHandler::MouseZone determineMouseZone(const QMouseEvent *event, int slotIndex);

    std::optional<Operation> determineOperationForMouseZone(MouseZone mouseZone);
    int sessionIndexForSlotIndex(int slotIndex);
    const ActivitySession &currentSession();
    void handleDragOperation(QMouseEvent *event);
    void handleMouseHover(const QMouseEvent *event);
    void handleResizeOperation();
    std::optional<Direction> determineDirection(const QMouseEvent *event);
    void handleLeftButtonPress(const QMouseEvent *event);
    void updateDraggedSession(int slotIndex);
};


struct SlotsMouseHandler::ZoneSize {
    explicit ZoneSize(int top = 0) : _top(top) {}

    int top() {
        return _top;
    }

    int bottom() {
        return top() + stretchZoneHeight;
    }

private:
    int _top = 0;
};


#endif //STRATEGR_SLOTSMOUSEHANDLER_H
