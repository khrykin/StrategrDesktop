#include "slotboard.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QLocale>
#include <QTime>
#include <algorithm>
#include <cmath>

SlotBoard::SlotBoard(QWidget *parent) : QWidget(parent) {
  _groupsList = new GroupsList();

  auto *mainLayout = new QHBoxLayout();
  setLayout(mainLayout);
  layout()->setSpacing(0);
  layout()->setMargin(0);

  _slotRuler = new SlotRuler(_groupsList->slotHeight());

  auto *rulerLayout = new QVBoxLayout();
  rulerLayout->setSpacing(0);
  rulerLayout->setMargin(0);
  rulerLayout->addWidget(_slotRuler);

  auto *slotsLayout = new QVBoxLayout();
  slotsLayout->setSpacing(0);
  slotsLayout->setMargin(0);

  auto *header = new QLabel("Start");
  header->setAlignment(Qt::AlignCenter);
  header->setFixedHeight(_groupsList->slotHeight());
  header->setStyleSheet("color: #c3c3c3;"
                        "font-weight: bold;"
                        "background: #f4f4f4;");

  auto *footer = new QLabel("End");
  footer->setAlignment(Qt::AlignCenter);
  footer->setFixedHeight(_groupsList->slotHeight());
  footer->setStyleSheet("color: #c3c3c3;"
                        "font-weight: bold;"
                        "background: #f4f4f4;");

  slotsLayout->addWidget(header);
  slotsLayout->addWidget(_groupsList);
  slotsLayout->addWidget(footer);
  slotsLayout->addStretch();

  mainLayout->addLayout(rulerLayout);
  mainLayout->addLayout(slotsLayout);

  _currentTimeMarker = new CurrentTimeMarker(this);

  currentTimeTimer = new QTimer(this);
  connect(currentTimeTimer, &QTimer::timeout, this,
          &SlotBoard::updateCurrentTimeMarker);

  currentTimeTimer->start(30 * 1000);
}

Strategy *SlotBoard::strategy() const { return _strategy; }

void SlotBoard::setStrategy(Strategy *strategy) {
  _strategy = strategy;
  _groupsList->setStrategy(strategy);

  QVector<QString> labels;

  auto timeFormat = QLocale().timeFormat(QLocale::ShortFormat);
  for (auto &mins : strategy->startTimes()) {
    labels.append(QTime(0, 0, 0).addSecs(mins * 60).toString(timeFormat));
  }

  labels.append(
      QTime(0, 0, 0).addSecs(strategy->endTime() * 60).toString(timeFormat));

  _slotRuler->setLabels(labels);

  updateCurrentTimeMarker();
}

GroupsList *SlotBoard::groupsList() const { return _groupsList; }

SlotRuler *SlotBoard::slotRuler() const { return _slotRuler; }

CurrentTimeMarker *SlotBoard::currentTimeMarker() const {
  return _currentTimeMarker;
}

int SlotBoard::currentTimeMarkerTopOffset() const {
  return _currentTimeMarkerTopOffset;
}

void SlotBoard::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  updateCurrentTimeMarker();
}

void SlotBoard::updateCurrentTimeMarker() {
  if (strategy() == nullptr) {
    return;
  }

  emit timerTick();

  auto currentTime = QTime::currentTime().msecsSinceStartOfDay() / 60 / 1000;
  auto startTime = strategy()->startTime();
  auto slotDuration = strategy()->slotDuration();
  auto slotHeight = _groupsList->slotHeight();

  auto pxInMins = slotHeight / static_cast<float>(slotDuration);

  auto topOffset = std::round(pxInMins * (currentTime - startTime)) +
                   _groupsList->geometry().top();

  _currentTimeMarkerTopOffset = static_cast<int>(topOffset);

  if (topOffset < 0 ||
      _currentTimeMarkerTopOffset >
          _groupsList->geometry().top() + groupsList()->height()) {
    if (_currentTimeMarker->isVisible()) {
      _currentTimeMarker->hide();
    }
  } else {
    if (_currentTimeMarker->isHidden()) {
      _currentTimeMarker->show();
    }
  }

  _currentTimeMarker->setGeometry(
      SlotRuler::defaultRulerWidth - CurrentTimeMarker::markerSize,
      _currentTimeMarkerTopOffset - CurrentTimeMarker::markerSize,
      _groupsList->geometry().width() + CurrentTimeMarker::markerSize,
      2 * CurrentTimeMarker::markerSize);
}
