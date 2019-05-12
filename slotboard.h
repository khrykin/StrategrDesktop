#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QWidget>
#include <QMouseEvent>
#include <QTimer>
#include "activitygroupwidget.h"
#include "strategy.h"

class SlotBoard : public QWidget
{
    Q_OBJECT
public:
    explicit SlotBoard(QWidget *parent = nullptr);

    void updateUI();
    Strategy *strategy() const;
    void setStrategy(Strategy *strategy);

signals:

public slots:
private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    int slotIndexForEvent(QMouseEvent *event);

    void copySlot(int fromIndex, int toIndex);
    void fillSlots(int fromIndex, int toIndex);
    void selectGroupAtIndex(int selectedGroupIndex);
    void deselectAlGroups();

    void selectSlotAtIndex(int slotIndex);
    void deselectAllSlots();

    ActivityGroupWidget *groupWidgetAtIndex(int index);

    const int SLOT_HEIGHT = 50;
    const int NUMBER_OF_SLOTS = 20;

    void paintEvent(QPaintEvent *) override;

    Strategy *_strategy;

    std::optional<int> _selectedGroupIndex;

    int _pulledFrom;
    int _pulledTo;

    bool _isPulling = false;

    QTimer *_longPressTimer;

    QWidget *selectionWidget;
};

#endif // SLOTBOARD_H
