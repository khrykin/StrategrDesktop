#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QColor>
#include <QWidget>
#include <optional>

class ColorPicker : public QWidget {
  Q_OBJECT
public:
  explicit ColorPicker(QWidget *parent = nullptr);

  std::optional<QColor> color() const;
  void setColor(const std::optional<QColor> &color);

signals:
  void colorChanged(const std::optional<QColor> &color);

public slots:
private:
  void deselectAll();
  std::optional<QColor> _color;
  const QVector<QColor> colors = ColorPicker::defaultColors;
  static inline const QVector<QColor> defaultColors = {
      QColor("#FF4136"), QColor("#FFB700"), QColor("#FFD700"),
      QColor("#A463F2"), QColor("#D5008F"), QColor("#19A974"),
      QColor("#357EDD"), QColor("#111111"), QColor("#777777")};
};

#endif // COLORPICKER_H
