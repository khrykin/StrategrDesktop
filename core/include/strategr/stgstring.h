//
// Created by Dmitry Khrykin on 15.06.2020.
//

#ifndef STRATEGR_STGSTRING_H
#define STRATEGR_STGSTRING_H

#include <string>
#include <type_traits>

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
        auto utf8_fold_case(const std::string &str) -> std::string;
        auto wstring_from_utf8_string(const std::string &str) -> std::wstring;
        auto utf8_string_from_wstring(const std::wstring &wstr) -> std::string;

        void strip_bounding_whitespaces(std::string &str);
    }
}


#endif//STRATEGR_STGSTRING_H
