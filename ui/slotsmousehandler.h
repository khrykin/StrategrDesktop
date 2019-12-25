//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_SLOTSMOUSEHANDLER_H
#define STRATEGR_SLOTSMOUSEHANDLER_H

#include <QMouseEvent>
#include <QFrame>
#include "Strategy.h"
#include "cursorprovider.h"
#include "colorprovider.h"

class SlotsWidget;
class SessionWidget;

class SlotsMouseHandler :
        public QFrame,
        public CursorProvider,
        public ColorProvider {
public:
    explicit SlotsMouseHandler(SlotsWidget *slotsWidget);

    void reset();
private:
    static const auto stretchZoneHeight = 3;

    struct StretchZonePosition;

    enum class Operation {
        Drag,
        StretchTop,
        StretchBottom,
        DragSelect,
        OpenActivities
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

    std::optional<Operation> operation = std::nullopt;
    std::optional<Direction> direction = std::nullopt;

    std::optional<MouseZone> currentMouseZone = std::nullopt;

    int currentSlotIndex = -1;
    int clickedSessionIndex = -1;

    int previousMouseTop = 0;
    int handleSlotIndex = -1;

    std::optional<int> resizeBoundarySlotIndex = std::nullopt;

    bool mousePressHappened = false;

    SessionWidget *sessionWidgetAtIndex(int sessionIndex);
    SessionWidget *sessionWidgetAtSlotIndex(int timeSlotIndex);

    Strategy &strategy();
    int slotHeight();

    int firstSlotTopOffset();

    template<class Event>
    int slotIndexForEvent(Event *event) {
        return (event->pos().y() - firstSlotTopOffset()) / slotHeight();
    }

    bool hasSelection();
    void selectSlotAtIndex(int slotIndex);
    void fillSelection(int fromIndex, int toIndex);

    void selectSessionAtSlotIndex(int slotIndex);
    void selectSessionsAtIndices(const std::vector<int> &sessionIndices);
    void selectStetchingSessions(int sourceIndex);
    void setSelectedForSessionIndex(int sessionIndex, bool isSelected);
    void deselectAllSessions();

    void updateCursor();
    SlotsMouseHandler::MouseZone determineMouseZone(const QMouseEvent *event, int slotIndex);

    std::optional<Operation> determineOperationForMouseZone(MouseZone mouseZone);
    std::optional<Direction> determineDirection(const QMouseEvent *event);

    int sessionIndexForSlotIndex(int slotIndex);

    void handleDragOperation(QMouseEvent *event);
    void handleMouseHover(const QMouseEvent *event);
    void handleStretchOperation(QMouseEvent *event);
    void handleLeftButtonPress(const QMouseEvent *event);
    void handleDirectionChange(const std::optional<Direction> &previousDirection);

    void checkForDirectionChange(const QMouseEvent *event);

    int getMouseTopDelta(int currentMouseTop, int previousMouseTop);

    bool isSlotIndexSelected(int slotIndex);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void initDragSelectedOperation();
};


struct SlotsMouseHandler::StretchZonePosition {
    explicit StretchZonePosition(int top = 0) : _top(top) {}

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
