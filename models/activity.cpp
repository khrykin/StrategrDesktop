#include <iostream>
#include <algorithm>

#include "activity.h"
#include "activityinvalidpropertyexception.h"

Activity::Activity(Name name, Color color) : _color(std::move(color)) {
    if (!isValid(name)) {
        throw emptyNameException();
    }

    _name = std::move(name);
}

bool Activity::isValid(const Activity::Name &name) {
    bool whiteSpacesOnly = std::all_of(name.begin(), name.end(), isspace);
    return !whiteSpacesOnly;
}

Activity::InvalidPropertyException Activity::emptyNameException() {
    const auto message = "activity name can't be empty";
    return InvalidPropertyException(message);
}

const Activity::Name &Activity::name() const {
    return _name;
}

const Activity::Color &Activity::color() const {
    return _color;
}

bool operator==(const Activity &lhs, const Activity &rhs) {
    return lhs._name == rhs._name &&
           lhs._color == rhs._color;
}

bool operator!=(const Activity &lhs, const Activity &rhs) {
    return !(lhs == rhs);
}

std::ostream &operator<<(std::ostream &os,
                         const Activity &activity) {
    os << "Activity("
       << activity.name()
       << ", "
       << activity.color()
       << ")";

    return os;
}

Activity Activity::copyChangingName(const Name &name) const {
    return Activity(name, color());
}

Activity Activity::copyChangingColor(const Color &color) const {
    return Activity(name(), color);
}
