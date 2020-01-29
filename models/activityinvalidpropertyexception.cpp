//
// created by dmitry khrykin on 2019-07-03.
//

#include "activityinvalidpropertyexception.h"
#include <utility>

const char *stg::activity::invalid_property_exception::what() const noexcept {
    return message.c_str();
}

stg::activity::invalid_property_exception::invalid_property_exception(std::string message) :
        std::exception(),
        message(std::move(message)) {}
