#ifndef SLOTRULER_H
#define SLOTRULER_H

#include <QStringList>
#include <QWidget>
#include <QLabel>

#include "applicationsettings.h"
#include "reactivelist.hpp"

class SlotRuler : public QWidget,
                  public ReactiveList<QLabel> {
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

    // ReactiveList

    int numberOfItems() override;
    QVBoxLayout *listLayout() override;
    void reuseItemAtIndex(int index, QLabel *itemWidget) override;
    QLabel *makeNewItemAtIndex(int index) override;
};

#endif // SLOTRULER_H
