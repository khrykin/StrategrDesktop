#ifndef GROUPSLIST_H
#define GROUPSLIST_H

#include <QAction>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "activitysessionwidget.h"
#include "strategy.h"
#include "slotruler.h"
#include "applicationsettings.h"
#include "slotsmousehandler.h"
#include "selectionwidget.h"
#include "reactivelist.hpp"

class MainScene;
class SlotsWidget : public QWidget,
                    public ReactiveList<ActivitySessionWidget> {
Q_OBJECT
public:
    explicit SlotsWidget(Strategy *strategy,
                         QWidget *parent = nullptr);

    void setStrategy(Strategy *newStrategy);
    int slotHeight() const;

    void deselectAllSlots();

    const SelectionWidget::RawSelectionState &selection();
signals:
    void activitySessionsChanged();
private:
    friend SlotsMouseHandler;
    Strategy *strategy;

    QVBoxLayout *slotsLayout = nullptr;
    SelectionWidget *selectionWidget = nullptr;

    SlotsMouseHandler mouseHandler{this};

    int _slotHeight = ApplicationSettings::defaultSlotHeight;

    QAction *setActivityAction = nullptr;
    QAction *deleteActivityAction = nullptr;
    QAction *clearSelectionAction = nullptr;
    QAction *shiftSlotsBelowAction = nullptr;

    QAction *selectAllAction = nullptr;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    void openActivitiesWindow();
    void deleteActivityInSelection();
    void selectAllSlots();
    void shiftAllSlotsBelow();

    void setupActions();

    void layoutChildWidgets();

    void updateUI();

    void onSelectionChange();

    bool onlyEmptySlotsSelected() const;

    MainScene *mainScene();

    // ReactiveList

    int numberOfItems() override;
    QVBoxLayout *listLayout() override;;
    void reuseItemAtIndex(int index, ActivitySessionWidget *itemWidget) override;
    ActivitySessionWidget *makeNewItemAtIndex(int index) override;
};

#endif // GROUPSLIST_H
