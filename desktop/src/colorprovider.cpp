//
// Created by Dmitry Khrykin on 2019-07-26.
//

#include "colorprovider.h"
#include "applicationsettings.h"

QColor ColorProvider::baseColor() {
    return QApplication::palette().color(QPalette::Base);
}

QColor ColorProvider::borderColor() {
    return ColorUtils::overlayWithAlpha(
        QColor(ApplicationSettings::rowBorderColor),
        0.25 * ColorUtils::shadesAlphaFactor(0));
}

QColor ColorProvider::hardBorderColor() {
    return ColorUtils::overlayWithAlpha(
        QColor(ApplicationSettings::rowBorderColor),
        0.9 * ColorUtils::shadesAlphaFactor(8, 0.1f));
}

QColor ColorProvider::highlightColor() {
    return QApplication::palette().color(QPalette::Highlight);
}

QColor ColorProvider::selectionColor() {
    auto selectionColor = highlightColor();
    selectionColor.setAlphaF(0.5);
    return selectionColor;
}

#ifndef Q_OS_MAC

QColor ColorProvider::controlColor() {
    auto color = highlightColor();
    color.setHsvF(color.hueF(), 1.0, color.valueF());
    return color;
}

#endif

QColor ColorProvider::textColor() {
    return QApplication::palette().color(QPalette::Text);
}

QColor ColorProvider::windowColor() {
    return QApplication::palette().color(QPalette::Window);
}

QColor ColorProvider::panelColor() {
    using namespace ColorUtils;

    auto base = QApplication::palette().color(QPalette::Base);
    auto overlay = base.lightnessF() > 0.5 ? darken(base, 0.15) : lighten(base, 0.15);

    return overlayWithAlpha(overlay, 0.6 * shadesAlphaFactor(0), base);
}

QColor ColorProvider::darkPanelColor() {
    using namespace ColorUtils;

    auto base = QApplication::palette().color(QPalette::Base);
    auto overlay = base.lightnessF() > 0.5 ? darken(base, 0.15) : lighten(base, 0.15);

    return overlayWithAlpha(overlay, 0.95 * shadesAlphaFactor(0), base);
}

QColor ColorProvider::highlightedTextColor() {
    return QApplication::palette().color(QPalette::HighlightedText);
}

QColor ColorProvider::secondaryTextColor() {
    return ColorUtils::overlayWithAlpha(
        QApplication::palette().color(QPalette::Text),
        0.5 * ColorUtils::shadesAlphaFactor(4, 0.95f));
}

QColor ColorProvider::tertiaryTextColor() {
    return ColorUtils::overlayWithAlpha(
        QApplication::palette().color(QPalette::Text),
        0.30 * ColorUtils::shadesAlphaFactor(0));
}

QColor ColorProvider::textColorJustLighter() {
    return ColorUtils::overlayWithAlpha(
        QApplication::palette().color(QPalette::Text),
        0.75);
}
