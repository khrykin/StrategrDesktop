//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include "sessionsmainwidget.h"

SessionsMainWidget::SessionsMainWidget(Strategy *strategy,
                               QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    layoutChildWidgets();

//        currentActivityWidget = new CurrentActivityWidget(this);
//        currentActivityWidget->setProgress(0.75);
//        currentActivityWidget->hide();

//        connect(currentActivityWidget, &CurrentActivityWidget::clicked, this,
//                &MainWindow::focusOnCurrentTime);



//        auto label = new QLabel("SessionsMainWidget");
//        label->setAlignment(Qt::AlignCenter);
//
//        layout()->addWidget(label);
}

void SessionsMainWidget::toggleStrategySettingsOpen() {
    if (strategySettingsWidget->isHidden()) {
        strategySettingsWidget->slideAndShow();
    } else {
        strategySettingsWidget->slideAndHide();
    }
}

void SessionsMainWidget::focusOnCurrentTime() {

}

void SessionsMainWidget::layoutChildWidgets() {
    strategySettingsWidget = new StrategySettingsWidget(nullptr, nullptr);
    layout()->addWidget(strategySettingsWidget);

    currentSessionWidget = new CurrentSessionWidget();
    layout()->addWidget(currentSessionWidget);


    slotBoardScrollArea = new QScrollArea();
    slotBoardScrollArea->setWidgetResizable(true);
    slotBoardScrollArea->setMouseTracking(true);
    slotBoardScrollArea->setFrameShape(QFrame::NoFrame);

    slotBoard = new SlotBoard(strategy);

    slotBoardScrollArea->setWidget(slotBoard);

    layout()->addWidget(slotBoardScrollArea);
}

void SessionsMainWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    slotBoard->setStrategy(newStrategy);
}

void SessionsMainWidget::clearSelection() {
    slotBoard->clearSelection();
}

const SelectionWidget::RawSelectionState &SessionsMainWidget::selection() {
    return slotBoard->selection();
}
