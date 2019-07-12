//
// Created by Dmitry Khrykin on 2019-07-11.
//

#ifndef STRATEGR_SELECTIONWIDGET_H
#define STRATEGR_SELECTIONWIDGET_H

#include <QWidget>
#include "applicationsettings.h"
#include "strategy.h"

class SelectionWidget : public QWidget {
Q_OBJECT
public:
    using RawSelectionState = std::vector<Strategy::TimeSlotIndex>;

    explicit SelectionWidget(int slotHeight,
                             QWidget *parent = nullptr);

    void selectAtIndex(int slotIndex);
    void deselectAll();
    void selectAll(int numberOfSlots);

    void setSlotHeight(int newSlotHeight);

    const RawSelectionState &selection() const;

    bool selectionIsContinuous() const;
signals:
    void selectionChanged();
private:
    using SelectionState = std::vector<RawSelectionState>;

    int slotHeight = ApplicationSettings::defaultSlotHeight;

    void updateUI();

    void resizeEvent(QResizeEvent *event) override;

    RawSelectionState rawSelectionState;
    SelectionState selectionState;

    static SelectionState makeSelectionState(RawSelectionState rawState);
};

#endif //STRATEGR_SELECTIONWIDGET_H
