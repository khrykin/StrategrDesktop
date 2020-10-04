//
// created by dmitry khrykin on 2019-07-03.
//

#include "activityinvalidpropertyexception.h"
#include <utility>

namespace stg {
    activity::invalid_property_exception::invalid_property_exception(std::string message) : std::exception(),
                                                                                            message(std::move(message)) {}

    auto activity::invalid_property_exception::what() const noexcept -> const char * {
        return message.c_str();
    }
}