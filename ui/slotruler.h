#ifndef SLOTRULER_H
#define SLOTRULER_H

#include <QStringList>
#include <QWidget>
#include <QLabel>

#include "applicationsettings.h"
#include "reactivelist.hpp"
#include "timelabel.h"

class SlotRuler : public QWidget,
                  public ReactiveList<QLabel> {
Q_OBJECT

public:
    explicit SlotRuler(QVector<TimeLabel> labels,
                       int cellHeight,
                       QWidget *parent = nullptr);

    QVector<TimeLabel> labels() const;
    void setLabels(const QVector<TimeLabel> &labels);

    int cellHeight() const;
    void setCellHeight(int cellHeight);

private:
    QVector<TimeLabel> _labels;
    int _cellHeight = ApplicationSettings::defaultSlotHeight;

    void paintEvent(QPaintEvent *) override;

    // ReactiveList

    int numberOfItems() override;
    QVBoxLayout *listLayout() override;
    void reuseItemAtIndex(int index, QLabel *itemWidget) override;
    QLabel *makeNewItemAtIndex(int index) override;
    QString makeStyleSheetForLabelIndex(int index) const;
    int calculateLabelWidth() const;
};

#endif // SLOTRULER_H
