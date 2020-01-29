//
// Created by Dmitry Khrykin on 2019-07-11.
//

#ifndef STRATEGR_SELECTIONWIDGET_H
#define STRATEGR_SELECTIONWIDGET_H

#include <QWidget>

#include "applicationsettings.h"
#include "strategy.h"
#include "colorprovider.h"

class SelectionWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    using RawSelectionState = std::vector<stg::strategy::time_slot_index_t>;

    explicit SelectionWidget(stg::strategy &strategy,
                             int slotHeight,
                             QWidget *parent = nullptr);

    void selectAtIndex(int slotIndex);
    void deselectAll();
    void selectAll(int numberOfSlots);
    void fillSelected(int fromIndex, int toIndex);

    void setSlotHeight(int newSlotHeight);

    const RawSelectionState &selection() const;

    bool selectionIsContinuous() const;
    bool isSlotIndexSelected(stg::strategy::time_slot_index_t slotIndex);

    void reloadStrategy();

    bool isClicked() const;
    void setIsClicked(bool isClicked);
signals:
    void selectionChanged();
private:
    using SelectionState = std::vector<RawSelectionState>;

    stg::strategy &strategy;
    RawSelectionState rawSelectionState;
    SelectionState selectionState;

    int slotHeight = ApplicationSettings::defaultSlotHeight;
    bool _isClicked = false;

    void updateUI();
    static SelectionState makeSelectionState(RawSelectionState rawState);
    void drawSelectionForItem(SelectionWidget::RawSelectionState &selectionItem, QPainter &painter);

    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif //STRATEGR_SELECTIONWIDGET_H
