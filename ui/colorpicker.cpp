#include <QDebug>
#include <QHBoxLayout>
#include <random>

#include "colorpicker.h"
#include "colorpickeritem.h"

const auto ColorPicker::defaultColors = QVector{
        QColor("#FF4136"),
        QColor("#FFB700"),
        QColor("#FFD700"),
        QColor("#A463F2"),
        QColor("#D5008F"),
        QColor("#19A974"),
        QColor("#357EDD"),
        QColor("#000000"),
        QColor("#777777")
};

ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,
                                   0,
                                   0,
                                   0);
            foreach (const auto &color, colors) {
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

    auto indexOfColor = colors.indexOf(*color);
    if (indexOfColor < 0)
        return deselectAll();

    _color = color;

    for (auto i = 0; i < layout()->count(); i++) {
        auto *item = qobject_cast<ColorPickerItem *>(layout()->itemAt(i)->widget());
        item->setIsSelected(indexOfColor == i);
    }

    emit colorChanged(_color);
}

void ColorPicker::setRandomColor() {
    std::random_device seeder;
    std::mt19937 engine(seeder());
    std::uniform_int_distribution<int> dist(0, colors.length() - 1);
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

