#ifndef GROUPSLIST_H
#define GROUPSLIST_H

#include <QAction>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "sessionwidget.h"
#include "Strategy.h"
#include "slotruler.h"
#include "applicationsettings.h"
#include "slotsmousehandler.h"
#include "selectionwidget.h"
#include "reactivelist.hpp"

class MainScene;
class SlotsWidget : public QWidget,
                    public ReactiveList<SessionWidget> {
Q_OBJECT
public:
    explicit SlotsWidget(Strategy &strategy,
                         QWidget *parent = nullptr);

    void reloadStrategy();
    int slotHeight() const;

    void deselectAllSlots();

    const SelectionWidget::RawSelectionState &selection();
signals:
    void sessionsChanged();
private:
    friend SlotsMouseHandler;
    Strategy &strategy;

    QVBoxLayout *slotsLayout = nullptr;
    SelectionWidget *selectionWidget = nullptr;

    SlotsMouseHandler *mouseHandler = nullptr;

    int _slotHeight = ApplicationSettings::defaultSlotHeight;

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

    void onSelectionChange();

    bool onlyEmptySlotsSelected() const;

    MainScene *mainScene();

    // ReactiveList

    int numberOfItems() override;
    QVBoxLayout *listLayout() override;;
    void reuseItemAtIndex(int index, SessionWidget *itemWidget) override;
    SessionWidget *makeNewItemAtIndex(int index) override;

    void paintEvent(QPaintEvent *event) override;
};

#endif // GROUPSLIST_H
