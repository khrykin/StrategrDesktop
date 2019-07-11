#ifndef SLOTRULER_H
#define SLOTRULER_H

#include <QStringList>
#include <QWidget>
#include "applicationsettings.h"

class SlotRuler : public QWidget {
Q_OBJECT

public:
    explicit SlotRuler(QStringList labels,
                       int cellHeight,
                       QWidget *parent = nullptr);

    QStringList labels() const;
    void setLabels(const QStringList &labels);

    int cellHeight() const;
    void setCellHeight(int cellHeight);

private:
    QStringList _labels;
    int _cellHeight = ApplicationSettings::defaultSlotHeight;

    void paintEvent(QPaintEvent *) override;
    void updateUI();

    void removeExtraCells() const;
    void reuseCellAtIndex(int cellIndex) const;
};

#endif // SLOTRULER_H
