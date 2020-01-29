#include <algorithm>

#include <QDebug>
#include <QHBoxLayout>
#include <QLocale>
#include <QTime>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QStyleOption>
#include <QGraphicsBlurEffect>

#include "slotboard.h"
#include "utils.h"
#include "mainwindow.h"
#include "slotruler.h"
#include "currenttimemarker.h"

SlotBoard::SlotBoard(stg::strategy &strategy, QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);

    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    layoutChildWidgets(mainLayout);

    setupCurrentTimeTimer();
}

void SlotBoard::setupCurrentTimeTimer() {
    currentTimeTimer.set_callback(this, &SlotBoard::timerCallback);
    currentTimeTimer.start(ApplicationSettings::currentTimeTimerSecondsInterval);
}

void SlotBoard::layoutChildWidgets(QHBoxLayout *mainLayout) {
    slotsWidget = new SlotsWidget(strategy);
    connect(slotsWidget,
            &SlotsWidget::sessionsChanged,
            this,
            &SlotBoard::handleTimeSlotsChange);

    slotRuler = new SlotRuler(makeLabelsForStrategy(),
                              slotsWidget->slotHeight());

    slotsLayout = new QVBoxLayout();
    slotsLayout->setSpacing(0);
    slotsLayout->addWidget(slotsWidget);
    slotsLayout->addStretch();

    updateSlotsLayout();

    mainLayout->addWidget(slotRuler);
    mainLayout->addLayout(slotsLayout);

    currentTimeMarkerWidget = new CurrentTimeMarkerWidget(this);
}

void SlotBoard::updateSlotsLayout() const {
    slotsLayout->setContentsMargins(0, slotsWidget->slotHeight() / 2, 0, 0);
}

void SlotBoard::reloadStrategy() {
    slotsWidget->reloadStrategy();

    updateUI();
}

void SlotBoard::updateUI() {
    slotRuler->setLabels(makeLabelsForStrategy());
    updateCurrentTimeMarker();
}

QVector<TimeLabel> SlotBoard::makeLabelsForStrategy() {
    QVector<TimeLabel> labels;

    for (auto &timeSlot : strategy.time_slots()) {
        auto label = QStringForMinutes(timeSlot.begin_time);
        labels.append(TimeLabel{label, timeSlot.begin_time});
    }

    auto endTimeLabel = QStringForMinutes(strategy.end_time());
    labels.append(TimeLabel{endTimeLabel, strategy.end_time()});

    return labels;
}

void SlotBoard::clearSelection() {
    slotsWidget->deselectAllSlots();
}

const SelectionWidget::RawSelectionState &SlotBoard::selection() {
    return slotsWidget->selection();
}

void SlotBoard::updateCurrentTimeMarker() {
    auto currentTimeMarker = stg::current_time_marker(strategy);

    auto rect = currentTimeMarker.rect_in_parent(slotsWidget->geometry(),
                                                 CurrentTimeMarkerWidget::markerSize);

    currentTimeMarkerWidget->setGeometry(rect);

    if (currentTimeMarker.is_hidden()
        && currentTimeMarkerWidget->isVisible()) {
        currentTimeMarkerWidget->hide();
    } else if (currentTimeMarker.is_visible()
               && currentTimeMarkerWidget->isHidden()) {
        currentTimeMarkerWidget->show();
    }
}

void SlotBoard::focusOnCurrentTime() {
    auto topOffset = stg::current_time_marker(strategy)
            .scroll_offset_in_parent(slotsWidget->geometry(),
                                     window()->geometry().height());

    auto scrollBar = parentScrollArea()->verticalScrollBar();

    auto animation = new QPropertyAnimation(scrollBar, "value");
    animation->setDuration(200);
    animation->setStartValue(scrollBar->value());
    animation->setEndValue(topOffset);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    animation->start();
}

QScrollArea *SlotBoard::parentScrollArea() {
    return qobject_cast<QScrollArea *>(parentWidget()->parentWidget());
}

void SlotBoard::handleTimeSlotsChange() {
    updateCurrentTimeMarker();
    slotRuler->setLabels(makeLabelsForStrategy());

    emit timeSlotsChange();
}

void SlotBoard::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());

    painter.drawRect(QRect(0, 0, width(), height()));
}

void SlotBoard::timerCallback() {
    dispatchToMainThread([&]() {
        emit timerTick();
        updateCurrentTimeMarker();
    });
}

void SlotBoard::resizeEvent(QResizeEvent *event) {
    updateCurrentTimeMarker();
}

