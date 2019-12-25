//
// Created by Dmitry Khrykin on 2019-11-25.
//

#include "CurrentTimeMarker.h"
#include "Strategy.h"

CurrentTimeMarker::CurrentTimeMarker(const Strategy &strategy)
        : strategy(strategy) {}

int CurrentTimeMarker::topOffsetIn(int totalHeight) const {
    auto relative = relativePosition();
    if (relative < 0)
        return 0;

    if (relative > 1) {
        return 1;
    }

    return static_cast<int>(totalHeight * relativePosition());
}

bool CurrentTimeMarker::isVisible() const {
    auto relative = relativePosition();
    return relative >= 0 && relative <= 1;
}

bool CurrentTimeMarker::isHidden() const {
    return !isVisible();
}

Strategr::Rect CurrentTimeMarker::rectInParent(const Strategr::Rect &parentRect,
                                               int markerRadius) const {
    return Strategr::Rect{
            parentRect.left - markerRadius,
            parentRect.top + topOffsetIn(parentRect.height) - markerRadius,
            parentRect.width + markerRadius,
            2 * markerRadius
    };
}

int CurrentTimeMarker::scrollOffsetInParent(const Strategr::Rect &parentRect,
                                            int windowHeight) const {
    auto rect = rectInParent(parentRect);
    auto topOffset = rect.top - windowHeight / 2;

    if (topOffset < 0) {
        topOffset = 0;
    } else if (topOffset > parentRect.height) {
        topOffset = parentRect.height;
    }

    return topOffset;
}

double CurrentTimeMarker::relativePosition() const {
    auto strategyDurationSeconds = strategy.duration() * 60;
    auto startOfStrategySeconds = strategy.beginTime() * 60;
    return static_cast<double>(TimeUtils::currentSeconds() - startOfStrategySeconds)
           / strategyDurationSeconds;
}
