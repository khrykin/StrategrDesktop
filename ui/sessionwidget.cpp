#include <cmath>

#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QStaticText>

#include "sessionwidget.h"
#include "utils.h"
#include "strategy.h"
#include "colorutils.h"
#include "fontutils.h"
#include "applicationsettings.h"

SessionWidget::SessionWidget(stg::session activitySession,
                             QWidget *parent)
        : activitySession(std::move(activitySession)),
          QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);

    updateUI();
}

void SessionWidget::paintEvent(QPaintEvent *) {
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

void SessionWidget::drawRulers(QPainter &painter) const {
    auto baseColor = palette().color(QPalette::Base);
    auto alphaFactor = ColorUtils::shadesAlphaFactor(8);

    auto rulerColor = ColorUtils::overlayWithAlpha(
            QColor(ApplicationSettings::rowBorderColor),
            0.06 * alphaFactor,
            baseColor);

    painter.setBrush(rulerColor);

    for (auto &timeSlot : activitySession.time_slots) {
        auto timeSlotIndex = static_cast<int>(&timeSlot - &activitySession.time_slots[0]);

        if (timeSlotIndex == activitySession.length() - 1) {
            break;
        }

        auto thickness = timeSlot.end_time() % 60 == 0 ? 2 : 1;
        auto rulerRect = QRect(0,
                               slotHeight * (timeSlotIndex + 1) - 1,
                               width(),
                               thickness);

        painter.drawRect(rulerRect);
    }
}

void SessionWidget::drawSelection(QPainter &painter) const {
    if (!activitySession.activity) {
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(selectedBackgroundColor());

    auto lastTimeSlot = activitySession.time_slots.back();
    auto bottomMargin = lastTimeSlot.end_time() % 60 == 0 || isBorderSelected ? 1 : 0;
    auto selectionRect = QRect(1,
                               2,
                               width() - 2,
                               height() - 5 - bottomMargin);

    painter.drawRoundedRect(selectionRect, 4, 4);
}

QColor SessionWidget::selectedBackgroundColor() const {
    using namespace ColorUtils;

    if (!activitySession.activity) {
        return overlayWithAlpha(textColor(),
                                0.05 * shadesAlphaFactor(0));
    }

    auto backgroundColor = sessionColor().lighter(110);
    backgroundColor.setAlphaF(0.02);

    auto selectedBackgroundColor = backgroundColor;
    selectedBackgroundColor.setAlphaF(0.1);

    return selectedBackgroundColor;
}

QColor SessionWidget::sessionColor() const {
    auto color = activitySession.activity
                 ? QColor(QString::fromStdString(activitySession.activity->color()))
                 : QColor();
    return color;
}


void SessionWidget::drawBorder(QPainter &painter) {
    auto thickBorder = activitySession.time_slots.back().end_time() % 60 == 0 ||
                       isBorderSelected;

    auto borderThickness = thickBorder ? 2 : 1;

    auto borderRect = QRect(0,
                            height() - borderThickness,
                            width(),
                            borderThickness);

    painter.setBrush(isBorderSelected ? highlightColor() : borderColor());
    painter.drawRect(borderRect);
}

QColor SessionWidget::borderColor() {
    return ColorUtils::overlayWithAlpha(
            QColor(ApplicationSettings::rowBorderColor),
            0.35,
            QApplication::palette().color(QPalette::Base));
}

bool SessionWidget::isSelected() const {
    return _isSelected;
}

void SessionWidget::setIsSelected(bool isSelected, bool doUpdate) {
    _isSelected = isSelected;

    if (!isSelected) {
        isBorderSelected = false;
    }

    if (doUpdate) {
        update();
    }
}

void SessionWidget::updateUI() {
    auto heightChanged = height() != expectedHeight();
    auto durationChanged
            = previousDuration != activitySession.duration();
    auto activityChanged
            = previousActivitySession.activity != activitySession.activity;
    auto endTimeChanged = previousEndTime != activitySession.end_time();

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

    previousActivitySession = activitySession;
    previousDuration = activitySession.duration();
    previousEndTime = activitySession.end_time();
}

void SessionWidget::setActivitySession(
        const stg::session &newActivitySession) {
    activitySession = newActivitySession;
    updateUI();
}

void SessionWidget::setSlotHeight(int newSlotHeight) {
    slotHeight = newSlotHeight;
}

int SessionWidget::expectedHeight() {
    return static_cast<int>(activitySession.length()) * slotHeight;
}

void SessionWidget::drawLabel(QPainter &painter) const {
    auto font = QFont();
    font.setBold(true);
    font.setPixelSize(ApplicationSettings::sessionFontSize);

    painter.setFont(font);

    auto textRect = QRect(ApplicationSettings::defaultPadding,
                          ApplicationSettings::defaultPadding,
                          width() - 2 * ApplicationSettings::defaultPadding,
                          height() - 2 * ApplicationSettings::defaultPadding);

    FontUtils::drawSessionTitle(activitySession,
                                painter,
                                textRect);

    painter.setPen(Qt::NoPen);
}

void SessionWidget::setSelectBorder(bool newIsBorderSelected) {
    isBorderSelected = newIsBorderSelected;

    update();
}
