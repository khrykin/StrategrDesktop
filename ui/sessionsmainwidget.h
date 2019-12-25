//
// Created by Dmitry Khrykin on 2019-07-10.
//

#ifndef STRATEGR_SESSIONSMAINWIDGET_H
#define STRATEGR_SESSIONSMAINWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>

#include "Strategy.h"
#include "colorprovider.h"
#include "selectionwidget.h"

class NotifierImplementation;
class StrategySettingsWidget;
class CurrentSessionWidget;
class QScrollArea;
class SlotBoard;
class OverviewWidget;

class SessionsMainWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit SessionsMainWidget(Strategy &strategy,
                                QWidget *parent = nullptr);

    void toggleStrategySettingsOpen();
    void focusOnCurrentTime();

    const SelectionWidget::RawSelectionState &selection();
    void clearSelection();
    void reloadStrategy();

    QScrollArea *slotBoardScrollArea() const;
private:
    Strategy &strategy;

    NotifierImplementation *notifier = nullptr;
    StrategySettingsWidget *strategySettingsWidget = nullptr;
    CurrentSessionWidget *currentSessionWidget = nullptr;
    QScrollArea *_slotBoardScrollArea = nullptr;
    SlotBoard *slotBoard = nullptr;
    OverviewWidget *overviewWidget = nullptr;

    void layoutChildWidgets();
    void updateTimerDependants();

    void paintEvent(QPaintEvent *paintEvent) override;
    void updateOverviewWidget() const;

    void resizeEvent(QResizeEvent *) override;
};


#endif //STRATEGR_SESSIONSMAINWIDGET_H
