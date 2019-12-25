//
// Created by Dmitry Khrykin on 2019-07-03.
//

#include "ActivityInvalidPropertyException.h"
#include <utility>

const char *Activity::InvalidPropertyException::what() const noexcept {
    return message.c_str();
}

Activity::InvalidPropertyException::InvalidPropertyException(std::string message) :
        std::exception(),
        message(std::move(message)) {}
