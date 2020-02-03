#include <iostream>
#include <algorithm>

#include "activity.h"
#include "activityinvalidpropertyexception.h"

stg::activity::activity(name_t name, color_t color) : _color(std::move(color)) {
    if (!is_valid(name)) {
        throw empty_name_exception();
    }

    _name = std::move(name);
}

bool stg::activity::is_valid(const activity::name_t &name) {
    bool white_spaces_only = name.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
    return !white_spaces_only;
}

stg::activity::invalid_property_exception stg::activity::empty_name_exception() {
    const auto message = "activity name can't be empty";
    return invalid_property_exception(message);
}

const stg::activity::name_t &stg::activity::name() const {
    return _name;
}

const stg::activity::color_t &stg::activity::color() const {
    return _color;
}

bool stg::operator==(const stg::activity &lhs, const stg::activity &rhs) {
    return lhs.name() == rhs.name() &&
           lhs.color() == rhs.color();
}

bool stg::operator!=(const stg::activity &lhs, const stg::activity &rhs) {
    return !(lhs == rhs);
}

std::ostream &stg::operator<<(std::ostream &os,
                              const stg::activity &activity) {
    os << "activity("
       << activity.name()
       << ", "
       << activity.color()
       << ")";

    return os;
}

stg::activity stg::activity::copy_changing_name(const name_t &name) const {
    return activity(name, _color);
}

stg::activity stg::activity::copy_changing_color(const color_t &color) const {
    return activity(_name, color);
}

nlohmann::json stg::activity::to_json() {
    nlohmann::json j;
    j[keys::name] = this->name();
    j[keys::color] = this->color();
    return j;
}

stg::activity stg::activity::from_json(const nlohmann::json &j) {
    auto name = j[keys::name];

    std::string color = activity::default_color;
    if (j.count(keys::color) && !j[keys::color].is_null()) {
        color = j[keys::color];
    }

    return activity{name, color};
}