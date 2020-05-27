//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_OVERVIEWWIDGET_H
#define STRATEGR_OVERVIEWWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <models/overview.h>
#include "strategy.h"
#include "colorprovider.h"

class SlotboardScrollArea;
class SlotBoardWidget;

class OverviewWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit OverviewWidget(stg::strategy &strategy,
                            SlotboardScrollArea *scrollArea,
                            QWidget *parent = nullptr);

    void reloadStrategy();

private:
    stg::strategy &strategy;
    SlotboardScrollArea *scrollArea;

    void paintEvent(QPaintEvent *) override;
    void drawElements(QPainter &painter, stg::overview &overview) const;
    void drawViewportMarker(QPainter &painter, stg::overview &overview) const;
    void drawCurrentTimeMarker(QPainter &painter, stg::overview &overview) const;
    void drawBorders(QPainter &painter) const;

    stg::overview::viewport_marker makeViewportMarker() const;
    SlotBoardWidget *slotboardWidget() const;

    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
};


#endif //STRATEGR_OVERVIEWWIDGET_H
