#ifndef SLOTRULER_H
#define SLOTRULER_H

#include <QVector>
#include <QWidget>

class SlotRuler : public QWidget {
  Q_OBJECT

public:
  explicit SlotRuler(int cellHeight, QWidget *parent = nullptr);

  QVector<QString> labels() const;
  void setLabels(QVector<QString> labels);
  static const auto defaultRulerWidth = 60;

  int cellHeight() const;
  void setCellHeight(int cellHeight);

private:
  void paintEvent(QPaintEvent *) override;
  QVector<QString> _labels;
  void updateUI();
  int _cellHeight = 50;
};

#endif // SLOTRULER_H
