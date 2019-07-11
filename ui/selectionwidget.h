//
// Created by Dmitry Khrykin on 2019-07-11.
//

#ifndef STRATEGR_SELECTIONWIDGET_H
#define STRATEGR_SELECTIONWIDGET_H

#include <QWidget>
#include "applicationsettings.h"
#include "strategy.h"

class SelectionWidget : public QWidget {
public:
    using RawSelectionState = std::vector<Strategy::TimeSlotIndex>;
    explicit SelectionWidget(int slotHeight,
                             QWidget *parent = nullptr);

    void selectAtIndex(int slotIndex);
    void deselectAll();
    void selectAll(int numberOfSlots);

    void setSlotHeight(int newSlotHeight);
    const RawSelectionState &selection() const;
private:
    int slotHeight = ApplicationSettings::defaultSlotHeight;

    void updateUI();

    void resizeEvent(QResizeEvent *event) override;

    using SelectionState = std::vector<RawSelectionState>;

    RawSelectionState rawSelectionState;

    static SelectionState makeSelectionState(RawSelectionState rawState);
};

#endif //STRATEGR_SELECTIONWIDGET_H
