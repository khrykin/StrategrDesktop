//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include "sessionsmainwidget.h"
#include "currentactivitysession.h"
#include <QDebug>

SessionsMainWidget::SessionsMainWidget(Strategy *strategy,
                                       QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    layoutChildWidgets();
}

void SessionsMainWidget::toggleStrategySettingsOpen() {
    if (strategySettingsWidget->isHidden()) {
        strategySettingsWidget->slideAndShow();
    } else {
        strategySettingsWidget->slideAndHide();
    }
}

void SessionsMainWidget::focusOnCurrentTime() {
    slotBoard->focusOnCurrentTime();
}

void SessionsMainWidget::layoutChildWidgets() {
    strategySettingsWidget = new StrategySettingsWidget(strategy);
    layout()->addWidget(strategySettingsWidget);

    currentSessionWidget = new CurrentSessionWidget();
    currentSessionWidget->hide();
    layout()->addWidget(currentSessionWidget);

    connect(currentSessionWidget,
            &CurrentSessionWidget::clicked, this,
            &SessionsMainWidget::focusOnCurrentTime);

    slotBoardScrollArea = new QScrollArea();
    slotBoardScrollArea->setWidgetResizable(true);
    slotBoardScrollArea->setMouseTracking(true);
    slotBoardScrollArea->setFrameShape(QFrame::NoFrame);

    slotBoard = new SlotBoard(strategy);

    connect(slotBoard,
            &SlotBoard::timerTick,
            this,
            &SessionsMainWidget::updateCurrentSessionWidget);

    slotBoardScrollArea->setWidget(slotBoard);

    layout()->addWidget(slotBoardScrollArea);
}

void SessionsMainWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    slotBoard->setStrategy(newStrategy);
    strategySettingsWidget->setStrategy(newStrategy);
}

void SessionsMainWidget::clearSelection() {
    slotBoard->clearSelection();
}

const SelectionWidget::RawSelectionState &SessionsMainWidget::selection() {
    return slotBoard->selection();
}

void SessionsMainWidget::updateCurrentSessionWidget() {
    strategySettingsWidget->setStrategy(strategy);

    auto currentSession = CurrentActivitySession::forStrategy(*strategy);
    if (currentSession) {
        currentSessionWidget->setActivitySession(*currentSession);
        if (!currentSessionWidgetIsVisible) {
            currentSessionWidgetIsVisible = true;
            currentSessionWidget->slideAndShow();
        }
    } else {
        if (currentSessionWidgetIsVisible) {
            currentSessionWidgetIsVisible = false;
            currentSessionWidget->slideAndHide();
        }
    }
}
