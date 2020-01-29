#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QTime>
#include <QTimer>

#include "currentsessionwidget.h"
#include "slidinganimator.h"
#include "utils.h"
#include "applicationsettings.h"
#include "fontutils.h"

#include "coloredlabel.h"

CurrentSessionWidget::CurrentSessionWidget(Strategy &strategy, QWidget *parent)
        : strategy(strategy), QWidget(parent) {
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

    auto *currentLabel = new ColoredLabel(tr("CURRENT"));
    currentLabel->setFontHeight(10);
    currentLabel->setBold(true);
    currentLabel->setDynamicColor(&ColorProvider::tertiaryTextColor);

    currentLabel->setAlignment(Qt::AlignBottom | Qt::AlignCenter);

    activityLabel = new ColoredLabel();
    activityLabel->setBold(true);
    activityLabel->setFontHeight(14);
    activityLabel->customRenderer = [&](QPainter *painter, const QString &) {
        auto activeSession = strategy.activeSession();
        if (!activeSession) {
            return;
        }

        auto textRect = QRect(0,
                              0,
                              activityLabel->width(),
                              activityLabel->height());

        FontUtils::drawSessionTitle(*activeSession,
                                    *painter,
                                    textRect);
    };

    activityLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    startTimeLabel = new ColoredLabel();
    startTimeLabel->setFontHeight(12);
    startTimeLabel->setBold(true);
    startTimeLabel->setDynamicColor(secondaryTextColor);
    startTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    startTimeLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    endTimeLabel = new ColoredLabel();
    endTimeLabel->setFontHeight(12);
    endTimeLabel->setBold(true);
    endTimeLabel->setDynamicColor(secondaryTextColor);
    endTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);
    endTimeLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    passedTimeLabel = new ColoredLabel();
    passedTimeLabel->setFontHeight(12);
    passedTimeLabel->setBold(true);
    passedTimeLabel->setDynamicColor(tertiaryTextColor);
    passedTimeLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    passedTimeLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    leftTimeLabel = new ColoredLabel();
    leftTimeLabel->setFontHeight(12);
    leftTimeLabel->setBold(true);
    leftTimeLabel->setDynamicColor(tertiaryTextColor);
    leftTimeLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    leftTimeLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

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

void CurrentSessionWidget::reloadStrategy() {
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
    auto activeSession = strategy.activeSession();

    if (!activeSession) {
        if (isVisible) {
            isVisible = false;
            slideAndHide();
        }

        return;
    }

    auto startTimeText = QStringForMinutes(activeSession->beginTime());
    auto endTimeText = QStringForMinutes(activeSession->endTime());

    auto activityText = makeActivitySessionTitle();

    auto passedTimeText = humanTimeForMinutes(activeSession->passedMinutes());
    auto leftTimeText = humanTimeForMinutes(activeSession->leftMinutes());

    startTimeLabel->setText(startTimeText);
    endTimeLabel->setText(endTimeText);

    passedTimeLabel->setText(passedTimeText);
    leftTimeLabel->setText(leftTimeText);

    activityLabel->setText(activityText);

    setProgress(activeSession->progress());
}

QString CurrentSessionWidget::makeActivitySessionTitle() const {
    auto activitySession = strategy.activeSession();
    return humanTimeForMinutes(activitySession->duration())
           + " "
           + "<font color=\""
           + QString::fromStdString(activitySession->activity->color())
           + "\">"
           + QString::fromStdString(activitySession->activity->name())
           + "</font>";
}

double CurrentSessionWidget::progress() const { return _progress; }

void CurrentSessionWidget::setProgress(double value) {
    _progress = value;
    update();
}

void CurrentSessionWidget::reloadSessionIfNeeded() {
    auto *activeSession = strategy.activeSession();

    if (activeSession) {
        if (*activeSession != previousSession) {
            previousSession = *activeSession;
            updateUI();
        } else {
            setProgress(activeSession->progress());
        }

        if (!isVisible) {
            isVisible = true;
            slideAndShow();
        }
    } else {
        if (isVisible) {
            isVisible = false;
            slideAndHide();
        }
    }
}

void CurrentSessionWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}
