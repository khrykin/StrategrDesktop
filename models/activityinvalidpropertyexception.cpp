//
// Created by Dmitry Khrykin on 2019-07-03.
//

#include "activityinvalidpropertyexception.h"


const char *Activity::InvalidPropertyException::what() const noexcept {
    auto whatMessage = propertyName + ": " + message;
    return whatMessage.data();
}

Activity::InvalidPropertyException::InvalidPropertyException(
        std::string propertyName,
        std::string message) :
        std::exception(),
        message(std::move(message)),
        propertyName(std::move(propertyName)) {}
