
#include <QDebug>
#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTime>

#include "currentsessionwidget.h"
#include "slidinganimator.h"
#include "utils.h"
#include "colorutils.h"
#include "applicationsettings.h"

CurrentSessionWidget::CurrentSessionWidget(QWidget *parent)
        : QWidget(parent) {
    setFixedHeight(ApplicationSettings::currentSessionHeight);
    setMouseTracking(true);

    auto mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(11, 0, 11, 0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    auto *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(0);
    auto *centerLayout = new QVBoxLayout();
    centerLayout->setSpacing(0);
    auto *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(0);

    //  setStyleSheet("border: 1px solid red;");
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(rightLayout);

    auto *currentLabel = new QLabel(tr("Current"));
    currentLabel->setStyleSheet("font-weight: bold;"
                                "text-transform: uppercase;"
                                "font-size: 10pt;"
                                "color: #999");
    currentLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

    activityLabel = new QLabel(tr("45 min Guitar"));
    activityLabel->setStyleSheet("font-weight: bold;"
                                 "font-size: 15pt;"
                                 "color: #888");
    activityLabel->setFixedHeight(25);
    activityLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    startTimeLabel = new QLabel(tr("5:00 PM"));
    startTimeLabel->setStyleSheet("font-weight: bold;"
                                  "font-size: 12pt;"
                                  "color: #777");

    startTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    endTimeLabel = new QLabel(tr("7:00 PM"));
    endTimeLabel->setStyleSheet("font-weight: bold;"
                                "font-size: 12pt;"
                                "color: #777");

    endTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);

    passedTimeLabel = new QLabel(tr("15 min"));
    passedTimeLabel->setStyleSheet("font-weight: bold;"
                                   "font-size: 12pt;"
                                   "color: #999");
    passedTimeLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    leftTimeLabel = new QLabel(tr("35 min"));
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

            foreach (auto *widget, findChildren<QWidget *>()) {
            widget->setAttribute(Qt::WA_TransparentForMouseEvents);
        };
}

void CurrentSessionWidget::paintEvent(QPaintEvent *event) {
    QPainter painter;
    painter.begin(this);
    auto baseRect = QRect(QPoint(0, 0), geometry().size());
    auto borderRect = QRect(baseRect.bottomLeft().x(),
                            baseRect.bottomLeft().y(),
                            baseRect.width(),
                            1);

    auto progressRect = QRect(baseRect.topLeft().x(),
                              baseRect.topLeft().y(),
                              baseRect.width() * progress(),
                              baseRect.height());

    auto borderColor = QColor("#ccc");
    auto bgColor = QColor("#F8F8F8");
    auto progressColor = QColor("#ECECEC");
    auto progressBorderColor = borderColor;

    progressBorderColor.setAlpha(0.5 * 255);

    painter.setPen(Qt::NoPen);

    painter.setBrush(bgColor);
    painter.drawRect(baseRect);

    painter.setBrush(progressColor);
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

    painter.setBrush(borderColor);
    painter.drawRect(borderRect);

    painter.end();
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

void CurrentSessionWidget::slideAndHide() {
    SlidingAnimator::hideWidget(this);
}

void CurrentSessionWidget::slideAndShow() {
    SlidingAnimator::showWidget(this);
}

void CurrentSessionWidget::updateUI() {
    auto startTimeText = qStringForMinutes(activitySession.beginTime());
    auto endTimeText = qStringForMinutes(activitySession.endTime());

    auto activityText = makeActivitySessionTitle();

    auto currentTime = currentMinutes();
    auto passedTime = currentTime - activitySession.beginTime();
    auto leftTime = activitySession.endTime() - currentTime;

    auto passedTimeText = humanTimeForMinutes(passedTime);
    auto leftTimeText = humanTimeForMinutes(leftTime);

    auto progress = static_cast<double>(passedTime) / activitySession.duration();

    if (activitySession != previousSession) {
        startTimeLabel->setText(startTimeText);
        endTimeLabel->setText(endTimeText);
    }

    passedTimeLabel->setText(passedTimeText);
    leftTimeLabel->setText(leftTimeText);
    activityLabel->setText(activityText);

    setProgress(progress);
}

const QString CurrentSessionWidget::makeActivitySessionTitle() const {
    return humanTimeForMinutes(activitySession.duration())
           + " "
           + "<font color=\""
           + QString::fromStdString(activitySession.activity->color())
           + "\">" + QString::fromStdString(activitySession.activity->name())
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
        updateUI();
    }
}
