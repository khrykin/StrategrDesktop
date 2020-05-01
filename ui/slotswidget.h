#ifndef GROUPSLIST_H
#define GROUPSLIST_H

#include <QAction>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "sessionwidget.h"
#include "strategy.h"
#include "slotruler.h"
#include "applicationsettings.h"
#include "selectionwidget.h"
#include "reactivelist.hpp"
#include "selection.h"
#include "colorprovider.h"

class SlotsMouseHandler;
class MainScene;
class SlotsWidget : public QWidget,
                    public ReactiveList<SessionWidget>,
                    public ColorProvider {
Q_OBJECT
public:
    explicit SlotsWidget(stg::strategy &strategy,
                         QWidget *parent = nullptr);

    int slotHeight() const;

    void reloadStrategy();
    void deselectAllSlots();

    int slotBeforeBoundaryIndex() const;

    const stg::selection &selection();
signals:
    void sessionsChanged();
    void resizeBoundaryChanged(int, int);
    void drawDraggedSession(int, int);

private:
    friend SlotsMouseHandler;
    stg::strategy &strategy;

    QVBoxLayout *slotsLayout = nullptr;
    SelectionWidget *selectionWidget = nullptr;

    SlotsMouseHandler *mouseHandler = nullptr;

    int _slotHeight = ApplicationSettings::defaultSlotHeight;
    int _slotBeforeBoundaryIndex = -2;

    QAction *setActivityAction = nullptr;
    QAction *deleteActivityAction = nullptr;
    QAction *clearSelectionAction = nullptr;
    QAction *shiftSlotsBelowAction = nullptr;
    QAction *selectAllAction = nullptr;

    void openActivitiesWindow();
    void deleteActivityInSelection();
    void selectAllSlots();
    void shiftAllSlotsBelow();

    void setupActions();

    void layoutChildWidgets();

    void updateUI();
    void updateContentsMargins();
    void updateResizeBoundary(int sessionBeforeBoundaryIndex,
                              int slotBeforeBoundaryIndex);

    void onSelectionChange();

    MainScene *mainScene();

    // ReactiveList

    int numberOfItems() override;
    QVBoxLayout *listLayout() override;;
    void reuseItemAtIndex(int index, SessionWidget *itemWidget) override;
    SessionWidget *makeNewItemAtIndex(int index) override;

    void paintEvent(QPaintEvent *event) override;
};

#endif // GROUPSLIST_H
