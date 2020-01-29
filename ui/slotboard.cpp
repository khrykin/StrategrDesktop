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
#include "CurrentTimeMarker.h"

SlotBoard::SlotBoard(Strategy &strategy, QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);

    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    layoutChildWidgets(mainLayout);

    setupCurrentTimeTimer();
}

void SlotBoard::setupCurrentTimeTimer() {
    currentTimeTimer.setCallback(this, &SlotBoard::timerCallback);
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

    for (auto &timeSlot : strategy.timeSlots()) {
        auto label = QStringForMinutes(timeSlot.beginTime);
        labels.append(TimeLabel{label, timeSlot.beginTime});
    }

    auto endTimeLabel = QStringForMinutes(strategy.endTime());
    labels.append(TimeLabel{endTimeLabel, strategy.endTime()});

    return labels;
}

void SlotBoard::clearSelection() {
    slotsWidget->deselectAllSlots();
}

const SelectionWidget::RawSelectionState &SlotBoard::selection() {
    return slotsWidget->selection();
}

void SlotBoard::updateCurrentTimeMarker() {
    auto currentTimeMarker = CurrentTimeMarker(strategy);

    auto rect = currentTimeMarker.rectInParent(
            slotsWidget->geometry(),
            CurrentTimeMarkerWidget::markerSize
    );

    currentTimeMarkerWidget->setGeometry(rect.to<QRect>());

    if (currentTimeMarker.isHidden()
        && currentTimeMarkerWidget->isVisible()) {
        currentTimeMarkerWidget->hide();
    } else if (currentTimeMarker.isVisible()
               && currentTimeMarkerWidget->isHidden()) {
        currentTimeMarkerWidget->show();
    }
}

void SlotBoard::focusOnCurrentTime() {
    auto topOffset = CurrentTimeMarker(strategy)
            .scrollOffsetInParent(
                    slotsWidget->geometry(),
                    window()->geometry().height()
            );

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

