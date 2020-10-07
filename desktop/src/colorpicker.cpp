#include <QDebug>
#include <QHBoxLayout>
#include <algorithm>
#include <random>

#include "colorpicker.h"
#include "colorpickeritem.h"

ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    for (const auto &color : colors) {
        auto *item = new ColorPickerItem(color, this);
        connect(item,
                &ColorPickerItem::clicked,
                this,
                &ColorPicker::setColorFromPalette);

        mainLayout->addWidget(item);
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

std::optional<QColor> ColorPicker::color() const { return _color; }

void ColorPicker::setColor(const std::optional<QColor> &color) {
    _color = std::nullopt;

    if (!color)
        return deselectAll();

    auto it = std::find(colors.begin(), colors.end(), *color);
    if (it == colors.end())
        return deselectAll();

    _color = color;

    auto indexOfColor = std::distance(colors.begin(), it);
    for (auto i = 0; i < layout()->count(); i++) {
        auto *item = qobject_cast<ColorPickerItem *>(layout()->itemAt(i)->widget());
        item->setIsSelected(indexOfColor == i);
    }

    emit colorChanged(_color);
}

void ColorPicker::setRandomColor() {
    std::random_device seeder;
    std::mt19937 engine(seeder());
    std::uniform_int_distribution<int> dist(0, colors.size() - 1);
    int index = dist(engine);

    setColor(colors[index]);
}

void ColorPicker::deselectAll() {
    for (auto i = 0; i < layout()->count(); i++) {
        auto *item = qobject_cast<ColorPickerItem *>(layout()->itemAt(i)->widget());
        item->setIsSelected(false);
    }
}

void ColorPicker::setColorFromPalette(const std::optional<QColor> &color) {
    setColor(color);
    emit colorChangedByClick(_color);
}

std::vector<QColor> ColorPicker::colorsFromDefaultColors() {
    std::vector<QColor> result;
    std::transform(stg::activity::default_colors().begin(),
                   stg::activity::default_colors().end(),
                   std::back_inserter(result),
                   [=](const auto &color_info) {
                       return QColor(color_info.first);
                   });

    return result;
}
