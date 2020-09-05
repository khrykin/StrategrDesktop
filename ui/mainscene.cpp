//
// Created by Dmitry Khrykin on 2019-07-09.
//

#include <QLabel>
#include <QLayout>

#include "macoswindow.h"
#include "mainscene.h"
#include "mainwindow.h"

MainScene::MainScene(stg::strategy &strategy, QWidget *parent)
        : SlidingStackedWidget(parent),
          _strategy(strategy) {
    sessionsMainWidget = new SessionsMainWidget(this);
    activitiesWidget = new ActivityListWidget(this);

    addWidget(sessionsMainWidget);
    addWidget(activitiesWidget);

    _action_center.on_show_activities = [this] {
        activitiesWidget->reloadStrategy();
        slideToWidget(activitiesWidget);
#ifdef Q_OS_MAC
        MacOSWindow::pageDidChanged(qobject_cast<MainWindow *>(window()), 1);
#endif
    };

    _action_center.on_show_sessions = [this] {
        slideToWidget(sessionsMainWidget);
#ifdef Q_OS_MAC
        MacOSWindow::pageDidChanged(qobject_cast<MainWindow *>(window()), 0);
#endif
    };
}

stg::strategy &MainScene::strategy() {
    return _strategy;
}

stg::selection &MainScene::selection() {
    return _action_center.selection();
}

stg::mouse_handler &MainScene::mouseHandler() {
    return _action_center.mouse_handler();
}

stg::action_center &MainScene::actionCenter() {
    return _action_center;
}

void MainScene::showStrategySettings() {
    if (currentWidget() == activitiesWidget)
        actionCenter().show_sessions();

    sessionsMainWidget->toggleStrategySettingsOpen();
}

void MainScene::reloadStrategy() {
    sessionsMainWidget->reloadStrategy();
    activitiesWidget->reloadStrategy();
}

void MainScene::showNewActivityMenu() {
    activitiesWidget->showNewActivityMenu();
}

SlotboardScrollArea *MainScene::scrollboardScrollArea() {
    return sessionsMainWidget->slotboardScrollArea();
}

SlotsWidget *MainScene::slotsWidget() {
    return sessionsMainWidget->findChild<SlotsWidget *>();
}
