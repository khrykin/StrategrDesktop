#include "colorpicker.h"
#include "colorpickeritem.h"
#include <QDebug>
#include <QHBoxLayout>

ColorPicker::ColorPicker(QWidget *parent) : QWidget(parent) {
  auto *mainLayout = new QHBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->setContentsMargins(mainLayout->contentsMargins().left(), 0,
                                 mainLayout->contentsMargins().right(),
                                 mainLayout->contentsMargins().bottom() / 2);
  foreach (const auto &color, colors) {
    auto *item = new ColorPickerItem(color, this);
    connect(item, &ColorPickerItem::clicked,
            [=](const QColor &color) { setColor(color); });
    mainLayout->addWidget(item);
  }
}

std::optional<QColor> ColorPicker::color() const { return _color; }

void ColorPicker::setColor(const std::optional<QColor> &color) {
  _color = std::nullopt;

  if (!color) {
    return deselectAll();
    ;
  }

  auto indexOfColor = colors.indexOf(color.value());
  qDebug() << "indexOfColor" << indexOfColor;

  if (indexOfColor < 0) {
    return deselectAll();
  }

  _color = color;

  for (auto i = 0; i < layout()->count(); i++) {
    auto *item = static_cast<ColorPickerItem *>(layout()->itemAt(i)->widget());
    item->setIsSelected(indexOfColor == i);
  }

  //  auto *item =
  //      static_cast<ColorPickerItem
  //      *>(layout()->itemAt(indexOfColor)->widget());
  //  if (!item->isSelected()) {
  //    item->setIsSelected(true);
  //  }
  emit colorChanged(color);
}

void ColorPicker::deselectAll() {
  for (auto i = 0; i < layout()->count(); i++) {
    auto *item = static_cast<ColorPickerItem *>(layout()->itemAt(i)->widget());
    item->setIsSelected(false);
  }

  return;
}
