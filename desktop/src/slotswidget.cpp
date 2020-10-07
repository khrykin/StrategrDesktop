#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>
#include <QVector>

#include "mainscene.h"
#include "mainwindow.h"
#include "slotswidget.h"
#include "utils.h"

SlotsWidget::SlotsWidget(QWidget *parent) : DataProviderWidget(parent) {
    setMouseTracking(true);

    strategy().sessions().add_on_change_callback([this] { reloadStrategy(); });

    setContentsMargins(0, 0, ApplicationSettings::defaultPadding, 0);

    layoutChildWidgets();
    setupActions();
    reloadStrategy();

    mouseHandler().on_select_sessions = [this](const auto &sessionIndices) {
        for (auto sessionIndex = 0; sessionIndex < strategy().sessions().size(); sessionIndex++) {
            auto *sessionWidget = dynamic_cast<SessionWidget *>(slotsLayout->itemAt(sessionIndex)->widget());

            if (!sessionWidget)
                continue;

            auto isSelected = std::find(sessionIndices.begin(),
                                        sessionIndices.end(),
                                        sessionIndex) != sessionIndices.end();
            sessionWidget->setIsSelected(isSelected);
            sessionWidget->setIsBorderSelected(false);
        }
    };

    mouseHandler().on_resize_boundary_change = [this]() {
        auto boundarySessionIndex = mouseHandler().resize_boundary().session_index;
        for (auto sessionIndex = 0; sessionIndex < strategy().sessions().size(); sessionIndex++) {
            auto *sessionWidget = dynamic_cast<SessionWidget *>(slotsLayout->itemAt(sessionIndex)->widget());

            if (!sessionWidget)
                continue;

            auto isBorderSelected = boundarySessionIndex + 1 == sessionIndex;
            sessionWidget->setIsBorderSelected(isBorderSelected);
        }

        update();
    };

    mouseHandler().on_cursor_change = [this](auto cursor) {
        switch (cursor) {
            case stg::mouse_handler::cursor::pointer:
                unsetCursor();
                break;
            case stg::mouse_handler::cursor::resize:
                setCursor(resizeVerticalCursor());
                break;
            case stg::mouse_handler::cursor::closed_hand:
                setCursor(closedHandCursor());
                break;
            case stg::mouse_handler::cursor::open_hand:
                setCursor(openHandCursor());
                break;
            case stg::mouse_handler::cursor::drag_copy:
                setCursor(dragCopyCursor());
                break;
        }
    };

    mouseHandler().on_auto_scroll_frame = [this](stg::gfloat offsetIncrement) {
        auto newOffset = slotboardScrollArea()->verticalScrollBar()->value() + (int) offsetIncrement;
        slotboardScrollArea()->setScrollOffset(newOffset);

        return mapFromGlobal(QCursor::pos());
    };

    mouseHandler().on_show_context_menu = [this](const auto &menu_configuration) {
        auto menu = QMenu(this);
        for (auto &action : menu_configuration.actions) {
            if (action->type == stg::action::separator) {
                menu.addSeparator();
                continue;
            }

            auto *qAction = menu.addAction(QString::fromStdString(action->name), *action);
            qAction->setEnabled(action->is_enabled());
        }

        menu.exec(mapToGlobal(menu_configuration.position));
    };
}

void SlotsWidget::layoutChildWidgets() {
    slotsLayout = new QVBoxLayout();
    slotsLayout->setSpacing(0);
    updateSlotsLayoutContentMargins();

    slotsWidget = new QWidget(this);
    slotsWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    slotsWidget->setContentsMargins(0, 0, 0, 0);
    slotsWidget->setLayout(slotsLayout);

    selectionWidget = new SelectionWidget(this);
}

