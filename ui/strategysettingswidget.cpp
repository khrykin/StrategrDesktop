#include <QKeyEvent>
#include <QPainter>
#include <QStyleOption>

#include "abstractspinboxdecorator.h"
#include "colorutils.h"
#include "rowwidget.h"
#include "slidinganimator.h"
#include "strategysettingswidget.h"
#include "utils.h"

StrategySettingsWidget::StrategySettingsWidget(QWidget *parent) : DataProviderWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    createLayout();
    createHeader();

    createSlotDurationForm();
    createStartTimeForm();
    createEndTimeForm();

    updateUI();

    setMinimumHeight(sizeHint().height());
}

void StrategySettingsWidget::reloadStrategy() {
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

    auto *label = new ColoredLabel(tr("Settings"));
    label->setStyleSheet("text-transform: uppercase;");
    label->setDynamicColor(&ColorProvider::secondaryTextColor);
    label->setBold(true);

    auto *closeButton = new QPushButton(tr("Done"));
    closeButton->setDefault(true);
    connect(closeButton,
            &QPushButton::clicked,
            [this]() {
                slideAndHide();
            });

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
    slotDurationEdit->installEventFilter(this);

    auto *slotDurationEditDecorator = new SpinBoxDecorator(slotDurationEdit, this);
    slotDurationEditDecorator->setSizePolicy(QSizePolicy::Fixed,
                                             QSizePolicy::Fixed);

    formWidget->layout()->addWidget(makeFormLabel("Slot Duration"));
    formWidget->layout()->addWidget(slotDurationEditDecorator);

    connect(slotDurationEdit,
            QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) {
                auto slotDuration = slotDurationEdit->value();
                strategy().set_time_slot_duration(slotDuration);
                reloadStrategy();
            });

    mainLayout->addWidget(formWidget);
}

void StrategySettingsWidget::createStartTimeForm() {
    auto *formWidget = makeFormRowWidget();

    beginTimeEdit = new SteppedTimeEdit();
    beginTimeEdit->setAlignment(Qt::AlignRight);
    beginTimeEdit->setStyleSheet("color: #888;");
    beginTimeEdit->installEventFilter(this);

    auto *beginTimeEditDecorator = new TimeEditDecorator(beginTimeEdit, this);
    beginTimeEditDecorator->setSizePolicy(QSizePolicy::Fixed,
                                          QSizePolicy::Fixed);

    formWidget->layout()->addWidget(makeFormLabel("Start Time"));
    formWidget->layout()->addWidget(beginTimeEditDecorator);

    connect(beginTimeEdit, &QTimeEdit::timeChanged,
            [this](const QTime &) {
                auto beginTime = QTimeToMinutes(beginTimeEdit->time());
                strategy().set_begin_time(beginTime);
                reloadStrategy();
            });

    mainLayout->addWidget(formWidget);
}

void StrategySettingsWidget::createEndTimeForm() {
    auto *formWidget = makeFormRowWidget();
    formWidget->setBorderColor([] {
        auto textColor = ColorProvider::textColor();
        textColor.setAlphaF(0.05);

        return textColor;
    });

    endTimeEdit = new SteppedTimeEdit();
    endTimeEdit->setAlignment(Qt::AlignRight);
    endTimeEdit->setStyleSheet("color: #888;");
    endTimeEdit->installEventFilter(this);

    auto *endTimeEditDecorator = new TimeEditDecorator(endTimeEdit, this);
    endTimeEditDecorator->setSizePolicy(QSizePolicy::Fixed,
                                        QSizePolicy::Fixed);

    formWidget->layout()->addWidget(makeFormLabel("End Time"));
    formWidget->layout()->addWidget(endTimeEditDecorator);

    mainLayout->addWidget(formWidget);

    connect(endTimeEdit, &QTimeEdit::timeChanged, this, [this] {
        auto endTime = QTimeToMinutes(endTimeEdit->time());
        strategy().set_end_time(endTime);
        reloadStrategy();
    });
}

QColor StrategySettingsWidget::labelColor() const {
    return ColorUtils::overlayWithAlpha(palette().color(QPalette::Text),
                                        0.75 * ColorUtils::shadesAlphaFactor(0),
                                        palette().color(QPalette::Base));
}

RowWidget *StrategySettingsWidget::makeFormRowWidget() {
    auto *formRowWidget = new RowWidget(this);
    formRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    auto *formLayout = new QHBoxLayout();
    formLayout->setSpacing(5);
    formLayout->setContentsMargins(8, 8, 8, 8);

    formRowWidget->setLayout(formLayout);

    return formRowWidget;
}

void StrategySettingsWidget::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QApplication::palette().color(QPalette::Base));

    painter.drawRect(QRect(0, 0, width(), height()));
}

ColoredLabel *StrategySettingsWidget::makeFormLabel(const QString &text) {
    auto *label = new ColoredLabel(text);
    label->setBold(true);
    label->setDynamicColor(&ColorProvider::textColorJustLighter);

    return label;
}

void StrategySettingsWidget::updateUI() {
    auto slotDuration = strategy().time_slot_duration();
    auto beginTime = QTimeFromMinutes(strategy().begin_time());
    auto endTime = QTimeFromMinutes(strategy().end_time());

    if (slotDuration != slotDurationEdit->value())
        slotDurationEdit->setValue(slotDuration);

    if (beginTime != beginTimeEdit->time())
        beginTimeEdit->setTime(beginTime);

    if (endTime != endTimeEdit->time())
        endTimeEdit->setTime(endTime);

    if (beginTimeEdit->minuteStepSize != strategy().time_slot_duration())
        beginTimeEdit->minuteStepSize = strategy().time_slot_duration();

    if (endTimeEdit->minuteStepSize != strategy().time_slot_duration())
        endTimeEdit->minuteStepSize = strategy().time_slot_duration();
}

void StrategySettingsWidget::slideAndHide(const std::function<void()> &onFinishedCallback) {
    SlidingAnimator::Options options;
    options.onFinishedCallback = onFinishedCallback;
    SlidingAnimator::hideWidget(this, options);
}

void StrategySettingsWidget::slideAndShow(const std::function<void()> &onFinishedCallback) {
    SlidingAnimator::Options options;
    options.onFinishedCallback = onFinishedCallback;
    SlidingAnimator::showWidget(this, options);
}

bool StrategySettingsWidget::eventFilter(QObject *object, QEvent *event) {
    // Disable all built-in shortcuts
    return event->type() == QEvent::ShortcutOverride;
}
