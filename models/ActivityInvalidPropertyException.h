//
// Created by Dmitry Khrykin on 2019-07-03.
//

#ifndef MODELS_ACTIVITYINVALIDPROPERTYEXCEPTION_H
#define MODELS_ACTIVITYINVALIDPROPERTYEXCEPTION_H

#include "Activity.h"

class Activity::InvalidPropertyException : public std::exception {
public:
    explicit InvalidPropertyException(std::string message);
    const char *what() const noexcept override;

private:
    std::string message;
};


#endif //MODELS_ACTIVITYINVALIDPROPERTYEXCEPTION_H
