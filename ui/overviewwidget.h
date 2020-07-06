//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_OVERVIEWWIDGET_H
#define STRATEGR_OVERVIEWWIDGET_H

#include <QWidget>
#include <QScrollArea>

#include "overview.h"
#include "dataproviderwidget.h"
#include "strategy.h"
#include "colorprovider.h"

class OverviewWidget : public DataProviderWidget,
                       public ColorProvider {
Q_OBJECT
public:
    explicit OverviewWidget(QWidget *parent);

    void reloadStrategy();

private:
    void paintEvent(QPaintEvent *) override;
    void drawElements(QPainter &painter, stg::overview &overview);
    void drawViewportMarker(QPainter &painter, stg::overview &overview);
    void drawCurrentTimeMarker(QPainter &painter, stg::overview &overview);
    void drawBorders(QPainter &painter);

    stg::overview::viewport_marker makeViewportMarker();

    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
};


#endif //STRATEGR_OVERVIEWWIDGET_H
