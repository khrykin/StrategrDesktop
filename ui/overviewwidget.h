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

class OverviewWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit OverviewWidget(Strategy *strategy,
                            QScrollArea *scrollArea,
                            QWidget *parent = nullptr);

    void setStrategy(Strategy *newStrategy);

private:
    Strategy *strategy;
    QScrollArea *scrollArea;

    void paintEvent(QPaintEvent *) override;
    void drawElements(QPainter &painter, Overview &overview) const;
    void drawViewportMarker(QPainter &painter, Overview &overview) const;
    void drawCurrentTimeMarker(QPainter &painter, Overview &overview) const;
    void drawBorders(QPainter &painter) const;

    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
};


#endif //STRATEGR_OVERVIEWWIDGET_H
