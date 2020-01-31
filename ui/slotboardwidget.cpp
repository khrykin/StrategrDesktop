#include <algorithm>
#include <functional>

#include <QHBoxLayout>
#include <QLocale>
#include <QTime>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QStyleOption>
#include <QGraphicsBlurEffect>

#include "slotboardwidget.h"
#include "utils.h"
#include "mainwindow.h"
#include "slotruler.h"
#include "currenttimemarker.h"
#include "slotboardcircleswidget.h"

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
    currentTimeTimer.set_callback(this, &SlotBoardWidget::timerCallback);
    currentTimeTimer.start(ApplicationSettings::currentTimeTimerSecondsInterval);
}

void SlotBoardWidget::layoutChildWidgets(QHBoxLayout *mainLayout) {
    slotsWidget = new SlotsWidget(strategy);
    connect(slotsWidget,
            &SlotsWidget::sessionsChanged,
            this,
            &SlotBoardWidget::handleTimeSlotsChange);


    slotRuler = new SlotRuler(makeLabelsForStrategy(),
                              slotsWidget->slotHeight());


    slotsLayout = new QVBoxLayout();
    slotsLayout->setSpacing(0);
    slotsLayout->addWidget(slotsWidget);
    slotsLayout->addStretch();

    updateSlotsLayout();

    mainLayout->addWidget(slotRuler);
    mainLayout->addLayout(slotsLayout);

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
}

void SlotBoardWidget::updateSlotsLayout() const {
    slotsLayout->setContentsMargins(0, slotsWidget->slotHeight() / 2, 0, 0);
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

    for (auto &timeSlot : strategy.time_slots()) {
        auto label = QStringForMinutes(timeSlot.begin_time);
        labels.append(TimeLabel{label, timeSlot.begin_time});
    }

    auto endTimeLabel = QStringForMinutes(strategy.end_time());
    labels.append(TimeLabel{endTimeLabel, strategy.end_time()});

    return labels;
}

void SlotBoardWidget::clearSelection() {
    slotsWidget->deselectAllSlots();
}

//const SelectionWidget::RawSelectionState &SlotBoardWidget::selection() {
//    return slotsWidget->selection();
//}

void SlotBoardWidget::updateCurrentTimeMarker() {
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

void SlotBoardWidget::focusOnCurrentTime() {
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
    dispatchToMainThread([this]() {
        emit timerTick();
        updateCurrentTimeMarker();
    });
}

void SlotBoardWidget::resizeEvent(QResizeEvent *event) {
    updateCurrentTimeMarker();

    circlesWidget->setGeometry(geometry());
}

