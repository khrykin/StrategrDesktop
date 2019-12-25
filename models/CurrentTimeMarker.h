//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_CURRENTTIMEMARKER_H
#define STRATEGR_CURRENTTIMEMARKER_H

#include <MacTypes.h>
#include "TimeUtils.h"
#include "Rect.hpp"

class Strategy;
class CurrentTimeMarker {
public:
    explicit CurrentTimeMarker(const Strategy &strategy);

    bool isVisible() const;
    bool isHidden() const;

    Strategr::Rect rectInParent(const Strategr::Rect &parentRect,
                                int markerRadius = 0) const;

    int scrollOffsetInParent(const Strategr::Rect &parentRect,
                             int windowHeight) const;
private:
    const Strategy &strategy;

    int topOffsetIn(int totalHeight) const;
    double relativePosition() const;
};

#endif //STRATEGR_CURRENTTIMEMARKER_H
