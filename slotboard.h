#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QWidget>
#include <QMouseEvent>
#include "slotwidget.h"
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
    void setSlotSelected(int index);
    void deselectAllSLots();

    SlotWidget *slotAtIndex(int index);

    const int SLOT_HEIGHT = 50;
    const int NUMBER_OF_SLOTS = 20;

    void paintEvent(QPaintEvent *) override;

    Strategy *_strategy;

    std::optional<int> _selectedSlotIndex;

    int _pulledFrom;
    int _pulledTo;

    bool _isPulling = false;
};

#endif // SLOTBOARD_H
