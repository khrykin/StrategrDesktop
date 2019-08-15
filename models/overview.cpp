//
// Created by Dmitry Khrykin on 2019-08-15.
//

#include "overview.h"

Overview::Overview(const Strategy &strategy) : strategy(strategy) {
}

std::vector<const Overview::OverviewItem>
Overview::elementsForWidth(int width) {
    return elementsForWidth(static_cast<float>(width));
}

std::vector<const Overview::OverviewItem>
Overview::elementsForWidth(float width) {
    auto &activitySessions = strategy.activitySessions();
    std::vector<const OverviewItem> result;

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
