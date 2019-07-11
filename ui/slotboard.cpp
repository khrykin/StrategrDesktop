#include <QDebug>
#include <QHBoxLayout>
#include <QLocale>
#include <QTime>
#include <algorithm>
#include <cmath>

#include "slotboard.h"
#include "utils.h"

SlotBoard::SlotBoard(Strategy *strategy, QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    auto *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);

    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    layoutChildWidgets(mainLayout);

//  _currentTimeMarker = new CurrentTimeMarker(this);
//
//  currentTimeTimer = new QTimer(this);
//  connect(currentTimeTimer, &QTimer::timeout, this,
//          &SlotBoard::updateCurrentTimeMarker);
//
//  currentTimeTimer->start(1 * 1000);
}

void SlotBoard::layoutChildWidgets(QHBoxLayout *mainLayout) {
    slotsWidget = new SlotsWidget(strategy);
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
}

QWidget *SlotBoard::makeHeader() {
    auto *header = new QLabel("Start");
    header->setAlignment(Qt::AlignCenter);
    header->setFixedHeight(slotsWidget->slotHeight());
    header->setStyleSheet("color: #c3c3c3;"
                          "font-weight: bold;"
                          "background: #f4f4f4;");
    return header;
}

QWidget *SlotBoard::makeFooter() {
    auto *footer = new QLabel("End");
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
}

QStringList SlotBoard::makeLabelsForStrategy() {
    QStringList labels;

    auto timeFormat = QLocale().timeFormat(QLocale::ShortFormat);
    for (auto &timeSlot : strategy->timeSlots()) {
        auto label = qTimeFromMinutes(timeSlot.beginTime)
                .toString(timeFormat);

        labels.append(label);
    }

    auto endTimeLabel = qTimeFromMinutes(strategy->endTime())
            .toString(timeFormat);

    labels.append(endTimeLabel);

    return labels;
}

void SlotBoard::clearSelection() {
    slotsWidget->deselectAllSlots();
}
//
//Strategy *SlotBoard::strategy() const { return _strategy; }
//
//void SlotBoard::setStrategy(Strategy *strategy) {
//    _strategy = strategy;
//    _groupsList->setStrategy(strategy);
//
//    QVector<QString> labels;
//
//    auto timeFormat = QLocale().timeFormat(QLocale::ShortFormat);
//    for (auto &mins : strategy->startTimes()) {
//        labels.append(QTime(0, 0, 0).addSecs(mins * 60).toString(timeFormat));
//    }
//
//    labels.append(
//            QTime(0, 0, 0).addSecs(strategy->endTime() * 60).toString(timeFormat));
//
//    _slotRuler->setLabels(labels);
//
//    updateCurrentTimeMarker();
//}
//
//GroupsList *SlotBoard::groupsList() const { return _groupsList; }
//
//SlotRuler *SlotBoard::slotRuler() const { return _slotRuler; }
//
//CurrentTimeMarker *SlotBoard::currentTimeMarker() const {
//    return _currentTimeMarker;
//}
//
//int SlotBoard::currentTimeMarkerTopOffset() const {
//    return _currentTimeMarkerTopOffset;
//}
//
//void SlotBoard::resizeEvent(QResizeEvent *event) {
//    QWidget::resizeEvent(event);
//    updateCurrentTimeMarker();
//}
//
//void SlotBoard::updateCurrentTimeMarker() {
//    if (strategy() == nullptr) {
//        return;
//    }
//
//    emit timerTick();
//
//    auto currentTime =
//            static_cast<double>(QTime::currentTime().msecsSinceStartOfDay()) / 60 /
//            1000;
//    auto startTime = strategy()->startTime();
//    auto slotDuration = strategy()->slotDuration();
//    auto slotHeight = _groupsList->slotHeight();
//
//    auto pxInMins = static_cast<double>(slotHeight) / slotDuration;
//
//    auto topOffset = std::round(pxInMins * (currentTime - startTime)) +
//                     _groupsList->geometry().top();
//
//    _currentTimeMarkerTopOffset = static_cast<int>(topOffset);
//
//    if (topOffset < 0 ||
//        _currentTimeMarkerTopOffset >
//        _groupsList->geometry().top() + groupsList()->height()) {
//        if (_currentTimeMarker->isVisible()) {
//            _currentTimeMarker->hide();
//        }
//    } else {
//        if (_currentTimeMarker->isHidden()) {
//            _currentTimeMarker->show();
//        }
//    }
//
//    _currentTimeMarker->setGeometry(
//            SlotRuler::defaultRulerWidth - CurrentTimeMarker::markerSize,
//            _currentTimeMarkerTopOffset - CurrentTimeMarker::markerSize,
//            _groupsList->geometry().width() + CurrentTimeMarker::markerSize,
//            2 * CurrentTimeMarker::markerSize);
//}
