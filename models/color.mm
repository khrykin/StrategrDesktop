//
// Created by Dmitry Khrykin on 26/04/2020.
//

#import <CoreGraphics/CGColor.h>
#include <iostream>

#include "color.h"

namespace stg {
    auto color::to_cg_color() const -> struct CGColor * {
        auto *color_space = CGColorSpaceCreateDeviceRGB();
        auto *cg_color = CGColorCreate(color_space, components().data());
        CGColorSpaceRelease(color_space);

        return cg_color;
    }

    auto color::from_cg_color(CGColor *cg_color) -> color {
        auto color_space_model = CGColorSpaceGetModel(CGColorGetColorSpace(cg_color));

        const auto *components = CGColorGetComponents(cg_color);
        if (color_space_model == kCGColorSpaceModelRGB) {
            return color(static_cast<uint8_t>(255u * components[0]),
                         static_cast<uint8_t>(255u * components[1]),
                         static_cast<uint8_t>(255u * components[2]),
                         static_cast<uint8_t>(255u * components[3]));

        } else if (color_space_model == kCGColorSpaceModelMonochrome) {
            return color(static_cast<uint8_t>(255u * components[0]),
                         static_cast<uint8_t>(255u * components[0]),
                         static_cast<uint8_t>(255u * components[0]));
        } else {
            std::cout << "stg::color Warning: Can't convert from CGColor: unsupported color space\n";

            return color();
        }
    }

}
