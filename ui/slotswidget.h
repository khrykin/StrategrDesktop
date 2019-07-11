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

class MainScene;
class SlotsWidget : public QWidget {
Q_OBJECT
public:
    explicit SlotsWidget(Strategy *strategy,
                         QWidget *parent = nullptr);

    void setStrategy(Strategy *newStrategy);
    int slotHeight() const;

    void deselectAllSlots();

    const SelectionWidget::RawSelectionState &selection();

private:
    friend SlotsMouseHandler;
    Strategy *strategy;

    QVBoxLayout *slotsLayout = nullptr;
    QVBoxLayout *selectionLayout = nullptr;
    SelectionWidget *selectionWidget = nullptr;

    SlotsMouseHandler mouseHandler{this};

    int _slotHeight = ApplicationSettings::defaultSlotHeight;

    QAction *setActivityAction = nullptr;
    QAction *deleteActivityAction = nullptr;
    QAction *clearSelectionAction = nullptr;

    QAction *selectAllAction = nullptr;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    void openActivitiesWindow();
    void deleteActivityInSelection();
    void selectAllSlots();

    void setupActions();

    void updateUI();

    void removeExtraWidgets() const;
    void reuseSessionSlotAtIndex(int sessionIndex) const;
    void layoutChildWidgets();

    MainScene *mainScene();
};

#endif // GROUPSLIST_H
