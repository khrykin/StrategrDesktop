#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <cmath>
#include <string>

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QPalette>

namespace ColorUtils {
    inline QColor scale(const QColor &color, double factor) {
        auto redComponent = static_cast<int>(factor * color.red());
        auto greenComponent = static_cast<int>(factor * color.green());
        auto blueComponent = static_cast<int>(factor * color.blue());

        return QColor(redComponent, greenComponent, blueComponent);
    }

    inline QColor lighten(const QColor &color, double factor) {
        return scale(color, 1 + factor);
    }

    inline QColor darken(const QColor &color, double factor) {
        return scale(color, 1 - factor);
    }

    inline QColor desaturated(QColor color, double factor) {
        color.setHslF(color.hueF(), factor, factor);
        return color;
    }

    inline QColor QColorFromStdString(const std::string &hex) {
        return QColor(QString::fromStdString(hex));
    }

    inline QString qColorToCSS(const QColor &color) {
        if (color.alpha() == 255) {
            return QString("rgb(%1, %2, %3)")
                .arg(color.red())
                .arg(color.green())
                .arg(color.blue());
        }

        return QString("rgba(%1, %2, %3, %4)")
            .arg(color.red())
            .arg(color.green())
            .arg(color.blue())
            .arg(color.alpha());
    }

    inline QColor overlayWithAlpha(const QColor &overlay,
                                   double alpha = 1.0,
                                   const QColor &substrate = QApplication::palette().color(QPalette::Base)) {
        auto redComponent = substrate.redF() + (overlay.redF() - substrate.redF()) * alpha;
        auto greenComponent = substrate.greenF() + (overlay.greenF() - substrate.greenF()) * alpha;
        auto blueComponent = substrate.blueF() + (overlay.blueF() - substrate.blueF()) * alpha;

        return QColor(static_cast<int>(255 * redComponent),
                      static_cast<int>(255 * greenComponent),
                      static_cast<int>(255 * blueComponent));
    }

    inline double shadesAlphaFactor(int power = 4,
                                    float offset = 0.5,
                                    const QColor &source = QApplication::palette().color(QPalette::Base)) {
        power = power - power % 2;
        return std::pow(2, power) * (1 - offset) * std::pow(source.lightnessF() - 0.5, power) + offset;
    }

    inline QColor safeForegroundColor(const QColor &color) {
        if (color == QColor(Qt::black) || color == QColor(Qt::white)) {
            return QApplication::palette().color(QPalette::Text);
        }

        return color;
    }
}// namespace ColorUtils

#endif// COLORUTILS_H
