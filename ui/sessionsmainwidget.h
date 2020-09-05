//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_SESSIONSMAINWIDGET_H
#define STRATEGR_SESSIONSMAINWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>

#include "dataproviderwidget.h"
#include "colorprovider.h"
#include "selectionwidget.h"
#include "notifier.h"
#include "notifierbackend.h"

class StrategySettingsWidget;
class CurrentSessionWidget;
class SlotBoardWidget;
class OverviewWidget;
class SlotboardScrollArea;

class SessionsMainWidget : public DataProviderWidget,
                           public ColorProvider {
public:
    explicit SessionsMainWidget(QWidget *parent);

    void toggleStrategySettingsOpen();

    void reloadStrategy();

    SlotboardScrollArea *slotboardScrollArea() const;
private:
    stg::notifier notifier = stg::notifier(strategy());

    StrategySettingsWidget *strategySettingsWidget = nullptr;
    CurrentSessionWidget *currentSessionWidget = nullptr;

    SlotboardScrollArea *_slotBoardScrollArea = nullptr;
    SlotBoardWidget *slotBoard = nullptr;
    OverviewWidget *overviewWidget = nullptr;

    void layoutChildWidgets();

    void paintEvent(QPaintEvent *paintEvent) override;
    void resizeEvent(QResizeEvent *) override;
    bool eventFilter(QObject *object, QEvent *event) override;
};


#endif //STRATEGR_SESSIONSMAINWIDGET_H
