#include <algorithm>
#include <cmath>

#include <QDebug>
#include <QHBoxLayout>
#include <QLocale>
#include <QTime>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QStyleOption>
#include <QApplication>

#include "slotboard.h"
#include "utils.h"
#include "mainwindow.h"
#include "slotruler.h"
#include "macoswindow.h"

SlotBoard::SlotBoard(Strategy *strategy, QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);

    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    layoutChildWidgets(mainLayout);

    setupCurrentTimeTimer();
}

void SlotBoard::setupCurrentTimeTimer() {
    currentTimeTimer = new QTimer(this);
    connect(currentTimeTimer,
            &QTimer::timeout,
            this,
            &SlotBoard::updateCurrentTimeMarker);

    currentTimeTimer->start(ApplicationSettings::currentTimeTimerInterval);
}

void SlotBoard::layoutChildWidgets(QHBoxLayout *mainLayout) {
    slotsWidget = new SlotsWidget(strategy);
    connect(slotsWidget,
            &SlotsWidget::activitySessionsChanged,
            this,
            &SlotBoard::timeSlotsChange);

    slotRuler = new SlotRuler(makeLabelsForStrategy(),
                              slotsWidget->slotHeight());

    slotsLayout = new QVBoxLayout();
    slotsLayout->setSpacing(0);
    slotsLayout->addWidget(slotsWidget);
    slotsLayout->addStretch();

    updateSlotsLayout();

    mainLayout->addWidget(slotRuler);
    mainLayout->addLayout(slotsLayout);

    currentTimeMarker = new CurrentTimeMarker(this);
}

void SlotBoard::updateSlotsLayout() const {
    slotsLayout->setContentsMargins(0, slotsWidget->slotHeight() / 2, 0, 0);
}

void SlotBoard::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    slotsWidget->setStrategy(strategy);

    updateUI();
}

void SlotBoard::updateUI() {
    slotRuler->setLabels(makeLabelsForStrategy());
    updateCurrentTimeMarker();
}

QVector<TimeLabel> SlotBoard::makeLabelsForStrategy() {
    QVector<TimeLabel> labels;

    for (auto &timeSlot : strategy->timeSlots()) {
        auto label = QStringForMinutes(timeSlot.beginTime);
        labels.append(TimeLabel{label, timeSlot.beginTime});
    }

    auto endTimeLabel = QStringForMinutes(strategy->endTime());
    labels.append(TimeLabel{endTimeLabel, strategy->endTime()});

    return labels;
}

void SlotBoard::clearSelection() {
    slotsWidget->deselectAllSlots();
}

const SelectionWidget::RawSelectionState &SlotBoard::selection() {
    return slotsWidget->selection();
}

void SlotBoard::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateCurrentTimeMarker();
}

void SlotBoard::updateCurrentTimeMarker() {
    emit timerTick();

    auto topOffset = calculateTimeMarkerTopOffset();
    currentTimeMarkerTopOffset = static_cast<int>(topOffset);

    auto timeMarkerLowerBoundary = slotsWidget->geometry().top()
                                   + slotsWidget->height();

    if (topOffset < 0
        || currentTimeMarkerTopOffset > timeMarkerLowerBoundary) {
        if (currentTimeMarker->isVisible()) {
            currentTimeMarker->hide();
        }
    } else {
        if (currentTimeMarker->isHidden()) {
            currentTimeMarker->show();
        }
    }

    auto timeMarkerGeometry = calculateCurrentTimeMarkerGeometry();

    currentTimeMarker->setGeometry(timeMarkerGeometry);
}

QRect SlotBoard::calculateCurrentTimeMarkerGeometry() const {
    auto timeMarkerGeometry = QRect(
            slotRuler->width() - CurrentTimeMarker::markerSize,
            currentTimeMarkerTopOffset - CurrentTimeMarker::markerSize,
            slotsWidget->geometry().width() + CurrentTimeMarker::markerSize,
            2 * CurrentTimeMarker::markerSize);
    return timeMarkerGeometry;
}

double SlotBoard::calculateTimeMarkerTopOffset() const {
    auto currentTime = static_cast<double>(currentMinutes());

    auto startTime = strategy->beginTime();
    auto slotDuration = strategy->timeSlotDuration();
    auto slotHeight = slotsWidget->slotHeight();

    auto pxInMins = static_cast<double>(slotHeight) / slotDuration;

    auto topOffset = round(pxInMins * (currentTime - startTime)) +
                     slotsWidget->geometry().top();
    return topOffset;
}

void SlotBoard::focusOnCurrentTime() {
    auto topOffset = currentTimeMarkerTopOffset - parentWindow()->geometry().height() / 2;

    if (topOffset < 0) {
        topOffset = 0;
    } else if (topOffset > slotsWidget->geometry().height()) {
        topOffset = slotsWidget->geometry().height();
    }

    auto scrollBar = parentScrollArea()->verticalScrollBar();

    QPropertyAnimation *animation = new QPropertyAnimation(scrollBar, "value");
    animation->setDuration(200);
    animation->setStartValue(scrollBar->value());
    animation->setEndValue(topOffset);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    animation->start();
}

QScrollArea *SlotBoard::parentScrollArea() {
    return qobject_cast<QScrollArea *>(parentWidget()->parentWidget());
}

void SlotBoard::timeSlotsChange() {
    updateCurrentTimeMarker();

    slotRuler->setLabels(makeLabelsForStrategy());
}

void SlotBoard::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());

    painter.drawRect(QRect(0, 0, width(), height()));
}

