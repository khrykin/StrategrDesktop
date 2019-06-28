#include "strategysettings.h"
#include "abstractspinboxdecorator.h"
#include "mainwindow.h"
#include <QPainter>
#include <QStyleOption>

StrategySettings::StrategySettings(QWidget *parent) : QWidget(parent) {

  setStyleSheet("StrategySettings {"
                "background-color: white;"
                "}");

  createLayout();
  createNavbar();

  createSlotDurationForm();
  createStartTimeForm();
  createEndTimeForm();

  mainLayout->addStretch();
}

Strategy *StrategySettings::getStrategy() const { return _strategy; }

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

void StrategySettings::createNavbar() {
  navBar = new Navbar();

  layout()->addWidget(navBar);

  navBar->setTitle("Settings");
  navBar->setLeftButton("‹ Back", this, &StrategySettings::getBack);
  navBar->setRightButton("Done", this, &StrategySettings::save);
}

void StrategySettings::createSlotDurationForm() {
  auto *formWidget = makeFormRowWidget();

  slotDurationEdit = new QSpinBox();
  slotDurationEdit->setRange(5, 60);
  slotDurationEdit->setSingleStep(5);
  slotDurationEdit->setSuffix(" min");

  auto slotDurationEditDecorator = new SpinBoxDecorator(slotDurationEdit, this);

  formWidget->layout()->addWidget(createFormLabel("Slot Duration"));
  formWidget->layout()->addWidget(slotDurationEditDecorator);

  mainLayout->addWidget(formWidget);
}

void StrategySettings::createStartTimeForm() {
  auto *formWidget = makeFormRowWidget();

  startTimeEdit = new SteppedTimeEdit();
  auto *startTimeEditDecorator = new TimeEditDecorator(startTimeEdit, this);

  formWidget->layout()->addWidget(createFormLabel("Start Time"));
  formWidget->layout()->addWidget(startTimeEditDecorator);

  mainLayout->addWidget(formWidget);
}

void StrategySettings::createEndTimeForm() {
  auto *formWidget = makeFormRowWidget();

  endTimeEdit = new SteppedTimeEdit();
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
  label->setStyleSheet("font-weight: bold;");

  return label;
}

void StrategySettings::updateUI() {
  slotDurationEdit->setValue(_strategy->slotDuration());
  startTimeEdit->setTime(QTime(0, 0).addSecs(_strategy->startTime() * 60));
  endTimeEdit->setTime(QTime(0, 0).addSecs(_strategy->endTime() * 60));

  startTimeEdit->minuteStepSize = _strategy->slotDuration();
  endTimeEdit->minuteStepSize = _strategy->slotDuration();
}

void StrategySettings::getBack() {
  mainWindow()->stackedWidget()->setCurrentWidget(getBackTo());
  setGetBackTo(nullptr);
}

void StrategySettings::save() {
  auto slotDuration = slotDurationEdit->value();
  auto startTime = startTimeEdit->time().msecsSinceStartOfDay() / 60 / 1000;
  auto endTime = endTimeEdit->time().msecsSinceStartOfDay() / 60 / 1000;
  auto numberOfSlots =
      static_cast<unsigned int>((endTime - startTime) / slotDuration);

  _strategy->setSlotDuration(slotDuration);
  _strategy->setStartTime(startTime);
  _strategy->setNumberOfSlots(numberOfSlots);

  emit strategySettingsUpdated();
  getBack();
}

void StrategySettings::endTimeChanged(const QTime &time) {
  auto slotDuration = slotDurationEdit->value();
  auto mins = time.msecsSinceStartOfDay() / 60 / 1000;
  if (mins <= startTimeEdit->time().msecsSinceStartOfDay() / 60 / 1000) {
    endTimeEdit->setTime(startTimeEdit->time().addSecs(slotDuration * 60));
    return;
  }

  auto remainder = mins % slotDuration;
  if (remainder != 0) {
    mins = remainder < slotDuration / 2 ? mins - remainder
                                        : mins - remainder + slotDuration;
  }

  endTimeEdit->setTime(QTime(0, 0).addSecs(mins * 60));
}

QWidget *StrategySettings::getBackTo() const {
  return _getBackTo != nullptr ? _getBackTo : mainWindow();
}

void StrategySettings::setGetBackTo(QWidget *value) { _getBackTo = value; }

MainWindow *StrategySettings::mainWindow() const {
  return static_cast<MainWindow *>(parentWidget()->parentWidget());
}
