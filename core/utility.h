//
// Created by Dmitry Khrykin on 2019-12-29.
//

#ifndef STRATEGR_UTILITY_H
#define STRATEGR_UTILITY_H

#include <vector>

#ifndef STG_FORWARD_DECLARE_OBJC_CLASS
#  ifdef __OBJC__
#    define STG_FORWARD_DECLARE_OBJC_CLASS(classname) @class classname
#  else
#    define STG_FORWARD_DECLARE_OBJC_CLASS(classname) typedef struct objc_object classname
#  endif
#endif

namespace stg {
    using raw_buffer = std::vector<uint8_t>;
}

#endif //STRATEGR_UTILITY_H
