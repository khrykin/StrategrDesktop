//
// Created by Dmitry Khrykin on 2019-07-09.
//


#include <QLabel>
#include <QLayout>

#include "macoswindow.h"
#include "mainscene.h"
#include "mainwindow.h"

MainScene::MainScene(stg::strategy &strategy, QWidget *parent)
        : SlidingStackedWidget(parent), strategy(strategy) {

    sessionsMainWidget = new SessionsMainWidget(strategy, this);
    activitiesWidget = new ActivityListWidget(strategy, this);

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
    MacOSWindow::pageChange(qobject_cast<MainWindow *>(window()), 1);
#endif
}

void MainScene::showSessions() {
    sessionsMainWidget->clearSelection();
    slideToWidget(sessionsMainWidget);

#ifdef Q_OS_MAC
    MacOSWindow::pageChange(qobject_cast<MainWindow *>(window()), 0);
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

void MainScene::reloadStrategy() {
    sessionsMainWidget->reloadStrategy();
    activitiesWidget->reloadStrategy();
}

void MainScene::clearSelection() {
    sessionsMainWidget->clearSelection();
}


void MainScene::showNewActivityMenu() {
    activitiesWidget->showNewActivityMenu();
}

void MainScene::reorderActivitiesByUsage() {
    strategy.reorder_activities_by_usage();
}
