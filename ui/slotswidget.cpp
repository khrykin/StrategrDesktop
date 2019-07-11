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

    strategy->activitySessions()
            .setOnChangeCallback(this, &SlotsWidget::updateUI);

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
    slotsLayout->addStretch();
}

void SlotsWidget::reuseSessionSlotAtIndex(int sessionIndex) const {
    ActivitySessionWidget *sessionWidget;
    auto &activitySession = strategy->activitySessions()[sessionIndex];

    auto *layoutItem = slotsLayout->itemAt(sessionIndex);
    if (layoutItem) {
        sessionWidget = qobject_cast<ActivitySessionWidget *>(layoutItem->widget());

        if (sessionWidget && sessionWidget->isHidden()) {
            sessionWidget->show();
        }

        if (!sessionWidget) {
            slotsLayout->removeItem(layoutItem);
        }
    }

    if (!sessionWidget || !layoutItem) {
        sessionWidget = new ActivitySessionWidget(&activitySession);
        sessionWidget->setSlotHeight(slotHeight());
        slotsLayout->addWidget(sessionWidget);
    }

    sessionWidget->setActivitySession(&activitySession);
}

void SlotsWidget::removeExtraWidgets() const {
    auto hideAtIndex = strategy->activitySessions().size();
    while (slotsLayout->itemAt(hideAtIndex) != nullptr) {
        auto itemToHide = slotsLayout->itemAt(hideAtIndex);
        if (itemToHide->widget()) {
            itemToHide->widget()->hide();
            auto isSessionWidget = qobject_cast<ActivitySessionWidget *>(itemToHide->widget());
            if (!isSessionWidget) {
                layout()->removeItem(itemToHide);
            }
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

void SlotsWidget::openActivitiesWindow() {
    if (!selectionWidget->selection().empty()) {
        mainScene()->showActivities();
    }
}

void SlotsWidget::deleteActivityInSelection() {

    strategy->emptyTimeSlotsAtIndices(selectionWidget->selection());
    selectionWidget->deselectAll();
}

void SlotsWidget::deselectAllSlots() {
    selectionWidget->deselectAll();
}

void SlotsWidget::selectAllSlots() {
    selectionWidget->selectAll(strategy->numberOfTimeSlots());
}

void SlotsWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    strategy->activitySessions()
            .setOnChangeCallback(this, &SlotsWidget::updateUI);

    updateUI();
    mouseHandler.reset();
}

MainScene *SlotsWidget::mainScene() {
    return findParentWidget<MainScene>(this);
}

const SelectionWidget::RawSelectionState &SlotsWidget::selection() {
    return selectionWidget->selection();
}



