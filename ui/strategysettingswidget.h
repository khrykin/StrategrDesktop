//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_STRATEGYSETTINGSWIDGET_H
#define STRATEGR_STRATEGYSETTINGSWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include "strategy.h"

class StrategySettingsWidget : public QWidget {
public:
    explicit StrategySettingsWidget(Strategy *strategy,
                                    QWidget *parent = nullptr);

    void slideAndHide();
    void slideAndShow();

    void setStrategy(Strategy *newStrategy);
private:
    Strategy *strategy;
};


#endif //STRATEGR_STRATEGYSETTINGSWIDGET_H
