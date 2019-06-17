#include "strategysettings.h"
#include "mainwindow.h"
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

StrategySettings::StrategySettings(QWidget *parent) : QWidget(parent) {
  auto *mainLayout = new QVBoxLayout();
  setLayout(mainLayout);

  layout()->setSpacing(0);
  layout()->setContentsMargins(0, 0, 0, 0);

  setStyleSheet("StrategySettings {"
                //                "background-color: white;"
                "}");

  navBar = new Navbar();

  layout()->addWidget(navBar);

  navBar->setTitle("Settings");
  navBar->setLeftButton("‹ Back", this, &StrategySettings::getBack);
  navBar->setRightButton("Done", this, &StrategySettings::save);

  auto *formLayout = new QVBoxLayout();
  formLayout->setSpacing(5);

  slotDurationEdit = new QSpinBox(this);
  slotDurationEdit->setRange(5, 60);
  slotDurationEdit->setSingleStep(5);
  slotDurationEdit->setSuffix(" min");

  formLayout->addWidget(createFormLabel("Slot Duration"));
  formLayout->addWidget(slotDurationEdit);

  startTimeEdit = new SteppedTimeEdit(this);
  formLayout->addWidget(createFormLabel("Start Time"));
  formLayout->addWidget(startTimeEdit);

  endTimeEdit = new SteppedTimeEdit(this);
  formLayout->addWidget(createFormLabel("End Time"));
  formLayout->addWidget(endTimeEdit);

  connect(endTimeEdit, &QTimeEdit::timeChanged, this,
          &StrategySettings::endTimeChanged);

  auto *saveAction = new QAction("Save", this);
  saveAction->setShortcut(QKeySequence(Qt::Key_Return));
  connect(saveAction, &QAction::triggered, this, &StrategySettings::save);
  addAction(saveAction);

  //  titleError = new QLabel("Already exists");
  //  titleError->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  //  titleError->setStyleSheet("font-weight: bold; color: red;");
  //  titleError->hide();

  //  colorEditor = new QLineEdit("#000000");

  //  auto *colorLabel = new QLabel("Color");
  //  colorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  //  colorLabel->setStyleSheet("font-weight: bold;");

  //  formLayout->addWidget(titleEditor);
  //  formLayout->addWidget(titleError);

  //  formLayout->addWidget(colorLabel);
  //  formLayout->addWidget(colorEditor);

  formLayout->setContentsMargins(8, 8, 8, 8);

  mainLayout->addLayout(formLayout);
  mainLayout->addStretch();
}

Strategy *StrategySettings::getStrategy() const { return _strategy; }

void StrategySettings::setStrategy(Strategy *strategy) {
  _strategy = strategy;
  updateUI();
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
