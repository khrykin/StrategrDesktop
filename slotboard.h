#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QWidget>
#include <QMouseEvent>
#include <QTimer>
#include "activitygroupwidget.h"
#include "strategy.h"
#include <QAction>

class SlotBoard : public QWidget
{
    Q_OBJECT
public:
    explicit SlotBoard(QWidget *parent = nullptr);

    void updateUI();
    Strategy *strategy() const;
    void setStrategy(Strategy *strategy);

    QVector<int> selectionSlots();
    bool hasSelection();
    void selectSlotAtIndex(int slotIndex);
    void deselectAllSlots();

signals:
    void wantToSetActivtyForSelection(QVector <int> selection);

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    int slotIndexForEvent(QContextMenuEvent *event);
    int slotIndexForEvent(QMouseEvent *event);

    void copySlot(int fromIndex, int toIndex);
    void fillSlots(int fromIndex, int toIndex);
    void selectGroupAtIndex(int selectedGroupIndex);
    void deselectAllGroups();



    ActivityGroupWidget *groupWidgetAtIndex(int index);

    const int SLOT_HEIGHT = 50;

    void paintEvent(QPaintEvent *) override;

    Strategy *_strategy;

    std::optional<int> _selectedGroupIndex;

    int _pulledFrom;
    int _pulledTo;

    bool _isPulling = false;

    QTimer *_longPressTimer;

    QWidget *selectionWidget;

    QAction *setActivityAction;
    QAction *deleteActivityAction;
    QAction *clearSelectionAction;

    void openActivitiesWindow();
    void deleteActivityInSelection();
    void clearCurrentSelection();
};

#endif // SLOTBOARD_H
