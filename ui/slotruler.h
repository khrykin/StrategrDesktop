#ifndef SLOTRULER_H
#define SLOTRULER_H

#include <QStringList>
#include <QWidget>

#include "applicationsettings.h"
#include "reactivelist.hpp"
#include "timelabel.h"
#include "coloredlabel.h"
#include "colorprovider.h"

class SlotRuler : public QWidget,
                  public ReactiveList<ColoredLabel>,
                  public ColorProvider {
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
    using ColorGetter = QColor (*)();
    QVector<TimeLabel> _labels;
    int _cellHeight = ApplicationSettings::defaultSlotHeight;

    int calculateLabelWidth() const;
    bool isIntegerHourAtIndex(int index) const;

    ColorGetter labelColorGetterAtIndex(int index);

    // ReactiveList
    int numberOfItems() override;
    QVBoxLayout *listLayout() override;
    void reuseItemAtIndex(int index, ColoredLabel *itemWidget) override;
    ColoredLabel *makeNewItemAtIndex(int index) override;

    void paintEvent(QPaintEvent *) override;
};

#endif // SLOTRULER_H
