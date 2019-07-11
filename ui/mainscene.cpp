//
// Created by Dmitry Khrykin on 2019-07-09.
//

#include "mainscene.h"

#include <QLabel>
#include <QLayout>

MainScene::MainScene(Strategy *strategy, QWidget *parent)
        : SlidingStackedWidget(parent), strategy(strategy) {

    sessionsMainWidget = new SessionsMainWidget(strategy);
    activitiesWidget = new ActivitiesListWidget(strategy);

    addWidget(sessionsMainWidget);
    addWidget(activitiesWidget);

    connect(activitiesWidget,
            &ActivitiesListWidget::wantToGetBack,
            this,
            &MainScene::showSessions);
}

void MainScene::showActivities() {
    slideToWidget(activitiesWidget);
}

void MainScene::showSessions() {
    sessionsMainWidget->clearSelection();
    slideToWidget(sessionsMainWidget);
}

void MainScene::focusOnCurrentTime() {
    sessionsMainWidget->focusOnCurrentTime();
}

void MainScene::openStrategySettings() {
    sessionsMainWidget->toggleStrategySettingsOpen();
}

void MainScene::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;

    sessionsMainWidget->setStrategy(newStrategy);
    activitiesWidget->setStrategy(newStrategy);
}

void MainScene::clearSelection() {
    sessionsMainWidget->clearSelection();
}

const SelectionWidget::RawSelectionState &MainScene::selection() {
    return sessionsMainWidget->selection();
}
