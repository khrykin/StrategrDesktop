//
// Created by Dmitry Khrykin on 2019-07-10.
//

#include <functional>

#include <QDebug>
#include <QPainter>

#include "sessionsmainwidget.h"
#include "overviewwidget.h"
#include "strategysettingswidget.h"
#include "currentsessionwidget.h"
#include "slotboardwidget.h"
#include "notifierbackend.h"
#include "mainwindow.h"
#include "slotboardscrollarea.h"

SessionsMainWidget::SessionsMainWidget(stg::strategy &strategy,
                                       QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, toolbarHeight(), 0, 0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    layoutChildWidgets();

    notifier.on_send_notiifcation = [this](const stg::notification &notification) {
        std::cout << "send notification: " << notification << "\n";
        notifierBackend.sendMessage(
                QString::fromStdString(notification.title),
                QString::fromStdString(notification.message));
    };

    notifierTimer = new QTimer(this);
    notifierTimer->setInterval(ApplicationSettings::notifierTimerMillisecondsInterval);
    connect(notifierTimer,
            &QTimer::timeout,
            std::bind(&stg::notifier::send_now_if_needed,
                      &notifier,
                      ApplicationSettings::notifierTimerMillisecondsInterval / 1000));

    notifierTimer->start();

    strategy.time_slots().add_on_change_callback([this] {
        strategySettingsWidget->reloadStrategy();
    });
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

    _slotBoardScrollArea = new SlotboardScrollArea(this);
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

    auto *topHangingWidget = new QWidget();
    topHangingWidget->setObjectName("topHangingWidget");
    topHangingWidget->installEventFilter(this);

    auto *secondaryLayout = new QVBoxLayout();
    secondaryLayout->setSpacing(0);
    secondaryLayout->setContentsMargins(0, 0, 0, 0);

    topHangingWidget->setLayout(secondaryLayout);

    secondaryLayout->addWidget(strategySettingsWidget);
    secondaryLayout->addWidget(overviewWidget);
    secondaryLayout->addWidget(currentSessionWidget);

    auto *mainLayout = qobject_cast<QVBoxLayout *>(layout());
    mainLayout->addWidget(topHangingWidget);
    mainLayout->addStretch();

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

    notifier.schedule();
}

void SessionsMainWidget::clearSelection() {
    slotBoard->clearSelection();
}

void SessionsMainWidget::updateTimerDependants() {
    overviewWidget->update();
    if (!strategy.is_dragging() && !strategy.is_resizing())
        currentSessionWidget->reloadSessionIfNeeded();
}

void SessionsMainWidget::paintEvent(QPaintEvent *paintEvent) {
    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());
    painter.drawRect(0, 0, width(), height());
}

void SessionsMainWidget::resizeEvent(QResizeEvent *event) {
    _slotBoardScrollArea->setGeometry(contentsRect());
}

SlotboardScrollArea *SessionsMainWidget::slotBoardScrollArea() const {
    return _slotBoardScrollArea;
}

bool SessionsMainWidget::eventFilter(QObject *object, QEvent *event) {
    if (object->objectName() == "topHangingWidget" && event->type() == QEvent::Resize) {
        auto *resizeEvent = dynamic_cast<QResizeEvent *>(event);
        auto newTopOffset = toolbarHeight() + resizeEvent->size().height();
        _slotBoardScrollArea->setViewportTopOffset(newTopOffset);
    }

    return false;
}

int SessionsMainWidget::toolbarHeight() {
    return MainWindow::toolbarHeightOf(this->window());
}

