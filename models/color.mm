//
// Created by Dmitry Khrykin on 26/04/2020.
//

#import <TargetConditionals.h>
#import <CoreGraphics/CGColor.h>

#include <cmath>

#include "color.h"

namespace stg {

#if !TARGET_OS_IOS
    const auto *cg_color_color_space_name = kCGColorSpaceGenericRGB;
#else
    const auto *cg_color_color_space_name = kCGColorSpaceSRGB;
#endif

    auto color::to_cg_color() const -> struct CGColor * {
        auto *color_space = CGColorSpaceCreateWithName(cg_color_color_space_name);
        auto *cg_color = CGColorCreate(color_space, components().data());

        CGColorSpaceRelease(color_space);
        CFAutorelease(cg_color);

        return cg_color;
    }

    auto color::from_cg_color(CGColor *cg_color) -> color {
        auto *color_space = CGColorSpaceCreateWithName(cg_color_color_space_name);
        auto *rgb_cg_color = CGColorCreateCopyByMatchingToColorSpace(color_space,
                                                                     kCGRenderingIntentDefault,
                                                                     cg_color,
                                                                     NULL);
        CGColorSpaceRelease(color_space);
        CFAutorelease(rgb_cg_color);

        const auto *components = CGColorGetComponents(rgb_cg_color);
        return color(static_cast<uint8_t>(std::round(255u * components[0])),
                     static_cast<uint8_t>(std::round(255u * components[1])),
                     static_cast<uint8_t>(std::round(255u * components[2])),
                     static_cast<uint8_t>(std::round(255u * components[3])));

    }

}
