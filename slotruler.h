#ifndef SLOTRULER_H
#define SLOTRULER_H

#include <QWidget>
#include <QVector>

class SlotRuler:  public QWidget
{
    Q_OBJECT

public:
    explicit SlotRuler(QWidget *parent = nullptr);

    QVector<QString> labels() const;
    void setLabels(QVector<QString> labels);

private:
    void paintEvent(QPaintEvent *) override;
    QVector<QString> _labels;
    void updateUI();
};

#endif // SLOTRULER_H
