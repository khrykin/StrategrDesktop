#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QPropertyAnimation>
#include <QScrollArea>
#include <QWidget>

#include "colorprovider.h"
#include "currenttimemarkerwidget.h"
#include "dataproviderwidget.h"
#include "slotruler.h"
#include "slotswidget.h"

class MainWindow;
class SlotboardScrollArea;

class SlotBoardWidget : public DataProviderWidget,
                        public ColorProvider {
public:
    explicit SlotBoardWidget(QWidget *parent);

    void reloadStrategy();

private:
    SlotsWidget *slotsWidget = nullptr;
    SessionWidget *draggedSessionWidget = nullptr;
    CurrentTimeMarkerWidget *currentTimeMarkerWidget = nullptr;
    SlotRuler *slotRuler = nullptr;

    QPropertyAnimation *draggedSessionAnimation = nullptr;

    void layoutChildWidgets(QHBoxLayout *mainLayout);

    void drawDraggedSession(int, int);
    void updateCurrentTimeMarker();

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void drawResizeBoundaryCircles(QPainter &painter);
};

#endif// SLOTBOARD_H
