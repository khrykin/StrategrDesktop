//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_SLOTSMOUSEHANDLER_H
#define STRATEGR_SLOTSMOUSEHANDLER_H

#include <QMouseEvent>
#include "strategy.h"

class SlotsWidget;
class ActivitySessionWidget;

class SlotsMouseHandler {
public:
    explicit SlotsMouseHandler(SlotsWidget *slotsWidget);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

    void reset();
private:
    SlotsWidget *slotsWidget;
    std::optional<int> _selectedGroupIndex;

    bool isPulling = false;
    int pulledFrom = -1;
    int pulledTo = -1;

    template<class Event>
    int slotIndexForEvent(Event *event) {
        return event->pos().y() / slotHeight();
    };

    ActivitySessionWidget *sessionWidgetAtIndex(int sessionIndex);
    Strategy *strategy();
    int slotHeight();

    bool hasSelection();
    void selectSlotAtIndex(int slotIndex);
    void selectSessionAtSlotIndex(int slotIndex);
    void deselectAllSessions();
    void setSelectedForSessionIndex(int sessionIndex, bool isSelected);

    void fillSlots(int fromIndex, int toIndex);
};


#endif //STRATEGR_SLOTSMOUSEHANDLER_H
