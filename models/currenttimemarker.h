//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_CURRENTTIMEMARKER_H
#define STRATEGR_CURRENTTIMEMARKER_H

#include "time_utils.h"
#include "rect.h"

namespace stg {
    class strategy;
    class current_time_marker {
    public:
        explicit current_time_marker(const strategy &strategy);

        bool is_visible() const;
        bool is_hidden() const;

        rect rect_in_parent(const rect &parent_rect,
                            int marker_radius = 0) const;

        int scroll_offset_in_parent(const rect &parent_rect,
                                    int window_height) const;
    private:
        const strategy &strategy;

        int top_offset_in(int total_height) const;
        double relative_position() const;
    };
}


#endif //STRATEGR_CURRENTTIMEMARKER_H
