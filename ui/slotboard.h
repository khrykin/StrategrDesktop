#ifndef SLOTBOARD_H
#define SLOTBOARD_H


#include <QWidget>

#include "strategy.h"
//#include "currenttimemarker.h"
#include "slotswidget.h"
#include "slotruler.h"

class SlotBoard : public QWidget {
Q_OBJECT
public:
    explicit SlotBoard(Strategy *strategy,
                       QWidget *parent = nullptr);

    void setStrategy(Strategy *newStrategy);

    void clearSelection();

    const SelectionWidget::RawSelectionState &selection();


//  GroupsList *groupsList() const;
//  SlotRuler *slotRuler() const;
//
//  CurrentTimeMarker *currentTimeMarker() const;
//
//  int currentTimeMarkerTopOffset() const;
//
//signals:
//
//  void timerTick();
//
//public slots:
private:
    Strategy *strategy;

    SlotsWidget *slotsWidget = nullptr;
    SlotRuler *slotRuler = nullptr;
//  CurrentTimeMarker *_currentTimeMarker;
//  QTimer *currentTimeTimer;
//  int _currentTimeMarkerTopOffset;
//
//  void resizeEvent(QResizeEvent *event);
//  void updateCurrentTimeMarker();

    QWidget *makeHeader();
    QWidget *makeFooter();
    QStringList makeLabelsForStrategy();;

    void updateUI();

    void layoutChildWidgets(QHBoxLayout *mainLayout);
};

#endif // SLOTBOARD_H
