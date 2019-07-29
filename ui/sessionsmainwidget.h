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
#include "strategy.h"
#include "notifier.h"
#include "colorprovider.h"

class SessionsMainWidget : public QWidget, public ColorProvider {
public:
    explicit SessionsMainWidget(Strategy *strategy,
                                QWidget *parent = nullptr);

    void toggleStrategySettingsOpen();
    void focusOnCurrentTime();

    const SelectionWidget::RawSelectionState &selection();
    void clearSelection();

    void setStrategy(Strategy *newStrategy);
private:
    Strategy *strategy;

    Notifier *notifier = nullptr;
    StrategySettingsWidget *strategySettingsWidget = nullptr;
    CurrentSessionWidget *currentSessionWidget = nullptr;
    QScrollArea *slotBoardScrollArea = nullptr;
    SlotBoard *slotBoard = nullptr;

    // Qt's isVisible() could be glitchy for some reason,
    // so we dont't rely on it and use this flag
    bool currentSessionWidgetIsVisible = false;

    void layoutChildWidgets();
    void updateCurrentSessionWidget();

    void paintEvent(QPaintEvent *paintEvent) override;
};


#endif //STRATEGR_SESSIONSMAINWIDGET_H
