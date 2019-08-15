//
// Created by Dmitry Khrykin on 2019-08-15.
//

#include "overview.h"
#include "currenttime.h"

Overview::Overview(const Strategy &strategy, float width) :
        strategy(strategy), width(width) {
}

Overview::Overview(const Strategy &strategy, int width) :
        strategy(strategy), width(static_cast<float>(width)) {
}

std::vector<Overview::OverviewItem>
Overview::elements() {
    auto &activitySessions = strategy.activitySessions();
    std::vector<OverviewItem> result;

    auto prevOriginX = 0;
    for (auto &item : activitySessions.overview()) {
        auto currentWidth = std::lround(item.durationPercentage * width);
        auto originX = std::lround(item.beginPercentage * width);

        if (originX != prevOriginX) {
            currentWidth += originX - prevOriginX;
            originX = prevOriginX;
        }

        if (item.color) {
            result.push_back(OverviewItem{static_cast<int>(originX),
                                          static_cast<int>(currentWidth),
                                          *item.color});
        }

        prevOriginX = originX + currentWidth;
    }

    return result;

}

Overview::ViewportMarker Overview::viewportMarkerFor(int viewportHeight,
                                                     int slotboardHeight,
                                                     int viewportTopOffset) {

    return viewportMarkerFor(static_cast<float>(viewportHeight),
                             static_cast<float>(slotboardHeight),
                             static_cast<float>(viewportTopOffset));
}

Overview::ViewportMarker Overview::viewportMarkerFor(float viewportHeight,
                                                     float slotboardHeight,
                                                     float viewportTopOffset) {
    auto viewportMarkerOriginX = std::roundl(viewportTopOffset / slotboardHeight * width);
    auto viewportMarkerWidth = std::roundl(viewportHeight / slotboardHeight * width);

    return ViewportMarker{
            static_cast<int>(viewportMarkerOriginX),
            static_cast<int>(viewportMarkerWidth)};
}

int Overview::currentTimePosition() {
    auto minutes = CurrentTime::currentMinutes();
    auto rel = static_cast<float>(minutes - strategy.beginTime()) / strategy.duration();

    return static_cast<int>(std::roundl(rel * width));
}


