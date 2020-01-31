//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_SESSIONSMAINWIDGET_H
#define STRATEGR_SESSIONSMAINWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>

#include "strategy.h"
#include "colorprovider.h"
#include "selectionwidget.h"

class NotifierImplementation;
class StrategySettingsWidget;
class CurrentSessionWidget;
class QScrollArea;
class SlotBoardWidget;
class OverviewWidget;

class SessionsMainWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit SessionsMainWidget(stg::strategy &strategy,
                                QWidget *parent = nullptr);

    void toggleStrategySettingsOpen();
    void focusOnCurrentTime();

    void clearSelection();
    void reloadStrategy();

    QScrollArea *slotBoardScrollArea() const;
private:
    stg::strategy &strategy;

    NotifierImplementation *notifier = nullptr;
    StrategySettingsWidget *strategySettingsWidget = nullptr;
    CurrentSessionWidget *currentSessionWidget = nullptr;
    QScrollArea *_slotBoardScrollArea = nullptr;
    SlotBoardWidget *slotBoard = nullptr;
    OverviewWidget *overviewWidget = nullptr;

    void layoutChildWidgets();
    void updateTimerDependants();

    void paintEvent(QPaintEvent *paintEvent) override;
    void updateOverviewWidget() const;

    void resizeEvent(QResizeEvent *) override;
};


#endif //STRATEGR_SESSIONSMAINWIDGET_H
