#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QTime>
#include <QTimer>

#include "currentsessionwidget.h"
#include "slidinganimator.h"
#include "utils.h"
#include "applicationsettings.h"

#include "coloredlabel.h"

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
                                "background-color: rgba(0,0,0,0);"
                                "font-size: 10px;"
                                "color: #999");

    currentLabel->setAlignment(Qt::AlignBottom | Qt::AlignCenter);

    activityLabel = new QLabel();
    activityLabel->setStyleSheet("font-weight: bold;"
                                 "font-size: 14px;"
                                 "color: #888");

    activityLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    startTimeLabel = new QLabel();
    startTimeLabel->setStyleSheet("font-weight: bold;"
                                  "font-size: 12px;"
                                  "color: #777");

    startTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    endTimeLabel = new QLabel();
    endTimeLabel->setStyleSheet("font-weight: bold;"
                                "font-size: 12px;"
                                "color: #777");

    endTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);

    passedTimeLabel = new QLabel();
    passedTimeLabel->setStyleSheet("font-weight: bold;"
                                   "font-size: 12px;"
                                   "color: #999");
    passedTimeLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    leftTimeLabel = new QLabel();
    leftTimeLabel->setStyleSheet("font-weight: bold;"
                                 "font-size: 12px;"
                                 "color: #999");

    leftTimeLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);

    leftLayout->addWidget(passedTimeLabel);
    leftLayout->addWidget(startTimeLabel);

    startTimeLabel->setFixedHeight(height() / 2);
    passedTimeLabel->setFixedHeight(height() / 2);

    centerLayout->addWidget(currentLabel);
    centerLayout->addWidget(activityLabel);

    activityLabel->setFixedHeight(static_cast<int>(0.6 * height()));

    rightLayout->addWidget(leftTimeLabel);
    rightLayout->addWidget(endTimeLabel);

    leftTimeLabel->setFixedHeight(height() / 2);
    endTimeLabel->setFixedHeight(height() / 2);

    for (auto *widget : findChildren<QWidget *>()) {
        widget->setAttribute(Qt::WA_TransparentForMouseEvents);
    }
}

void CurrentSessionWidget::paintEvent(QPaintEvent *event) {
    auto painter = QPainter(this);

    auto backgroundRect = QRect(QPoint(0, 0), geometry().size());

    auto progressRect = QRect(backgroundRect.x(),
                              backgroundRect.y(),
                              static_cast<int>(backgroundRect.width() * progress()),
                              backgroundRect.height());

    auto progressBorderColor = hardBorderColor();
    progressBorderColor.setAlpha(0.5 * 255);

    painter.setPen(Qt::NoPen);

    painter.setBrush(panelColor());
    painter.drawRect(backgroundRect);

    painter.setBrush(darkPanelColor());
    painter.drawRect(progressRect);

    painter.setPen(progressBorderColor);
    painter.drawLine(progressRect.bottomRight(), progressRect.topRight());

    painter.setPen(Qt::NoPen);

    if (isHovered) {
        auto fadeColor = QColor("#000");
        fadeColor.setAlpha(0.05 * 255);
        painter.setBrush(fadeColor);
        painter.drawRect(backgroundRect);
    }

    if (isClicked) {
        auto fadeColor = QColor("#000");
        fadeColor.setAlpha(0.08 * 255);
        painter.setBrush(fadeColor);
        painter.drawRect(backgroundRect);
    }

    painter.setBrush(hardBorderColor());
    painter.drawRect(QRect(0,
                           height() - 1,
                           width(),
                           1));

    auto topBorderColor = QColor(Qt::white);
    topBorderColor.setAlphaF(0.4 * ColorUtils::shadesAlphaFactor(10, 0.1));

    painter.setBrush(topBorderColor);
    painter.drawRect(QRect(0,
                           0,
                           width(),
                           1));
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

void CurrentSessionWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
//    visualEffectWidget->setGeometry(geometry());
}
