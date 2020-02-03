//
// Created by Dmitry Khrykin on 2019-07-11.
//

#ifndef STRATEGR_SELECTIONWIDGET_H
#define STRATEGR_SELECTIONWIDGET_H

#include <QWidget>

#include "applicationsettings.h"
#include "strategy.h"
#include "colorprovider.h"
#include "selection.h"

class SelectionWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    stg::selection selection;

    explicit SelectionWidget(stg::strategy &strategy,
                             int slotHeight,
                             QWidget *parent = nullptr);

private:
    stg::strategy &strategy;

    int slotHeight = ApplicationSettings::defaultSlotHeight;

    void drawSelectionForItem(const stg::grouped_selection_element &selectionItem,
                              QPainter &painter);

    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif //STRATEGR_SELECTIONWIDGET_H
