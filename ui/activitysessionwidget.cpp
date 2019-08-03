#include <cmath>

#include <QDebug>
#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QStaticText>

#include "activitysessionwidget.h"
#include "utils.h"
#include "strategy.h"
#include "colorutils.h"
#include "fontutils.h"

#include "third-party/stacklayout.h"

ActivitySessionWidget::ActivitySessionWidget(const ActivitySession &activitySession,
                                             QWidget *parent)
        : activitySession(activitySession),
          QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);

    updateUI();
}

void ActivitySessionWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.setPen(Qt::NoPen);

    drawBorder(painter);

    if (activitySession.activity) {
        drawRulers(painter);
        drawLabel(painter);
    }

    if (isSelected()) {
        drawSelection(painter);
    }
}

void ActivitySessionWidget::drawRulers(QPainter &painter) const {
    auto baseColor = palette().color(QPalette::Base);
    auto alphaFactor = ColorUtils::shadesAlphaFactor(8);

    auto rulerColor = ColorUtils::qColorOverlayWithAlpha(
            QColor(ApplicationSettings::rowBorderColor),
            0.06 * alphaFactor,
            baseColor);

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
    if (!activitySession.activity) {
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(selectedBackgroundColor());

    auto lastTimeSlot = activitySession.timeSlots.back();
    auto bottomMargin = lastTimeSlot->endTime() % 60 == 0 || isBorderSelected ? 1 : 0;
    auto selectionRect = QRect(1,
                               2,
                               width() - 2,
                               height() - 5 - bottomMargin);

    painter.drawRoundedRect(selectionRect, 4, 4);
}

QColor ActivitySessionWidget::selectedBackgroundColor() const {
    using namespace ColorUtils;

    if (!activitySession.activity) {
        return qColorOverlayWithAlpha(textColor(),
                                      0.05 * shadesAlphaFactor());
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


void ActivitySessionWidget::drawBorder(QPainter &painter) {
    auto thickBorder = activitySession.timeSlots.back()->endTime() % 60 == 0 ||
                       isBorderSelected;

    auto borderThickness = thickBorder ? 2 : 1;

    auto borderRect = QRect(0,
                            height() - borderThickness,
                            width(),
                            borderThickness);

    painter.setBrush(isBorderSelected ? highlightColor() : borderColor());
    painter.drawRect(borderRect);
}

QColor ActivitySessionWidget::borderColor() {
    return ColorUtils::qColorOverlayWithAlpha(
            QColor(ApplicationSettings::rowBorderColor),
            0.35,
            QApplication::palette().color(QPalette::Base));
}

bool ActivitySessionWidget::isSelected() const {
    return _isSelected;
}

void ActivitySessionWidget::setIsSelected(bool isSelected, bool doUpdate) {
    _isSelected = isSelected;

    if (!isSelected) {
        isBorderSelected = false;
    }

    if (doUpdate) {
        update();
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
    } else {
        update();
    }
//
//    if (activityChanged || endTimeChanged) {
//        update();
//    }
//
//    if (activityChanged || durationChanged) {
//        updateLabel();
//    }

    previousActivitySession = activitySession;
    previousDuration = activitySession.duration();
    previousEndTime = activitySession.endTime();
}

void ActivitySessionWidget::updateLabel() {
//    auto currentLabelText = makeLabelTextComponents();
//    if (currentLabelText != previousLabelText) {
//        setLabelText(makeLabelTextComponents);
//        previousLabelText = currentLabelText;
//    }
}

QStringList ActivitySessionWidget::makeLabelTextComponents() const {
    using namespace ColorUtils;
    auto activity = activitySession.activity;

    if (!activity) {
        return QStringList();
    }

    auto name = QString::fromStdString(activity->name());
    auto time = humanTimeForMinutes(activitySession.duration());

    return QStringList{time, name};
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

void ActivitySessionWidget::drawLabel(QPainter &painter) const {
    using namespace ColorUtils;

    auto font = QFont();
    font.setBold(true);
    font.setPointSize(15);

    painter.setFont(font);

    auto durationColor = ColorUtils::qColorOverlayWithAlpha(
            palette().color(QPalette::WindowText),
            0.5);

    auto color = safeForegroundColor(
            qColorFromStdString(activitySession.activity->color())
    );

    auto textComponents = makeLabelTextComponents();

    FontUtils::drawTruncatedTitleFromComponents(textComponents,
                                                painter,
                                                geometry(),
                                                durationColor,
                                                color);

    painter.setPen(Qt::NoPen);
}

void ActivitySessionWidget::setSelectBorder(bool newIsBorderSelected) {
    isBorderSelected = newIsBorderSelected;

    update();
}
