//
// Created by Dmitry Khrykin on 2019-07-09.
//

#include "mainscene.h"

#include <QLabel>
#include <QLayout>

MainScene::MainScene(Strategy &strategy, QWidget *parent)
        : QWidget(parent), strategy(strategy) {
    setLayout(new QHBoxLayout());
    auto label = new QLabel("MainScene");
    label->setAlignment(Qt::AlignCenter);

    layout()->addWidget(label);
}

void MainScene::showActivitiesList() {

}

void MainScene::focusOnCurrentTime() {

}

void MainScene::openStrategySettings() {

}

void MainScene::setStrategy(const Strategy &newStrategy) {
    strategy = newStrategy;
    updateUI();
}

void MainScene::updateUI() {

}
