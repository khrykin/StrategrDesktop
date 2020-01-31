//
// Created by Dmitry Khrykin on 2020-01-31.
//

#ifndef STRATEGR_SLOTBOARDCIRCLESWIDGET_H
#define STRATEGR_SLOTBOARDCIRCLESWIDGET_H

#include <functional>
#include <iostream>
#include <QWidget>
#include <QRect>

#include "colorprovider.h"

class SlotBoardCirclesWidget : public QWidget,
                               public ColorProvider {
public:
    explicit SlotBoardCirclesWidget(const std::function<int()> &getSlotSize,
                                    const std::function<const QRect()> &getSlotsRect,
                                    QWidget *parent);

    void updateResizeBoundary(int, int slotBeforeResizeBoundaryIndex);
    int slotBeforeResizeBoundaryIndex() const;

private:
    std::function<int()> getSlotHeight;
    std::function<const QRect()> getSlotsRect;
    int _slotBeforeResizeBoundaryIndex = -2;

    void paintEvent(QPaintEvent *) override;
};


#endif //STRATEGR_SLOTBOARDCIRCLESWIDGET_H
