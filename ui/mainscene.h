//
// Created by Dmitry Khrykin on 2019-07-09.
//

#ifndef STRATEGR_MAINSCENE_H
#define STRATEGR_MAINSCENE_H

#include "third-party/slidingstackedwidget.h"
#include "strategy.h"

#include "sessionsmainwidget.h"
#include "activitylistwidget.h"
#include "parentwindowaccessible.h"

class MainScene :
        public SlidingStackedWidget,
        public ParentWindowAccessible<MainScene> {
Q_OBJECT
public:
    explicit MainScene(Strategy *strategy,
                       QWidget *parent = nullptr);

    void showActivities();
    void showSessions();

    void focusOnCurrentTime();
    void showStrategySettings();

    void showNewActivityMenu();

    void clearSelection();

    const SelectionWidget::RawSelectionState &selection();

    void setStrategy(Strategy *newStrategy);
private:
    Strategy *strategy;
    SessionsMainWidget *sessionsMainWidget;
    ActivityListWidget *activitiesWidget;
};

#endif //STRATEGR_MAINSCENE_H
