//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include <QDebug>
#include <QPainter>

#include "sessionsmainwidget.h"
#include "currentactivitysession.h"

SessionsMainWidget::SessionsMainWidget(Strategy *strategy,
                                       QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    layoutChildWidgets();

    notifier = new Notifier(strategy, this);
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

    currentSessionWidget = new CurrentSessionWidget();
    currentSessionWidget->hide();

    connect(currentSessionWidget,
            &CurrentSessionWidget::clicked, this,
            &SessionsMainWidget::focusOnCurrentTime);

    slotBoardScrollArea = new QScrollArea();
    slotBoardScrollArea->setWidgetResizable(true);
    slotBoardScrollArea->setFrameShape(QFrame::NoFrame);

    slotBoard = new SlotBoard(strategy);

    connect(slotBoard,
            &SlotBoard::timerTick,
            this,
            &SessionsMainWidget::updateTimerDependants);

    connect(slotBoard,
            &SlotBoard::timeSlotsChange,
            this,
            &SessionsMainWidget::updateOverviewWidget);

    slotBoardScrollArea->setWidget(slotBoard);
    overviewWidget = new OverviewWidget(strategy, slotBoardScrollArea);

    layout()->addWidget(strategySettingsWidget);
    layout()->addWidget(overviewWidget);
    layout()->addWidget(currentSessionWidget);
    layout()->addWidget(slotBoardScrollArea);
}

void SessionsMainWidget::updateOverviewWidget() const { overviewWidget->update(); }

void SessionsMainWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    slotBoard->setStrategy(newStrategy);
    strategySettingsWidget->setStrategy(newStrategy);
    notifier->setStrategy(newStrategy);
    overviewWidget->setStrategy(newStrategy);
}

void SessionsMainWidget::clearSelection() {
    slotBoard->clearSelection();
}

const SelectionWidget::RawSelectionState &SessionsMainWidget::selection() {
    return slotBoard->selection();
}

void SessionsMainWidget::updateTimerDependants() {
    strategySettingsWidget->setStrategy(strategy);
    overviewWidget->update();

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

void SessionsMainWidget::paintEvent(QPaintEvent *paintEvent) {
    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());
    painter.drawRect(0, 0, width(), height());
}

void SessionsMainWidget::resizeEvent(QResizeEvent *) {
    overviewWidget->update();
}
