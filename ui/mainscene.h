//
// Created by Dmitry Khrykin on 2019-07-09.
//

#ifndef STRATEGR_MAINSCENE_H
#define STRATEGR_MAINSCENE_H

#include "third-party/slidingstackedwidget.h"
#include "strategy.h"

#include "sessionsmainwidget.h"
#include "activitylistwidget.h"

class MainScene : public SlidingStackedWidget {
Q_OBJECT
public:
    explicit MainScene(stg::strategy &strategy,
                       QWidget *parent = nullptr);

    void showActivities();
    void showSessions();

    void focusOnCurrentTime();
    void showStrategySettings();

    void showNewActivityMenu();

    stg::selection &selection() {
        return findChildren<SelectionWidget *>().first()->selection;
    }

    void clearSelection();
    void reloadStrategy();

    void reorderActivitiesByUsage();

private:
    stg::strategy &strategy;
    SessionsMainWidget *sessionsMainWidget;
    ActivityListWidget *activitiesWidget;
};

#endif //STRATEGR_MAINSCENE_H
