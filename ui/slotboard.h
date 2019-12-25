#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QWidget>
#include <QScrollArea>
#include <models/Timer.h>

#include "Strategy.h"
#include "currenttimemarkerwidget.h"
#include "slotswidget.h"
#include "slotruler.h"
#include "timelabel.h"
#include "colorprovider.h"
#include "Timer.h"

class MainWindow;
class SlotBoard :
        public QWidget,
        public ColorProvider {
Q_OBJECT
public:
    explicit SlotBoard(Strategy &strategy,
                       QWidget *parent = nullptr);

    void reloadStrategy();

    void clearSelection();
    void focusOnCurrentTime();

    const SelectionWidget::RawSelectionState &selection();

signals:
    void timerTick();
    void timeSlotsChange();
private:
    Strategy &strategy;

    SlotsWidget *slotsWidget = nullptr;
    SlotRuler *slotRuler = nullptr;
    QVBoxLayout *slotsLayout = nullptr;

    CurrentTimeMarkerWidget *currentTimeMarkerWidget = nullptr;
    Strategr::Timer currentTimeTimer;

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
