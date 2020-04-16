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

    if (_drawsBorders)
        drawBorder(painter);

    drawBackground(painter);

    if (activitySession.activity) {
        if (_drawsBorders)
            drawRulers(painter);

        drawLabel(painter);
    }

//    if (dimmed) {
//        auto dimColor = baseColor();
//        dimColor.setAlphaF(0.2);
//
//        painter.setBrush(dimColor);
//        painter.drawRect(contentsRect());
//    }
}

void SessionWidget::drawRulers(QPainter &painter) const {
    auto blackColor = QColor(Qt::black);
    auto desaturatedColor = sessionColor();
    desaturatedColor.setHslF(desaturatedColor.hueF(), 0.3, 0.75);

    auto rulerColor = isSelected()
                      ? ColorUtils::overlayWithAlpha(blackColor, 0.3, selectedBackgroundColor())
                      : desaturatedColor;

    if (isSelected() && rulerColor.lightnessF() < 0.1) {
        rulerColor = QColor(Qt::white);
    }

    rulerColor.setAlphaF(0.2);

    painter.setBrush(rulerColor);

    for (auto &timeSlot : activitySession.time_slots) {
        auto timeSlotIndex = static_cast<int>(&timeSlot - &activitySession.time_slots[0]);

        if (timeSlotIndex == activitySession.length() - 1) {
            break;
        }

        auto thickness = timeSlot.end_time() % 60 == 0 ? 2 : 1;
        auto rulerRect = QRect(0,
                               slotHeight * (timeSlotIndex + 1) - thickness,
                               width(),
                               thickness);

        painter.drawRect(rulerRect);
    }
}

void SessionWidget::drawBackground(QPainter &painter) const {
    if (!activitySession.activity) {
        return;
    }

    auto color = isSelected() ? selectedBackgroundColor() : backgroundColor();

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(color);

    auto lastTimeSlot = activitySession.time_slots.back();
    auto bottomMargin = lastTimeSlot.end_time() % 60 == 0 || isBorderSelected ? 1 : 0;
    auto selectionRect = QRect(0,
                               2,
                               width(),
                               height() - 5 - bottomMargin);

    painter.drawRoundedRect(selectionRect, 4, 4);
}

QColor SessionWidget::backgroundColor() const {
    using namespace ColorUtils;

    if (!activitySession.activity) {
        return QColor(255, 255, 255, 255);
    }

    auto backgroundColor = sessionColor();
    backgroundColor.setAlphaF(0.15);

    return backgroundColor;
}

QColor SessionWidget::selectedBackgroundColor() const {
    auto selectedBackgroundColor = backgroundColor();
    selectedBackgroundColor.setAlphaF(1);

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

    painter.setBrush(isBorderSelected ? controlColor() : borderColor());
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

void SessionWidget::setIsSelected(bool isSelected) {
    _isSelected = isSelected;

    if (!isSelected) {
        isBorderSelected = false;
    }

    update();
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

    auto sessionColorDesaturated
            = ColorUtils::overlayWithAlpha(textColor(), 0.5, selectedBackgroundColor());
    sessionColorDesaturated.setHsvF(sessionColorDesaturated.hueF(), 0.2, 0.5);

    auto selectedDurationColor
            = ColorUtils::overlayWithAlpha(baseColor(), 0.7, selectedBackgroundColor());

    auto durationColor = isSelected()
                         ? selectedDurationColor
                         : sessionColorDesaturated;

    auto titleColor = isSelected() ? Qt::white : sessionColor();

    if (sessionColor() == QColor(Qt::black)) {
        titleColor = textColor();
    }

    if (sessionColor().lightnessF() < 0.2) {
        if (isSelected()) {
            titleColor = Qt::white;
            durationColor = ColorUtils::overlayWithAlpha(Qt::white, 0.5, sessionColor());
        }
    }

    FontUtils::drawSessionTitle(activitySession,
                                painter,
                                textRect,
                                durationColor,
                                titleColor);

    painter.setPen(Qt::NoPen);
}

void SessionWidget::setSelectBorder(bool newIsBorderSelected) {
    isBorderSelected = newIsBorderSelected;

    update();
}

void SessionWidget::setDimmed(bool dimmed) {
    this->dimmed = dimmed;
    update();
}

bool SessionWidget::drawsBorders() const {
    return _drawsBorders;
}

void SessionWidget::setDrawsBorders(bool drawsBorders) {
    _drawsBorders = drawsBorders;
    update();
}
