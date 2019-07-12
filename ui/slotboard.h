#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QWidget>
#include <QScrollArea>

#include "strategy.h"
#include "currenttimemarker.h"
#include "slotswidget.h"
#include "slotruler.h"
#include "parentwindowaccessible.h"

class MainWindow;
class SlotBoard :
        public QWidget,
        public ParentWindowAccessible<SlotBoard> {
Q_OBJECT
public:
    explicit SlotBoard(Strategy *strategy,
                       QWidget *parent = nullptr);

    void setStrategy(Strategy *newStrategy);

    void clearSelection();
    void focusOnCurrentTime();

    const SelectionWidget::RawSelectionState &selection();

signals:
    void timerTick();

private:
    Strategy *strategy;

    SlotsWidget *slotsWidget = nullptr;
    SlotRuler *slotRuler = nullptr;

    CurrentTimeMarker *currentTimeMarker;
    QTimer *currentTimeTimer;
    int currentTimeMarkerTopOffset;

    void resizeEvent(QResizeEvent *event) override;
    void updateCurrentTimeMarker();

    QWidget *makeHeader();
    QWidget *makeFooter();
    QStringList makeLabelsForStrategy();;

    void updateUI();

    void layoutChildWidgets(QHBoxLayout *mainLayout);
    double calculateTimeMarkerTopOffset() const;
    QRect calculateCurrentTimeMarkerGeometry() const;

    QScrollArea *parentScrollArea();
};

#endif // SLOTBOARD_H
