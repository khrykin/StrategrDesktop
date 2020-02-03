//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_SLOTSMOUSEHANDLER_H
#define STRATEGR_SLOTSMOUSEHANDLER_H

#include <QMouseEvent>
#include <QFrame>
#include "strategy.h"
#include "cursorprovider.h"
#include "colorprovider.h"
#include "sessionsmainwidget.h"
#include "mousehandler.h"
#include "slotswidget.h"

class SessionWidget;
class QPropertyAnimation;

class SlotsMouseHandler :
        public QFrame,
        public CursorProvider,
        public ColorProvider {
Q_OBJECT
public:
    explicit SlotsMouseHandler(SlotsWidget *slotsWidget);
signals:
    void resizeBoundaryChanged(int sessionBeforeBoundaryIndex,
                               int slotBeforeBoundaryIndex);
private:
    SlotsWidget *slotsWidget;

    stg::mouse_handler handler{strategy(),
                               selection(),
                               [this] { return slotHeight(); },
                               [this] { return slotsWidget->geometry(); },
                               [this] { return viewportRect(); }};

    stg::strategy &strategy();
    stg::selection &selection();
    int slotHeight();

    QTimer *autoscrollAnimation = nullptr;

    SessionWidget *sessionWidgetAtIndex(int sessionIndex);
    SlotBoardWidget *slotBoard();
    QScrollArea *slotBoardScrollArea();
    QScrollBar *verticalScrollBar();
    stg::rect viewportRect();

    void selectSessionsAtIndices(const std::vector<int> &sessionIndices);
    void setSelectedForSessionIndex(int sessionIndex, bool isSelected);

    void updateCursor(stg::mouse_handler::cursor new_cursor);
    void updateResizeBoundary(int sessionBeforeBoundaryIndex,
                              int slotBeforeBoundaryIndex);

    void startAutoScroll(stg::mouse_handler::scroll_direction direction);
    void stopAutoScroll();

    void showContextMenu(const stg::point &position, int slotIndex, int sessionIndex);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};
#endif //STRATEGR_SLOTSMOUSEHANDLER_H
