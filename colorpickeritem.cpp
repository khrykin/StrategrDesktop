#include "colorpickeritem.h"
#include "colorutils.h"
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>

ColorPickerItem::ColorPickerItem(QColor color, QWidget *parent)
    : QWidget(parent), _color(color) {
  setFixedWidth(24);
  setFixedHeight(24);
}

void ColorPickerItem::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  auto bgColor = QColor(_color);
  if (isClicked) {
    bgColor = ColorUtils::darken(bgColor, 0.2);
  }

  painter.setRenderHint(QPainter::Antialiasing);

  if (_isSelected || isHovered) {
    painter.setPen(QColor("#999"));
    auto brushColor = isHovered ? QColor(0, 0, 0, 50) : QColor(0, 0, 0, 0);
    painter.setBrush(brushColor);
    painter.drawEllipse(event->rect().adjusted(1, 1, -1, -1));
  }

  painter.setBrush(bgColor);
  painter.setPen(ColorUtils::darken(bgColor, 0.2));
  painter.drawEllipse(event->rect().adjusted(5, 5, -5, -5));

  painter.end();
}

void ColorPickerItem::mousePressEvent(QMouseEvent *event) {
  isClicked = true;
  update();
}

void ColorPickerItem::enterEvent(QEvent *event) {
  qDebug() << "ENTER";
  isHovered = true;
  update();
}

void ColorPickerItem::leaveEvent(QEvent *event) {
  qDebug() << "LEAVE";
  isHovered = false;
  update();
}

void ColorPickerItem::mouseReleaseEvent(QMouseEvent *event) {
  if (isClicked) {
    qDebug() << "Clicked color";
    isClicked = false;
    toggleIsSelected();
    emit clicked(_color);
  }
}

bool ColorPickerItem::isSelected() const { return _isSelected; }

void ColorPickerItem::setIsSelected(bool isSelected) {
  _isSelected = isSelected;
  update();
}

void ColorPickerItem::toggleIsSelected() {
  _isSelected = !_isSelected;
  update();
}

QColor ColorPickerItem::color() const { return _color; }

void ColorPickerItem::setColor(const QColor &color) { _color = color; }
