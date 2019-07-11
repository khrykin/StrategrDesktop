//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_SESSIONSWIDGET_H
#define STRATEGR_SESSIONSWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>

#include "strategysettingswidget.h"
#include "currentsessionwidget.h"
#include "slotboard.h"
#include "strategy.h"

class SessionsWidget : public QWidget {
public:
    explicit SessionsWidget(Strategy *strategy,
                            QWidget *parent = nullptr);

    void toggleStrategySettingsOpen();
    void focusOnCurrentTime();

    void clearSelection();

    void setStrategy(Strategy *newStrategy);
private:
    Strategy *strategy;

    StrategySettingsWidget *strategySettingsWidget = nullptr;
    CurrentSessionWidget *currentSessionWidget = nullptr;
    QScrollArea *slotBoardScrollArea = nullptr;
    SlotBoard *slotBoard = nullptr;

    void layoutChildWidgets();
};


#endif //STRATEGR_SESSIONSWIDGET_H
