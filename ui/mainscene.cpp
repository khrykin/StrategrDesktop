//
// Created by Dmitry Khrykin on 2019-07-09.
//


#include <QLabel>
#include <QLayout>

#include "macoswindow.h"
#include "mainscene.h"
#include "mainwindow.h"

MainScene::MainScene(Strategy *strategy, QWidget *parent)
        : SlidingStackedWidget(parent), strategy(strategy) {

    sessionsMainWidget = new SessionsMainWidget(strategy);
    activitiesWidget = new ActivityListWidget(strategy);

    addWidget(sessionsMainWidget);
    addWidget(activitiesWidget);

    connect(activitiesWidget,
            &ActivityListWidget::wantToGetBack,
            this,
            &MainScene::showSessions);
}

void MainScene::showActivities() {
    slideToWidget(activitiesWidget);

#ifdef Q_OS_MAC
    MacOSWindow::pageChange(parentWindow(), 1);
#endif
}

void MainScene::showSessions() {
    sessionsMainWidget->clearSelection();
    slideToWidget(sessionsMainWidget);

#ifdef Q_OS_MAC
    MacOSWindow::pageChange(parentWindow(), 0);
#endif
}

void MainScene::focusOnCurrentTime() {
    sessionsMainWidget->focusOnCurrentTime();
}

void MainScene::showStrategySettings() {
    if (currentWidget() == activitiesWidget) {
        showSessions();
    }

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

void MainScene::showNewActivityMenu() {
    activitiesWidget->showNewActivityMenu();
}
