//
// Created by Dmitry Khrykin on 2019-12-29.
//

#ifndef STRATEGR_UTILITY_H
#define STRATEGR_UTILITY_H

#include <string>
#include <codecvt>
#include <utf8proc.h>

namespace stg::text {
    inline std::wstring wstring_from_utf8_string(const std::string &str) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    inline std::string string_from_utf8_wstring(const std::wstring &wstr) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes(wstr);
    }

    inline std::string utf8_fold_case(const std::string &str) {
        auto *lowered = (char *) utf8proc_NFKC_Casefold((utf8proc_uint8_t *) str.c_str());
        return std::string(lowered);
    }
}
#endif //STRATEGR_UTILITY_H
