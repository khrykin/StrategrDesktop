//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include "strategysettingswidget.h"
#include "slidinganimator.h"

StrategySettingsWidget::StrategySettingsWidget(Strategy *strategy, QWidget *parent) : QWidget(parent) {
    setLayout(new QHBoxLayout());

    auto label = new QLabel("StrategySettingsWidget");
    label->setAlignment(Qt::AlignCenter);

    layout()->addWidget(label);

    hide();
}

void StrategySettingsWidget::slideAndHide() {
    SlidingAnimator::hideWidget(this);
}

void StrategySettingsWidget::slideAndShow() {
    SlidingAnimator::showWidget(this);
}

void StrategySettingsWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
}

