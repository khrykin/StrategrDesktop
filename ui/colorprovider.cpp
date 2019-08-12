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
            0.25 * ColorUtils::shadesAlphaFactor());
}

QColor ColorProvider::hardBorderColor() {
    return ColorUtils::overlayWithAlpha(
            QColor(ApplicationSettings::rowBorderColor),
            0.9 * ColorUtils::shadesAlphaFactor(100));
}

QColor ColorProvider::highlightColor() {
    return QApplication::palette().color(QPalette::Highlight);
}

QColor ColorProvider::selectionColor() {
    auto selectionColor = highlightColor();
    selectionColor.setAlphaF(0.5);
    return selectionColor;

}

QColor ColorProvider::textColor() {
    return QApplication::palette().color(QPalette::Text);
}

QColor ColorProvider::windowColor() {
    return QApplication::palette().color(QPalette::Window);
}

QColor ColorProvider::panelColor() {
    return ColorUtils::overlayWithAlpha(
            QApplication::palette().color(QPalette::Window),
            0.6 * ColorUtils::shadesAlphaFactor());
}

QColor ColorProvider::darkPanelColor() {
    return ColorUtils::overlayWithAlpha(
            QApplication::palette().color(QPalette::Window),
            0.95 * ColorUtils::shadesAlphaFactor());
}

QColor ColorProvider::highlightedTextColor() {
    return QApplication::palette().color(QPalette::HighlightedText);
}

QColor ColorProvider::secondaryTextColor() {
    return ColorUtils::overlayWithAlpha(
            QApplication::palette().color(QPalette::Text),
            0.5 * ColorUtils::shadesAlphaFactor(2));
}

QColor ColorProvider::tertiaryTextColor() {
    return ColorUtils::overlayWithAlpha(
            QApplication::palette().color(QPalette::Text),
            0.35 * ColorUtils::shadesAlphaFactor());
}

QColor ColorProvider::textColorJustLighter() {
    return ColorUtils::overlayWithAlpha(
            QApplication::palette().color(QPalette::Text),
            0.75);
}


