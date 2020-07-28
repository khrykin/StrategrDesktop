#ifndef SLOTBOARD_H
#define SLOTBOARD_H

#include <QWidget>
#include <QScrollArea>
#include <QPropertyAnimation>

#include "dataproviderwidget.h"
#include "currenttimemarkerwidget.h"
#include "slotswidget.h"
#include "slotruler.h"
#include "colorprovider.h"

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
};

#endif // SLOTBOARD_H
