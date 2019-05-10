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

    SlotWidget *slotAtIndex(int index);

    Strategy *_strategy;

    const int SLOT_HEIGHT = 50;
    const int NUMBER_OF_SLOTS = 20;

    void paintEvent(QPaintEvent *) override;

    int pulledFrom;
    int pulledTo;

    bool isPulling = false;
};

#endif // SLOTBOARD_H
