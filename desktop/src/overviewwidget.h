//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_OVERVIEWWIDGET_H
#define STRATEGR_OVERVIEWWIDGET_H

#include <QScrollArea>
#include <QWidget>

#include <strategr/overview.h>
#include <strategr/strategy.h>

#include "colorprovider.h"
#include "dataproviderwidget.h"

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

    stg::overview overview = stg::overview(strategy(), [this] { return width(); });
    stg::overview::viewport_marker makeViewportMarker();

    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
};


#endif//STRATEGR_OVERVIEWWIDGET_H