void SlotsWidget::setupActions() {
    setActivityAction = new QAction(tr("Set Activity"), this);
    setActivityAction->setShortcut(QKeySequence(Qt::Key_Return));

    connect(setActivityAction, &QAction::triggered, actionCenter().show_activities);
    addAction(setActivityAction);

    deleteActivityAction = new QAction(QString::fromStdString(actionCenter().empty_selection.name), this);

    QList<QKeySequence> deleteShortcuts;
    deleteShortcuts << QKeySequence(Qt::Key_Delete)
                    << QKeySequence(Qt::Key_Backspace);

    deleteActivityAction->setShortcuts(deleteShortcuts);

    connect(deleteActivityAction, &QAction::triggered, actionCenter().empty_selection);
    addAction(deleteActivityAction);

    clearSelectionAction = new QAction(actionCenter().deselect.name.c_str(), this);
    clearSelectionAction->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(clearSelectionAction, &QAction::triggered, actionCenter().deselect);
    addAction(clearSelectionAction);

    selectAllAction = new QAction(actionCenter().select_all.name.c_str(), this);
    selectAllAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    connect(selectAllAction, &QAction::triggered, actionCenter().select_all);
    addAction(selectAllAction);

    shiftSlotsBelowAction = new QAction(actionCenter().make_room.name.c_str(), this);
    shiftSlotsBelowAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
    connect(shiftSlotsBelowAction, &QAction::triggered, actionCenter().make_room);
    addAction(shiftSlotsBelowAction);
}

void SlotsWidget::reloadStrategy() {
    updateSlotsLayoutContentMargins();
    updateList();
}

int SlotsWidget::numberOfItems() {
    return strategy().sessions().size();
}

QVBoxLayout *SlotsWidget::listLayout() {
    return slotsLayout;
}

void SlotsWidget::reuseItemAtIndex(int index, SessionWidget *itemWidget) {
    const auto session = strategy().sessions()[index];
    itemWidget->setSession(session);
    itemWidget->setIsBorderSelected(mouseHandler().resize_boundary().session_index + 1 == index);
    itemWidget->setIsSelected(false);
}

SessionWidget *SlotsWidget::makeNewItemAtIndex(int index) {
    const auto session = strategy().sessions()[index];
    auto *itemWidget = new SessionWidget(session, this);
    return itemWidget;
}

void SlotsWidget::mouseMoveEvent(QMouseEvent *event) {
    mouseHandler().mouse_move(event);
}

void SlotsWidget::mousePressEvent(QMouseEvent *event) {
    mouseHandler().mouse_press(event);
}

void SlotsWidget::mouseReleaseEvent(QMouseEvent *event) {
    mouseHandler().mouse_release(event);
}

void SlotsWidget::keyPressEvent(QKeyEvent *event) {
    mouseHandler().key_down(event);
}

void SlotsWidget::keyReleaseEvent(QKeyEvent *event) {
    mouseHandler().key_up(event);
}

void SlotsWidget::paintEvent(QPaintEvent *event) {
    using namespace ApplicationSettings;
    QPainter painter(this);

    auto isResizeBoundary = mouseHandler().resize_boundary().slot_index == strategy().number_of_time_slots() - 1;
    auto borderColor = isResizeBoundary ? controlColor() : ColorProvider::borderColor();
    auto thickness = isResizeBoundary ? 2 : topLineThickness();

    painter.setPen(Qt::NoPen);
    painter.setBrush(borderColor);

    auto borderRect = QRect(0,
                            slotHeight() / 2 + strategy().number_of_time_slots() * slotHeight(),
                            width() - ApplicationSettings::defaultPadding,
                            thickness);

    painter.drawRect(borderRect);
}

int SlotsWidget::topLineThickness() {
    return strategy().end_time() % 60 == 0 ? 2 : 1;
}

void SlotsWidget::updateSlotsLayoutContentMargins() {
    slotsLayout->setContentsMargins(0, slotHeight() / 2, 0, 0);
}

void SlotsWidget::resizeEvent(QResizeEvent *event) {
    slotsWidget->setGeometry(contentsRect());
    selectionWidget->setGeometry(contentsRect());
}

void SlotsWidget::enterEvent(QEvent *event) {
    setFocus();
}
