#include <QPainter>
#include <QStyleOption>

#include "strategysettingswidget.h"
#include "abstractspinboxdecorator.h"
#include "mainwindow.h"
#include "slidinganimator.h"
#include "utils.h"

StrategySettingsWidget::StrategySettingsWidget(Strategy *strategy,
                                               QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    setStyleSheet("StrategySettingsWidget {"
                  "background-color: white;"
                  "}");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    createLayout();
    createHeader();

    createSlotDurationForm();
    createStartTimeForm();
    createEndTimeForm();

    hide();
    updateUI();
}

void StrategySettingsWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    updateUI();
}

void StrategySettingsWidget::createLayout() {
    mainLayout = new QVBoxLayout(this);
    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
}

void StrategySettingsWidget::createHeader() {
    auto *headerWidget = makeFormRowWidget();

    auto *label = new QLabel(tr("Settings"));
    label->setStyleSheet("QLabel {"
                         "font-weight: bold;"
                         "text-transform: uppercase;"
                         "color: #888;"
                         "}");

    auto *closeButton = new QPushButton(tr("Done"));
    closeButton->setDefault(true);
    connect(closeButton,
            &QPushButton::clicked,
            this,
            &StrategySettingsWidget::slideAndHide);

    closeButton->setSizePolicy(QSizePolicy::Fixed,
                               QSizePolicy::Fixed);

    headerWidget->layout()->addWidget(label);
    headerWidget->layout()->addWidget(closeButton);

    layout()->addWidget(headerWidget);
}

void StrategySettingsWidget::createSlotDurationForm() {
    auto *formWidget = makeFormRowWidget();

    slotDurationEdit = new QSpinBox();
    slotDurationEdit->setRange(5, 60);
    slotDurationEdit->setSingleStep(5);
    slotDurationEdit->setSuffix(" min");
    slotDurationEdit->setAlignment(Qt::AlignRight);
    slotDurationEdit->setStyleSheet("color: #888;");

    auto slotDurationEditDecorator = new SpinBoxDecorator(slotDurationEdit, this);

    formWidget->layout()->addWidget(makeFormLabel("Slot Duration"));
    formWidget->layout()->addWidget(slotDurationEditDecorator);

    connect(slotDurationEdit, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) {
                beginTimeEdit->minuteStepSize = value;
                endTimeEdit->minuteStepSize = value;
                save();
            });

    mainLayout->addWidget(formWidget);
}

void StrategySettingsWidget::createStartTimeForm() {
    auto *formWidget = makeFormRowWidget();

    beginTimeEdit = new SteppedTimeEdit();
    beginTimeEdit->setAlignment(Qt::AlignRight);
    beginTimeEdit->setStyleSheet("color: #888;");

    auto *beginTimeEditDecorator = new TimeEditDecorator(beginTimeEdit, this);

    formWidget->layout()->addWidget(makeFormLabel("Start Time"));
    formWidget->layout()->addWidget(beginTimeEditDecorator);

    connect(beginTimeEdit, &QTimeEdit::timeChanged,
            [this](const QTime &time) {
                save();
            });

    mainLayout->addWidget(formWidget);
}

void StrategySettingsWidget::createEndTimeForm() {
    auto *formWidget = makeFormRowWidget();
    formWidget->setStyleSheet("[FormRow] { border-bottom: 1px solid #ccc; }");

    endTimeEdit = new SteppedTimeEdit();
    endTimeEdit->setAlignment(Qt::AlignRight);
    endTimeEdit->setStyleSheet("color: #888;");

    auto *endTimeEditDecorator = new TimeEditDecorator(endTimeEdit, this);

    formWidget->layout()->addWidget(makeFormLabel("End Time"));
    formWidget->layout()->addWidget(endTimeEditDecorator);

    mainLayout->addWidget(formWidget);

    connect(endTimeEdit, &QTimeEdit::timeChanged, this,
            &StrategySettingsWidget::endTimeChanged);
}

QWidget *StrategySettingsWidget::makeFormRowWidget() {
    auto formRowWidget = new QWidget(this);
    formRowWidget->setProperty("FormRow", true);
    formRowWidget->setStyleSheet("[FormRow] {"
                                 "border-bottom: 1px solid #eee;"
                                 "}");

    auto *formLayout = new QHBoxLayout();
    formLayout->setSpacing(5);
    formLayout->setContentsMargins(8, 8, 8, 8);

    formRowWidget->setLayout(formLayout);

    return formRowWidget;
}

void StrategySettingsWidget::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QLabel *StrategySettingsWidget::makeFormLabel(QString text) {
    auto label = new QLabel(text);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    label->setStyleSheet("font-weight: bold; color: #333;");

    return label;
}

void StrategySettingsWidget::updateUI() {
    auto slotDuration = strategy->timeSlotDuration();
    auto beginTime = qTimeFromMinutes(strategy->beginTime());
    auto endTime = qTimeFromMinutes(strategy->endTime());

    dontSave = true;

    if (slotDuration != slotDurationEdit->value()) {
        slotDurationEdit->setValue(slotDuration);
    }

    if (beginTime != beginTimeEdit->time()) {
        beginTimeEdit->setTime(beginTime);
    }

    if (endTime != endTimeEdit->time()) {
        endTimeEdit->setTime(endTime);
    }

    if (beginTimeEdit->minuteStepSize != strategy->timeSlotDuration()) {
        beginTimeEdit->minuteStepSize = strategy->timeSlotDuration();
    }

    if (endTimeEdit->minuteStepSize != strategy->timeSlotDuration()) {
        endTimeEdit->minuteStepSize = strategy->timeSlotDuration();
    }

    dontSave = false;
}

void StrategySettingsWidget::getBack() {
    save();
    slideAndHide();
}

void StrategySettingsWidget::save() {
    if (dontSave) {
        return;
    }

    auto slotDuration = slotDurationEdit->value();
    auto beginTime = minutesFromQTime(beginTimeEdit->time());
    auto endTime = minutesFromQTime(endTimeEdit->time());
    auto numberOfSlots = (endTime - beginTime) / slotDuration;

    if (slotDuration != strategy->timeSlotDuration()) {
        strategy->setTimeSlotDuration(slotDuration);
    }

    if (beginTime != strategy->beginTime()) {
        strategy->setBeginTime(beginTime);
    }

    if (numberOfSlots != strategy->numberOfTimeSlots()) {
        strategy->setNumberOfTimeSlots(numberOfSlots);
    }
}

void StrategySettingsWidget::endTimeChanged(const QTime &time) {
    auto slotDuration = slotDurationEdit->value();
    auto mins = minutesFromQTime(time);
    if (mins <= minutesFromQTime(beginTimeEdit->time())) {
        endTimeEdit->setTime(beginTimeEdit->time().addSecs(slotDuration * 60));
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

void StrategySettingsWidget::slideAndHide() {
    SlidingAnimator::hideWidget(this);
}

void StrategySettingsWidget::slideAndShow() {
    SlidingAnimator::showWidget(this);
}
