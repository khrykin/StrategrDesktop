#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QWidget>
#include <QScrollArea>

#include "strategy.h"
#include "currenttimemarker.h"
#include "slotswidget.h"
#include "parentwindowaccessible.h"
#include "slotruler.h"
#include "timelabel.h"
#include "colorprovider.h"

class MainWindow;
class SlotBoard :
        public QWidget,
        public ParentWindowAccessible<SlotBoard>,
        public ColorProvider {
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
    void timeSlotsChange();
private:
    Strategy *strategy;

    SlotsWidget *slotsWidget = nullptr;
    SlotRuler *slotRuler = nullptr;
    QVBoxLayout *slotsLayout = nullptr;

    CurrentTimeMarker *currentTimeMarker = nullptr;
    QTimer *currentTimeTimer = nullptr;

    int currentTimeMarkerTopOffset = 0;

    void updateCurrentTimeMarker();

    QVector<TimeLabel> makeLabelsForStrategy();

    void handleTimeSlotsChange();
    void updateUI();

    void layoutChildWidgets(QHBoxLayout *mainLayout);

    double calculateTimeMarkerTopOffset() const;
    QRect calculateCurrentTimeMarkerGeometry() const;

    QScrollArea *parentScrollArea();
    void setupCurrentTimeTimer();

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void updateSlotsLayout() const;
};

#endif // SLOTBOARD_H
