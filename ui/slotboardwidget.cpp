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
#include "slotboardscrollarea.h"

SlotBoardWidget::SlotBoardWidget(stg::strategy &strategy, QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);

    setMouseTracking(true);

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
    _slotsWidget = new SlotsWidget(strategy, this);
    connect(_slotsWidget,
            &SlotsWidget::sessionsChanged,
            this,
            &SlotBoardWidget::handleTimeSlotsChange);

    slotRuler = new SlotRuler(makeLabels(), _slotsWidget->slotHeight(), this);

    _slotsLayout = new QVBoxLayout();
    _slotsLayout->setSpacing(0);
    _slotsLayout->addWidget(_slotsWidget);
    _slotsLayout->addStretch();

    updateSlotsLayout();

    mainLayout->addWidget(slotRuler);
    mainLayout->addLayout(_slotsLayout);

    circlesWidget = new SlotBoardCirclesWidget(std::bind(&SlotsWidget::slotHeight, _slotsWidget),
                                               std::bind(&SlotsWidget::geometry, _slotsWidget),
                                               this);

    connect(_slotsWidget,
            &SlotsWidget::resizeBoundaryChanged,
            circlesWidget,
            &SlotBoardCirclesWidget::updateResizeBoundary);

    connect(_slotsWidget,
            &SlotsWidget::resizeBoundaryChanged,
            slotRuler,
            &SlotRuler::updateList);

    connect(_slotsWidget,
            &SlotsWidget::drawDraggedSession,
            this,
            &SlotBoardWidget::drawDraggedSession);

    circlesWidget->setGeometry(geometry());

    currentTimeMarkerWidget = new CurrentTimeMarkerWidget(this);

    updateCurrentTimeMarker();
}

void SlotBoardWidget::updateSlotsLayout() const {
    _slotsLayout->setContentsMargins(0, _slotsWidget->slotHeight() / 2, 0, 0);
}

void SlotBoardWidget::reloadStrategy() {
    _slotsWidget->reloadStrategy();

    updateUI();
}

void SlotBoardWidget::updateUI() {
    slotRuler->setLabels(makeLabels());
    updateCurrentTimeMarker();
}

QVector<TimeLabel> SlotBoardWidget::makeLabels() {
    QVector<TimeLabel> labels;

    for (auto &time : strategy.time_slots().times()) {
        auto labelText = QStringForMinutes(time);
        auto label = TimeLabel{labelText, time};

        labels.append(label);
    }

    return labels;
}

void SlotBoardWidget::clearSelection() {
    _slotsWidget->deselectAllSlots();
}

void SlotBoardWidget::updateCurrentTimeMarker() {
    auto currentTimeMarker = stg::current_time_marker(strategy, CurrentTimeMarkerWidget::markerSize);

    auto rect = QRect(currentTimeMarker.rect_in_parent(_slotsWidget->geometry())).translated(0, 1);

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
    auto slotsRect = QRect(0,
                           slotsLayout()->contentsMargins().top(),
                           slotsWidget()->width(),
                           slotsWidget()->height());

    auto topOffset = stg::current_time_marker(strategy, 5)
            .scroll_offset(slotsRect,
                           scrollArea()->viewportRect());

    topOffset++;

    auto *scrollBar = scrollArea()->verticalScrollBar();

    auto *animation = new QPropertyAnimation(scrollBar, "value");
    animation->setDuration(200);
    animation->setStartValue(scrollBar->value());
    animation->setEndValue(topOffset);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    animation->start();
}

SlotboardScrollArea *SlotBoardWidget::scrollArea() {
    return dynamic_cast<SlotboardScrollArea *>(parentWidget()->parentWidget());
}

void SlotBoardWidget::handleTimeSlotsChange() {
    updateCurrentTimeMarker();
    slotRuler->setLabels(makeLabels());

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
    updateSlotsLayout();

    circlesWidget->setGeometry(QRect(0, 0, width(), height()));
}

