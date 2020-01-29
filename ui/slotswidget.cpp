#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyleOption>
#include <QVector>
#include <QStyleOption>

#include "slotswidget.h"
#include "third-party/stacklayout.h"
#include "mainwindow.h"
#include "utils.h"

SlotsWidget::SlotsWidget(stg::strategy &strategy, QWidget *parent)
        : strategy(strategy),
          QWidget(parent) {
    strategy.sessions()
            .add_on_change_callback(this, &SlotsWidget::updateUI);

    setLayout(new StackLayout());
    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    layoutChildWidgets();

    setupActions();

    updateList();
}

void SlotsWidget::layoutChildWidgets() {
    slotsLayout = new QVBoxLayout();
    slotsLayout->setSpacing(0);

    auto *slotsWidget = new QWidget();
    slotsWidget->setLayout(slotsLayout);

    selectionWidget = new SelectionWidget(strategy, _slotHeight, nullptr);
    connect(selectionWidget,
            &SelectionWidget::selectionChanged,
            this,
            &SlotsWidget::onSelectionChange);

    mouseHandler = new SlotsMouseHandler(this);

    layout()->addWidget(slotsWidget);
    layout()->addWidget(selectionWidget);
    layout()->addWidget(mouseHandler);

    updateContentsMargins();
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

    shiftSlotsBelowAction = new QAction(tr("Make Room Shifting Below"), this);
    shiftSlotsBelowAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
    connect(shiftSlotsBelowAction,
            &QAction::triggered,
            this,
            &SlotsWidget::shiftAllSlotsBelow);

    shiftSlotsBelowAction->setEnabled(false);
    addAction(shiftSlotsBelowAction);
}

int SlotsWidget::slotHeight() const { return _slotHeight; }

void SlotsWidget::openActivitiesWindow() {
    if (!selectionWidget->selection().empty()) {
        mainScene()->showActivities();
    }
}

void SlotsWidget::deleteActivityInSelection() {
    strategy.make_empty_at(selectionWidget->selection());
    selectionWidget->deselectAll();
}

void SlotsWidget::deselectAllSlots() {
    selectionWidget->deselectAll();
}

void SlotsWidget::selectAllSlots() {
    selectionWidget->selectAll(strategy.number_of_time_slots());
}

void SlotsWidget::reloadStrategy() {
    strategy.sessions()
            .add_on_change_callback(this, &SlotsWidget::updateUI);

    selectionWidget->reloadStrategy();

    updateList();
    mouseHandler->reset();
}

MainScene *SlotsWidget::mainScene() {
    return findParentWidget<MainScene>(this);
}

const SelectionWidget::RawSelectionState &SlotsWidget::selection() {
    return selectionWidget->selection();
}

int SlotsWidget::numberOfItems() {
    return strategy.sessions().size();
}

QVBoxLayout *SlotsWidget::listLayout() {
    return slotsLayout;
}

void SlotsWidget::reuseItemAtIndex(int index, SessionWidget *itemWidget) {
    const auto session = strategy.sessions()[index];
    itemWidget->setActivitySession(session);
}

SessionWidget *SlotsWidget::makeNewItemAtIndex(int index) {
    const auto session = strategy.sessions()[index];
    auto itemWidget = new SessionWidget(session);
    itemWidget->setSlotHeight(slotHeight());

    return itemWidget;
}

void SlotsWidget::updateUI() {
    emit sessionsChanged();

    updateContentsMargins();
    updateList();
}

void SlotsWidget::onSelectionChange() {
    auto isEnabled = selectionWidget->selectionIsContinuous()
                     && !onlyEmptySlotsSelected();

    shiftSlotsBelowAction->setEnabled(isEnabled);
}

void SlotsWidget::shiftAllSlotsBelow() {
    if (!selectionWidget->selectionIsContinuous()) {
        return;
    }

    auto bottomTimeSlotIndex = selectionWidget->selection().front();
    strategy.shift_below_time_slot(bottomTimeSlotIndex,
                                   selectionWidget->selection().size());

    selectionWidget->deselectAll();
}

bool SlotsWidget::onlyEmptySlotsSelected() const {
    for (auto slotIndex : selectionWidget->selection()) {
        if (strategy.time_slots()[slotIndex].activity
            != stg::strategy::no_activity) {
            return false;
        }
    }

    return true;
}

void SlotsWidget::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(SessionWidget::borderColor());

    auto thickness = strategy.begin_time() % 60 == 0 ? 2 : 1;
    auto borderRect = QRect(8, 0, width() - 8 * 2, thickness);

    painter.drawRect(borderRect);
}

void SlotsWidget::updateContentsMargins() {
    auto thickness = strategy.begin_time() % 60 == 0 ? 2 : 1;
    slotsLayout->setContentsMargins(8, thickness, 8, 0);
    selectionWidget->setContentsMargins(8, thickness, 8, 0);
}




