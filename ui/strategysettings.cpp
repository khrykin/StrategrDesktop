#include "strategysettings.h"
#include "abstractspinboxdecorator.h"
#include "mainwindow.h"
#include "slidinganimator.h"
#include "utils.h"
#include <QPainter>
#include <QStyleOption>

StrategySettings::StrategySettings(QWidget *parent) : QWidget(parent) {
  setStyleSheet("StrategySettings {"
                "background-color: white;"
                "}");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  createLayout();

  createHeader();
  createSlotDurationForm();
  createStartTimeForm();
  createEndTimeForm();

  updateUI();
}

Strategy *StrategySettings::strategy() const { return _strategy; }

void StrategySettings::setStrategy(Strategy *strategy) {
  _strategy = strategy;
  updateUI();
}

void StrategySettings::createLayout() {
  mainLayout = new QVBoxLayout(this);
  layout()->setSpacing(0);
  layout()->setContentsMargins(0, 0, 0, 0);

  setLayout(mainLayout);
}

void StrategySettings::createHeader() {}

void StrategySettings::createSlotDurationForm() {
  auto *formWidget = makeFormRowWidget();

  slotDurationEdit = new QSpinBox();
  slotDurationEdit->setRange(5, 60);
  slotDurationEdit->setSingleStep(5);
  slotDurationEdit->setSuffix(" min");
  slotDurationEdit->setAlignment(Qt::AlignRight);
  slotDurationEdit->setStyleSheet("color: #888;");

  auto slotDurationEditDecorator = new SpinBoxDecorator(slotDurationEdit, this);

  formWidget->layout()->addWidget(createFormLabel("Slot Duration"));
  formWidget->layout()->addWidget(slotDurationEditDecorator);

  connect(slotDurationEdit, QOverload<int>::of(&QSpinBox::valueChanged),
          [this](int value) {
            startTimeEdit->minuteStepSize = value;
            endTimeEdit->minuteStepSize = value;
            save();
          });

  mainLayout->addWidget(formWidget);
}

void StrategySettings::createStartTimeForm() {
  auto *formWidget = makeFormRowWidget();

  startTimeEdit = new SteppedTimeEdit();
  startTimeEdit->setAlignment(Qt::AlignRight);
  startTimeEdit->setStyleSheet("color: #888;");

  auto *startTimeEditDecorator = new TimeEditDecorator(startTimeEdit, this);

  formWidget->layout()->addWidget(createFormLabel("Start Time"));
  formWidget->layout()->addWidget(startTimeEditDecorator);

  connect(startTimeEdit, &QTimeEdit::timeChanged,
          [this](const QTime &time) { save(); });

  mainLayout->addWidget(formWidget);
}

void StrategySettings::createEndTimeForm() {
  auto *formWidget = makeFormRowWidget();
  formWidget->setStyleSheet("[FormRow] { border-bottom: 1px solid #ccc; }");

  endTimeEdit = new SteppedTimeEdit();
  endTimeEdit->setAlignment(Qt::AlignRight);
  endTimeEdit->setStyleSheet("color: #888;");

  auto *endTimeEditDecorator = new TimeEditDecorator(endTimeEdit, this);

  formWidget->layout()->addWidget(createFormLabel("End Time"));
  formWidget->layout()->addWidget(endTimeEditDecorator);

  mainLayout->addWidget(formWidget);

  connect(endTimeEdit, &QTimeEdit::timeChanged, this,
          &StrategySettings::endTimeChanged);
}

QWidget *StrategySettings::makeFormRowWidget() {
  auto formRowWidget = new QWidget(this);
  formRowWidget->setProperty("FormRow", true);
  formRowWidget->setStyleSheet("[FormRow] { border-bottom: 1px solid #eee; }");

  auto *formLayout = new QHBoxLayout();
  formLayout->setSpacing(5);
  formLayout->setContentsMargins(8, 8, 8, 8);

  formRowWidget->setLayout(formLayout);

  return formRowWidget;
}

void StrategySettings::paintEvent(QPaintEvent *) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QLabel *StrategySettings::createFormLabel(QString text) {
  auto label = new QLabel(text);
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  label->setStyleSheet("font-weight: bold; color: #333;");

  return label;
}

void StrategySettings::updateUI() {
  //  if (_strategy == nullptr) {
  //    return;
  //  }

  //  auto slotDuration = _strategy->slotDuration();
  //  auto startTime = qTimeFromMinutes(_strategy->startTime());
  //  auto endTime = qTimeFromMinutes(_strategy->endTime());

  //  qDebug() << "startTime" << qTimeFromMinutes(_strategy->startTime())
  //           << _strategy->startTime();

  //  if (slotDuration != slotDurationEdit->value()) {
  //    slotDurationEdit->setValue(_strategy->slotDuration());
  //  }

  //  if (startTime != startTimeEdit->time()) {
  //    startTimeEdit->setTime(startTime);
  //  }

  //  if (endTime != endTimeEdit->time()) {
  //    endTimeEdit->setTime(endTime);
  //  }

  //  qDebug() << "startTimeEdit" << startTimeEdit->time();

  //  if (startTimeEdit->minuteStepSize != _strategy->slotDuration()) {
  //    startTimeEdit->minuteStepSize = _strategy->slotDuration();
  //  }

  //  if (endTimeEdit->minuteStepSize != _strategy->slotDuration()) {
  //    endTimeEdit->minuteStepSize = _strategy->slotDuration();
  //  }
}

void StrategySettings::getBack() {
  save();
  slideAndHide();
}

void StrategySettings::save() {
  //  auto slotDuration = slotDurationEdit->value();
  //  auto startTime = minutesFromQTime(startTimeEdit->time());
  //  auto endTime = minutesFromQTime(endTimeEdit->time());
  //  auto numberOfSlots =
  //      static_cast<unsigned int>((endTime - startTime) / slotDuration);

  //  if (!_strategy) {
  //    return;
  //  }

  //  _strategy->setSlotDuration(slotDuration);
  //  _strategy->setStartTime(startTime);
  //  //  _strategy->setNumberOfSlots(numberOfSlots);

  //  emit strategySettingsUpdated();
}

void StrategySettings::endTimeChanged(const QTime &time) {
  auto slotDuration = slotDurationEdit->value();
  auto mins = minutesFromQTime(time);
  if (mins <= minutesFromQTime(startTimeEdit->time())) {
    endTimeEdit->setTime(startTimeEdit->time().addSecs(slotDuration * 60));
    return;
  }

  auto remainder = mins % slotDuration;
  if (remainder != 0) {
    mins = remainder < slotDuration / 2 ? mins - remainder
                                        : mins - remainder + slotDuration;
  }

  if (mins != minutesFromQTime(time)) {
    endTimeEdit->setTime(qTimeFromMinutes(mins));
  }
  save();
}

QWidget *StrategySettings::getBackTo() const {
  return _getBackTo != nullptr ? _getBackTo : mainWindow();
}

void StrategySettings::slideAndHide() { SlidingAnimator::hideWidget(this); }

void StrategySettings::slideAndShow() { SlidingAnimator::showWidget(this); }

void StrategySettings::setGetBackTo(QWidget *value) { _getBackTo = value; }

MainWindow *StrategySettings::mainWindow() const {
  return static_cast<MainWindow *>(parentWidget()->parentWidget());
}
