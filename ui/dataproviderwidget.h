//
// Created by Dmitry Khrykin on 16.06.2020.
//

#ifndef STRATEGR_DATAPROVIDERWIDGET_H
#define STRATEGR_DATAPROVIDERWIDGET_H

#include <QWidget>

#include "actioncenter.h"
#include "mousehandler.h"
#include "selection.h"
#include "strategy.h"

class MainScene;
class SlotboardScrollArea;
class SlotsWidget;
class DataProviderWidget : public QWidget {
public:
    using QWidget::QWidget;

protected:
    MainScene *mainScene();
    stg::strategy &strategy();
    stg::selection &selection();
    stg::mouse_handler &mouseHandler();
    stg::action_center &actionCenter();

    SlotboardScrollArea *slotboardScrollArea();
    SlotsWidget *slotsWidget();

    int toolbarHeight();
    int slotHeight();
};


#endif//STRATEGR_DATAPROVIDERWIDGET_H
