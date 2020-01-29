//
// Created by Dmitry Khrykin on 2019-12-29.
//

#ifndef STRATEGR_UTILITY_H
#define STRATEGR_UTILITY_H

#include <string>
#include <utf8proc.h>

namespace stg {
    inline std::string utf8_fold_case(const std::string &str) {
        auto *lowered = (char *) utf8proc_NFKC_Casefold((utf8proc_uint8_t *) str.c_str());
        return std::string(lowered);
    }

    inline bool utf8_is_equal_case_insensitive(const std::string &str1, const std::string &str2) {
        char *lower1 = (char *) utf8proc_NFKC_Casefold((utf8proc_uint8_t *) str1.c_str());
        char *lower2 = (char *) utf8proc_NFKC_Casefold((utf8proc_uint8_t *) str2.c_str());

        return strcmp(lower1, lower2) == 0;
    }
}
#endif //STRATEGR_UTILITY_H
