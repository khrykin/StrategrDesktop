//
// Created by Dmitry Khrykin on 2019-07-11.
//

#include "selectionwidget.h"
#include <QLayout>

SelectionWidget::SelectionWidget(int slotHeight, QWidget *parent)
        : QWidget(parent) {
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
}

void SelectionWidget::setSlotHeight(int newSlotHeight) {
    slotHeight = newSlotHeight;
    updateUI();
}

void SelectionWidget::updateUI() {
    auto selectionState = makeSelectionState(rawSelectionState);

    for (auto *child : children()) {
        delete child;
    }

    for (auto i = selectionState.begin(); i != selectionState.end(); ++i) {
        auto &selectionItem = *i;
        auto topPosition = slotHeight * selectionItem.front();

        auto widgetHeight = selectionItem.size() * slotHeight;
        auto styleSheet = "background-color: rgba(189, 214, 241, 0.5);"
                          "border-top: 1px solid rgba(0, 122, 255, 0.5);"
                          "border-bottom: 1px solid rgba(0, 122, 255, 0.5);";

        auto *widget = new QWidget(this);
        auto rect = QRect(0,
                          topPosition,
                          width(),
                          widgetHeight + 1);

        // widgetHeight + 1 to overflow
        // next session's border

        widget->setGeometry(rect);
        widget->setStyleSheet(styleSheet);
        widget->show();
    }
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
    updateUI();
}

void SelectionWidget::selectAll(int numberOfSlots) {
    rawSelectionState = RawSelectionState(numberOfSlots);
    std::iota(rawSelectionState.begin(), rawSelectionState.end(), 0);
    updateUI();
}

const SelectionWidget::RawSelectionState &SelectionWidget::selection() const {
    return rawSelectionState;
}
