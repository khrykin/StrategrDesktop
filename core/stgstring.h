//
// Created by Dmitry Khrykin on 15.06.2020.
//

#ifndef STRATEGR_STGSTRING_H
#define STRATEGR_STGSTRING_H

#include <string>
#include <type_traits>
#include <locale>
#include <string>
#include <regex>
#include <codecvt>
#include <utf8proc.h>

#ifdef __OBJC__
@class NSString;
#endif

namespace stg {
    template<typename T, typename = std::string>
    struct is_qstring_like : std::false_type {
    };

    template<typename T>
    struct is_qstring_like<T,
            decltype(std::declval<T>().toStdString())> : std::true_type {
    };

    template<typename T, std::enable_if_t<is_qstring_like<T>::value, int> = 0>
    auto string_cast(const T &other) -> std::string {
        return other.toStdString();
    }

    template<typename T, std::enable_if_t<is_qstring_like<T>::value, int> = 0>
    auto string_cast(std::string &other) -> T * {
        return T::fromStdString(other);
    }

#ifdef __OBJC__

    std::string to_string(NSString *other);
    NSString *to_nsstring(const std::string &other);

#endif

    auto make_string_uuid() -> std::string;

    namespace string {
        inline std::wstring wstring_from_utf8_string(const std::string &str) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.from_bytes(str);
        }

        inline std::string utf8_string_from_wstring(const std::wstring &wstr) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.to_bytes(wstr);
        }

        inline std::string utf8_fold_case(const std::string &str) {
            auto *lowered = (char *) utf8proc_NFKC_Casefold((utf8proc_uint8_t *) str.c_str());
            return std::string(std::unique_ptr<char>(lowered).get());
        }

        inline void strip_bounding_whitespaces(std::string &str) {
            str = std::regex_replace(str, std::regex("^\\s*"), "");
            str = std::regex_replace(str, std::regex("\\s*$"), "");
        }
    }
}


#endif //STRATEGR_STGSTRING_H
