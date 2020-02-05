#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QWidget>
#include <QScrollArea>

#include "strategy.h"
#include "currenttimemarkerwidget.h"
#include "slotswidget.h"
#include "slotruler.h"
#include "timelabel.h"
#include "colorprovider.h"

class MainWindow;
class SlotBoardCirclesWidget;
class QTimer;

class SlotBoardWidget :
        public QWidget,
        public ColorProvider {
Q_OBJECT
public:
    explicit SlotBoardWidget(stg::strategy &strategy,
                             QWidget *parent = nullptr);

    void reloadStrategy();
    void updateCurrentTimeMarker();

    void clearSelection();
    void focusOnCurrentTime();
    QVBoxLayout *slotsLayout() const;
signals:
    void timerTick();
    void timeSlotsChange();
private:
    stg::strategy &strategy;

    SlotsWidget *slotsWidget = nullptr;
    SlotBoardCirclesWidget *circlesWidget = nullptr;

    SlotRuler *slotRuler = nullptr;
    QVBoxLayout *_slotsLayout = nullptr;


    QTimer *currentTimeTimer = nullptr;
    CurrentTimeMarkerWidget *currentTimeMarkerWidget = nullptr;

    int slotBeforeResizeBoundaryIndex = -2;

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
