#include "activitysessionwidget.h"
#include "third-party/stacklayout.h"
#include "utils.h"
#include <QDebug>
#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <models/strategy.h>
#include "colorutils.h"

ActivitySessionWidget::ActivitySessionWidget(const ActivitySession &activitySession,
                                             QWidget *parent)
        : activitySession(activitySession),
          QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);

    layoutChildWidgets();

    updateUI();
    updateStyleSheet();
}

void ActivitySessionWidget::layoutChildWidgets() {
    auto *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    titleLabel = new QLabel();
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 15pt;"
                              "font-weight: bold;");
    titleLabel->setWordWrap(true);

    layout()->addWidget(titleLabel);
}

void ActivitySessionWidget::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    painter.setPen(Qt::NoPen);

    if (isSelected()) {
        drawSelection(painter);
    }

    if (activitySession.activity) {
        drawRulers(painter);
    }

}

void ActivitySessionWidget::drawRulers(QPainter &painter) const {
    auto rulerColor = QColor(ApplicationSettings::timeSlotBorderColor);
    rulerColor.setAlphaF(0.2);

    painter.setBrush(rulerColor);

    for (auto &timeSlot : activitySession.timeSlots) {
        auto timeSlotIndex = &timeSlot - &activitySession.timeSlots[0];

        if (timeSlotIndex == activitySession.length() - 1) {
            break;
        }

        auto thickness = timeSlot->endTime() % 60 == 0 ? 2 : 1;
        auto rulerRect = QRect(0,
                               slotHeight * (timeSlotIndex + 1) - 1,
                               width(),
                               thickness);

        painter.drawRect(rulerRect);
    }
}

void ActivitySessionWidget::drawSelection(QPainter &painter) const {
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(selectedBackgroundColor());

    auto lastTimeSlot = activitySession.timeSlots.back();
    auto bottomMargin = lastTimeSlot->endTime() % 60 == 0 ? 1 : 0;
    auto selectionRect = QRect(1,
                               2,
                               width() - 2,
                               height() - 5 - bottomMargin);

    painter.drawRoundedRect(selectionRect, 4, 4);
}

QColor ActivitySessionWidget::selectedBackgroundColor() const {
    if (!activitySession.activity) {
        return QColor("#f5f5f5");
    }

    auto backgroundColor = sessionColor().lighter(110);
    backgroundColor.setAlphaF(0.02);

    auto selectedBackgroundColor = backgroundColor;
    selectedBackgroundColor.setAlphaF(0.1);

    return selectedBackgroundColor;
}

QColor ActivitySessionWidget::sessionColor() const {
    auto color = activitySession.activity
                 ? QColor(QString::fromStdString(activitySession.activity->color()))
                 : QColor();
    return color;
}


void ActivitySessionWidget::updateStyleSheet() {
    auto color = activitySession.activity
                 ? ColorUtils::qColorFromStdString(activitySession.activity->color())
                 : QColor();


    auto leftBorderStyle = activitySession.activity
                           ? QString("border-left: 2px solid %1")
                                   .arg(color.name())
                           : "";

    auto thickBorder = activitySession.timeSlots.back()->endTime() % 60 == 0;
    auto borderColor = ApplicationSettings::timeSlotBorderColor;
    auto borderBottomThickness = thickBorder ? 2 : 1;

    auto borderBottomStyle = QString("border-bottom: %1px solid %2;")
            .arg(QString::number(borderBottomThickness))
            .arg(borderColor);

    setStyleSheet("ActivitySessionWidget { "
                  + borderBottomStyle
                  + "}");
}

bool ActivitySessionWidget::isSelected() const {
    return _isSelected;
}

void ActivitySessionWidget::setIsSelected(bool isSelected, bool doUpdate) {
    _isSelected = isSelected;

    if (doUpdate) {
        updateStyleSheet();
    }
}

void ActivitySessionWidget::updateUI() {
    auto heightChanged = height() != expectedHeight();
    auto durationChanged
            = previousDuration != activitySession.duration();
    auto activityChanged
            = previousActivitySession.activity != activitySession.activity;
    auto endTimeChanged = previousEndTime != activitySession.endTime();

    if (!activityChanged
        && !durationChanged
        && !heightChanged
        && !endTimeChanged) {
        return;
    }

    if (heightChanged) {
        setFixedHeight(expectedHeight());
    }

    if (activityChanged || endTimeChanged) {
        updateStyleSheet();
    }

    if (activityChanged || durationChanged) {
        updateLabel();
    }

    previousActivitySession = activitySession;
    previousDuration = activitySession.duration();
    previousEndTime = activitySession.endTime();
}

void ActivitySessionWidget::updateLabel() const {
    if (activitySession.activity) {
        titleLabel->setText(labelText());
    } else {
        titleLabel->setText("");
    }
}

QString ActivitySessionWidget::labelText() const {
    auto activity = activitySession.activity;

    auto name = QString::fromStdString(activity->name());
    auto color = QString::fromStdString(activity->color());
    auto time = humanTimeForMinutes(activitySession.duration());

    return QString("<font color=\"#888\"><b>%1</b></font>"
                   " "
                   "<font color=\"%2\"><b>%3</b></font>")
            .arg(time).arg(color).arg(name);
}

void ActivitySessionWidget::setActivitySession(
        const ActivitySession &newActivitySession) {
    activitySession = newActivitySession;
    updateUI();
}

void ActivitySessionWidget::setSlotHeight(int newSlotHeight) {
    slotHeight = newSlotHeight;
}

int ActivitySessionWidget::expectedHeight() {
    return activitySession.length() * slotHeight;
}