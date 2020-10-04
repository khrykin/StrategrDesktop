#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QColor>
#include <QWidget>
#include <optional>

#include "activity.h"

class ColorPicker : public QWidget {
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = nullptr);

    std::optional<QColor> color() const;
    void setColor(const std::optional<QColor> &color);
    void setRandomColor();

signals:
    void colorChanged(const std::optional<QColor> &color);
    void colorChangedByClick(const std::optional<QColor> &color);

private:
    static const QVector<QColor> defaultColors;

    std::optional<QColor> _color;
    const std::vector<QColor> colors = colorsFromDefaultColors();

    void deselectAll();

    void setColorFromPalette(const std::optional<QColor> &color);

    static std::vector<QColor> colorsFromDefaultColors();
};

#endif// COLORPICKER_H
