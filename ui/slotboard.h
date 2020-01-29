#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QWidget>
#include <QScrollArea>
#include <models/timer.h>

#include "strategy.h"
#include "currenttimemarkerwidget.h"
#include "slotswidget.h"
#include "slotruler.h"
#include "timelabel.h"
#include "colorprovider.h"
#include "timer.h"

class MainWindow;
class SlotBoard :
        public QWidget,
        public ColorProvider {
Q_OBJECT
public:
    explicit SlotBoard(stg::strategy &strategy,
                       QWidget *parent = nullptr);

    void reloadStrategy();

    void clearSelection();
    void focusOnCurrentTime();

    const SelectionWidget::RawSelectionState &selection();

signals:
    void timerTick();
    void timeSlotsChange();
private:
    stg::strategy &strategy;

    SlotsWidget *slotsWidget = nullptr;
    SlotRuler *slotRuler = nullptr;
    QVBoxLayout *slotsLayout = nullptr;

    CurrentTimeMarkerWidget *currentTimeMarkerWidget = nullptr;
    stg::timer currentTimeTimer;

    void updateCurrentTimeMarker();

    QVector<TimeLabel> makeLabelsForStrategy();

    void handleTimeSlotsChange();
    void updateUI();

    void layoutChildWidgets(QHBoxLayout *mainLayout);

    QScrollArea *parentScrollArea();
    void setupCurrentTimeTimer();
    void timerCallback();

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void updateSlotsLayout() const;
};

#endif // SLOTBOARD_H
