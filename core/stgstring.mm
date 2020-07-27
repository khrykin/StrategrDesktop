//
// Created by Dmitry Khrykin on 15.06.2020.
//
#import <Foundation/NSString.h>

#include "stgstring.h"

namespace stg {
    auto to_string(NSString *other) -> std::string {
        return other.UTF8String;
    }

    auto to_nsstring(const std::string &other) -> NSString * {
        return [NSString stringWithUTF8String:other.c_str()];
    }
}
