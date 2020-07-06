//
// Created by Dmitry Khrykin on 2019-07-09.
//

#ifndef STRATEGR_MAINSCENE_H
#define STRATEGR_MAINSCENE_H

#include "third-party/slidingstackedwidget.h"
#include "strategy.h"
#include "applicationsettings.h"

#include "sessionsmainwidget.h"
#include "activitylistwidget.h"
#include "mousehandler.h"
#include "actioncenter.h"
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
    stg::selection _selection = stg::selection(_strategy);
    stg::mouse_handler _mouse_handler = stg::mouse_handler(_strategy,
                                                           _selection,
                                                           [] { return ApplicationSettings::defaultSlotHeight; },
                                                           [this] { return scrollboardScrollArea()->viewportRectRelativeToContent(); });

    stg::action_center _action_center = stg::action_center(_strategy, _mouse_handler, _selection);

    SessionsMainWidget *sessionsMainWidget;
    ActivityListWidget *activitiesWidget;
};

#endif //STRATEGR_MAINSCENE_H
