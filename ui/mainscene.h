//
// Created by Dmitry Khrykin on 2019-07-09.
//

#ifndef STRATEGR_MAINSCENE_H
#define STRATEGR_MAINSCENE_H

#include "third-party/slidingstackedwidget.h"
#include "strategy.h"

#include "sessionswidget.h"
#include "activitieswidget.h"

class MainScene : public SlidingStackedWidget {
Q_OBJECT
public:
    explicit MainScene(Strategy *strategy,
                       QWidget *parent = nullptr);

    void showActivities();
    void showSessions();

    void focusOnCurrentTime();
    void openStrategySettings();

    void clearSelection();

    void setStrategy(Strategy *newStrategy);
private:
    Strategy *strategy;
    SessionsWidget *sessionsWidget;
    ActivitiesWidget *activitiesWidget;
};

#endif //STRATEGR_MAINSCENE_H
