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
    using RawSelectionState = std::vector<Strategy::TimeSlotIndex>;

    explicit SelectionWidget(Strategy *strategy,
                             int slotHeight,
                             QWidget *parent = nullptr);

    void selectAtIndex(int slotIndex);
    void deselectAll();
    void selectAll(int numberOfSlots);

    void setSlotHeight(int newSlotHeight);

    const RawSelectionState &selection() const;

    bool selectionIsContinuous() const;
    void setStrategy(Strategy *strategy);


signals:
    void selectionChanged();
private:
    using SelectionState = std::vector<RawSelectionState>;
    Strategy *strategy;

    int slotHeight = ApplicationSettings::defaultSlotHeight;

    void updateUI();

    RawSelectionState rawSelectionState;
    SelectionState selectionState;

    static SelectionState makeSelectionState(RawSelectionState rawState);
    void drawSelectionForItem(SelectionWidget::RawSelectionState &selectionItem, QPainter &painter);

    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif //STRATEGR_SELECTIONWIDGET_H