QVBoxLayout *SlotBoardWidget::slotsLayout() const {
    return _slotsLayout;
}

SlotsWidget *SlotBoardWidget::slotsWidget() const {
    return _slotsWidget;
}

void SlotBoardWidget::drawDraggedSession(int sessionIndex, int firstSlotIndex) {
    auto getRect = [=]() -> QRect {
        const auto &session = strategy.sessions()[sessionIndex];

        auto lastSlotIndex = firstSlotIndex + session.length();
        const auto &lastSlot = strategy.time_slots()[lastSlotIndex];
        auto bottomMargin = lastSlot.end_time() % 60 == 0 ? 2 : 4;
        auto horizontalMargin = 8;
        auto rect = QRect(slotRuler->width(),
                          firstSlotIndex * slotsWidget()->slotHeight() + slotsWidget()->geometry().top() + 2,
                          slotsWidget()->width() - horizontalMargin,
                          session.length() * slotsWidget()->slotHeight() - bottomMargin);

        return rect;
    };

    auto makeBigRect = [=](const QRect &smallRect) -> QRect {
        auto bigRect = smallRect;
        bigRect.setX(smallRect.x() - 2);
        bigRect.setWidth(smallRect.width() + 4);
        return bigRect;
    };

    auto makeSmallRect = [=](const QRect &bigRect) -> QRect {
        auto smallRect = bigRect;
        smallRect.setX(bigRect.x() + 2);
        smallRect.setWidth(bigRect.width() - 4);
        return smallRect;
    };

    bool initial = false;

    if (!draggedSessionWidget && sessionIndex >= 0) {
        initial = true;

        draggedSessionWidget = new SessionWidget(strategy.sessions()[sessionIndex], this);
        draggedSessionWidget->setDrawsBorders(false);
        draggedSessionWidget->setIsSelected(true);
        currentTimeMarkerWidget->raise();

        auto shadowColor = textColor();
        shadowColor.setAlphaF(0.2);

        auto *effect = new QGraphicsDropShadowEffect;
        effect->setBlurRadius(25);
        effect->setXOffset(0);
        effect->setYOffset(0);
        effect->setColor(shadowColor);

        draggedSessionWidget->setGraphicsEffect(effect);

        draggedSessionWidget->setVisible(true);

        draggedSessionAnimation = new QPropertyAnimation(draggedSessionWidget, "geometry");
        draggedSessionAnimation->setDuration(50);

        auto smallRect = getRect();
        auto bigRect = makeBigRect(smallRect);

        draggedSessionWidget->setGeometry(smallRect);
        draggedSessionAnimation->setStartValue(smallRect);
        draggedSessionAnimation->setEndValue(bigRect);

        draggedSessionAnimation->start();
    }

    if (draggedSessionWidget && sessionIndex < 0) {
        if (draggedSessionAnimation)
            draggedSessionAnimation->deleteLater();

        draggedSessionAnimation = new QPropertyAnimation(draggedSessionWidget, "geometry");
        draggedSessionAnimation->setDuration(50);
        auto draggedSessionWidgetLocal = draggedSessionWidget;
        draggedSessionWidget = nullptr;

        connect(draggedSessionAnimation, &QPropertyAnimation::finished,
                [this, draggedSessionWidgetLocal]() {
                    draggedSessionAnimation->deleteLater();
                    draggedSessionWidgetLocal->deleteLater();

                    draggedSessionAnimation = nullptr;
                });

        auto smallRect = makeSmallRect(draggedSessionWidgetLocal->geometry());

        draggedSessionAnimation->setStartValue(draggedSessionWidgetLocal->geometry());
        draggedSessionAnimation->setEndValue(smallRect);

        draggedSessionAnimation->start();
    }

    if (draggedSessionWidget && !initial) {
        auto smallRect = getRect();
        auto bigRect = makeBigRect(smallRect);

        draggedSessionAnimation->stop();
        draggedSessionWidget->setGeometry(bigRect);
    }
}


