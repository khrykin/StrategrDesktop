#ifndef SLOTRULER_H
#define SLOTRULER_H

#include <QStringList>
#include <QWidget>

#include "applicationsettings.h"
#include "reactivelist.hpp"
#include "timelabel.h"
#include "coloredlabel.h"
#include "colorprovider.h"
#include "selection.h"

class SlotsWidget;
class SlotRuler : public QWidget,
                  public ReactiveList<ColoredLabel>,
                  public ColorProvider {
Q_OBJECT
public:
    explicit SlotRuler(const QVector<TimeLabel> &labels,
                       int cellHeight,
                       QWidget *parent = nullptr);

    const QVector<TimeLabel> &labels() const;
    void setLabels(const QVector<TimeLabel> &labels);

    const stg::selection &selection() const;

    int cellHeight() const;
    void setCellHeight(int cellHeight);

    void updateList() {
        ReactiveList<ColoredLabel>::updateList();
    }

private:
    using ColorGetter = QColor (*)();
    QVector<TimeLabel> _labels;
    int _cellHeight = ApplicationSettings::defaultSlotHeight;

    int calculateLabelWidth() const;
    bool isIntegerHourAtIndex(int index) const;

    ColorGetter labelColorGetterAtIndex(int index);

    SlotsWidget *slotsWidget() const;

    // ReactiveList
    int numberOfItems() override;
    QVBoxLayout *listLayout() override;
    void reuseItemAtIndex(int index, ColoredLabel *itemWidget) override;
    ColoredLabel *makeNewItemAtIndex(int index) override;

    void paintEvent(QPaintEvent *) override;

//#ifdef Q_OS_WIN
//    static const int bigFontHeight = 12;
//    static const int smallFontHeight = 10;
//#else
    static const int bigFontHeight = 10;
    static const int smallFontHeight = 9;
//#endif
};

#endif // SLOTRULER_H
