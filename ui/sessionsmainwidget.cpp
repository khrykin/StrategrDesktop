//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include <QDebug>
#include <QPainter>

#include "sessionsmainwidget.h"
#include "notifierimplementation.h"
#include "overviewwidget.h"
#include "strategysettingswidget.h"
#include "currentsessionwidget.h"
#include "slotboardwidget.h"

SessionsMainWidget::SessionsMainWidget(stg::strategy &strategy,
                                       QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    layoutChildWidgets();

    notifier = new NotifierImplementation(&strategy, this);
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

    currentSessionWidget = new CurrentSessionWidget(strategy);
    currentSessionWidget->hide();

    connect(currentSessionWidget,
            &CurrentSessionWidget::clicked,
            this,
            &SessionsMainWidget::focusOnCurrentTime);

    _slotBoardScrollArea = new QScrollArea();
    _slotBoardScrollArea->setWidgetResizable(true);
    _slotBoardScrollArea->setFrameShape(QFrame::NoFrame);

    slotBoard = new SlotBoardWidget(strategy);

    connect(slotBoard,
            &SlotBoardWidget::timerTick,
            this,
            &SessionsMainWidget::updateTimerDependants);

    connect(slotBoard,
            &SlotBoardWidget::timeSlotsChange,
            this,
            &SessionsMainWidget::updateOverviewWidget);

    _slotBoardScrollArea->setWidget(slotBoard);
    overviewWidget = new OverviewWidget(strategy, _slotBoardScrollArea);

    layout()->addWidget(strategySettingsWidget);
    layout()->addWidget(overviewWidget);
    layout()->addWidget(currentSessionWidget);
    layout()->addWidget(_slotBoardScrollArea);

    slotBoard->updateCurrentTimeMarker();
}

void SessionsMainWidget::updateOverviewWidget() const {
    overviewWidget->update();
}

void SessionsMainWidget::reloadStrategy() {
    slotBoard->reloadStrategy();
    strategySettingsWidget->reloadStrategy();
    overviewWidget->reloadStrategy();
    currentSessionWidget->reloadStrategy();

    notifier->setStrategy(&strategy);
}

void SessionsMainWidget::clearSelection() {
    slotBoard->clearSelection();
}


void SessionsMainWidget::updateTimerDependants() {
    overviewWidget->update();
    currentSessionWidget->reloadSessionIfNeeded();
}

void SessionsMainWidget::paintEvent(QPaintEvent *paintEvent) {
    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());
    painter.drawRect(0, 0, width(), height());
}

void SessionsMainWidget::resizeEvent(QResizeEvent *event) {
}

QScrollArea *SessionsMainWidget::slotBoardScrollArea() const {
    return _slotBoardScrollArea;
}
