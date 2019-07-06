//
// Created by Dmitry Khrykin on 2019-07-03.
//

#ifndef MODELS_ACTIVITYINVALIDPROPERTYEXCEPTION_H
#define MODELS_ACTIVITYINVALIDPROPERTYEXCEPTION_H

#include "activity.h"

class Activity::InvalidPropertyException : public std::exception {
public:
    static constexpr auto nameProperty = "name";
    static constexpr auto colorProperty = "color";
    static constexpr auto defaultMessage = "invalid value";

    explicit InvalidPropertyException(std::string propertyName,
                                      std::string message = defaultMessage);

    const char *what() const noexcept override;

private:
    std::string propertyName;
    std::string message;
};


#endif //MODELS_ACTIVITYINVALIDPROPERTYEXCEPTION_H
