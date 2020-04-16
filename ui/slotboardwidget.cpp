#include <algorithm>
#include <functional>

#include <QHBoxLayout>
#include <QLocale>
#include <QTimer>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QStyleOption>
#include <QGraphicsBlurEffect>
#include <QPainter>

#include "slotboardwidget.h"
#include "utils.h"
#include "mainwindow.h"
#include "slotruler.h"
#include "currenttimemarker.h"
#include "slotboardcircleswidget.h"
#include "slotsmousehandler.h"

SlotBoardWidget::SlotBoardWidget(stg::strategy &strategy, QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);

    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    layoutChildWidgets(mainLayout);

    setupCurrentTimeTimer();
}

void SlotBoardWidget::setupCurrentTimeTimer() {
    currentTimeTimer = new QTimer(this);
    currentTimeTimer->setInterval(1000 * ApplicationSettings::currentTimeTimerSecondsInterval);
    connect(currentTimeTimer,
            &QTimer::timeout,
            this,
            &SlotBoardWidget::timerCallback);

    currentTimeTimer->start();
}

void SlotBoardWidget::layoutChildWidgets(QHBoxLayout *mainLayout) {
    slotsWidget = new SlotsWidget(strategy);
    connect(slotsWidget,
            &SlotsWidget::sessionsChanged,
            this,
            &SlotBoardWidget::handleTimeSlotsChange);


    slotRuler = new SlotRuler(makeLabelsForStrategy(),
                              slotsWidget->slotHeight());

    _slotsLayout = new QVBoxLayout();
    _slotsLayout->setSpacing(0);
    _slotsLayout->addWidget(slotsWidget);
    _slotsLayout->addStretch();

    updateSlotsLayout();

    mainLayout->addWidget(slotRuler);
    mainLayout->addLayout(_slotsLayout);

    circlesWidget = new SlotBoardCirclesWidget(
            std::bind(&SlotsWidget::slotHeight, slotsWidget),
            std::bind(&SlotsWidget::geometry, slotsWidget),
            this
    );

    connect(slotsWidget,
            &SlotsWidget::resizeBoundaryChanged,
            circlesWidget,
            &SlotBoardCirclesWidget::updateResizeBoundary);

    circlesWidget->setGeometry(geometry());

    currentTimeMarkerWidget = new CurrentTimeMarkerWidget(this);

    updateCurrentTimeMarker();
}

void SlotBoardWidget::updateSlotsLayout() const {
    _slotsLayout->setContentsMargins(0, slotsWidget->slotHeight() / 2, 0, 0);
}

void SlotBoardWidget::reloadStrategy() {
    slotsWidget->reloadStrategy();

    updateUI();
}

void SlotBoardWidget::updateUI() {
    slotRuler->setLabels(makeLabelsForStrategy());
    updateCurrentTimeMarker();
}

QVector<TimeLabel> SlotBoardWidget::makeLabelsForStrategy() {
    QVector<TimeLabel> labels;

    for (auto &time : strategy.time_slots().times()) {
        auto labelText = QStringForMinutes(time);
        auto label = TimeLabel{labelText, time};

        labels.append(label);
    }

    return labels;
}

void SlotBoardWidget::clearSelection() {
    slotsWidget->deselectAllSlots();
}

void SlotBoardWidget::updateCurrentTimeMarker() {
    auto currentTimeMarker = stg::current_time_marker(strategy);

    auto rect = currentTimeMarker.rect_in_parent(slotsWidget->geometry(),
                                                 CurrentTimeMarkerWidget::markerSize);

    currentTimeMarkerWidget->setGeometry(rect);

    if (currentTimeMarker.is_hidden() &&
        !currentTimeMarkerWidget->isHidden()) {
        currentTimeMarkerWidget->hide();

    } else if (currentTimeMarker.is_visible() &&
               currentTimeMarkerWidget->isHidden()) {
        currentTimeMarkerWidget->show();
    }
}

void SlotBoardWidget::focusOnCurrentTime() {
    auto topOffset = stg::current_time_marker(strategy)
            .scroll_offset_in_parent(slotsWidget->geometry(),
                                     parentWidget()->contentsRect().height());

    auto scrollBar = parentScrollArea()->verticalScrollBar();

    auto animation = new QPropertyAnimation(scrollBar, "value");
    animation->setDuration(200);
    animation->setStartValue(scrollBar->value());
    animation->setEndValue(topOffset);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    animation->start();
}

QScrollArea *SlotBoardWidget::parentScrollArea() {
    return qobject_cast<QScrollArea *>(parentWidget()->parentWidget());
}

void SlotBoardWidget::handleTimeSlotsChange() {
    updateCurrentTimeMarker();
    slotRuler->setLabels(makeLabelsForStrategy());

    emit timeSlotsChange();
}

void SlotBoardWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());

    painter.drawRect(QRect(0, 0, width(), height()));
}

void SlotBoardWidget::timerCallback() {
    emit timerTick();
    updateCurrentTimeMarker();
}

void SlotBoardWidget::resizeEvent(QResizeEvent *event) {
    updateCurrentTimeMarker();

    circlesWidget->setGeometry(QRect(0, 0, width(), height()));
}

QVBoxLayout *SlotBoardWidget::slotsLayout() const {
    return _slotsLayout;
}

