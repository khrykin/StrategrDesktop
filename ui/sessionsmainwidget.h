//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_SESSIONSMAINWIDGET_H
#define STRATEGR_SESSIONSMAINWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>

#include "strategysettingswidget.h"
#include "currentsessionwidget.h"
#include "slotboard.h"
#include "Strategy.h"
#include "notifierimplementation.h"
#include "colorprovider.h"
#include "overviewwidget.h"

class SessionsMainWidget : public QWidget, public ColorProvider {
public:
    explicit SessionsMainWidget(Strategy &strategy,
                                QWidget *parent = nullptr);

    void toggleStrategySettingsOpen();
    void focusOnCurrentTime();

    const SelectionWidget::RawSelectionState &selection();
    void clearSelection();

    void reloadStrategy();
private:
    Strategy &strategy;

    NotifierImplementation *notifier = nullptr;
    StrategySettingsWidget *strategySettingsWidget = nullptr;
    CurrentSessionWidget *currentSessionWidget = nullptr;
    QScrollArea *slotBoardScrollArea = nullptr;
    SlotBoard *slotBoard = nullptr;
    OverviewWidget *overviewWidget = nullptr;

    // Qt's isVisible() could be glitchy for some reason,
    // so we dont't rely on it and use this flag
    bool currentSessionWidgetIsVisible = false;

    void layoutChildWidgets();
    void updateTimerDependants();

    void paintEvent(QPaintEvent *paintEvent) override;
    void updateOverviewWidget() const;

    void resizeEvent(QResizeEvent *) override;
};


#endif //STRATEGR_SESSIONSMAINWIDGET_H
