//
// Created by Dmitry Khrykin on 2019-07-11.
//

#include <QLayout>
#include <QPainter>

#include "selectionwidget.h"
#include "colorutils.h"

SelectionWidget::SelectionWidget(stg::strategy &strategy,
                                 int slotHeight,
                                 QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    setMouseTracking(false);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

SelectionWidget::SelectionState
SelectionWidget::makeSelectionState(RawSelectionState rawState) {
    using namespace std;
    SelectionState result;
    RawSelectionState currentItem;

    for (auto i = rawState.begin(); i != rawState.end(); ++i) {
        auto current = *i;
        auto previous = i != rawState.begin()
                        ? make_optional(*prev(i))
                        : nullopt;

        if (!previous || current == *previous + 1) {
            currentItem.push_back(current);
        } else {
            result.push_back(currentItem);
            currentItem = {current};
        }

        if (i == prev(rawState.end())) {
            result.push_back(currentItem);
        }
    }

    return result;
}

void SelectionWidget::selectAtIndex(int slotIndex) {
    auto foundIt = std::find(rawSelectionState.begin(),
                             rawSelectionState.end(),
                             slotIndex);

    auto alreadySelected = foundIt != rawSelectionState.end();
    if (alreadySelected) {
        rawSelectionState.erase(foundIt);
    } else {
        rawSelectionState.push_back(slotIndex);
        std::sort(rawSelectionState.begin(),
                  rawSelectionState.end());
    }

    updateUI();

    emit selectionChanged();
}

void SelectionWidget::setSlotHeight(int newSlotHeight) {
    slotHeight = newSlotHeight;
    updateUI();
}

void SelectionWidget::updateUI() {
    selectionState = makeSelectionState(rawSelectionState);

    update();
}

void SelectionWidget::drawSelectionForItem(RawSelectionState &selectionItem,
                                           QPainter &painter) {
    auto topPosition = slotHeight * selectionItem.front();

    auto widgetHeight = selectionItem.size() * slotHeight;

    const auto &lastTimeSlot = strategy.time_slots()[selectionItem.back()];
    auto bottomMargin = lastTimeSlot.end_time() % 60 == 0 ? 1 : 0;

    auto rect = QRect(contentsMargins().left(),
                      contentsMargins().top() + topPosition + 2,
                      width() - contentsMargins().right() - contentsMargins().left(),
                      widgetHeight - 5 - bottomMargin);

    painter.drawRoundedRect(rect, 4, 4);
}

void SelectionWidget::resizeEvent(QResizeEvent *event) {
    for (auto *child : children()) {
        auto *widget = qobject_cast<QWidget *>(child);
        if (widget) {
            widget->setFixedWidth(width());
        }
    }
}

void SelectionWidget::deselectAll() {
    rawSelectionState = {};
    _isClicked = false;
    updateUI();
    emit selectionChanged();
}

void SelectionWidget::selectAll(int numberOfSlots) {
    rawSelectionState = RawSelectionState(numberOfSlots);
    std::iota(rawSelectionState.begin(), rawSelectionState.end(), 0);

    updateUI();
    emit selectionChanged();
}

const SelectionWidget::RawSelectionState &SelectionWidget::selection() const {
    return rawSelectionState;
}

bool SelectionWidget::selectionIsContinuous() const {
    return selectionState.size() == 1;
}

void SelectionWidget::reloadStrategy() {
    updateUI();
}

void SelectionWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);

    auto clickedColor = selectionColor();
    clickedColor.setAlphaF(clickedColor.alphaF() * 1.25);

    painter.setBrush(_isClicked ? clickedColor : selectionColor());

    for (auto &selectionItem : selectionState) {
        drawSelectionForItem(selectionItem, painter);
    }
}

bool SelectionWidget::isSlotIndexSelected(stg::strategy::time_slot_index_t slotIndex) {
    return std::find(rawSelectionState.begin(),
                     rawSelectionState.end(),
                     slotIndex) != rawSelectionState.end();
}

bool SelectionWidget::isClicked() const {
    return _isClicked;
}

void SelectionWidget::setIsClicked(bool isClicked) {
    _isClicked = isClicked;

    update();
}

void SelectionWidget::fillSelected(int fromIndex, int toIndex) {

    if (fromIndex > toIndex) {
        std::swap(fromIndex, toIndex);
    }

    if (fromIndex <= 0) {
        fromIndex = 0;
    }

    if (toIndex >= strategy.number_of_time_slots()) {
        fromIndex = strategy.number_of_time_slots() - 1;
    }

    for (auto i = fromIndex; i <= toIndex; i++) {
        if (!isSlotIndexSelected(i))
            rawSelectionState.push_back(i);
    }

    std::sort(rawSelectionState.begin(),
              rawSelectionState.end());

    updateUI();
}
