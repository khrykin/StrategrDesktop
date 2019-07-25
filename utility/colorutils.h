#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <QColor>
#include <string>

namespace ColorUtils {
    inline QColor scale(QColor &color, double factor) {
        auto redComponent = static_cast<int>(factor * color.red());
        auto greenComponent = static_cast<int>(factor * color.green());
        auto blueComponent = static_cast<int>(factor * color.blue());

        return QColor(redComponent, greenComponent, blueComponent);
    }

    inline QColor lighten(QColor &color, double factor) {
        return scale(color, 1 + factor);
    }

    inline QColor darken(QColor &color, double factor) {
        return scale(color, 1 - factor);
    }

    inline QColor qColorFromStdString(const std::string &hex) {
        return QColor(QString::fromStdString(hex));
    }

    inline QString qColorToCSS(const QColor &color) {
        return QString("rgba(%1, %2, %3, %4)")
                .arg(color.red())
                .arg(color.green())
                .arg(color.blue())
                .arg(color.alpha());
    }
} // namespace ColorUtils

#endif // COLORUTILS_H
