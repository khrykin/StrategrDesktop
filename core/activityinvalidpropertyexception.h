//
// created by dmitry khrykin on 2019-07-03.
//

#ifndef STRATEGR_ACTIVITYINVALIDPROPERTYEXCEPTION_H
#define STRATEGR_ACTIVITYINVALIDPROPERTYEXCEPTION_H

#include "activity.h"

namespace stg {
    class activity::invalid_property_exception : public std::exception {
    public:
        explicit invalid_property_exception(std::string message);
        auto what() const noexcept -> const char * override;

    private:
        std::string message;
    };
};


#endif//STRATEGR_ACTIVITYINVALIDPROPERTYEXCEPTION_H
