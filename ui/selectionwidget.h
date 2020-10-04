//
// Created by Dmitry Khrykin on 2019-07-11.
//

#ifndef STRATEGR_SELECTIONWIDGET_H
#define STRATEGR_SELECTIONWIDGET_H

#include <QWidget>

#include "applicationsettings.h"
#include "colorprovider.h"
#include "dataproviderwidget.h"
#include "selection.h"

class SelectionWidget : public DataProviderWidget,
                        public ColorProvider {
public:
    explicit SelectionWidget(QWidget *parent);

private:
    void drawSelectionForItem(const stg::grouped_selection_element &selectionItem,
                              QPainter &painter);

    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif//STRATEGR_SELECTIONWIDGET_H
