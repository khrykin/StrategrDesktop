//
// Created by Dmitry Khrykin on 2019-07-11.
//

#include "selectionwidget.h"
#include <QLayout>

SelectionWidget::SelectionWidget(Strategy *strategy,
                                 int slotHeight,
                                 QWidget *parent)
        : strategy(strategy), QWidget(parent) {
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

    for (auto *child : children()) {
        delete child;
    }

    for (auto &selectionItem : selectionState) {
        auto topPosition = slotHeight * selectionItem.front();

        auto widgetHeight = selectionItem.size() * slotHeight;
        auto *widget = new QWidget(this);

        const auto &lastTimeSlot = strategy->timeSlots()[selectionItem.back()];
        auto bottomMargin = lastTimeSlot.endTime() % 60 == 0 ? 1 : 0;

        auto rect = QRect(contentsMargins().left(),
                          contentsMargins().top() + topPosition + 2,
                          width() - contentsMargins().right(),
                          widgetHeight - 5 - bottomMargin);

        widget->setGeometry(rect);
        widget->setStyleSheet("background-color: rgba(189, 214, 241, 0.5);"
                              "border-radius: 4px;");
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

void SelectionWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    updateUI();
}
