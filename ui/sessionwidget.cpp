#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QStaticText>
#include <QStyleOption>
#include <QTextDocument>

#include "application.h"
#include "applicationsettings.h"
#include "colorutils.h"
#include "drawingutils.h"
#include "fontutils.h"
#include "sessionwidget.h"
#include "theme.h"

SessionWidget::SessionWidget(const stg::session &session, QWidget *parent)
    : session(session), DataProviderWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);

    reloadSession();
}

void SessionWidget::setSession(const stg::session &newSession) {
    if (newSession == session) {
        return;
    }

    session = newSession;
    reloadSession();
}


void SessionWidget::setIsBorderSelected(bool isBorderSelected) {
    if (_isBorderSelected == isBorderSelected) {
        return;
    }

    _isBorderSelected = isBorderSelected;
    update();
}

int SessionWidget::expectedHeight() {
    return (int) session.length() * slotHeight();
}

void SessionWidget::setDrawsBorders(bool drawsBorders) {
    if (_drawsBorders == drawsBorders) {
        return;
    }

    _drawsBorders = drawsBorders;
    update();
}

void SessionWidget::reloadSession() {
    auto heightChanged = height() != expectedHeight();
    auto durationChanged = previousDuration != session.duration();
    auto activityChanged = previousSession.activity != session.activity;
    auto endTimeChanged = previousEndTime != session.end_time();

    if (!activityChanged &&
        !durationChanged &&
        !heightChanged &&
        !endTimeChanged) {
        return;
    }

    if (heightChanged) {
        setFixedHeight(expectedHeight());
    } else {
        update();
    }

    previousSession = session;
    previousDuration = session.duration();
    previousEndTime = session.end_time();
}


void SessionWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.setPen(Qt::NoPen);

    if (_drawsBorders) {
        drawBorder(painter);
    }

    drawBackground(painter);

    if (_drawsBorders) {
        drawRulers(painter);
    }

    if (session.activity) {
        drawLabel(painter);
    }
}

void SessionWidget::drawRulers(QPainter &painter) {
    QColor rulerColor = session.activity
                            ? QColor(Application::theme().session_ruler_color(session, _isSelected))
                            : borderColor();

    painter.setBrush(rulerColor);

    for (const auto &timeSlot : session.time_slots) {
        auto timeSlotIndex = static_cast<int>(&timeSlot - &session.time_slots[0]);

        if (timeSlotIndex == 0) {
            continue;
        }

        auto thickness = timeSlot.begin_time % 60 == 0 ? 2 : 1;
        auto rulerRect = QRect(0,
                               slotHeight() * (timeSlotIndex),
                               width(),
                               thickness);

        painter.drawRect(rulerRect);
    }
}

void SessionWidget::drawBackground(QPainter &painter) {
    if (!session.activity) {
        return;
    }

    QColor color = Application::theme()
                       .session_background_color(session, _isSelected);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(color);

    const auto backgroundRect = QRect(0,
                                      2 + topMargin(),
                                      width(),
                                      height() - 4 - topMargin());
    const auto radius = 5;
    const auto roundness = 0.2;

    painter.drawPath(DrawingUtils::squirclePath(backgroundRect, radius, roundness));
}

QColor SessionWidget::selectedBackgroundColor() const {
    return session.activity->color();
}

QColor SessionWidget::sessionColor() const {
    auto color = session.activity
                     ? QColor(session.activity->color())
                     : QColor();
    return color;
}


void SessionWidget::drawBorder(QPainter &painter) {
    auto thickBorder = session.time_slots.front().begin_time % 60 == 0 ||
                       _isBorderSelected;

    auto borderThickness = thickBorder ? 2 : 1;

    auto borderRect = QRectF(0,
                             0,
                             width(),
                             borderThickness);

    painter.setBrush(_isBorderSelected ? controlColor() : borderColor());
    painter.drawRect(borderRect);
}

QColor SessionWidget::borderColor() {
    return ColorUtils::overlayWithAlpha(QColor(ApplicationSettings::rowBorderColor),
                                        0.35 * ColorUtils::shadesAlphaFactor(12),
                                        QApplication::palette().color(QPalette::Base));
}

void SessionWidget::setIsSelected(bool isSelected) {
    if (_isSelected == isSelected) {
        return;
    }

    _isSelected = isSelected;

    if (!isSelected) {
        _isBorderSelected = false;
    }

    update();
}

void SessionWidget::drawLabel(QPainter &painter) {
    using namespace ApplicationSettings;

    auto font = QFont();
    font.setBold(true);
    font.setPixelSize(sessionFontSize);

    painter.setFont(font);

    auto textRect = QRect(defaultPadding,
                          defaultPadding + topMargin(),
                          width() - 2 * defaultPadding,
                          height() - 2 * defaultPadding - topMargin());

    auto durationColor = Application::theme().session_duration_color(session, _isSelected);
    auto titleColor = Application::theme().session_title_color(session, _isSelected);

    FontUtils::drawSessionTitle(session,
                                painter,
                                textRect,
                                durationColor,
                                titleColor);

    painter.setPen(Qt::NoPen);
}

int SessionWidget::topMargin() {
    return session.time_slots.front().begin_time % 60 == 0 || _isBorderSelected ? 2 : 1;
}
