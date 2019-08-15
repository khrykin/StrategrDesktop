//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_OVERVIEWWIDGET_H
#define STRATEGR_OVERVIEWWIDGET_H

#include <QWidget>
#include "strategy.h"
#include "colorprovider.h"

class OverviewWidget : public QWidget, public ColorProvider {
public:
    explicit OverviewWidget(Strategy *strategy, QWidget *parent = nullptr);
    void setStrategy(Strategy *newStrategy);

private:
    Strategy *strategy;
    void paintEvent(QPaintEvent *) override;
};


#endif //STRATEGR_OVERVIEWWIDGET_H
