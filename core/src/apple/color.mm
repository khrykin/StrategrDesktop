//
// Created by Dmitry Khrykin on 26/04/2020.
//

#import <TargetConditionals.h>

#include <cmath>

#include "color.h"

namespace stg {

#if !TARGET_OS_IOS
    static const auto *cg_color_color_space_name = kCGColorSpaceGenericRGB;
#else
    static const auto *cg_color_color_space_name = kCGColorSpaceSRGB;
#endif

    color::color(CGColorRef cg_color) {
        auto *color_space = CGColorSpaceCreateWithName(cg_color_color_space_name);
        auto *rgb_cg_color = CGColorCreateCopyByMatchingToColorSpace(color_space,
                                                                     kCGRenderingIntentDefault,
                                                                     cg_color,
                                                                     NULL);

        const auto *components = CGColorGetComponents(rgb_cg_color);
        set_rgba(static_cast<uint8_t>(std::round(255u * components[0])),
                 static_cast<uint8_t>(std::round(255u * components[1])),
                 static_cast<uint8_t>(std::round(255u * components[2])),
                 static_cast<uint8_t>(std::round(255u * components[3])));

        CGColorSpaceRelease(color_space);
        CGColorRelease(rgb_cg_color);
    }

    color::operator CGColorRef() const {
        auto *color_space = CGColorSpaceCreateWithName(cg_color_color_space_name);
        auto *cg_color = CGColorCreate(color_space, rgb_components().data());

        CGColorSpaceRelease(color_space);
        CFAutorelease(cg_color);

        return cg_color;
    }
}
