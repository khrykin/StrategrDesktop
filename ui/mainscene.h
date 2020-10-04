//
// Created by Dmitry Khrykin on 2019-07-09.
//

#ifndef STRATEGR_MAINSCENE_H
#define STRATEGR_MAINSCENE_H

#include "applicationsettings.h"
#include "strategy.h"
#include "third-party/slidingstackedwidget.h"

#include "actioncenter.h"
#include "activitylistwidget.h"
#include "mousehandler.h"
#include "sessionsmainwidget.h"
#include "slotboardscrollarea.h"
#include "slotswidget.h"

class MainScene : public SlidingStackedWidget {
public:
    explicit MainScene(stg::strategy &strategy, QWidget *parent = nullptr);

    stg::strategy &strategy();
    stg::selection &selection();
    stg::mouse_handler &mouseHandler();
    stg::action_center &actionCenter();

    SlotboardScrollArea *scrollboardScrollArea();
    SlotsWidget *slotsWidget();

    void showStrategySettings();
    void showNewActivityMenu();

    void reloadStrategy();

private:
    stg::strategy &_strategy;
    stg::action_center _action_center = stg::action_center(
        _strategy,
        [] { return ApplicationSettings::defaultSlotHeight; },
        [this] { return scrollboardScrollArea()->viewportRectRelativeToContent(); });
    SessionsMainWidget *sessionsMainWidget;
    ActivityListWidget *activitiesWidget;
};

#endif//STRATEGR_MAINSCENE_H
