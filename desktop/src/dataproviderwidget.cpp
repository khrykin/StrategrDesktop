//
// Created by Dmitry Khrykin on 16.06.2020.
//

#include "dataproviderwidget.h"
#include "applicationsettings.h"
#include "mainscene.h"
#include "mainwindow.h"
#include "slotswidget.h"

MainScene *DataProviderWidget::mainScene() {
    auto *mainWindow = dynamic_cast<MainWindow *>(window());
    return mainWindow->scene() ? mainWindow->scene() : mainWindow->findChild<MainScene *>();
}

stg::strategy &DataProviderWidget::strategy() {
    return mainScene()->strategy();
}

stg::mouse_handler &DataProviderWidget::mouseHandler() {
    return mainScene()->mouseHandler();
}

stg::action_center &DataProviderWidget::actionCenter() {
    return mainScene()->actionCenter();
}

stg::selection &DataProviderWidget::selection() {
    return mainScene()->selection();
}

int DataProviderWidget::toolbarHeight() {
    auto *mainWindow = dynamic_cast<MainWindow *>(window());
    return mainWindow->toolbarHeight();
}

int DataProviderWidget::slotHeight() {
    return ApplicationSettings::defaultSlotHeight;
}

SlotboardScrollArea *DataProviderWidget::slotboardScrollArea() {
    return mainScene()->scrollboardScrollArea();
}

SlotsWidget *DataProviderWidget::slotsWidget() {
    return mainScene()->slotsWidget();
}
