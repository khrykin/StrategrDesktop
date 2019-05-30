#ifndef SLOTRULER_H
#define SLOTRULER_H

#include <QWidget>

class SlotRuler:  public QWidget
{
    Q_OBJECT

public:
    explicit SlotRuler(QWidget *parent = nullptr);

    int numberOfSlots() const;
    void setNumberOfSlots(int numberOfSlots);

private:
    void paintEvent(QPaintEvent *) override;


    int _numberOfSlots;
    void updateUI();
};

#endif // SLOTRULER_H
