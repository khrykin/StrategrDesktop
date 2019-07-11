//
// Created by Dmitry Khrykin on 2019-07-09.
//

#include "mainscene.h"

#include <QLabel>
#include <QLayout>

MainScene::MainScene(Strategy *strategy, QWidget *parent)
        : SlidingStackedWidget(parent), strategy(strategy) {

    sessionsWidget = new SessionsWidget(strategy);
    activitiesWidget = new ActivitiesWidget(this);

    addWidget(sessionsWidget);
    addWidget(activitiesWidget);

    connect(activitiesWidget,
            &ActivitiesWidget::wantToGetBack,
            this,
            &MainScene::showSessions);
}

void MainScene::showActivities() {
    slideToWidget(activitiesWidget);
}

void MainScene::showSessions() {
    sessionsWidget->clearSelection();
    slideToWidget(sessionsWidget);
}

void MainScene::focusOnCurrentTime() {
    sessionsWidget->focusOnCurrentTime();
}

void MainScene::openStrategySettings() {
    sessionsWidget->toggleStrategySettingsOpen();
}

void MainScene::setStrategy(Strategy *newStrategy) {
    strategy = std::ref(newStrategy);
    sessionsWidget->setStrategy(newStrategy);
}

void MainScene::clearSelection() {
    sessionsWidget->clearSelection();
}
