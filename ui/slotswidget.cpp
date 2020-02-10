#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>
#include <QVector>

#include "slotswidget.h"
#include "mainwindow.h"
#include "mainscene.h"
#include "utils.h"
#include "slotsmousehandler.h"
#include "third-party/stacklayout.h"

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
    selectionWidget->selection.add_on_change_callback(this, &SlotsWidget::onSelectionChange);

    mouseHandler = new SlotsMouseHandler(this);
    connect(mouseHandler,
            &SlotsMouseHandler::resizeBoundaryChanged,
            this,
            &SlotsWidget::updateResizeBoundary);

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

    deleteActivityAction = new QAction(tr("Delete"), this);
    QList<QKeySequence> deleteShortcuts;
    deleteShortcuts << QKeySequence(Qt::Key_Delete)
                    << QKeySequence(Qt::Key_Backspace);
    deleteActivityAction->setShortcuts(deleteShortcuts);
    connect(deleteActivityAction,
            &QAction::triggered,
            this,
            &SlotsWidget::deleteActivityInSelection);
    addAction(deleteActivityAction);

    clearSelectionAction = new QAction(tr("Deselect"), this);
    clearSelectionAction->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(clearSelectionAction,
            &QAction::triggered,
            this,
            &SlotsWidget::deselectAllSlots);
    addAction(clearSelectionAction);

    selectAllAction = new QAction(tr("Select All"), this);
    selectAllAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    connect(selectAllAction,
            &QAction::triggered,
            this,
            &SlotsWidget::selectAllSlots);
    addAction(selectAllAction);

    shiftSlotsBelowAction = new QAction(tr("Make Room"), this);
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
    mainScene()->showActivities();
}

void SlotsWidget::deleteActivityInSelection() {
    strategy.make_empty_at(selectionWidget->selection);
    selectionWidget->selection.deselect_all();
}

void SlotsWidget::deselectAllSlots() {
    selectionWidget->selection.deselect_all();
}

void SlotsWidget::selectAllSlots() {
    selectionWidget->selection.select_all();
}

void SlotsWidget::reloadStrategy() {
    updateList();
}

MainScene *SlotsWidget::mainScene() {
    return findParentWidget<MainScene>(this);
}

const stg::selection &SlotsWidget::selection() {
    return selectionWidget->selection;
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
    auto isEnabled = selectionWidget->selection.is_continuous()
                     && selectionWidget->selection.only_non_empty_selected();


    shiftSlotsBelowAction->setEnabled(isEnabled);
}

void SlotsWidget::shiftAllSlotsBelow() {
    if (!selectionWidget->selection.is_continuous()) {
        return;
    }

    auto bottomTimeSlotIndex = selectionWidget->selection.front();
    strategy.shift_below_time_slot(bottomTimeSlotIndex,
                                   selectionWidget->selection.size());

    selectionWidget->selection.deselect_all();
}

void SlotsWidget::paintEvent(QPaintEvent *event) {
    using namespace ApplicationSettings;

    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    painter.setPen(Qt::NoPen);

    auto borderColor = slotBeforeBoundaryIndex == -1
                       ? highlightColor()
                       : ColorProvider::borderColor();

    painter.setBrush(borderColor);

    auto thickness = strategy.begin_time() % 60 == 0 ? 2 : 1;
    auto borderRect = QRect(8, 0, width() - 8 * 2, thickness);

    painter.drawRect(borderRect);
}

void SlotsWidget::updateContentsMargins() {
    auto thickness = strategy.begin_time() % 60 == 0 ? 2 : 1;
    slotsLayout->setContentsMargins(8, thickness, 8, 0);
    selectionWidget->setContentsMargins(8, thickness, 8, 0);
}

void SlotsWidget::updateResizeBoundary(int sessionBeforeBoundaryIndex,
                                       int slotBeforeBoundaryIndex) {
    this->slotBeforeBoundaryIndex = slotBeforeBoundaryIndex;
    update();

    emit resizeBoundaryChanged(sessionBeforeBoundaryIndex, slotBeforeBoundaryIndex);
}




