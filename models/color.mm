//
// Created by Dmitry Khrykin on 26/04/2020.
//

#import <CoreGraphics/CGColor.h>
#include <cmath>

#include "color.h"

namespace stg {
    auto color::to_cg_color() const -> struct CGColor * {
        auto *cg_color = CGColorCreateGenericRGB(red_component(),
                                                 green_component(),
                                                 blue_component(),
                                                 alpha_component());

        CFAutorelease(cg_color);

        return cg_color;
    }

    auto color::from_cg_color(CGColor *cg_color) -> color {
        auto *color_space_model = CGColorSpaceCreateDeviceRGB();
        auto *rgb_cg_color = CGColorCreateCopyByMatchingToColorSpace(color_space_model,
                                                                     kCGRenderingIntentDefault,
                                                                     cg_color,
                                                                     NULL);
        CGColorSpaceRelease(color_space_model);
        CFAutorelease(rgb_cg_color);

        const auto *components = CGColorGetComponents(cg_color);
        return color(static_cast<uint8_t>(std::round(255u * components[0])),
                     static_cast<uint8_t>(std::round(255u * components[1])),
                     static_cast<uint8_t>(std::round(255u * components[2])),
                     static_cast<uint8_t>(std::round(255u * components[3])));

    }

}
