
#include <QDebug>
#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTime>
#include <QTimer>

#include "currentsessionwidget.h"
#include "slidinganimator.h"
#include "utils.h"
#include "colorutils.h"
#include "applicationsettings.h"

CurrentSessionWidget::CurrentSessionWidget(QWidget *parent)
        : QWidget(parent) {
    setMouseTracking(true);

    setFixedHeight(ApplicationSettings::currentSessionHeight);

    auto mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(8, 0, 8, 0);
    setLayout(mainLayout);

    auto *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(0);

    auto *centerLayout = new QVBoxLayout();
    centerLayout->setSpacing(0);

    auto *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(0);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(rightLayout);

    auto *currentLabel = new QLabel(tr("Current"));
    currentLabel->setStyleSheet("font-weight: bold;"
                                "text-transform: uppercase;"
                                "font-size: 10pt;"
                                "color: #999");

    currentLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

    activityLabel = new QLabel();
    activityLabel->setStyleSheet("font-weight: bold;"
                                 "font-size: 14pt;"
                                 "color: #888");

    activityLabel->setFixedHeight(25);
    activityLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    startTimeLabel = new QLabel();
    startTimeLabel->setStyleSheet("font-weight: bold;"
                                  "font-size: 12pt;"
                                  "color: #777");

    startTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    endTimeLabel = new QLabel();
    endTimeLabel->setStyleSheet("font-weight: bold;"
                                "font-size: 12pt;"
                                "color: #777");

    endTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);

    passedTimeLabel = new QLabel();
    passedTimeLabel->setStyleSheet("font-weight: bold;"
                                   "font-size: 12pt;"
                                   "color: #999");
    passedTimeLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    leftTimeLabel = new QLabel();
    leftTimeLabel->setStyleSheet("font-weight: bold;"
                                 "font-size: 12pt;"
                                 "color: #999");

    leftTimeLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);

    leftLayout->addWidget(passedTimeLabel);
    leftLayout->addWidget(startTimeLabel);

    centerLayout->addWidget(currentLabel);
    centerLayout->addWidget(activityLabel);

    rightLayout->addWidget(leftTimeLabel);
    rightLayout->addWidget(endTimeLabel);

    for (auto *widget : findChildren<QWidget *>()) {
        widget->setAttribute(Qt::WA_TransparentForMouseEvents);
    }
}

void CurrentSessionWidget::paintEvent(QPaintEvent *event) {
    auto painter = QPainter(this);

    auto baseRect = QRect(QPoint(0, 0), geometry().size());
    auto borderRect = QRect(baseRect.bottomLeft().x(),
                            baseRect.bottomLeft().y(),
                            baseRect.width(),
                            1);

    auto progressRect = QRect(baseRect.topLeft().x(),
                              baseRect.topLeft().y(),
                              static_cast<int>(baseRect.width() * progress()),
                              baseRect.height());

    auto progressBorderColor = hardBorderColor();
    progressBorderColor.setAlpha(0.5 * 255);

    painter.setPen(Qt::NoPen);

    painter.setBrush(panelColor());
    painter.drawRect(baseRect);

    painter.setBrush(darkPanelColor());
    painter.drawRect(progressRect);

    painter.setPen(progressBorderColor);
    painter.drawLine(progressRect.bottomRight(), progressRect.topRight());

    painter.setPen(Qt::NoPen);

    if (isHovered) {
        auto fadeColor = QColor("#000");
        fadeColor.setAlpha(0.05 * 255);
        painter.setBrush(fadeColor);
        painter.drawRect(geometry());
    }

    if (isClicked) {
        auto fadeColor = QColor("#000");
        fadeColor.setAlpha(0.08 * 255);
        painter.setBrush(fadeColor);
        painter.drawRect(geometry());
    }

    painter.setBrush(hardBorderColor());
    painter.drawRect(borderRect);
}

void CurrentSessionWidget::mousePressEvent(QMouseEvent *) {
    isHovered = false;
    isClicked = true;
    update();
}

void CurrentSessionWidget::enterEvent(QEvent *) {
    isHovered = true;
    update();
}

void CurrentSessionWidget::leaveEvent(QEvent *) {
    isHovered = false;
    update();
}

void CurrentSessionWidget::mouseReleaseEvent(QMouseEvent *) {
    isClicked = false;
    emit clicked();
    update();
}

Strategy *CurrentSessionWidget::strategy() const { return _strategy; }

void CurrentSessionWidget::setStrategy(Strategy *strategy) {
    _strategy = strategy;
    updateUI();
}

void CurrentSessionWidget::slideAndHide(const std::function<void()> &onFinishedCallback) {
    SlidingAnimator::Options options;
    options.onFinishedCallback = onFinishedCallback;

    QTimer::singleShot(ApplicationSettings::currentSessionShowDelay, [=]() {
        SlidingAnimator::hideWidget(this, options);
    });
}

void CurrentSessionWidget::slideAndShow(const std::function<void()> &onFinishedCallback) {
    SlidingAnimator::Options options;
    options.onFinishedCallback = onFinishedCallback;

    QTimer::singleShot(ApplicationSettings::currentSessionShowDelay, [=]() {
        SlidingAnimator::showWidget(this);
    });
}

void CurrentSessionWidget::updateUI() {
    auto startTimeText = QStringForMinutes(activitySession.beginTime());
    auto endTimeText = QStringForMinutes(activitySession.endTime());

    auto activityText = makeActivitySessionTitle();

    auto currentTime = static_cast<double>(currentMinutes());
    auto passedTime = qRound(currentTime - activitySession.beginTime());
    auto leftTime = activitySession.duration() - passedTime;

    auto passedTimeText = humanTimeForMinutes(passedTime);
    auto leftTimeText = humanTimeForMinutes(leftTime);

    auto progress = getProgress();

    if (activitySession != previousSession) {
        startTimeLabel->setText(startTimeText);
        endTimeLabel->setText(endTimeText);
    }

    passedTimeLabel->setText(passedTimeText);
    leftTimeLabel->setText(leftTimeText);
    activityLabel->setText(activityText);

    setProgress(progress);
}

double CurrentSessionWidget::getProgress() const {
    auto secondsPassed = static_cast<double>(currentSeconds() - activitySession.beginTime() * 60);
    auto secondsDuration = 60 * activitySession.duration();
    return secondsPassed / secondsDuration;
}

const QString CurrentSessionWidget::makeActivitySessionTitle() const {
    return humanTimeForMinutes(activitySession.duration())
           + " "
           + "<font color=\""
           + QString::fromStdString(activitySession.activity->color())
           + "\">"
           + QString::fromStdString(activitySession.activity->name())
           + "</font>";
}

double CurrentSessionWidget::progress() const { return _progress; }

void CurrentSessionWidget::setProgress(double value) {
    _progress = value;
    update();
}

void CurrentSessionWidget::setActivitySession(
        const ActivitySession &newActivitySession) {
    if (!newActivitySession.activity) {
        return;
    }

    auto activityPropertiesChanged = !activitySession.activity
                                     || *activitySession.activity != *newActivitySession.activity;

    if (activitySession != newActivitySession || activityPropertiesChanged) {
        previousSession = activitySession;
        activitySession = newActivitySession;
    }

    updateUI();
}