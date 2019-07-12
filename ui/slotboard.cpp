#include <algorithm>
#include <cmath>

#include <QDebug>
#include <QHBoxLayout>
#include <QLocale>
#include <QTime>
#include <QPropertyAnimation>
#include <QScrollBar>

#include "slotboard.h"
#include "utils.h"
#include "mainwindow.h"

SlotBoard::SlotBoard(Strategy *strategy, QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);

    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    layoutChildWidgets(mainLayout);

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

    auto *slotsLayout = new QVBoxLayout();
    slotsLayout->setSpacing(0);
    slotsLayout->setMargin(0);

    auto *header = makeHeader();
    auto *footer = makeFooter();

    slotsLayout->addWidget(header);
    slotsLayout->addWidget(slotsWidget);
    slotsLayout->addWidget(footer);
    slotsLayout->addStretch();

    mainLayout->addWidget(slotRuler);
    mainLayout->addLayout(slotsLayout);

    currentTimeMarker = new CurrentTimeMarker(this);
}

QWidget *SlotBoard::makeHeader() {
    auto *header = new QLabel(tr("Begin"));
    header->setAlignment(Qt::AlignCenter);
    header->setFixedHeight(slotsWidget->slotHeight());
    header->setStyleSheet("color: #c3c3c3;"
                          "font-weight: bold;"
                          "background: #f4f4f4;");
    return header;
}

QWidget *SlotBoard::makeFooter() {
    auto *footer = new QLabel(tr("End"));
    footer->setAlignment(Qt::AlignCenter);
    footer->setFixedHeight(slotsWidget->slotHeight());
    footer->setStyleSheet("color: #c3c3c3;"
                          "font-weight: bold;"
                          "background: #f4f4f4;");
    return footer;
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

QStringList SlotBoard::makeLabelsForStrategy() {
    QStringList labels;

    for (auto &timeSlot : strategy->timeSlots()) {
        auto label = qStringForMinutes(timeSlot.beginTime);
        labels.append(label);
    }

    auto endTimeLabel = qStringForMinutes(strategy->endTime());
    labels.append(endTimeLabel);

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

