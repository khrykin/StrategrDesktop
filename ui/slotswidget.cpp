#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyleOption>
#include <QVector>

#include "slotswidget.h"
#include "third-party/stacklayout.h"
#include "mainwindow.h"
#include "utils.h"

SlotsWidget::SlotsWidget(Strategy *strategy, QWidget *parent)
        : strategy(strategy),
          QWidget(parent) {

    setLayout(new StackLayout());
    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    layoutChildWidgets();

    setupActions();

    setStyleSheet("SlotsWidget {"
                  "border-top: 1px solid #d8d8d8;"
                  "}");

    updateUI();
}

void SlotsWidget::layoutChildWidgets() {
    slotsLayout = new QVBoxLayout();
    slotsLayout->setSpacing(0);
    slotsLayout->setContentsMargins(0, 1, 0, 0);

    auto *slotsWidget = new QWidget();
    slotsWidget->setLayout(slotsLayout);

    selectionWidget = new SelectionWidget(_slotHeight);

    layout()->addWidget(slotsWidget);
    layout()->addWidget(selectionWidget);
}

void SlotsWidget::updateUI() {
    for (auto i = 0; i < strategy->activitySessions().size(); i++) {
        reuseSessionSlotAtIndex(i);
    }

    removeExtraWidgets();
}

void SlotsWidget::reuseSessionSlotAtIndex(int sessionIndex) const {
    ActivitySessionWidget *groupWidget;
    auto &activitySession = strategy->activitySessions()[sessionIndex];

    auto *layoutItem = slotsLayout->itemAt(sessionIndex);
    if (layoutItem) {
        groupWidget = qobject_cast<ActivitySessionWidget *>(layoutItem->widget());

        if (!groupWidget) {
            slotsLayout->removeItem(layoutItem);
            return;
        }

        if (groupWidget->isHidden()) {
            groupWidget->show();
        }
    } else {
        groupWidget = new ActivitySessionWidget(&activitySession);
        groupWidget->setSlotHeight(slotHeight());
        slotsLayout->addWidget(groupWidget);
    }

    groupWidget->setActivitySession(&activitySession);
}

void SlotsWidget::removeExtraWidgets() const {
    auto hideAtIndex = strategy->activitySessions().size();
    while (slotsLayout->itemAt(hideAtIndex) != nullptr) {
        auto itemToHide = slotsLayout->itemAt(hideAtIndex);
        if (itemToHide->widget() != nullptr) {
            itemToHide->widget()->hide();
        }

        hideAtIndex++;
    }
}


void SlotsWidget::setupActions() {
    setActivityAction = new QAction(tr("Set Activity"), this);
    setActivityAction->setShortcut(QKeySequence(Qt::Key_Return));

    connect(setActivityAction,
            &QAction::triggered,
            this,
            &SlotsWidget::openActivitiesWindow);
    addAction(setActivityAction);

    deleteActivityAction = new QAction(tr("Make Empty"), this);
    QList<QKeySequence> deleteShortcuts;
    deleteShortcuts << QKeySequence(Qt::Key_Delete)
                    << QKeySequence(Qt::Key_Backspace);
    deleteActivityAction->setShortcuts(deleteShortcuts);
    connect(deleteActivityAction,
            &QAction::triggered,
            this,
            &SlotsWidget::deleteActivityInSelection);
    addAction(deleteActivityAction);

    clearSelectionAction = new QAction(tr("Deselect All Slots"), this);
    clearSelectionAction->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(clearSelectionAction,
            &QAction::triggered,
            this,
            &SlotsWidget::deselectAllSlots);
    addAction(clearSelectionAction);

    selectAllAction = new QAction(tr("Select All Slots"), this);
    selectAllAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    connect(selectAllAction,
            &QAction::triggered,
            this,
            &SlotsWidget::selectAllSlots);
    addAction(selectAllAction);

    undoAction = new QAction(tr("Undo"), this);
    undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    connect(undoAction,
            &QAction::triggered,
            this,
            &SlotsWidget::undo);
    addAction(undoAction);

    redoAction = new QAction(tr("Redo"), this);
    redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
    connect(redoAction,
            &QAction::triggered,
            this,
            &SlotsWidget::redo);
    addAction(redoAction);
}

void SlotsWidget::mousePressEvent(QMouseEvent *event) {
    mouseHandler.mousePressEvent(event);
}


void SlotsWidget::mouseMoveEvent(QMouseEvent *event) {
    mouseHandler.mouseMoveEvent(event);
}

void SlotsWidget::mouseReleaseEvent(QMouseEvent *event) {
    mouseHandler.mouseReleaseEvent(event);
}

void SlotsWidget::contextMenuEvent(QContextMenuEvent *event) {
    mouseHandler.contextMenuEvent(event);
}

int SlotsWidget::slotHeight() const { return _slotHeight; }

void SlotsWidget::undo() {
    strategy->undo();
    updateUI();
}

void SlotsWidget::redo() {
    strategy->redo();
    updateUI();
}

void SlotsWidget::openActivitiesWindow() {
    if (!selectionWidget->selection().empty()) {
        mainScene()->showActivities();
    }
}

void SlotsWidget::deleteActivityInSelection() {

    strategy->emptyTimeSlotsAtIndices(selectionWidget->selection());
    selectionWidget->deselectAll();
    updateUI();
}

void SlotsWidget::deselectAllSlots() {
    selectionWidget->deselectAll();
}

void SlotsWidget::selectAllSlots() {
    selectionWidget->selectAll(strategy->numberOfTimeSlots());
}

void SlotsWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    updateUI();
    mouseHandler.reset();
}

MainScene *SlotsWidget::mainScene() {
    return findParentWidget<MainScene>(this);
}



